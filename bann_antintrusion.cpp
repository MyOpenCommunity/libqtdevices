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


#include "bann_antintrusion.h"
#include "generic_functions.h" // void getZoneName(...), getBostikName
#include "fontmanager.h" // bt_global::font
#include "btbutton.h"
#include "skinmanager.h" // bt_global::skin
#include "devices_cache.h" // bt_global::devices_cache
#include "icondispatcher.h" //bt_global::icons_cache
#include "deviceold.h"
#include "keypad.h"
#include "xml_functions.h"
#include "openclient.h"

#include <QDebug>
#include <QTimer>
#include <QLabel>
#include <QHBoxLayout>


BannSingleLeft::BannSingleLeft() :
	Bann2Buttons(0)
{
}

void BannSingleLeft::initBanner(const QString &_left_on, const QString &_left_off,
				States init_state, const QString &banner_text)
{
	Bann2Buttons::initBanner(_left_off, QString(), banner_text);

	left_on = _left_on;
	left_off = _left_off;

	setState(init_state);
}

void BannSingleLeft::setState(States new_state)
{
	switch (new_state)
	{
	case PARTIAL_OFF:
		left_button->setImage(left_off);
		break;
	case PARTIAL_ON:
		left_button->setImage(left_on);
		break;
	}
}



AntintrusionZone::AntintrusionZone(const QString &name, const QString &_where) :
	BannSingleLeft()
{
	where = _where;

	QString zone = getZoneName(bt_global::skin->getImage("zone"), where);
	initBanner(bt_global::skin->getImage("partial_on"), bt_global::skin->getImage("partial_off"),
		   PARTIAL_OFF, name);
	is_on = false;
	connect(left_button, SIGNAL(clicked()), SLOT(toggleParzializza()));
	already_changed = false;

	dev = bt_global::add_device_to_cache(new zonanti_device(where));
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)), SLOT(status_changed(QList<device_status*>)));

	abilitaParz(true);
}

void AntintrusionZone::status_changed(QList<device_status *> sl)
{
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("AntintusionZone::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::ZONANTI:
		{
			qDebug("Zon.anti status variation");
			ds->read(device_status_zonanti::ON_OFF_INDEX, curr_status);
			int s = curr_status.get_val();
			qDebug("stat is %d", s);
			if (!is_on && s)
			{
				setParzializzaOn(true);
				qDebug("new status = %d", s);
				aggiorna = true;
			}
			else if (is_on && !s)
			{
				setParzializzaOn(false);
				qDebug("new status = %d", s);
				aggiorna = true;
			}
		}
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}

	if (aggiorna && !already_changed)
	{
		already_changed = true;
		emit partChanged(this);
	}
}

void AntintrusionZone::setParzializzaOn(bool parz)
{
	is_on = parz;
	setState(parz ? PARTIAL_ON : PARTIAL_OFF);
}

void AntintrusionZone::abilitaParz(bool ab)
{
	qDebug("AntintusionZone::abilitaParz(%d)", ab);
	left_button->setVisible(ab);
}

void AntintrusionZone::toggleParzializza()
{
	qDebug("AntintusionZone::toggleParzializza()");
	setParzializzaOn(!is_on);
	if (!already_changed)
	{
		already_changed = true;
		emit partChanged(this);
	}
}

void AntintrusionZone::clearChanged()
{
	already_changed = false;
}

int AntintrusionZone::getIndex()
{
	QString addr = where;
	addr.remove(0, 1);
	return addr.toInt();
}

void AntintrusionZone::inizializza(bool forza)
{
	dev->sendInit("*#5*" + where + "##");
}

bool AntintrusionZone::isActive()
{
	return is_on;
}

#if 0

zonaAnti::zonaAnti(QWidget *parent, const QString &name, QString indirizzo, QString iconzona, QString IconDisactive,
	QString IconActive) : bannOnIcons(parent)
{
	setAddress(indirizzo);
	qDebug("zonaAnti::zonaAnti()");
	// Mail agresta 22/06
	parzIName = bt_global::skin->getImage("partial_on");
	sparzIName = bt_global::skin->getImage("partial_off");

	SetIcons(1, sparzIName);
	SetIcons(2, getZoneName(iconzona, indirizzo));
	SetIcons(3, IconDisactive);

	if (BannerText)
	{
		BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		BannerText->setFont(bt_global::font->get(FontManager::BANNERTEXT));
		BannerText->setText(name);
	}

	zonaAttiva = IconActive;
	zonaNonAttiva = IconDisactive;
	already_changed = false;
	// Crea o preleva il dispositivo dalla cache
	dev = bt_global::devices_cache.get_zonanti_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));

	abilitaParz(true);
	SetIcons();
	zonaAnti::Draw();
}

void zonaAnti::SetIcons()
{
	if (isActive())
	{
		SetIcons(1, sparzIName);
		SetIcons(3, zonaAttiva);
	}
	else
	{
		SetIcons(1, parzIName);
		SetIcons(3, zonaNonAttiva);
	}
}

void zonaAnti::Draw()
{
	qDebug("sxButton = %p", sxButton);
	sxButton->setPixmap(*Icon[1]);
	if (pressIcon[1])
		sxButton->setPressedPixmap(*pressIcon[1]);
	BannerIcon->setPixmap(*Icon[2]);
	BannerIcon2->setPixmap(*Icon[3]);
}

int zonaAnti::getIndex()
{
	QString addr = getAddress();
	addr.remove(0, 1);
	return addr.toInt();
}

void zonaAnti::status_changed(QList<device_status*> sl)
{
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("zonAnti::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		int s;
		switch (ds->get_type())
		{
		case device_status::ZONANTI:
			qDebug("Zon.anti status variation");
			ds->read(device_status_zonanti::ON_OFF_INDEX, curr_status);
			s = curr_status.get_val();
			qDebug("stat is %d", s);
			if (!isActive() && s)
			{
				impostaAttivo(1);
				qDebug("new status = %d", s);
				aggiorna = true;
			}
			else if (isActive() && !s)
			{
				impostaAttivo(0);
				qDebug("new status = %d", s);
				aggiorna = true;
			}
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}

	if (aggiorna)
	{
		if (!already_changed)
		{
			already_changed = true;
			emit partChanged(this);
		}
		SetIcons();
		Draw();
	}
}

void zonaAnti::ToggleParzializza()
{
	qDebug("zonaAnti::ToggleParzializza()");
	impostaAttivo(!isActive());
	if (!already_changed)
	{
		already_changed = true;
		emit partChanged(this);
	}
	SetIcons();
	Draw();
}

void zonaAnti::abilitaParz(bool ab)
{
	qDebug("zonaAnti::abilitaParz(%d)", ab);
	if (ab)
	{
		connect(this, SIGNAL(sxClick()), this, SLOT(ToggleParzializza()));
		mostra(BUT1);
	}
	else
	{
		disconnect(this, SIGNAL(sxClick()), this, SLOT(ToggleParzializza()));
		nascondi(BUT1);
	}
	Draw();
}

void zonaAnti::clearChanged()
{
	already_changed = false;
}

void zonaAnti::inizializza(bool forza)
{
	dev->sendInit("*#5*" + getAddress() + "##");
}

#endif

impAnti::impAnti(QWidget *parent, QString IconOn, QString IconOff, QString IconInfo, QString Icon)
	: bann3ButLab(parent)
{
	banner_height = 60;

	tasti = NULL;

	SetIcons(IconInfo, IconOff, getBostikName(Icon, "dis"), IconOn, getBostikName(Icon, "ins"));
	send_part_msg = false;
	inserting = false;
	memset(le_zone, 0, sizeof(le_zone));
	nascondi(BUT2);
	impostaAttivo(0);
	Draw();
	// BUT2 and 4 are actually both on the left of the banner.
	connect(this,SIGNAL(dxClick()),this,SLOT(Disinserisci()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Inserisci()));

	// probably needs to be set for all banners (or minimumSizeHint defined)
	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

	dev = bt_global::add_device_to_cache(new impanti_device(QString("0")));
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));

	connect(client_comandi, SIGNAL(openAckRx()), SLOT(openAckRx()));
	connect(client_comandi, SIGNAL(openNakRx()), SLOT(openNakRx()));
}

void impAnti::status_changed(QList<device_status*> sl)
{
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("impAnti::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		int s;
		switch (ds->get_type())
		{
		case device_status::IMPANTI:
			qDebug("Imp.anti status variation");
			ds->read(device_status_impanti::ON_OFF_INDEX, curr_status);
			s = curr_status.get_val();
			qDebug("status = %d", s);
			if (!isActive() && s)
			{
				impostaAttivo(2);
				nascondi(BUT4);
				nascondi(BUT1);
				mostra(BUT2);
				aggiorna=1;
				qDebug("IMPIANTO INSERITO !!");
				emit impiantoInserito();
				emit abilitaParz(false);
				QTimer::singleShot(5000, this, SLOT(inizializza()));
				send_part_msg = false;
			}
			else if (isActive() && !s)
			{
				impostaAttivo(0);
				nascondi(BUT2);
				mostra(BUT4);
				aggiorna=1;
				qDebug("IMPIANTO DISINSERITO !!");
				emit abilitaParz(true);
				emit clearChanged();
				send_part_msg = false;
			}
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}

	if (aggiorna)
		Draw();
}

int impAnti::getIsActive(int zona)
{
	if (le_zone[zona] != NULL)
		return le_zone[zona]->isActive();
	else
		return -1;
}

void impAnti::ToSendParz(bool s)
{
	send_part_msg = s;
	if (!send_part_msg)
		emit clearChanged();
}

void impAnti::Inserisci()
{
	qDebug("impAnti::Inserisci()");
	int s[MAX_ZONE];
	for (int i=0; i<MAX_ZONE; i++)
	{
		if (le_zone[i] !=  NULL)
			s[i] = le_zone[i]->isActive();
		else
			s[i] = -1;
	}

	if (tasti)
		delete tasti;
	inserting = true;
	tasti = new KeypadWithState(s);
	connect(tasti, SIGNAL(Closed()), SLOT(Insert1()));
	connect(tasti, SIGNAL(Closed()), SIGNAL(pageClosed()));
	tasti->setMode(Keypad::HIDDEN);
	tasti->showPage();
}

void impAnti::Disinserisci()
{
	if (tasti)
		delete tasti;
	tasti = new Keypad();
	connect(tasti, SIGNAL(Closed()), SLOT(DeInsert()));
	connect(tasti, SIGNAL(Closed()), SIGNAL(pageClosed()));
	tasti->setMode(Keypad::HIDDEN);
	tasti->showPage();
}

void impAnti::Insert1()
{
	if (tasti->getText().isEmpty())
		return;

	passwd = tasti->getText();

	qDebug("impAnti::Insert()");
	if (!send_part_msg)
	{
		Insert3();
		return;
	}
	QString f = "*5*50#" + passwd + "#";
	for (int i = 0; i < MAX_ZONE; i++)
		f += le_zone[i] && le_zone[i]->isActive() ? "0" : "1";
	f += "*0##";
	qDebug() << "sending part frame" << f;
	dev->sendFrame(f);

	send_part_msg = false;
	part_msg_sent = true;
}

void impAnti::Insert2()
{
	qDebug("impAnti::Insert2()");
	if (!inserting)
		return;
	// 6 seconds between first open ack and open insert messages
	QTimer::singleShot(6000, this, SLOT(Insert3()));
}

void impAnti::Insert3()
{
	qDebug("impAnti::Insert3()");
	emit clearAlarms();
	dev->sendFrame("*5*36#" + passwd + "*0##");
	inserting = false;
	QTimer::singleShot(5000, this, SLOT(inizializza()));
	ToSendParz(false);
}

void impAnti::DeInsert()
{
	qDebug("impAnti::DeInsert()");
	QString pwd = tasti->getText();
	if (!pwd.isEmpty())
		dev->sendFrame("*5*36#" + pwd + "*0##");
}

void impAnti::openAckRx()
{
	qDebug("impAnti::openAckRx()");
	if (!inserting)
	{
		qDebug("Not inserting");
		return;
	}
	if (!part_msg_sent)
		return;
	part_msg_sent = false;
	qDebug("waiting 5 seconds before sending insert message");
	// Do second step of insert
	Insert2();
}

void impAnti::openNakRx()
{
	qDebug("impAnti::openNakRx()");
	if (!part_msg_sent)
		return;
	//Agre - per sicurezza provo a continuare per evitare di non inserire l'impianto
	openAckRx();
	part_msg_sent = false;
}

void impAnti::setZona(AntintrusionZone *za)
{
	int index = za->getIndex() - 1;
	if ((index >= 0) && (index < MAX_ZONE))
		le_zone[index] = za;
}

void impAnti::partChanged(AntintrusionZone *za)
{
	qDebug("impAnti::partChanged");
	send_part_msg = true;
}

void impAnti::inizializza(bool forza)
{
	qDebug("impAnti::inizializza()");
	dev->sendInit("*#5*0##");
}

