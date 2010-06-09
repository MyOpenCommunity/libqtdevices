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


#include "banntemperature.h"
#include "fontmanager.h" // bt_global::font
#include "probe_device.h"
#include "main.h" // (*bt_global::config)
#include "scaleconversion.h"
#include "icondispatcher.h"
#include "skinmanager.h"

#include <QLabel>
#include <QHBoxLayout>


BannTemperature::BannTemperature(QString descr, NonControlledProbeDevice *dev)
	: BannerNew(0)
{
	temperature_scale = static_cast<TemperatureScale>((*bt_global::config)[TEMPERATURE_SCALE].toInt());

	QLabel *descr_label = new QLabel(descr);
	descr_label->setFont(bt_global::font->get(FontManager::EXTERNAL_PROBE));

	QVBoxLayout *t = new QVBoxLayout(this);

#ifdef LAYOUT_TOUCHX
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
