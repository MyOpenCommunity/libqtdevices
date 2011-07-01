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


#include "bann_thermal_regulation.h"

#include "thermal_device.h"
#include "skinmanager.h"
#include "btbutton.h"
#include "icondispatcher.h"
#include "probe_device.h" // NonControlledProbeDevice
#include "scaleconversion.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>


BannOff::BannOff(QWidget *parent, ThermalDevice *_dev) : BannCenteredButton(parent)
{
	initBanner(bt_global::skin->getImage("off_button"));
	dev = _dev;
	connect(center_button, SIGNAL(clicked()), SLOT(performAction()));
	connect(center_button, SIGNAL(clicked()), SIGNAL(clicked()));
}

void BannOff::performAction()
{
	dev->setOff();
}


BannAntifreeze::BannAntifreeze(QWidget *parent, ThermalDevice *_dev) : BannCenteredButton(parent)
{
	initBanner(bt_global::skin->getImage("regulator_antifreeze"));
	dev = _dev;
	connect(center_button, SIGNAL(clicked()), SLOT(performAction()));
	connect(center_button, SIGNAL(clicked()), SIGNAL(clicked()));
}

void BannAntifreeze::performAction()
{
	dev->setProtection();
}


BannOffAntifreeze::BannOffAntifreeze(QWidget *parent, ThermalDevice *_dev)
{
	initBanner(bt_global::skin->getImage("off_button"), bt_global::skin->getImage("regulator_antifreeze"), "");
	dev = _dev;

	connect(center_left, SIGNAL(clicked()), SLOT(setOff()));
	connect(center_left, SIGNAL(clicked()), SIGNAL(clicked()));
	connect(center_right, SIGNAL(clicked()), SLOT(setAntifreeze()));
	connect(center_right, SIGNAL(clicked()), SIGNAL(clicked()));
}

void BannOffAntifreeze::setOff()
{
	dev->setOff();
}

void BannOffAntifreeze::setAntifreeze()
{
	dev->setProtection();
}


BannSummerWinter::BannSummerWinter(QWidget *parent, ThermalDevice *_dev)
{
	initBanner(bt_global::skin->getImage("winter"), bt_global::skin->getImage("summer"), "");
	dev = _dev;

	connect(center_left, SIGNAL(clicked()), SLOT(setWinter()));
	connect(center_left, SIGNAL(clicked()), SIGNAL(clicked()));
	connect(center_right, SIGNAL(clicked()), SLOT(setSummer()));
	connect(center_right, SIGNAL(clicked()), SIGNAL(clicked()));
}

void BannSummerWinter::setSummer()
{
	dev->setSummer();
}

void BannSummerWinter::setWinter()
{
	dev->setWinter();
}


BannWeekly::BannWeekly(int _index) : BannSinglePuls(0)
{
	index = _index;
	connect(right_button, SIGNAL(clicked()), SLOT(performAction()));
}

void BannWeekly::performAction()
{
	emit programNumber(index);
}


BannTemperature::BannTemperature(QString descr, NonControlledProbeDevice *dev)
{
	temperature_scale = static_cast<TemperatureScale>((*bt_global::config)[TEMPERATURE_SCALE].toInt());

	QLabel *descr_label = new QLabel(descr);
	descr_label->setFont(bt_global::font->get(FontManager::EXTERNAL_PROBE));

	QVBoxLayout *t = new QVBoxLayout(this);

#ifdef LAYOUT_TS_10
	descr_label->setFixedHeight(40);

	QLabel *sep = new QLabel;
	sep->setPixmap(*bt_global::icons_cache.getIcon(bt_global::skin->getImage("horizontal_separator")));

	t->addWidget(sep);
#endif

	temperature_label = new QLabel;
	temperature_label->setFont(bt_global::font->get(FontManager::EXTERNAL_PROBE));
	updateTemperature(1235);

	QHBoxLayout *l = new QHBoxLayout;
	l->setContentsMargins(0, 0, 10, 0);
	l->setSpacing(10);
	l->addWidget(descr_label, 0, Qt::AlignLeft);
	l->addWidget(temperature_label, 0, Qt::AlignRight);

	t->addLayout(l);

	connect(dev, SIGNAL(valueReceived(DeviceValues)),
			SLOT(valueReceived(DeviceValues)));
}

void BannTemperature::valueReceived(const DeviceValues &values_list)
{
	if (!values_list.contains(NonControlledProbeDevice::DIM_TEMPERATURE))
		return;

	updateTemperature(values_list[NonControlledProbeDevice::DIM_TEMPERATURE].toInt());
}

void BannTemperature::updateTemperature(int temperature)
{
	switch (temperature_scale)
	{
		case CELSIUS:
			temperature_label->setText(celsiusString(bt2Celsius(temperature)));
			break;
		case FAHRENHEIT:
			temperature_label->setText(fahrenheitString(bt2Fahrenheit(temperature)));
			break;
		default:
			qWarning("BannTemperature: unknown scale");
	}
}
