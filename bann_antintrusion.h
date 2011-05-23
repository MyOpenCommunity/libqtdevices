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


#ifndef BANN_ANTINTRUSION_H
#define BANN_ANTINTRUSION_H

#include "bann2_buttons.h"

#ifdef LAYOUT_TS_3_5
#include "bann4_buttons.h"
#endif

#include <QWidget>

class AntintrusionDevice;
class QLabel;


/*!
	\ingroup Antintrusion
	\brief Represents a zone of the antintrusion system.
*/
class AntintrusionZone :
#ifdef LAYOUT_TS_3_5
	public Bann4Buttons
#else
	public Bann2Buttons
#endif
{
Q_OBJECT

public:
	AntintrusionZone(int zone, QString descr);
	void setPartialization(bool partialized);
	void enablePartialization(bool enabled);
	bool isPartialized() const;
	QString zoneDescription() const;

signals:
	void requestPartialization(int zone_number, bool partialize);

private slots:
	void leftClicked();

private:
	QString left_on, left_off;
#ifdef LAYOUT_TS_3_5
	QString status_on, status_off;
#else
	QString disabled_left_on, disabled_left_off;
#endif
	QString zone_description;
	int zone_number;
	bool partialized;
	bool partialization_enabled;
};


/*!
	\ingroup Antintrusion
	\brief Controls the antintrusion system.
*/
class BannAntintrusion : public Bann2Buttons
{
Q_OBJECT
public:
	BannAntintrusion(QWidget *parent = 0);

	// Update the icons to respect the state of the system
	void setState(bool on);

	// Show/hide the button to see the alarm list
	void showAlarmsButton(bool show);

signals:
	void showAlarms();
	void toggleActivation();

private:
	QString turn_on, turn_off, state_on, state_off;
};

#endif
