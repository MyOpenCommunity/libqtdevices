/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** sorgentiaux.cpp
 **
 **definizioni delle sorgenti aux implementate
 **
 ****************************************************************/

#include "sorgentiaux.h"
#include "main.h" // ICON_CICLA, ICON_FFWD, ICON_REW
#include "aux.h" // class myAux
#include "device_cache.h" // btouch_device_cache
#include "device.h"

#include <openwebnet.h> // class openwebnet

/*****************************************************************
 **sorgente_aux
 ****************************************************************/
sorgente_aux::sorgente_aux(QWidget *parent,const char *name,char* indirizzo, bool vecchio, char *ambdescr)
	: bannCiclaz(parent, name, vecchio ? 4 : 3)
{
	SetIcons(ICON_CICLA,NULL,ICON_FFWD,ICON_REW);

	vecchia = vecchio;
	setAddress(indirizzo);
	dev = btouch_device_cache.get_device(getAddress());

	if (vecchio)
	{
		connect(this  ,SIGNAL(sxClick()),this,SLOT(ciclaSorg()));
		connect(this  ,SIGNAL(csxClick()),this,SLOT(decBrano()));
		connect(this  ,SIGNAL(cdxClick()),this,SLOT(aumBrano()));
		nascondi(BUT2);
	}
	else
	{
		myAux = new aux(NULL, name, ambdescr);
		myAux->setBGColor(parentWidget(TRUE)->backgroundColor());
		myAux->setFGColor(parentWidget(TRUE)->foregroundColor());
		// Get freezed events
		connect(parent, SIGNAL(frez(bool)), myAux, SLOT(freezed(bool)));
	}
}

void sorgente_aux::gestFrame(char*)
{
}

void sorgente_aux::ciclaSorg()
{
	openwebnet msg_open;
	char pippo[50];
	char amb[3];

	sprintf(amb, getAddress());
	memset(pippo,'\000',sizeof(pippo));
	sprintf(pippo,"*22*22#4#1*5#2#%c##", amb[2]);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	dev->sendFrame(msg_open.frame_open);
}

void sorgente_aux::decBrano()
{
	openwebnet msg_open;
	msg_open.CreateMsgOpen("16","6101",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}

void sorgente_aux::aumBrano()
{
	openwebnet msg_open;
	char amb[2];
	sprintf(amb, getAddress());
	if (!vecchia)
		if (amb[1] == '0')
			amb[1] = '1';

	msg_open.CreateMsgOpen("16","6001",amb,"");
	dev->sendFrame(msg_open.frame_open);
}

void sorgente_aux::inizializza(bool forza)
{
}

void sorgente_aux::hide()
{
	qDebug("sorgente::hide()");
	banner::hide();
	if (vecchia)
		return;
	myAux->hide();
}

/*****************************************************************
 ** Sorgente aux diffusione sonora multicanale
 ****************************************************************/

sorgenteMultiAux::sorgenteMultiAux(QWidget *parent,const char *name,char* indirizzo,char* Icona1,char* Icona2, char *Icona3, char *ambdescr)
	: sorgente_aux(parent, name, indirizzo, false, ambdescr)
{
	qDebug("sorgenteMultiAux::sorgenteMultiAux() : %s %s %s", Icona1, Icona2, Icona3);
	SetIcons(Icona1, Icona2, NULL, Icona3);
	indirizzo_semplice = QString(indirizzo);
	indirizzi_ambienti.clear();
	connect(this, SIGNAL(dxClick()), myAux, SLOT(showAux()));
	connect(this, SIGNAL(sxClick()), this, SLOT(attiva()));
	connect(myAux, SIGNAL(Closed()), myAux, SLOT(hide()));
	connect(myAux, SIGNAL(Closed()), this, SLOT(show()));
	connect(myAux, SIGNAL(Btnfwd()), this, SLOT(aumBrano()));
	multiamb = false;
}

void sorgenteMultiAux::attiva()
{
	qDebug("sorgenteMultiAux::attiva()");
	char pippo[50];
	openwebnet msg_open;
  
	if (!multiamb)
	{
		memset(pippo,'\000',sizeof(pippo));
		sprintf(pippo,"*22*35#4#%d#%d*4#%d##",indirizzo_ambiente, indirizzo_semplice.toInt(), indirizzo_ambiente);
		msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
		dev->sendFrame(msg_open.frame_open);
		emit active(indirizzo_ambiente, indirizzo_semplice.toInt());
	}
	else
	{
		qDebug("DA INSIEME AMBIENTI. CI SONO %d INDIRIZZI", indirizzi_ambienti.count());
		for (QStringList::Iterator it = indirizzi_ambienti.begin(); it != indirizzi_ambienti.end(); ++it)
		{
			memset(pippo,'\000',sizeof(pippo));
			strcat(pippo,"*22*0#4#");
			strcat(pippo,(*it));
			strcat(pippo,"*6");
			strcat(pippo,"##");
			msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
			dev->sendFrame(msg_open.frame_open);
			memset(pippo,'\000',sizeof(pippo));
			strcat(pippo,"*#16*1000*11##");
			msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
			dev->sendFrame(msg_open.frame_open);
			memset(pippo,'\000',sizeof(pippo));
			strcat(pippo,"*22*1#4#");
			strcat(pippo,(*it));
			strcat(pippo,"*2#");
			strcat(pippo, indirizzo_semplice);
			strcat(pippo,"##");
			msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
			dev->sendFrame(msg_open.frame_open);
			memset(pippo,'\000',sizeof(pippo));
			strcat(pippo,"*#16*1000*11##");
			msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
			dev->sendFrame(msg_open.frame_open);
		}
	}
}

void sorgenteMultiAux::ambChanged(const QString & ad, bool multi, char *indamb)
{
	qDebug("sorgenteMultiAux::ambChanged(%s, %d, %s)", ad.ascii(), multi, indamb);
	if (!multi)
	{
		multiamb = false;
		indirizzo_ambiente = QString((const char *)indamb).toInt();
		QString *dove = new QString(QString::number(100 + QString((const char *)indamb).toInt() * 10 +
			indirizzo_semplice.toInt(), 10));
		qDebug("Source where now = %s", dove->ascii());
		setAddress((char *)dove->ascii());
		delete dove;
	}
	else
	{
		QString *dove = new QString(QString::number(100 + indirizzo_semplice.toInt(), 10));
		qDebug("Source where is now %s", dove->ascii());
		setAddress((char *)dove->ascii());
		delete dove;
		multiamb = true;
	}
	myAux->setAmbDescr(ad);
}

void sorgenteMultiAux::addAmb(char *a)
{
	qDebug("sorgenteMultiAux::addAmb(%s)", a);
	indirizzi_ambienti += QString(a);
}
