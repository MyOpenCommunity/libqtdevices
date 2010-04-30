/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "sorgentiradio.h"
#include "main.h" // ICON_CICLA
#include "radio.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "deviceold.h"
#include "generic_functions.h" // createCommandFrame
#include "btbutton.h" // BtButton
#include "skinmanager.h" // bt_global::skin
#include "icondispatcher.h" // bt_global::icons_cache
#include "media_device.h" // RadioSourceDevice

#include <QWidget>
#include <QDebug>
#include <QChar>
#include <QHBoxLayout>
#include <QLabel>


RadioSource::RadioSource(const QString &area, RadioSourceDevice *dev) :
	AudioSource(area, dev)
{
	left_button = new BtButton;
	center_left_button = new BtButton;
	center_right_button = new BtButton;
	right_button = new BtButton;
	dummy = new QLabel;
	initBanner(bt_global::skin->getImage("cycle"), bt_global::skin->getImage("previous"), bt_global::skin->getImage("radio_dummy"),
		bt_global::skin->getImage("next"), bt_global::skin->getImage("details"));
	QHBoxLayout *hbox = new QHBoxLayout(this);
	// these margins are the same as BannerContent
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(5);
	hbox->addWidget(left_button);
	hbox->addWidget(center_left_button);
	hbox->addWidget(dummy);
	hbox->addWidget(center_right_button);
	hbox->addWidget(right_button);
}

void RadioSource::initBanner(const QString &left, const QString &center_left, const QString &center,
	const QString &center_right, const QString &right)
{
	initButton(left_button, left);
	initButton(center_left_button, center_left);
	initButton(center_right_button, center_right);
	initButton(right_button, right);
	dummy->setPixmap(*bt_global::icons_cache.getIcon(center));
}


/*****************************************************************
 **sorgente_Radio
 ****************************************************************/
banradio::banradio(QWidget *parent, QString indirizzo, int nbut, const QString & ambdescr)
	: bannCiclaz(parent, nbut)
{
	SetIcons(ICON_CICLA,ICON_IMPOSTA,ICON_FFWD,ICON_REW);
	setAddress(indirizzo);
	myRadio = new radio(ambdescr);
	myRadio->setRDS("");
	myRadio->setFreq(0.00);

	myRadio->setStaz((uchar)0);

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

	connect(this, SIGNAL(dxClick()), myRadio, SLOT(showPage()));
	connect(this , SIGNAL(dxClick()),this,SLOT(startRDS()));
	if (!old_diffson)
		connect(this, SIGNAL(dxClick()), this, SLOT(richFreq()));

	// TODO: probably we should emit a pageClosed() signal here...
	connect(myRadio, SIGNAL(Closed()), SIGNAL(pageClosed()));
	connect(myRadio, SIGNAL(Closed()), SLOT(stopRDS()));

	connect(myRadio,SIGNAL(decFreqAuto()),this,SLOT(decFreqAuto()));
	connect(myRadio,SIGNAL(aumFreqAuto()),this,SLOT(aumFreqAuto()));
	connect(myRadio,SIGNAL(decFreqMan()),this,SLOT(decFreqMan()));
	connect(myRadio,SIGNAL(aumFreqMan()),this,SLOT(aumFreqMan()));
	connect(myRadio,SIGNAL(changeStaz()),this,SLOT(changeStaz()));
	connect(myRadio,SIGNAL(memoFreq(uchar)),this,SLOT(memoStaz(uchar)));
	connect(myRadio,SIGNAL(richFreq()),this,SLOT(richFreq()));

	dev = bt_global::add_device_to_cache(new radio_device(getAddress()));
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)), this, SLOT(status_changed(QList<device_status*>)));
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
			qDebug() << "*** setting freq to " << freq;
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
			break;
		}
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}
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
	dev->sendFrame(QString("*22*10*2#%1##").arg(getAddress().at(2)));
}

void banradio::aumBrano()
{
	dev->sendFrame(QString("*22*9*2#%1##").arg(getAddress().at(2)));
}

void banradio::aumFreqAuto()
{
	myRadio->setFreq(0.00);
	myRadio->setRDS("- - - - ");
	dev->sendFrame(createCommandFrame("16", "5000", getAddress()));
}

void banradio::decFreqAuto()
{
	myRadio->setFreq(0.00);
	myRadio->setRDS("- - - - ");
	dev->sendFrame(createCommandFrame("16", "5100", getAddress()));
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
	dev->sendFrame(createCommandFrame("16", "5001", getAddress()));
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
	dev->sendFrame(createCommandFrame("16", "5101", getAddress()));
}

void banradio::changeStaz()
{
	dev->sendFrame(QString("*22*9*2#%1##").arg(getAddress().at(2)));
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
sorgenteMultiRadio::sorgenteMultiRadio(QWidget *parent, QString indirizzo, QString Icona1, QString Icona2, QString Icona3, const QString &ambDescr)
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
		QString f = QString("*22*35#4#%1#%2*3#%1#0##").arg(indirizzo_ambiente).arg(indirizzo_semplice.toInt());
		dev->sendFrame(f);
		emit active(indirizzo_ambiente, indirizzo_semplice.toInt());
	}
	else
	{
		qDebug("DA INSIEME AMBIENTI. CI SONO %d INDIRIZZI", indirizzi_ambienti.count());
		for (QStringList::Iterator it = indirizzi_ambienti.begin(); it != indirizzi_ambienti.end(); ++it)
		{
			dev->sendFrame("*22*35#4#" + *it + "#" + indirizzo_semplice + "*3#" + *it + "#0##");
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
	for (QStringList::Iterator it = indirizzi_ambienti.begin(); it != indirizzi_ambienti.end(); ++it)
		if (*it == a)
			return;
	qDebug() << "sorgenteMultiRadio::addAmb" << a;
	indirizzi_ambienti += a;
}
