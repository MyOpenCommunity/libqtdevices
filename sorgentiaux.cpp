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


#include "sorgentiaux.h"
#include "main.h" // ICON_CICLA, ICON_FFWD, ICON_REW
#include "aux.h" // class Aux
#include "generic_functions.h" // createCommandFrame
#include "btbutton.h"
#include "skinmanager.h"
#include "icondispatcher.h"
#include "media_device.h"

#include <QWidget>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>


AuxSource::AuxSource(const QString &area, SourceDevice *dev) :
	AudioSource(area, dev)
{
	left_button = new BtButton;
	center_left_button = new BtButton;
	center_right_button = new BtButton;
	dummy = new QLabel;
	initBanner(bt_global::skin->getImage("on"), bt_global::skin->getImage("previous"), bt_global::skin->getImage("aux_dummy"),
		bt_global::skin->getImage("next"));
	QHBoxLayout *hbox = new QHBoxLayout(this);
	// these margins are the same as BannerContent
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(5);
	hbox->addWidget(left_button);
	hbox->addWidget(center_left_button);
	hbox->addWidget(dummy);
	hbox->addWidget(center_right_button);
	hbox->addStretch(1);

	connect(left_button, SIGNAL(clicked()), SLOT(turnOn()));
	connect(center_left_button, SIGNAL(clicked()), dev, SLOT(prevTrack()));
	connect(center_right_button, SIGNAL(clicked()), dev, SLOT(nextTrack()));
}

void AuxSource::initBanner(const QString &left, const QString &center_left, const QString &center,
	const QString &center_right)
{
	initButton(left_button, left);
	initButton(center_left_button, center_left);
	initButton(center_right_button, center_right);
	dummy->setPixmap(*bt_global::icons_cache.getIcon(center));
}


sorgente_aux::sorgente_aux(QWidget *parent, QString indirizzo, bool vecchio)
	: bannCiclaz(parent, vecchio ? 4 : 3)
{
	SetIcons(ICON_CICLA, QString(), ICON_FFWD, ICON_REW);
	setAddress(indirizzo);

	if (vecchio)
	{
		connect(this, SIGNAL(sxClick()), this, SLOT(ciclaSorg()));
		connect(this, SIGNAL(csxClick()), this, SLOT(decBrano()));
		connect(this, SIGNAL(cdxClick()), this, SLOT(aumBrano()));
		nascondi(BUT2);
	}
}

void sorgente_aux::ciclaSorg()
{
	sendFrame(QString("*22*22#4#1*5#2#%1##").arg(getAddress().at(2)));
}

void sorgente_aux::decBrano()
{
	sendFrame(QString("*22*10*2#%1##").arg(getAddress().at(2)));
}

void sorgente_aux::aumBrano()
{
	sendFrame(QString("*22*9*2#%1##").arg(getAddress().at(2)));
}


sorgenteMultiAux::sorgenteMultiAux(QWidget *parent, QString aux_name, QString indirizzo, QString Icona1, QString Icona2,
	QString Icona3, QString ambdescr) : sorgente_aux(parent, indirizzo, false)
{
	qDebug("sorgenteMultiAux::sorgenteMultiAux()");
	SetIcons(Icona1, Icona2, QString(), Icona3);
	indirizzo_semplice = indirizzo;
	indirizzi_ambienti.clear();
	myAux = new Aux(aux_name, ambdescr);
	connect(this, SIGNAL(dxClick()), myAux, SLOT(showPage()));
	connect(this, SIGNAL(sxClick()), SLOT(attiva()));
	connect(myAux, SIGNAL(Closed()), SIGNAL(pageClosed()));
	connect(myAux, SIGNAL(Btnfwd()), SLOT(aumBrano()));
	multiamb = false;
}

sorgenteMultiAux::~sorgenteMultiAux()
{
	delete myAux;
}

void sorgenteMultiAux::attiva()
{
	qDebug("sorgenteMultiAux::attiva()");

	if (!multiamb)
	{
		QString f = QString("*22*35#4#%1#%2*3#%1#0##").arg(indirizzo_ambiente).arg(indirizzo_semplice.toInt());
		sendFrame(f);
		emit active(indirizzo_ambiente, indirizzo_semplice.toInt());
	}
	else
	{
		qDebug("DA INSIEME AMBIENTI. CI SONO %d INDIRIZZI", indirizzi_ambienti.count());
		for (QStringList::Iterator it = indirizzi_ambienti.begin(); it != indirizzi_ambienti.end(); ++it)
		{
			sendFrame("*22*35#4#" + *it + "#" + indirizzo_semplice + "*3#" + *it + "#0##");
		}
	}
}

void sorgenteMultiAux::ambChanged(const QString & ad, bool multi, QString indamb)
{
	qDebug() << "sorgenteMultiRadio::ambChanged(" << ad << ", " << multi << ", " << indamb << ")";
	if (!multi)
	{
		multiamb = false;
		indirizzo_ambiente = indamb.toInt();
		QString dove(QString::number(100 + indamb.toInt() * 10 + indirizzo_semplice.toInt(), 10));
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

void sorgenteMultiAux::addAmb(QString a)
{
	for (QStringList::Iterator it = indirizzi_ambienti.begin(); it != indirizzi_ambienti.end(); ++it)
		if (*it == a) 
			return;
	qDebug() << "sorgenteMultiAux::addAmb" << a;
	indirizzi_ambienti += a;
}
