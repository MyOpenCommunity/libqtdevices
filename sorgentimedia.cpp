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
#include "btmain.h"
#include "main.h" // ICON_CICLA, ICON_FFWD, ICON_REW, ICON_IMPOSTA, BTouch
#include "btmain.h"

#include <openwebnet.h> // class openwebnet

#include <QByteArray>
#include <QDebug>

/*****************************************************************
 **SorgenteMultimedia
 ****************************************************************/
BannerSorgenteMultimedia::BannerSorgenteMultimedia(QWidget *parent, QString indirizzo, int where, int nbut) :
	bannCiclaz(parent, nbut), source_menu(NULL, where) // where is the number of source in mono and indirizzo in multi!
{
	SetIcons(ICON_CICLA, ICON_IMPOSTA, ICON_FFWD, ICON_REW);
	setAddress(indirizzo);

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
}

void BannerSorgenteMultimedia::ciclaSorg()
{
	qDebug("BannerSorgenteMultimedia::ciclaSorg()");
	BTouch->sendFrame(QString("*22*22#4#1*5#2#%1##").arg(getAddress().at(2)));
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
	sprintf(amb, getAddress().toAscii().constData());
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

void BannerSorgenteMultimedia::hideEvent(QHideEvent *event)
{
	qDebug("BannerSorgenteMultimedia::hideEvent()");
	source_menu.hide();
}

void BannerSorgenteMultimedia::inizializza(bool forza)
{
	qDebug("BannerSorgenteMultimedia::inizializza()");
	BTouch->sendInit(QString("*#22*7*#15*%1***4**0**1*1**0##").arg(getAddress().at(2)));
}

/*
 * Banner Sorgente Multimediale Multicanale
 */
BannerSorgenteMultimediaMC::BannerSorgenteMultimediaMC(QWidget *parent, QString indirizzo, int where,
	QString icon_onoff, QString icon_cycle, QString icon_settings) : BannerSorgenteMultimedia(parent, indirizzo, where, 3)
{
	SetIcons(icon_onoff, QString(), icon_cycle, icon_settings);

	indirizzo_semplice = indirizzo;
	indirizzi_ambienti.clear();

	connect(this, SIGNAL(sxClick()), this, SLOT(attiva()));
	multiamb = false;
}

void BannerSorgenteMultimediaMC::attiva()
{
	qDebug("BannerSorgenteMultimediaMC::attiva()");

	if (!multiamb)
	{
		QString f = QString("*22*35#4#%1#%2*4#%1##").arg(indirizzo_ambiente).arg(indirizzo_semplice.toInt());
		BTouch->sendFrame(f);
		emit active(indirizzo_ambiente, indirizzo_semplice.toInt());
		source_menu.enableSource(false);
		source_menu.resume();
	}
	else
	{
		QStringList::Iterator it;
		for (it = indirizzi_ambienti.begin(); it != indirizzi_ambienti.end(); ++it)
		{
			BTouch->sendFrame("*22*0#4#" + *it + "*6##");
			BTouch->sendFrame("*#16*1000*11##");
			BTouch->sendFrame("*22*1#4#" + *it + "*2#" + indirizzo_semplice + "##");
			BTouch->sendFrame("*#16*1000*11##");
		}
		source_menu.enableSource(false);
		source_menu.resume();
	}
}

void BannerSorgenteMultimediaMC::ambChanged(const QString & ad, bool multi, QString indamb)
{
	// FIXME: PROPAGA LA VARIAZIONE DI DESCRIZIONE AMBIENTE
	qDebug() << "BannerSorgenteMultimediaMC::ambChanged(" << ad << ", " << multi << ", " << indamb << ")";
	QString dove;

	if (!multi)
	{
		multiamb = false;
		indirizzo_ambiente = indamb.toInt();
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

	BTouch->sendInit(QString("*#22*7*#15*%1***4**0*%2*1*1**0##").arg(indirizzo_semplice).arg(indirizzo_semplice));
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
