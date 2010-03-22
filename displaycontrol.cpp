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


#include "displaycontrol.h"
#include "generic_functions.h" // setCfgValue
#include "hardware_functions.h" // setBrightnessLevel, setBacklightOn


DisplayControl::DisplayControl()
{
	forced_operative_mode = false;
	operative_brightness = 10;
	setBrightness(BRIGHTNESS_NORMAL);
	setState(DISPLAY_OPERATIVE);
}

static inline int min_brightness(int a, int b)
{
	return qMin(qMax(a, b), 255);
}

void DisplayControl::updateBrightnessData()
{
	// always use low/medium brightness values that are below current brightness level
	int brightness_max = operative_brightness;
	int brightness_min = 255;
	int brightness_low = min_brightness(210, brightness_max + 200);
	int brightness_medium = min_brightness(50, brightness_max + 40);

	switch (current_brightness)
	{
	case BRIGHTNESS_OFF:
		data[DISPLAY_FREEZED].brightness = brightness_max;
		data[DISPLAY_FREEZED].backlight = false;
		data[DISPLAY_FREEZED].screensaver = false;
		data[DISPLAY_SCREENSAVER].brightness = brightness_max;
		data[DISPLAY_SCREENSAVER].backlight = false;
		data[DISPLAY_SCREENSAVER].screensaver = false;
		break;

	case BRIGHTNESS_LOW:
		data[DISPLAY_FREEZED].brightness = brightness_min;
		data[DISPLAY_FREEZED].backlight = true;
		data[DISPLAY_FREEZED].screensaver = false;
		data[DISPLAY_SCREENSAVER].brightness = brightness_min;
		data[DISPLAY_SCREENSAVER].backlight = true;
		data[DISPLAY_SCREENSAVER].screensaver = true;
		break;

	case BRIGHTNESS_NORMAL:
		data[DISPLAY_FREEZED].brightness = brightness_low;
		data[DISPLAY_FREEZED].backlight = true;
		data[DISPLAY_FREEZED].screensaver = false;
		data[DISPLAY_SCREENSAVER].brightness = brightness_low;
		data[DISPLAY_SCREENSAVER].backlight = true;
		data[DISPLAY_SCREENSAVER].screensaver = true;
		break;

	case BRIGHTNESS_HIGH:
		data[DISPLAY_FREEZED].brightness = brightness_medium;
		data[DISPLAY_FREEZED].backlight = true;
		data[DISPLAY_FREEZED].screensaver = false;
		data[DISPLAY_SCREENSAVER].brightness = brightness_medium;
		data[DISPLAY_SCREENSAVER].backlight = true;
		data[DISPLAY_SCREENSAVER].screensaver = true;
		break;

	default:
		qFatal("Unknown level for brightness");
	}

	// Off and operative status have the same values for all levels
	data[DISPLAY_OFF].brightness = brightness_max;
	data[DISPLAY_OFF].backlight = false;
	data[DISPLAY_OFF].screensaver = false;

	data[DISPLAY_OPERATIVE].brightness = operative_brightness;
	data[DISPLAY_OPERATIVE].backlight = true;
	data[DISPLAY_OPERATIVE].screensaver = false;

	if (data[current_state].backlight)
		setBrightnessLevel(data[current_state].brightness);
}

void DisplayControl::setBrightness(BrightnessLevel level)
{
	current_brightness = level;
	updateBrightnessData();

#ifdef CONFIG_BTOUCH
	// TODO: to be changed on TouchX
	setCfgValue("brightness/level", level, DISPLAY);
#endif
}

void DisplayControl::setOperativeBrightness(int brightness)
{
	operative_brightness = brightness;
	updateBrightnessData();
}

int DisplayControl::operativeBrightness()
{
	return operative_brightness;
}

BrightnessLevel DisplayControl::currentBrightness()
{
	return current_brightness;
}

void DisplayControl::forceOperativeMode(bool enable)
{
	forced_operative_mode = enable;
	if (enable)
		setState(DISPLAY_OPERATIVE);
}

bool DisplayControl::isForcedOperativeMode()
{
	return forced_operative_mode;
}

void DisplayControl::setState(DisplayStatus status)
{
	if (!forced_operative_mode || (forced_operative_mode && status == DISPLAY_OPERATIVE))
	{
		setBacklightOn(data[status].backlight);
		setBrightnessLevel(data[status].brightness);
	}

	// We have to re-initialize the screen when the GUI exit from the state DISPLAY_OFF
	if (current_state == DISPLAY_OFF && status != DISPLAY_OFF)
		initScreen();
	current_state = status;
}

DisplayStatus DisplayControl::currentState()
{
	return current_state;
}

void DisplayControl::setScreenSaver(ScreenSaver::Type t)
{
	// TODO find the correct place to save the information
#ifdef CONFIG_BTOUCH
	setCfgValue("screensaver/type", t, DISPLAY);
#endif
	current_screensaver = t;
}

ScreenSaver::Type DisplayControl::currentScreenSaver()
{
	return current_screensaver;
}


// The global definition of display
DisplayControl *bt_global::display = 0;
