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


#ifndef BANNTEMPERATURE_H
#define BANNTEMPERATURE_H

#include "banner.h"
#include "main.h"  // TemperatureScale

class NonControlledProbeDevice;
class QLabel;
typedef QHash<int, QVariant> DeviceValues;


/**
 * A banner to display the temperature for not controlled and external probes.
 */
class BannTemperature : public BannerNew
{
Q_OBJECT
public:
	BannTemperature(QString descr, NonControlledProbeDevice *dev);

public slots:
	void valueReceived(const DeviceValues &values_list);

private:
	void updateTemperature(int temperature);

private:
	QLabel *temperature_label;

	TemperatureScale temperature_scale;
};

#endif // BANNTEMPERATURE_H

