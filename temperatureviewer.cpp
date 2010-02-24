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


#include "temperatureviewer.h"
#include "fontmanager.h" // bt_global::font
#include "scaleconversion.h"
#include "page.h"
#include "main.h" // bt_global::config
#include "probe_device.h"
#include "devices_cache.h"

#include <openmsg.h>

#include <QLCDNumber>
#include <QString>
#include <QLabel>
#include <QDebug>

#define H_SCR_TEMP 20


TemperatureViewer::TemperatureViewer(Page *page) : linked_page(page)
{
	temp_scale = static_cast<TemperatureScale>(bt_global::config[TEMPERATURE_SCALE].toInt());
}

void TemperatureViewer::add(QString where, int openserver_id, int x, int y, int width, int height, QString descr, QString ext)
{
	TemperatureData temp;
	QLCDNumber *l = new QLCDNumber(linked_page);
	int style = QFrame::Plain;
	int line = 3; // line width
	temp.lcd = l;
	temp.lcd->setGeometry(x, y, width, height - H_SCR_TEMP);
	temp.lcd->setFrameStyle(style);
	temp.lcd->setLineWidth(line);
	temp.lcd->setNumDigits(9);
	temp.lcd->setSegmentStyle(QLCDNumber::Flat);

	if (!descr.isEmpty())
	{
		temp.text = new QLabel(linked_page);
		temp.text->setFont(bt_global::font->get(FontManager::TEXT));
		temp.text->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		temp.text->setText(descr);
		temp.text->setGeometry(x, y + height - H_SCR_TEMP, width, H_SCR_TEMP);
	}

	temp.device = bt_global::add_device_to_cache(new NonControlledProbeDevice(where, ext == "1" ?
		NonControlledProbeDevice::EXTERNAL : NonControlledProbeDevice::INTERNAL, openserver_id));
	connect(temp.device, SIGNAL(status_changed(StatusList)), SLOT(status_changed(StatusList)));

	unsigned default_bt_temp = 1235;
	updateDisplay(default_bt_temp, &temp);
	temp_list.append(temp);
}

void TemperatureViewer::inizializza()
{
}

void TemperatureViewer::updateDisplay(unsigned new_bt_temperature, TemperatureData *temp)
{
	QString displayed_temp;

	switch (temp_scale)
	{
	case CELSIUS:
		displayed_temp = celsiusString(bt2Celsius(new_bt_temperature));
		break;
	case FAHRENHEIT:
		displayed_temp = fahrenheitString(bt2Fahrenheit(new_bt_temperature));
		break;
	default:
		qWarning("Wrong temperature scale, defaulting to celsius");
		temp_scale = CELSIUS;
		displayed_temp = celsiusString(bt2Celsius(new_bt_temperature));
	}
	// qlcdnumber can display the degree sign, but not as utf-8 text.
	// We have to change the char TEMP_DEGREES with the single quote char (')
	displayed_temp = displayed_temp.replace(TEMP_DEGREES, " '");

	temp->lcd->display(displayed_temp);
}

void TemperatureViewer::status_changed(const StatusList &sl)
{
	if (!sl.contains(NonControlledProbeDevice::DIM_TEMPERATURE))
		return;

	device *dev = static_cast<device *>(sender());

	foreach (TemperatureData temp, temp_list)
		if (dev == temp.device)
			updateDisplay(sl[NonControlledProbeDevice::DIM_TEMPERATURE].toInt(), &temp);
}
