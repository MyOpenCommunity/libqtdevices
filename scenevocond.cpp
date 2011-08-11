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


#include "scenevocond.h"
#include "generic_functions.h" // setCfgValue
#include "btbutton.h"
#include "skinmanager.h" // bt_global::skin
#include "xml_functions.h" // getTextChild
#include "scenevodevicescond.h"
#include "datetime.h" //BtTimeEdit

#include <QLabel>


// TODO remove after debugging is complete
namespace
{
	inline PullMode getPullMode(const QDomNode &node)
	{
		QDomNode element = getChildWithName(node, "pul");
		Q_ASSERT_X(!element.isNull(), "getPullMode", qPrintable(QString("Pull device node %1 without <pul> child").arg(getTextChild(node, "where"))));
		bool ok;
		int value = element.toElement().text().toInt(&ok);
		Q_ASSERT_X(ok && (value == 0 || value == 1), "getPullMode", qPrintable(QString("Pull device %1 with invalid <pul> child").arg(getTextChild(node, "where"))));

		return value ? PULL : NOT_PULL;
	}
}


int ScenEvoCondition::get_serial_number()
{
	return serial_number;
}

void ScenEvoCondition::set_serial_number(int n)
{
	serial_number = n;
}


ScenEvoTimeCondition::ScenEvoTimeCondition(int _item_id, const QDomNode &config_node)
{
	time_edit = new BtTimeEdit(this);
	item_id = _item_id;

	QString h = getTextChild(config_node, "hour");
	QString m = getTextChild(config_node, "minute");
	time_edit->setMaxHours(24);
	time_edit->setMaxMinutes(60);

	time_edit->setTime(QTime(h.toInt(), m.toInt(), 0));

	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), SLOT(scaduta()));

	QVBoxLayout *main_layout = new QVBoxLayout(this);

#ifdef LAYOUT_TS_3_5
	main_layout->setSpacing(20);
	main_layout->setContentsMargins(10, 5, 0, 20);

	QLabel *top_image = new QLabel;
	top_image->setPixmap(bt_global::skin->getImage("watch"));
	main_layout->addWidget(top_image, 0, Qt::AlignHCenter);
#else
	main_layout->setSpacing(0);
	main_layout->setContentsMargins(0, 5, 0, 10);
#endif
	main_layout->addWidget(time_edit, 0, Qt::AlignHCenter);

	cond_time.setHMS(h.toInt(), m.toInt(), 0);
	setupTimer();
}

void ScenEvoTimeCondition::setupTimer()
{
	QTime now = QTime::currentTime();
	int msecsto = now.msecsTo(cond_time);

	while (msecsto <= 0) // Do it tomorrow
		msecsto += 24 * 60 * 60 * 1000;

	while (msecsto >  24 * 60 * 60 * 1000)
		msecsto -= 24 * 60 * 60 * 1000;

	qDebug("(re)starting timer with interval = %d", msecsto);
	timer.stop();
	timer.start(msecsto);
}

void ScenEvoTimeCondition::Apply()
{
	BtTime tmp = time_edit->time();
	cond_time.setHMS(tmp.hour(), tmp.minute(), 0);
	setupTimer();
}

void ScenEvoTimeCondition::scaduta()
{
	emit condSatisfied();
	setupTimer();
}

void ScenEvoTimeCondition::save()
{
	qDebug("ScenEvoTimeCondition::save()");

	QMap<QString, QString> data;
	data["scen/time/hour"] = cond_time.toString("hh");
	data["scen/time/minute"] = cond_time.toString("mm");
	setCfgValue(data, item_id);
}

void ScenEvoTimeCondition::reset()
{
	time_edit->setTime(cond_time);
}


ScenEvoDeviceCondition::ScenEvoDeviceCondition(int _item_id, const QDomNode &config_node)
{
	item_id = _item_id;

	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);

	QString trigger = getTextChild(config_node, "trigger");
	int condition_type = getTextChild(config_node, "objectID").toInt();

	description = getTextChild(config_node, "descr");
	QString w = getTextChild(config_node, "where");
	int oid = getTextChild(config_node, "openserver_id").toInt();
	bool external = false;
	DeviceConditionDisplay *condition_display = 0;

	switch (condition_type)
	{
	case DeviceCondition::LIGHT:
		icon = bt_global::skin->getImage("light");
		condition_display = new DeviceConditionDisplayOnOff(this, description, icon);
		device_cond = new DeviceConditionLight(condition_display, trigger, w, oid, getPullMode(config_node));
		break;
	case DeviceCondition::DIMMING:
		icon = bt_global::skin->getImage("dimmer");
		condition_display = new DeviceConditionDisplayDimming(this, description, icon);
		device_cond = new DeviceConditionDimming(condition_display, trigger, w, oid, getPullMode(config_node));
		break;
	case DeviceCondition::EXTERNAL_PROBE:
		external = true;
		w += "00";
	case DeviceCondition::PROBE:
	case DeviceCondition::TEMPERATURE:
		icon = bt_global::skin->getImage("probe");
		condition_display = new DeviceConditionDisplayTemperature(this, description, icon);
		device_cond = new DeviceConditionTemperature(condition_display, trigger, w, external, oid);
		break;
	case DeviceCondition::AUX:
		icon = bt_global::skin->getImage("aux");
		condition_display = new DeviceConditionDisplayOnOff(this, description, icon);
		device_cond = new DeviceConditionAux(condition_display, trigger, w);
		break;
	case DeviceCondition::AMPLIFIER:
		icon = bt_global::skin->getImage("amplifier");
		condition_display = new DeviceConditionDisplayVolume(this, description, icon);
		device_cond = new DeviceConditionVolume(condition_display, trigger, w);
		break;
	case DeviceCondition::DIMMING100:
		icon = bt_global::skin->getImage("dimmer");
		condition_display = new DeviceConditionDisplayDimming(this, description, icon);
		device_cond = new DeviceConditionDimming100(condition_display, trigger, w, oid, getPullMode(config_node));
		break;
	default:
		qFatal("Unknown device condition: %d", condition_type);
	}

	connect(condition_display, SIGNAL(upClicked()), device_cond, SLOT(Up()));
	connect(condition_display, SIGNAL(downClicked()), device_cond, SLOT(Down()));
	connect(device_cond, SIGNAL(condSatisfied()), SIGNAL(condSatisfied()));

	main_layout->addWidget(condition_display);
}

ScenEvoDeviceCondition::~ScenEvoDeviceCondition()
{
	delete device_cond;
}

void ScenEvoDeviceCondition::Apply()
{
	device_cond->save();
}

void ScenEvoDeviceCondition::save()
{
	qDebug("ScenEvoDeviceCondition::save()");
	QString s = device_cond->getConditionAsString();
	setCfgValue("scen/device/trigger", s, item_id);
	device_cond->reset();
}

void ScenEvoDeviceCondition::reset()
{
	qDebug("ScenEvoDeviceCondition::reset()");
	device_cond->reset();
}

bool ScenEvoDeviceCondition::isTrue()
{
	return device_cond ? device_cond->isTrue() : false;
}

