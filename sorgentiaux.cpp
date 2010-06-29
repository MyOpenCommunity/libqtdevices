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
#include "aux.h" // class myAux
#include "generic_functions.h" // createMsgOpen
#include "devices_cache.h" // device_status_sound_matr::AMBx_INDEX 

#include <QWidget>
#include <QDebug>

/*****************************************************************
 **sorgente_aux
 ****************************************************************/
sorgente_aux::sorgente_aux(QWidget *parent, QString aux_name, QString indirizzo, bool vecchio, QString ambdescr)
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
		myAux = 0;
	}
	else
		myAux = new aux(NULL, aux_name, ambdescr);
}

sorgente_aux::~sorgente_aux()
{
	delete myAux;
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

void sorgente_aux::inizializza(bool forza)
{
}

void sorgente_aux::hideEvent(QHideEvent *event)
{
	if (myAux)
		myAux->hide();
}

/*****************************************************************
 ** Sorgente aux diffusione sonora multicanale
 ****************************************************************/

sorgenteMultiAux::sorgenteMultiAux(QWidget *parent, QString aux_name, QString indirizzo, QString Icona1, QString Icona2,
	QString Icona3, QString ambdescr) : sorgente_aux(parent, aux_name, indirizzo, false, ambdescr)
{
	qDebug("sorgenteMultiAux::sorgenteMultiAux()");
	SetIcons(Icona1, Icona2, QString(), Icona3);
	indirizzo_semplice = indirizzo;
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
	QString f;
	qDebug("sorgenteMultiAux::attiva()");

	if (!multiamb)
	{
		f = QString("*22*35#4#%1#%2*3#%1#0##").arg(indirizzo_ambiente).arg(indirizzo_semplice.toInt());
		sendFrame(f);
		emit active(indirizzo_ambiente, indirizzo_semplice.toInt());
	}
	else
	{
		qDebug("DA INSIEME AMBIENTI. CI SONO 8 INDIRIZZI");
		for (int it = device_status_sound_matr::AMB1_INDEX; it <= device_status_sound_matr::AMB8_INDEX; ++it)
		{
			f = QString("*22*35#4#%1#%2*3#%1#0##").arg(it + 1).arg(indirizzo_semplice.toInt());
			sendFrame(f);
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
