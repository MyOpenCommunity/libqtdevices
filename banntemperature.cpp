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
#include "bann1_button.h" // bannPuls
#include "fontmanager.h" // bt_global::font
#include "device.h"
#include "device_status.h"
#include "main.h" // (*bt_global::config)
#include "scaleconversion.h"

#include <QLabel>
#include <QFont>


BannTemperature::BannTemperature(QWidget *parent, QString where, QString descr, device *dev) : banner(parent)
{
	probe_descr = descr;
	temperature = -235;
	temp_scale = static_cast<TemperatureScale>((*bt_global::config)[TEMPERATURE_SCALE].toInt());

	if (!where.isNull())
		setAddress(where);

	descr_label = new QLabel(this);
	descr_label->setGeometry(BORDER_WIDTH, 0, DESCRIPTION_WIDTH, BANPULS_ICON_DIM_Y);
	temp_label = new QLabel(this);
	temp_label->setGeometry(DESCRIPTION_WIDTH, 0, TEMPERATURE_WIDTH, BANPULS_ICON_DIM_Y);

	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			SLOT(status_changed(QList<device_status*>)));
}

void BannTemperature::status_changed(QList<device_status*> sl)
{
	bool update = false;

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (ds->get_type() == device_status::TEMPERATURE_PROBE)
		{
			stat_var curr_temp(stat_var::TEMPERATURE);
			ds->read(device_status_temperature_probe::TEMPERATURE_INDEX, curr_temp);
			temperature = curr_temp.get_val();
			update = true;
		}
	}

	if (update)
		Draw();
}

void BannTemperature::Draw()
{
	descr_label->setFont(bt_global::font->get(FontManager::SUBTITLE));
	descr_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	descr_label->setText(probe_descr);

	temp_label->setFont(bt_global::font->get(FontManager::SUBTITLE));
	temp_label->setAlignment(Qt::AlignCenter);
	switch (temp_scale)
	{
		case CELSIUS:
			temp_label->setText(celsiusString(temperature));
			break;
		case FAHRENHEIT:
			// we don't have a direct conversion from celsius degrees to farhrenheit degrees
			temp_label->setText(fahrenheitString(bt2Fahrenheit(celsius2Bt(temperature))));
			break;
		default:
			qWarning("BannTemperature: unknown scale");
	}
	banner::Draw();
}
