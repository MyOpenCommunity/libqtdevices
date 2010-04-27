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


#include "bann_amplifiers.h"
#include "generic_functions.h" // createCommandFrame, trasformaVol
#include "device_status.h"
#include "deviceold.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "skinmanager.h" // bt_global::skin
#include "media_device.h"
#include "btbutton.h"

// TODO: in poweramplifier.h there's a base graphic banner to handle volume and state changes for amplifiers
// use it also for Amplifier

Amplifier::Amplifier(const QString &descr, const QString &where) : BannLevel(0)
{
	volume_value = 0;
	center_left_active = bt_global::skin->getImage("volume_on_left");
	center_right_active = bt_global::skin->getImage("volume_on_right");
	center_left_inactive = bt_global::skin->getImage("volume_off_left");
	center_right_inactive = bt_global::skin->getImage("volume_off_right");
	initBanner(bt_global::skin->getImage("off"), getBostikName(center_left_inactive, QString::number(volume_value)),
		getBostikName(center_right_inactive, QString::number(volume_value)), bt_global::skin->getImage("on"), descr);

	dev = bt_global::add_device_to_cache(new AmplifierDevice(where));

	connect(left_button, SIGNAL(clicked()), SLOT(turnOff()));
	connect(right_button, SIGNAL(clicked()), SLOT(turnOn()));
	connect(this, SIGNAL(center_left_clicked()), SLOT(volumeDown()));
	connect(this, SIGNAL(center_right_clicked()), SLOT(volumeUp()));

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
}

void Amplifier::volumeUp()
{
	dev->volumeUp();
}

void Amplifier::volumeDown()
{
	dev->volumeDown();
}

void Amplifier::turnOn()
{
	dev->turnOn();
}

void Amplifier::turnOff()
{
	dev->turnOff();
}

void Amplifier::setIcons()
{
	int index = trasformaVol(volume_value);

	setCenterLeftIcon(getBostikName(active ? center_left_active : center_left_inactive, QString::number(index)));
	setCenterRightIcon(getBostikName(active ? center_right_active : center_right_inactive, QString::number(index)));
}

void Amplifier::valueReceived(const DeviceValues &values_list)
{
	if (values_list.contains(AmplifierDevice::DIM_VOLUME))
		volume_value = values_list[AmplifierDevice::DIM_VOLUME].toInt();

	if (values_list.contains(AmplifierDevice::DIM_STATUS))
		active = values_list[AmplifierDevice::DIM_STATUS].toBool();

	setIcons();
}


AmplifierGroup::AmplifierGroup(QStringList addresses, const QString &descr) : BannLevel(0)
{
	center_left_active = bt_global::skin->getImage("volume_on_left");
	center_right_active = bt_global::skin->getImage("volume_on_right");
	center_left_inactive = bt_global::skin->getImage("volume_off_left");
	center_right_inactive = bt_global::skin->getImage("volume_off_right");
	initBanner(bt_global::skin->getImage("off"), getBostikName(center_left_inactive, "0"),
		getBostikName(center_right_inactive, "0"), bt_global::skin->getImage("on"), descr);

	foreach (const QString &where, addresses)
		devices.append(bt_global::add_device_to_cache(new AmplifierDevice(where)));

	connect(left_button, SIGNAL(clicked()), SLOT(turnOff()));
	connect(right_button, SIGNAL(clicked()), SLOT(turnOn()));
	connect(this, SIGNAL(center_left_clicked()), SLOT(volumeDown()));
	connect(this, SIGNAL(center_right_clicked()), SLOT(volumeUp()));
}

void AmplifierGroup::volumeUp()
{
	foreach (AmplifierDevice *dev, devices)
		dev->volumeUp();
}

void AmplifierGroup::volumeDown()
{
	foreach (AmplifierDevice *dev, devices)
		dev->volumeDown();
}

void AmplifierGroup::turnOn()
{
	foreach (AmplifierDevice *dev, devices)
		dev->turnOn();

	// always use the 0-volume icons, since groups do not have a definite volume
	setCenterLeftIcon(getBostikName(center_left_active, "0"));
	setCenterRightIcon(getBostikName(center_right_active, "0"));
}

void AmplifierGroup::turnOff()
{
	foreach (AmplifierDevice *dev, devices)
		dev->turnOff();

	// always use the 0-volume icons, since groups do not have a definite volume
	setCenterLeftIcon(getBostikName(center_left_inactive, "0"));
	setCenterRightIcon(getBostikName(center_right_inactive, "0"));
}


amplificatore::amplificatore(QWidget *parent, QString indirizzo, QString IconaSx, QString IconaDx, QString icon, QString inactiveIcon)
	: bannRegolaz(parent)
{
	qDebug("amplificatore::amplificatore()");
	setRange(1,9);
	SetIcons(IconaSx, IconaDx, icon, inactiveIcon, true);
	setAddress(indirizzo);
	connect(this,SIGNAL(sxClick()),this,SLOT(Accendi()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Spegni()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
	connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
	setValue(1);
	impostaAttivo(0);
	// Crea o preleva il dispositivo dalla cache
	dev = bt_global::add_device_to_cache(new sound_device(getAddress()));
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void amplificatore::status_changed(QList<device_status*> sl)
{
	stat_var curr_lev(stat_var::LEV);
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("amplificatore::status_changed");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::AMPLIFIER :
			qDebug("Ampli status variation");
			ds->read(device_status_amplifier::ON_OFF_INDEX, curr_status);
			ds->read(device_status_amplifier::AUDIO_LEVEL_INDEX, curr_lev);
			qDebug("status = %d, lev = %d", curr_status.get_val(),
					curr_lev.get_val());
			if ((isActive() && !curr_status.get_val()) || (!isActive() && curr_status.get_val()))
			{
				impostaAttivo(curr_status.get_val() != 0);
				aggiorna = true;
			}
			if (getValue() != curr_lev.get_val())
			{
				setValue(trasformaVol(curr_lev.get_val()));
				aggiorna = true ;
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

void amplificatore::Accendi()
{
	QString addr = getAddress();
	qDebug("amplificatore::Accendi()");
	dev->sendFrame(QString("*22*34#4#%1*3#%1#%2##").arg(addr.at(0)).arg(addr.at(1)));
}

void amplificatore::Spegni()
{
	QString addr = getAddress();
	qDebug("amplificatore::Spegni()");
	dev->sendFrame(QString("*22*0#4#%1*3#%1#%2##").arg(addr.at(0)).arg(addr.at(1)));
}

void amplificatore::Aumenta()
{
	qDebug("amplificatore::Aumenta()");
	dev->sendFrame(createCommandFrame("16", "1001", getAddress()));
}

void amplificatore::Diminuisci()
{
	qDebug("amplificatore::Diminuisci()");
	dev->sendFrame(createCommandFrame("16", "1101",getAddress()));
}

void amplificatore::inizializza(bool forza)
{
	dev->sendInit("*#16*" + getAddress() + "*1##");
	dev->sendInit("*#16*" + getAddress() + "*5##");
}


grAmplificatori::grAmplificatori(QWidget *parent, QList<QString> indirizzi, QString IconaSx,
	QString IconaDx, QString iconsx, QString icondx) : bannRegolaz(parent), elencoDisp(indirizzi)
{
	SetIcons(IconaSx, IconaDx, icondx, iconsx);
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
	connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
}

void grAmplificatori::sendActivationFrame(QString argm)
{
	for (int i = 0; i < elencoDisp.size(); ++i)
	{
		QString ind = elencoDisp.at(i);
		QString f;
		if (ind == "0")
			f = QString("*22*%2#4#%1*5#3#%1#%1##").arg(ind.at(0)).arg(argm);
		else if (ind.at(0) == '#')
			f = QString("*22*%2#4#%1*4#%1##").arg(ind.at(1)).arg(argm);
		else
			f = QString("*22*%3#4#%1*3#%1#%2##").arg(ind.at(0)).arg(ind.at(1)).arg(argm);

		sendFrame(f);
	}
}

void grAmplificatori::Attiva()
{
	sendActivationFrame("34");
}

void grAmplificatori::Disattiva()
{
	sendActivationFrame("0");
}

void grAmplificatori::Aumenta()
{
	for (int i = 0; i < elencoDisp.size(); ++i)
		sendFrame(createCommandFrame("16", "1001", elencoDisp.at(i)));
}

void grAmplificatori::Diminuisci()
{
	for (int i = 0; i < elencoDisp.size(); ++i)
		sendFrame(createCommandFrame("16", "1101", elencoDisp.at(i)));
}

