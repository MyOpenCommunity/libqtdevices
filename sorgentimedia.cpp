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


#include "sorgentimedia.h"
#include "main.h" // ICON_CICLA, ICON_FFWD, ICON_REW, ICON_IMPOSTA
#include "devices_cache.h" // device_status_sound_matr::AMBx_INDEX 

#include <openwebnet.h> // class openwebnet

#include <QDebug>


BannerSorgenteMultimedia::BannerSorgenteMultimedia(QWidget *parent, const QDomNode &config_node, QString indirizzo,
	int where, int nbut) : bannCiclaz(parent, nbut), source_menu(config_node, where) // where is the number of source in mono and indirizzo in multi!
{
	SetIcons(ICON_CICLA, ICON_IMPOSTA, ICON_FFWD, ICON_REW);
	setAddress(indirizzo);

	connect(this, SIGNAL(dxClick()), &source_menu, SLOT(showPage()));
	if (nbut == 4)
	{
		connect(this, SIGNAL(sxClick()), this, SLOT(ciclaSorg()));
		connect(this, SIGNAL(csxClick()), this, SLOT(decBrano()));
		connect(this, SIGNAL(cdxClick()), this, SLOT(aumBrano()));
	}
	connect(&source_menu, SIGNAL(Closed()), SIGNAL(pageClosed()));
}

void BannerSorgenteMultimedia::ciclaSorg()
{
	qDebug("BannerSorgenteMultimedia::ciclaSorg()");
	sendFrame(QString("*22*22#4#1*5#2#%1##").arg(getAddress().at(2)));
}

void BannerSorgenteMultimedia::decBrano()
{
	source_menu.prevTrack();
}

void BannerSorgenteMultimedia::aumBrano()
{
	source_menu.nextTrack();
}

void BannerSorgenteMultimedia::gestFrame(char *frame)
{
	openwebnet msg_open;
	char amb[3];
	qDebug("BannerSorgenteMultimedia::gestFrame()");

	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
	sprintf(amb, getAddress().toAscii().constData());
	if ((!strcmp(msg_open.Extract_chi(),"22")) &&
		(!strcmp(msg_open.Extract_grandezza(),"12")) &&
		(!strcmp(msg_open.Extract_dove(),"5")) &&
		(!strcmp(msg_open.Extract_livello(),"2")))
	{
		if (!strcmp(msg_open.Extract_interfaccia(), amb+2) &&
		   (!strcmp(msg_open.Extract_valori(0), "0")))
		{
			source_menu.disableSource(false);
			source_menu.pause();
			set_status(false);
		}
	}
	else if ((!strcmp(msg_open.Extract_chi(),"22")) &&
		(!strncmp(msg_open.Extract_cosa(),"2", 1)) &&
		(!strcmp(msg_open.Extract_dove(),"5")) && 
		(!strcmp(msg_open.Extract_livello(),"2")))
	{
		if (!strcmp(msg_open.Extract_interfaccia(), amb+2))
		{
			source_menu.enableSource(false);
			source_menu.resume();
			set_status(true);
		}
		else
		{
			source_menu.disableSource(false);
			source_menu.pause();
			set_status(false);
		}
	}
	else if ((!strcmp(msg_open.Extract_chi(),"22")) &&
		(!strcmp(msg_open.Extract_cosa(),"9")) &&
		(((!strcmp(msg_open.Extract_dove(),"5")) &&
		(!strcmp(msg_open.Extract_livello(),"3"))) ||
		(((!strcmp(msg_open.Extract_dove(),"2")) &&
		(!strcmp(msg_open.Extract_livello(), amb+2))))))
	{
		if (get_status())
			source_menu.nextTrack();
	}
	else if ((!strcmp(msg_open.Extract_chi(),"22")) &&
		(!strcmp(msg_open.Extract_cosa(),"10")) &&
		(((!strcmp(msg_open.Extract_dove(),"5")) &&
		(!strcmp(msg_open.Extract_livello(),"3"))) ||
		(((!strcmp(msg_open.Extract_dove(),"2")) &&
		(!strcmp(msg_open.Extract_livello(), amb+2))))))
	{
		if (get_status())
			source_menu.prevTrack();
	}
}

void BannerSorgenteMultimedia::inizializza(bool forza)
{
	qDebug("BannerSorgenteMultimedia::inizializza()");
	sendInit(QString("*#22*7*#15*%1***4**0**1*1**1##").arg(getAddress().at(2)));
	status = false;
}

void BannerSorgenteMultimedia::set_status(bool stat)
{
	qDebug("BannerSorgenteMultimedia::set_status()");

	status = stat;
}

bool BannerSorgenteMultimedia::get_status()
{
	qDebug("BannerSorgenteMultimedia::get_status()");

	return status;
}


/*
 * Banner Sorgente Multimediale Multicanale
 */
BannerSorgenteMultimediaMC::BannerSorgenteMultimediaMC(QWidget *parent, const QDomNode &config_node,
	QString indirizzo, int where, QString icon_onoff, QString icon_cycle, QString icon_settings)
	: BannerSorgenteMultimedia(parent, config_node, indirizzo, where, 3)
{
	SetIcons(icon_onoff, QString(), icon_cycle, icon_settings);

	indirizzo_semplice = indirizzo;
	indirizzi_ambienti.clear();

	connect(this, SIGNAL(sxClick()), this, SLOT(attiva()));
	multiamb = false;
}

void BannerSorgenteMultimediaMC::attiva()
{
	QString f;
	qDebug("BannerSorgenteMultimediaMC::attiva()");

	if (!multiamb)
	{
		f = QString("*22*35#4#%1#%2*3#%1#0##").arg(indirizzo_ambiente).arg(indirizzo_semplice.toInt());
		sendFrame(f);
		emit active(indirizzo_ambiente, indirizzo_semplice.toInt());
		source_menu.enableSource(false);
		source_menu.resume();
	}
	else
	{
		source_menu.enableSource(false);
		for (int it = device_status_sound_matr::AMB1_INDEX; it <= device_status_sound_matr::AMB8_INDEX; ++it)
		{
			f = QString("*22*35#4#%1#%2*3#%1#0##").arg(it + 1).arg(indirizzo_semplice.toInt());
			sendFrame(f);
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

void BannerSorgenteMultimediaMC::addAmb(QString a)
{
	indirizzi_ambienti += a;
}

void BannerSorgenteMultimediaMC::inizializza(bool forza)
{
	qDebug("BannerSorgenteMultimediaMC::inizializza()");

	sendInit(QString("*#22*7*#15*%1***4**0*%2*1*1**1##").arg(indirizzo_semplice).arg(indirizzo_semplice));

	for (int i = 1; i < MAX_AMB; i++)
		status_Amb[i] = false;
}

void BannerSorgenteMultimediaMC::setstatusAmb(int Amb, bool status)
{
	qDebug("BannerSorgenteMultimediaMC::setstatusAmb(%d, %d)", Amb, status);

	status_Amb[Amb] = status;
}

bool BannerSorgenteMultimediaMC::statusAmb(int Amb)
{
	qDebug("BannerSorgenteMultimediaMC::statusAmb(%d)", Amb);

	return status_Amb[Amb];
}

bool BannerSorgenteMultimediaMC::get_status()
{
	qDebug("BannerSorgenteMultimediaMC::get_status()");

	for (int i = 1; i < MAX_AMB; i++)
	{
		if (status_Amb[i])
			return true;
	}

	return false;
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
		   (!strcmp(msg_open.Extract_valori(0), "0")))
		{
			source_menu.disableSource(false);
			source_menu.pause();
		}
	}
	else if ((!strcmp(msg_open.Extract_chi(),"22")) &&
		(!strncmp(msg_open.Extract_cosa(),"2#4#", strlen("2#4#"))) &&
		(!strcmp(msg_open.Extract_dove(),"5")) &&
		(!strcmp(msg_open.Extract_livello(),"2")))
	{
		if (indirizzo_semplice == msg_open.Extract_interfaccia())
		{
			source_menu.enableSource(false);
			source_menu.resume();
			setstatusAmb(atoi(msg_open.Extract_cosa()+4), true);
		}
		else
			setstatusAmb(atoi(msg_open.Extract_cosa()+4), false);

	}
	else if ((!strcmp(msg_open.Extract_chi(),"22")) &&
		(!strcmp(msg_open.Extract_cosa(),"9")))
	{
		if ((!strcmp(msg_open.Extract_dove(),"5")) &&
		   (!strcmp(msg_open.Extract_livello(),"3")))
		{
			if (statusAmb(atoi(msg_open.Extract_interfaccia())))
				source_menu.nextTrack();
		}
		else if ((!strcmp(msg_open.Extract_dove(),"2")) &&
			(indirizzo_semplice == msg_open.Extract_livello()))
		{
			if (get_status())
				source_menu.nextTrack();
		}
	}
	else if ((!strcmp(msg_open.Extract_chi(),"22")) &&
		(!strcmp(msg_open.Extract_cosa(),"10")))
	{
		if ((!strcmp(msg_open.Extract_dove(),"5")) &&
		   (!strcmp(msg_open.Extract_livello(),"3")))
		{
			if (statusAmb(atoi(msg_open.Extract_interfaccia())))
				source_menu.prevTrack();
		}
		else if ((!strcmp(msg_open.Extract_dove(),"2")) &&
			(indirizzo_semplice == msg_open.Extract_livello()))
		{
			if (get_status())
				source_menu.prevTrack();
		}
	}
}
