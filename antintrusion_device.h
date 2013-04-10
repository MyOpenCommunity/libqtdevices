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


/*!
	\ingroup Antintrusion
	\brief Manages the burglar alarms system.

	Can be used to activate/deactivate the system calling toggleActivation(),
	receive alarms for standard zones, auxiliaries or special zones.
	Standard zones can be partialized or inserted calling partializeZone() followed
	by setPartialization().

	\section AntintrusionDevice-dimensions Dimensions
	\startdim
	\dim{DIM_SYSTEM_INSERTED,bool,,System status\, true if the alarm is on.}
	\dim{DIM_ZONE_PARTIALIZED,int,1 - 8, Notify that the zone has been partialized.}
	\dim{DIM_ZONE_INSERTED,int,1 - 8, Notify that the zone has been inserted.}
	\dim{DIM_INTRUSION_ALARM,int,1 - 8, An intrusion alarm has triggered on the zone.}
	\dim{DIM_TAMPER_ALARM,int,0 - 16,A tamper alarm has triggered on the zone.}
	\dim{DIM_ANTIPANIC_ALARM,int,9,An antipanic alarm has triggered on the zone.}
	\dim{DIM_TECHNICAL_ALARM,int,1 - 16, A technical alarm has triggered on the zone.}
	\dim{DIM_RESET_TECHNICAL_ALARM,int,1 - 16, Reset a previously triggered technical alarm.}
	\enddim
*/
class AntintrusionDevice : public device
{
friend class TestAntintrusionDevice;
friend class TestAntintrusionSystem;
Q_OBJECT
public:
	AntintrusionDevice(int openserver_id = 0);

	/*!
		\brief Activate/disactivate the system.

		Requires the \a password of the system.
	*/
	void toggleActivation(const QString &password);

	/*!
		\brief Activate the partialization of the zones.

		Requires the \a password of the system.

		\sa partializeZone();
	*/
	void setPartialization(const QString &password);

	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

	/*!
		\refdim{AntintrusionDevice}
	*/
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
	/*!
		\brief Partialize or activate the zone \a num_zone.
	*/
	void partializeZone(int num_zone, bool partialize);

	/*!
		\brief Request the status of the system and its zones.
	*/
	void requestStatus();

private slots:
	void sendActivationFrame(const QString &password);
	void sendPartializationFrame(const QString &password);

private:
	bool _partializeZone(int num_zone, bool partialize);

	bool zones[NUM_ZONES]; // true if a zone is partialized

	bool partialization_needed;
	bool is_inserted;
};

#endif
