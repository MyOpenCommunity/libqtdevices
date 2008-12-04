/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** sorgentiradio.cpp
 **
 **definizioni delle sorgenti radio implementate
 **
 ****************************************************************/

#include "sorgentiradio.h"
#include "main.h" // ICON_CICLA
#include "radio.h"
#include "device_cache.h" // btouch_device_cache
#include "device.h"
#include "generic_functions.h" // createMsgOpen

#include <QByteArray>
#include <QWidget>
#include <QDebug>
#include <QChar>


/*****************************************************************
 **sorgente_Radio
 ****************************************************************/
banradio::banradio(QWidget *parent, QString indirizzo, int nbut, const QString & ambdescr)
	: bannCiclaz(parent, nbut)
{
	SetIcons(ICON_CICLA,ICON_IMPOSTA,ICON_FFWD,ICON_REW);
	setAddress(indirizzo);
	myRadio = new radio(NULL, ambdescr);
	myRadio->setRDS("");
	myRadio->setFreq(0.00);
	// TODO: non e' bello fare affidamento su chi e' il grandad!
	QWidget *grandad = this->parentWidget()->parentWidget();

	myRadio->setStaz((uchar)1);

	if (nbut == 4)
	{
		// Old difson
		old_diffson = true;
		connect(this  ,SIGNAL(sxClick()),this,SLOT(ciclaSorg()));
		connect(this  ,SIGNAL(csxClick()),this,SLOT(decBrano()));
		connect(this  ,SIGNAL(cdxClick()),this,SLOT(aumBrano()));
	} 
	else
		old_diffson = false;

	connect(this  ,SIGNAL(dxClick()),myRadio,SLOT(showRadio()));
	connect(this , SIGNAL(dxClick()),this,SLOT(startRDS()));
	if (!old_diffson)
		connect(this, SIGNAL(dxClick()), this, SLOT(richFreq()));

	connect(myRadio,SIGNAL(Closed()), grandad, SLOT(show()));
	connect(myRadio,SIGNAL(Closed()),myRadio,SLOT(hide()));
	connect(myRadio,SIGNAL(Closed()),this,SLOT(stopRDS()));

	connect(myRadio,SIGNAL(decFreqAuto()),this,SLOT(decFreqAuto()));
	connect(myRadio,SIGNAL(aumFreqAuto()),this,SLOT(aumFreqAuto()));
	connect(myRadio,SIGNAL(decFreqMan()),this,SLOT(decFreqMan()));
	connect(myRadio,SIGNAL(aumFreqMan()),this,SLOT(aumFreqMan()));
	connect(myRadio,SIGNAL(changeStaz()),this,SLOT(changeStaz()));
	connect(myRadio,SIGNAL(memoFreq(uchar)),this,SLOT(memoStaz(uchar)));
	connect(myRadio,SIGNAL(richFreq()),this,SLOT(richFreq()));

	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_radio_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)), this, SLOT(status_changed(QList<device_status*>)));
}

void banradio::grandadChanged(QWidget *newGrandad)
{
	qDebug("banradio::grandadChanged (%p)", newGrandad);
	QWidget *grandad = this->parentWidget()->parentWidget();
	disconnect(myRadio,SIGNAL(Closed()), grandad, SLOT(showFullScreen()));
	grandad = newGrandad;
	connect(myRadio,SIGNAL(Closed()), grandad, SLOT(showFullScreen()));
}

void banradio::status_changed(QList<device_status*> sl)
{
	// TODO: remove duplicate code from RDS stat var!!
	stat_var curr_freq(stat_var::FREQ);
	stat_var curr_staz(stat_var::STAZ);
	stat_var curr_rds0(stat_var::RDS0);
	stat_var curr_rds1(stat_var::RDS1);
	stat_var curr_rds2(stat_var::RDS2);
	stat_var curr_rds3(stat_var::RDS3);
	stat_var curr_rds4(stat_var::RDS4);
	stat_var curr_rds5(stat_var::RDS5);
	stat_var curr_rds6(stat_var::RDS6);
	stat_var curr_rds7(stat_var::RDS7);
	bool aggiorna = false;
	qDebug("bannradio::status_changed()");
	float freq;

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::RADIO:
		{
			qDebug("Radio status variation");
			ds->read(device_status_radio::FREQ_INDEX, curr_freq);
			ds->read(device_status_radio::STAZ_INDEX, curr_staz);
			ds->read(device_status_radio::RDS0_INDEX, curr_rds0);
			ds->read(device_status_radio::RDS1_INDEX, curr_rds1);
			ds->read(device_status_radio::RDS2_INDEX, curr_rds2);
			ds->read(device_status_radio::RDS3_INDEX, curr_rds3);
			ds->read(device_status_radio::RDS4_INDEX, curr_rds4);
			ds->read(device_status_radio::RDS5_INDEX, curr_rds5);
			ds->read(device_status_radio::RDS6_INDEX, curr_rds6);
			ds->read(device_status_radio::RDS7_INDEX, curr_rds7);
			freq = (float)curr_freq.get_val()/1000.0F;
			myRadio->setFreq(freq);
			myRadio->setStaz((uchar)curr_staz.get_val());

			QString qrds;
			qrds += QChar(curr_rds0.get_val());
			qrds += QChar(curr_rds1.get_val());
			qrds += QChar(curr_rds2.get_val());
			qrds += QChar(curr_rds3.get_val());
			qrds += QChar(curr_rds4.get_val());
			qrds += QChar(curr_rds5.get_val());
			qrds += QChar(curr_rds6.get_val());
			qrds += QChar(curr_rds7.get_val());
			qDebug() << "*** setting rds to " << qrds;
			myRadio->setRDS(qrds);
			aggiorna = 1;
			break;
		}
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}

	if (aggiorna)
		myRadio->draw();
}

void banradio::pre_show()
{
	if (old_diffson)
		dev->sendInit("*#16*" + getAddress() + "*6##");
}

void banradio::showEvent(QShowEvent *event)
{
	pre_show();

	if (!old_diffson)
	{
		if (parentWidget()->parentWidget()->parentWidget())
			nascondi(BUT2);
		else
			mostra(BUT2);
	}
	mostra(BUT2);
}

void banradio::hideEvent(QHideEvent *event)
{
	qDebug("banradio::hideEvent()");
	if (!myRadio->isHidden())
		stopRDS();
	myRadio->hide();
}

void banradio::setText(const QString & qtext)
{
	banner::setText(qtext);
	myRadio->setName(qtext);
}

void banradio::ciclaSorg()
{
	qDebug("banradio::ciclaSorg()");
	dev->sendFrame(QString("*22*22#4#1*5#2#%1##").arg(getAddress().at(2)));
}

void banradio::decBrano()
{
	dev->sendFrame(createMsgOpen("16", "6101", getAddress()));
}

void banradio::aumBrano()
{
	dev->sendFrame(createMsgOpen("16", "6001", getAddress()));
}

void banradio::aumFreqAuto()
{
	myRadio->setFreq(0.00);
	myRadio->setRDS("- - - - ");
	myRadio->draw();
	dev->sendFrame(createMsgOpen("16", "5000", getAddress()));
}

void banradio::decFreqAuto()
{
	myRadio->setFreq(0.00);
	myRadio->setRDS("- - - - ");
	myRadio->draw();
	dev->sendFrame(createMsgOpen("16", "5100", getAddress()));
}

void banradio::aumFreqMan()
{
	float f;

	f = myRadio->getFreq();
	if (f < 108.00)
		f += 0.05;
	else
		f = 87.50;
	myRadio->setFreq(f);
	myRadio->draw();
	dev->sendFrame(createMsgOpen("16", "5001", getAddress()));
}

void banradio::decFreqMan()
{
	float f;

	f = myRadio->getFreq();
	if (f > 87.50)
		f -= 0.05;
	else
		f = 108.00;
	myRadio->setFreq(f);
	myRadio->draw();
	dev->sendFrame(createMsgOpen("16", "5101", getAddress()));
}

void banradio::changeStaz()
{
	QString addr = getAddress();
	if (!old_diffson)
		if (addr.at(1) == '0')
			addr[1] = '1';
	dev->sendFrame(createMsgOpen("16", "6001", addr));
}

void banradio::memoStaz(uchar st)
{
	// TODO: sistemare questo metodo quando sara' possibile provarlo!
	char    pippo[50],pippa[10];
	unsigned int ic;

	memset(pippo,'\000',sizeof(pippo));
	memset(pippa,'\000',sizeof(pippa));
	strcat(pippo,"*#16*");
	strcat(pippo,getAddress().toAscii().constData());
	strcat(pippo,"*#10*");
	memset(pippa,'\000',sizeof(pippa));
	ic = (unsigned int)st;
	sprintf(pippa,"%01hu",st);
	strcat(pippo,pippa);
	strcat(pippo,"##");
	dev->sendFrame(pippo);
}

void banradio::startRDS()
{
	dev->sendFrame("*16*101*" + getAddress() + "##");
}

void banradio::stopRDS()
{
	dev->sendFrame("*16*102*" + getAddress() + "##");
}

void banradio::richFreq()
{
	dev->sendFrame("*#16*" + getAddress() + "*6##");
}

void banradio::inizializza(bool forza)
{
}


/*****************************************************************
 ** Sorgente radio diffusione sonora multicanale
 ****************************************************************/
sorgenteMultiRadio::sorgenteMultiRadio(QWidget *parent, QString indirizzo, QString Icona1, QString Icona2, QString Icona3, char *ambDescr)
	: banradio(parent, indirizzo, 3, ambDescr)
{
	qDebug("sorgenteMultiRadio::sorgenteMultiRadio()");
	
	SetIcons(Icona1, Icona2, QString(), Icona3);
	
	connect(this, SIGNAL(sxClick()), this, SLOT(attiva()));
	indirizzo_semplice = indirizzo;
	indirizzi_ambienti.clear();
	multiamb = false;
}

void sorgenteMultiRadio::attiva()
{
	qDebug("sorgenteMultiRadio::attiva()");

	if (!multiamb)
	{
		QString f = QString("*22*35#4#%1#%2*4#%1##").arg(indirizzo_ambiente).arg(indirizzo_semplice.toInt());
		dev->sendFrame(f);
		emit active(indirizzo_ambiente, indirizzo_semplice.toInt());
	}
	else
	{
		qDebug("DA INSIEME AMBIENTI. CI SONO %d INDIRIZZI", indirizzi_ambienti.count());
		for (QStringList::Iterator it = indirizzi_ambienti.begin(); it != indirizzi_ambienti.end(); ++it)
		{
			dev->sendFrame("*22*0#4#" + *it + "*6##");
			dev->sendFrame("*#16*1000*11##");
			dev->sendFrame("*22*1#4#" + *it + "*2#" + indirizzo_semplice + "##");
			dev->sendFrame("*#16*1000*11##");
		}
	}
}

void sorgenteMultiRadio::ambChanged(const QString & ad, bool multi, QString indamb)
{
	// FIXME: PROPAGA LA VARIAZIONE DI DESCRIZIONE AMBIENTE
	qDebug() << "sorgenteMultiRadio::ambChanged(" << ad << ", " << multi << ", " << indamb << ")";
	if (!multi)
	{
		multiamb = false;
		indirizzo_ambiente = indamb.toInt();
		QString dove(QString::number(100 + indirizzo_ambiente * 10 + indirizzo_semplice.toInt(),10));
		qDebug() << "Source where is now " << dove;
		setAddress(dove);
	}
	else
	{
		multiamb = true;
		QString dove(QString::number(100 + indirizzo_semplice.toInt(), 10));
		qDebug() << "Source where is now " << dove;
		setAddress(dove);
	}
	myRadio->setAmbDescr(ad);
}

void sorgenteMultiRadio::showEvent(QShowEvent *event)
{
	banradio::pre_show();
}

void sorgenteMultiRadio::addAmb(QString a)
{
	qDebug() << "sorgenteMultiRadio::addAmb" << a;
	indirizzi_ambienti += a;
}
