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


#include "actuators.h"
#include "device_status.h"
#include "device.h"
#include "btbutton.h"
#include "generic_functions.h" // getPressName, createMsgOpen
#include "devices_cache.h" // bt_global::devices_cache
#include "lighting_device.h"
#include "skinmanager.h"
#include "xml_functions.h"

#include <QDebug>


SingleActuator::SingleActuator(QWidget *parent, const QDomNode &config_node, QString address)
	: BannOnOffState(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	initBanner(bt_global::skin->getImage("off"), bt_global::skin->getImage("actuator_state"),
		bt_global::skin->getImage("on"), OFF, getTextChild(config_node, "descr"));

	// TODO: read pull mode from config
	dev = bt_global::add_device_to_cache(new LightingDevice(address));

	connect(left_button, SIGNAL(clicked()), SLOT(deactivate()));
	connect(right_button, SIGNAL(clicked()), SLOT(activate()));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
}

void SingleActuator::activate()
{
	dev->turnOn();
}

void SingleActuator::deactivate()
{
	dev->turnOff();
}

void SingleActuator::inizializza(bool forza)
{
	dev->requestStatus();
}

void SingleActuator::status_changed(const StatusList &status_list)
{
	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
	{
		switch (it.key())
		{
		case LightingDevice::DIM_DEVICE_ON:
			it.value().toBool() ? setState(ON) : setState(OFF);
			break;
		}
		++it;
	}
}



ButtonActuator::ButtonActuator(QWidget *parent, const QDomNode &config_node, int t) :
	BannSinglePuls(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	initBanner(bt_global::skin->getImage("on"), bt_global::skin->getImage("action_icon"),
		getTextChild(config_node, "descr"));
	setAddress(getTextChild(config_node, "where"));

	type = t;

	connect(right_button, SIGNAL(pressed()), SLOT(activate()));
	connect(right_button, SIGNAL(released()), SLOT(deactivate()));
}

// This banner should have a real device but 1) no vct devices are implemented, 2) three classes should be
// implemented, which really seems to me an overkill
void ButtonActuator::activate()
{
	switch (type)
	{
	case  AUTOMAZ:
		sendFrame(createMsgOpen("1", "1", getAddress()));
		break;
	case  VCT_SERR:
		sendFrame(createMsgOpen("6", "10", getAddress()));
		break;
	case  VCT_LS:
		sendFrame(createMsgOpen("6", "12", getAddress()));
		break;
	}
}

void ButtonActuator::deactivate()
{
	switch (type)
	{
	case  AUTOMAZ:
		sendFrame(createMsgOpen("1", "0", getAddress()));
		break;
	case  VCT_SERR:
		break;
	case  VCT_LS:
		sendFrame(createMsgOpen("6", "11", getAddress()));
		break;
	}
}



#if 0
attuatAutom::attuatAutom(QWidget *parent,QString where,QString IconaSx, QString IconaDx, QString icon,
	QString pressedIcon) : bannOnOff(parent)
{
	SetIcons(IconaSx, IconaDx, icon, pressedIcon);
	setAddress(where);
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
	// Crea o preleva il dispositivo dalla cache
	dev = bt_global::devices_cache.get_light(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void attuatAutom::status_changed(QList<device_status*> sl)
{
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("attuatAutom::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
			case device_status::LIGHTS:
				ds->read(device_status_light::ON_OFF_INDEX, curr_status);
				qDebug("status = %d", curr_status.get_val());
				// this avoids changing state when the same state arrives
				// eg when in ACTIVE arrives ON, we must stay in ACTIVE
				if ((!curr_status.get_val() && isActive()) ||
					(curr_status.get_val() && !isActive()))
				{
					aggiorna = true;
					impostaAttivo(isActive() ? 0 : 1);
				}
				break;
			default:
				qDebug("attuatAutom variation, ignored");
				break;
		}
	}

	if (aggiorna)
		Draw();
}

void attuatAutom::Attiva()
{
	dev->sendFrame(createMsgOpen("1", "1", getAddress()));
}

void attuatAutom::Disattiva()
{
	dev->sendFrame(createMsgOpen("1", "0", getAddress()));
}

void attuatAutom::inizializza(bool forza)
{
	QString f = "*#1*" + getAddress() + "##";
	if (!forza)
		emit richStato(f);
	else
		dev->sendInit(f);
}

attuatPuls::attuatPuls(QWidget *parent, QString where, QString IconaSx, QString icon, char tipo)
	: bannPuls(parent)
{
	SetIcons(IconaSx, QString(), icon, QString());
	setAddress(where);
	connect(this,SIGNAL(sxPressed()),this,SLOT(Attiva()));
	connect(this,SIGNAL(sxReleased()),this,SLOT(Disattiva()));
	type = tipo;
	impostaAttivo(1);
}

void attuatPuls::Attiva()
{
	switch (type)
	{
	case  AUTOMAZ:
		sendFrame(createMsgOpen("1", "1", getAddress()));
		break;
	case  VCT_SERR:
		sendFrame(createMsgOpen("6", "10", getAddress()));
		break;
	case  VCT_LS:
		sendFrame(createMsgOpen("6", "12", getAddress()));
		break;
	}
}

void attuatPuls::Disattiva()
{
	switch (type)
	{
	case  AUTOMAZ:
		sendFrame(createMsgOpen("1", "0", getAddress()));
		break;
	case  VCT_SERR:
		break;
	case  VCT_LS:
		sendFrame(createMsgOpen("6", "11", getAddress()));
		break;
	}
}
#endif

