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

#include <QWidget>

class AntintrusionDevice;
class QLabel;


class AntintrusionZone : public Bann2Buttons
{
Q_OBJECT

public:
	AntintrusionZone(int zone, QString descr);
	void setPartialization(bool partialized);
	void enablePartialization(bool enabled);
	bool isPartialized() const;

signals:
	void requestPartialization(int zone_number, bool partialize);

private slots:
	void leftClicked();

private:
	QString left_on, left_off, disabled_left_on, disabled_left_off;
	int zone_number;
	bool partialized;
	bool partialization_enabled;
};



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
