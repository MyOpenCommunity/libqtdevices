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


#ifndef ACTUATORS_H
#define ACTUATORS_H

#include "bann2_buttons.h" // BannOnOffState
#include "bann1_button.h" // BannSinglePuls
#include "device.h" //StatusList

class LightingDevice;


enum ActuatorType
{
	AUTOMAZ,  /*!< driving as a pulse an automation actuator */
	VCT_SERR, /*!< driving as a pulse a video-doorentrysystem actuator configured as "lock" */
	VCT_LS,   /*!< driving as a pulse a video-doorentrysystem actuator configured as "stairlight" */
};


class SingleActuator : public BannOnOffState
{
Q_OBJECT
public:
	SingleActuator(const QString &descr, const QString &where, int openserver_id);

private slots:
	void activate();
	void deactivate();
	void status_changed(const StatusList &status_list);

private:
	LightingDevice *dev;
};


class ButtonActuator : public BannSinglePuls
{
Q_OBJECT
public:
	ButtonActuator(const QString &descr, const QString &_where, int t);

private slots:
	void activate();
	void deactivate();

private:
	int type;
	QString where;
};

#endif
