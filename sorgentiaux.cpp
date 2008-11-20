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
#include "main.h" // ICON_CICLA, ICON_FFWD, ICON_REW, BTouch
#include "aux.h" // class myAux
#include "btmain.h"
#include "generic_functions.h" // createMsgOpen

#include <QByteArray>
#include <QWidget>
#include <QDebug>

/*****************************************************************
 **sorgente_aux
 ****************************************************************/
sorgente_aux::sorgente_aux(QWidget *parent,const char *name,char* indirizzo, bool vecchio, char *ambdescr)
	: bannCiclaz(parent, vecchio ? 4 : 3)
{
	SetIcons(ICON_CICLA,NULL,ICON_FFWD,ICON_REW);

	vecchia = vecchio;
	setAddress(indirizzo);

	if (vecchio)
	{
		connect(this  ,SIGNAL(sxClick()),this,SLOT(ciclaSorg()));
		connect(this  ,SIGNAL(csxClick()),this,SLOT(decBrano()));
		connect(this  ,SIGNAL(cdxClick()),this,SLOT(aumBrano()));
		nascondi(BUT2);
	}
	else
		myAux = new aux(NULL, name, ambdescr);
}

void sorgente_aux::gestFrame(char*)
{
}

void sorgente_aux::ciclaSorg()
{
	char amb[3];
	sprintf(amb, getAddress());
	BTouch->sendFrame(QString("*22*22#4#1*5#2#%1##").arg(amb[2]));
}

void sorgente_aux::decBrano()
{
	BTouch->sendFrame(createMsgOpen("16","6101", getAddress()));
}

void sorgente_aux::aumBrano()
{
	char amb[2];
	sprintf(amb, getAddress());
	if (!vecchia)
		if (amb[1] == '0')
			amb[1] = '1';
	BTouch->sendFrame(createMsgOpen("16","6001", amb));
}

void sorgente_aux::inizializza(bool forza)
{
}

void sorgente_aux::hideEvent(QHideEvent *event)
{
	qDebug("sorgente::hideEvent()");
	if (vecchia)
		return;
	myAux->hide();
}

/*****************************************************************
 ** Sorgente aux diffusione sonora multicanale
 ****************************************************************/

sorgenteMultiAux::sorgenteMultiAux(QWidget *parent, const char *name, char *indirizzo, QString Icona1, QString Icona2, QString Icona3, char *ambdescr)
	: sorgente_aux(parent, name, indirizzo, false, ambdescr)
{
	qDebug("sorgenteMultiAux::sorgenteMultiAux()");
	SetIcons(Icona1, Icona2, QString(), Icona3);
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

	if (!multiamb)
	{
		QString f = QString("*22*35#4#%1#%2*4#%1##").arg(indirizzo_ambiente).arg(indirizzo_semplice.toInt());
		BTouch->sendFrame(f);
		emit active(indirizzo_ambiente, indirizzo_semplice.toInt());
	}
	else
	{
		qDebug("DA INSIEME AMBIENTI. CI SONO %d INDIRIZZI", indirizzi_ambienti.count());
		for (QStringList::Iterator it = indirizzi_ambienti.begin(); it != indirizzi_ambienti.end(); ++it)
		{
			BTouch->sendFrame("*22*0#4#" + *it + "*6##");
			BTouch->sendFrame("*#16*1000*11##");
			BTouch->sendFrame("*22*1#4#" + *it + "*2#" + indirizzo_semplice + "##");
			BTouch->sendFrame("*#16*1000*11##");
		}
	}
}

void sorgenteMultiAux::ambChanged(const QString & ad, bool multi, char *indamb)
{
	qDebug() << "sorgenteMultiRadio::ambChanged(" << ad << ", " << multi << ", " << indamb << ")";
	if (!multi)
	{
		multiamb = false;
		indirizzo_ambiente = QString((const char *)indamb).toInt();
		QString dove(QString::number(100 + QString((const char *)indamb).toInt() * 10 +
			indirizzo_semplice.toInt(), 10));
		qDebug() << "Source where is now " << dove;
		setAddress(dove);
	}
	else
	{
		QString dove(QString::number(100 + indirizzo_semplice.toInt(), 10));
		qDebug() << "Source where is now " << dove;
		setAddress(dove);
		multiamb = true;
	}
	myAux->setAmbDescr(ad);
}

void sorgenteMultiAux::addAmb(char *a)
{
	qDebug("sorgenteMultiAux::addAmb(%s)", a);
	indirizzi_ambienti += QString(a);
}
