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
typedef QHash<int, QVariant> StatusList;


class BannTemperature : public BannerOld
{
Q_OBJECT
public:
	/**
	 * Creates a banner to display temperature information got from
	 * single (not controlled) and external probes.
	 *
	 * \param parent The parent widget
	 * \param name   The widget name
	 * \param config The node in the Dom tree that acts as root of this device
	 * \param dev    The probe (device) connected with this banner
	 */
	BannTemperature(QWidget *parent, QString where, QString descr, NonControlledProbeDevice *dev);

	virtual void inizializza(bool forza=false);

public slots:
	/**
	 * Manages a change in status, like temperature change.
	 */
	void status_changed(const StatusList &sl);
private:
	void setTemperature();
	/// Probed temperature (arrives from frame_interpreter directly in celsius, not in bt)
	int temperature;
	/// Temperature label
	QLabel  *temp_label;
	TemperatureScale temp_scale;
	NonControlledProbeDevice *dev;
};

#endif // BANNTEMPERATURE_H

