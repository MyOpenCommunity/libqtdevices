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


#ifndef ANTINTRUSION_H
#define ANTINTRUSION_H

#include "bannerpage.h"
#include "skinmanager.h" // SkinManager::CidState
#include "antintrusion_device.h" // NUM_ZONES, AntintrusionDevice

class BtButton;
class BannAntintrusion;
class KeypadWithState;
class AntintrusionZone;
class QDomNode;


/*!
	\defgroup Antintrusion Antintrusion

	Allows the user to control the anti-intrusion system, enabling or disabling
	the alarm, partializing or inserting the zones and displaying the alarm history.
*/
class Antintrusion : public BannerPage
{
Q_OBJECT
public:
	Antintrusion(const QDomNode &config_node);

	virtual int sectionId() const;

private slots:
	void partialize();
	void toggleActivation();
	void doAction();
	void valueReceived(const DeviceValues &values_list);

private:
	enum Action
	{
		NONE = 0,
		ACTIVE,
		PARTIALIZE
	};

	SkinManager::CidState skin_cid;
	BtButton *partial_button;
	AntintrusionDevice *dev;
	BannAntintrusion *antintrusion_system;
	KeypadWithState *keypad;
	Action action;
	AntintrusionZone *zones[NUM_ZONES];

	void loadZones(const QDomNode &config_node);
	void updateKeypadStates();
};


#endif

