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


#ifndef ANTINTRUSION_DEVICE_H
#define ANTINTRUSION_DEVICE_H

#include "device.h"

#define NUM_ZONES 8


/*
 * This class manages the antintrusion system.
 */
class AntintrusionDevice : public device
{
Q_OBJECT
public:
	AntintrusionDevice(int openserver_id = 0);

	// Call this method to activate/disactivate the antintrusion system
	void toggleActivation(const QString &password);

	// This method set the antintrusion system in order to exclude some zones
	// from the "list of zones watched"
	void setPartialization(const QString &password);

	void requestStatus();

	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

	enum Type
	{
		DIM_SYSTEM_INSERTED,
		DIM_ZONE_PARTIALIZED = 18,
		DIM_ZONE_INSERTED = 11,
		DIM_INTRUSION_ALARM = 15,
		DIM_TAMPER_ALARM = 16,
		DIM_ANTIPANIC_ALARM = 17,
		DIM_TECHNICAL_ALARM = 12,
		DIM_RESET_TECHNICAL_ALARM = 13,
	};

	virtual void init() { requestStatus(); }

public slots:
	// Partialize or activate a zone (the zones are numbered from 1 to 8)
	void partializeZone(int num_zone, bool partialize);

private:
	bool zones[NUM_ZONES];
};

#endif
