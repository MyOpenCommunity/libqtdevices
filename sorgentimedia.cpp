/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** sorgentimedia.cpp
 **
 **definizioni dei vari sorgenti multimediali implementati
 **
 ****************************************************************/

#include "sorgentimedia.h"
#include "main.h" // ICON_CICLA, ICON_FFWD, ICON_REW, ICON_IMPOSTA
#include "device_cache.h"
#include "device.h"

#include <openwebnet.h> // class openwebnet

#include <QByteArray>
#include <QDebug>

/*****************************************************************
 **SorgenteMultimedia
 ****************************************************************/
BannerSorgenteMultimedia::BannerSorgenteMultimedia(QWidget *parent, const char *name, char *indirizzo, int where, int nbut) :
	bannCiclaz(parent, name, nbut), source_menu(NULL, name, indirizzo, where)
{
	SetIcons(ICON_CICLA, ICON_IMPOSTA, ICON_FFWD, ICON_REW);

	setAddress(indirizzo);
	dev = btouch_device_cache.get_device(getAddress());
	connect(parentWidget(), SIGNAL(frez(bool)), &source_menu, SLOT(freezed(bool)));

	connect(this, SIGNAL(dxClick()), &source_menu, SLOT(showPage()));
	if (nbut == 4)
	{
		connect(this, SIGNAL(sxClick()), this, SLOT(ciclaSorg()));
		connect(this  ,SIGNAL(csxClick()),this,SLOT(decBrano()));
		connect(this  ,SIGNAL(cdxClick()),this,SLOT(aumBrano()));
	}

	QWidget *sotto_menu = this->parentWidget()->parentWidget();
	connect(&source_menu, SIGNAL(Closed()), sotto_menu, SLOT(show()));
	connect(&source_menu, SIGNAL(Closed()), &source_menu, SLOT(hide()));
	connect(&source_menu, SIGNAL(sendFrame(char *)), this, SIGNAL(sendFrame(char *)));
}

void BannerSorgenteMultimedia::ciclaSorg()
{
	openwebnet msg_open;
	qDebug("BannerSorgenteMultimedia::ciclaSorg()");
	char pippo[50];
	char amb[3];

	sprintf(amb, getAddress());
	memset(pippo,'\000',sizeof(pippo));
	sprintf(pippo,"*22*22#4#1*5#2#%c##", amb[2]);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	dev->sendFrame(msg_open.frame_open);
}

void BannerSorgenteMultimedia::decBrano()
{
	source_menu.prevTrack();
}

void BannerSorgenteMultimedia::aumBrano()
{
	source_menu.nextTrack();
}

void BannerSorgenteMultimedia::menu() {}

void BannerSorgenteMultimedia::gestFrame(char *frame)
{
	openwebnet msg_open;
	char amb[3];
	qDebug("BannerSorgenteMultimedia::gestFrame()");

	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
	sprintf(amb, getAddress());
	if ((!strcmp(msg_open.Extract_chi(),"22")) &&
	    (!strncmp(msg_open.Extract_cosa(),"2", 1)) &&
	    (!strcmp(msg_open.Extract_dove(),"5")) && 
	    (!strcmp(msg_open.Extract_livello(),"2")))
	{
		if (!strcmp(msg_open.Extract_interfaccia(), amb+2))
		{
			source_menu.enableSource(false);
			source_menu.resume();
		}
		else
		{
			source_menu.disableSource(false);
			source_menu.pause();
		}
	}
}

void BannerSorgenteMultimedia::hide() 
{
	qDebug("BannerSorgenteMultimedia::hide()");
	banner::hide();
	source_menu.hide();
}

void BannerSorgenteMultimedia::inizializza(bool forza)
{
	qDebug("BannerSorgenteMultimedia::inizializza()");
	openwebnet msg_open;
	char amb[3];

	sprintf(amb, getAddress());
	dev->sendInit(QString("*#22*7*#15*%1***4**0**1*1**0##").arg(amb[2]));
}

/*
 * Banner Sorgente Multimediale Multicanale
 */
BannerSorgenteMultimediaMC::BannerSorgenteMultimediaMC(QWidget *parent, const char *name, char *indirizzo, int where,
	const char *icon_onoff, const char *icon_cycle, const char *icon_settings) :
	BannerSorgenteMultimedia(parent, name, indirizzo, where, 3)
{
	SetIcons(icon_onoff, NULL, icon_cycle, icon_settings);

	indirizzo_semplice = QString(indirizzo);
	indirizzi_ambienti.clear();

	connect(this, SIGNAL(sxClick()), this, SLOT(attiva()));
	multiamb = false;
}

void BannerSorgenteMultimediaMC::attiva()
{
	char    pippo[50];
	openwebnet msg_open;

	qDebug("BannerSorgenteMultimediaMC::attiva()");
	if (!multiamb)
	{
		memset(pippo,'\000',sizeof(pippo));
		sprintf(pippo,"*22*35#4#%d#%d*4#%d##",indirizzo_ambiente, indirizzo_semplice.toInt(), indirizzo_ambiente);
		msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
		dev->sendFrame(msg_open.frame_open);
		emit active(indirizzo_ambiente, indirizzo_semplice.toInt());
		source_menu.enableSource(false);
		source_menu.resume();
	}
	else
	{
		QStringList::Iterator it;
		for (it = indirizzi_ambienti.begin(); it != indirizzi_ambienti.end(); ++it)
		{
			QByteArray buf = (*it).toAscii();
			memset(pippo,'\000',sizeof(pippo));
			strcat(pippo,"*22*0#4#");
			strcat(pippo,buf.constData());
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
			strcat(pippo,buf.constData());
			strcat(pippo,"*2#");
			QByteArray buf_ind = indirizzo_semplice.toAscii();
			strcat(pippo, buf_ind.constData());
			strcat(pippo,"##");
			msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
			dev->sendFrame(msg_open.frame_open);
			memset(pippo,'\000',sizeof(pippo));
			strcat(pippo,"*#16*1000*11##");
			msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
			dev->sendFrame(msg_open.frame_open);
		}
		source_menu.enableSource(false);
		source_menu.resume();
	}
}

void BannerSorgenteMultimediaMC::ambChanged(const QString & ad, bool multi, char *indamb)
{
	// FIXME: PROPAGA LA VARIAZIONE DI DESCRIZIONE AMBIENTE
	qDebug() << "BannerSorgenteMultimediaMC::ambChanged(" << ad << ", " << multi << ", " << indamb << ")";
	QString dove;

	if (!multi)
	{
		multiamb = false;
		indirizzo_ambiente = QString((const char *)indamb).toInt();
		dove = QString::number(100 + indirizzo_ambiente * 10 + indirizzo_semplice.toInt(), 10);
	}
	else
	{
		multiamb = true;
		dove = QString::number(100 + indirizzo_semplice.toInt(), 10);
	}
	setAddress(dove);
	source_menu.setAmbDescr(ad);

	qDebug() << "Source where is now %s" << dove;
}

void BannerSorgenteMultimediaMC::addAmb(char *a)
{
	indirizzi_ambienti += QString(a);
}

void BannerSorgenteMultimediaMC::inizializza(bool forza)
{
	qDebug("BannerSorgenteMultimediaMC::inizializza()");

	dev->sendInit(QString("*#22*7*#15*%1***4**0*%2*1*1**0##").arg(indirizzo_semplice).arg(indirizzo_semplice));
}

void BannerSorgenteMultimediaMC::gestFrame(char *frame)
{
	openwebnet msg_open;
	qDebug("BannerSorgenteMultimediaMC::gestFrame()");

	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
	if ((!strcmp(msg_open.Extract_chi(),"22")) &&
	    (!strcmp(msg_open.Extract_grandezza(),"12")) &&
	    (!strcmp(msg_open.Extract_dove(),"5")) && 
	    (!strcmp(msg_open.Extract_livello(),"2")))
	{

		if (indirizzo_semplice == msg_open.Extract_interfaccia() &&
		    !strcmp(msg_open.Extract_valori(0), "0"))
		{
			source_menu.disableSource(false);
			source_menu.pause();
		}
	}
}
