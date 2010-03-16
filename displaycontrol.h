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


#ifndef DISPLAYCONTROL_H
#define DISPLAYCONTROL_H

#include "screensaver.h"

#include <QMap>


enum BrightnessLevel
{
	BRIGHTNESS_OFF,
	BRIGHTNESS_LOW,
	BRIGHTNESS_NORMAL,
	BRIGHTNESS_HIGH
};

enum DisplayStatus
{
	DISPLAY_OPERATIVE,
	DISPLAY_FREEZED,
	DISPLAY_SCREENSAVER,
	DISPLAY_OFF
};

/*
 * This class is a global controller of the properties of display.
 */
class DisplayControl
{
friend class BtMain;
public:
	DisplayControl();
	// Set the state of the display
	void setState(DisplayStatus status);
	DisplayStatus currentState();

	// set the brightness to be used in operative mode
	void setOperativeBrightness(int brightness);
	int operativeBrightness();

	// Brightness methods
	BrightnessLevel currentBrightness();
	void setBrightness(BrightnessLevel level);

	// Screensaver methods
	void setScreenSaver(ScreenSaver::Type t);
	ScreenSaver::Type currentScreenSaver();

	// To force (or to remove the forcing) the display to run in the operative mode.
	void forceOperativeMode(bool enable);
	bool isForcedOperativeMode();

private:
	void updateBrightnessData();

private:
	struct DisplayData
	{
		int brightness;
		bool backlight;
		bool screensaver;
	};

	QMap<DisplayStatus, DisplayData> data;
	BrightnessLevel current_brightness;
	ScreenSaver::Type current_screensaver;
	DisplayStatus current_state;
	bool forced_operative_mode;
	int operative_brightness;
};

namespace bt_global { extern DisplayControl display; }

#endif //DISPLAYCONTROL_H
