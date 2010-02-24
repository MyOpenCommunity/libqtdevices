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


#ifndef TEMPERATUREVIEWER_H
#define TEMPERATUREVIEWER_H

#include "frame_receiver.h"
#include "main.h" // TemperatureScale
#include "device.h" // StatusList

#include <QObject>
#include <QList>

class Page;
class NonControlledProbeDevice;

class QLabel;
class QString;
class QLCDNumber;


class TemperatureViewer : public QObject
{
Q_OBJECT
public:
	TemperatureViewer(Page *page);
	void add(QString where, int x, int y, int width, int height, QString descr, QString ext);
	void inizializza();

private slots:
	void status_changed(const StatusList &sl);

private:
	Page *linked_page; /// the page in which the temperature objects are displayed.

	struct TemperatureData
	{
		QLCDNumber *lcd;
		QLabel *text;
		NonControlledProbeDevice *device;
	};

	QList<TemperatureData> temp_list;

	/// The temperature scale set in config file (either Celsius or Fahrenheit)
	TemperatureScale temp_scale;

	void updateDisplay(unsigned new_bt_temperature, TemperatureData *temp);
};

#endif // TEMPERATUREVIEWER_H
