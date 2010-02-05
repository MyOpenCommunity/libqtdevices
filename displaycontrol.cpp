#include "displaycontrol.h"
#include "generic_functions.h" // setCfgValue
#include "hardware_functions.h" // setBrightnessLevel, setBacklightOn


DisplayControl::DisplayControl()
{
	forced_operative_mode = false;
	setBrightness(BRIGHTNESS_NORMAL);
	setState(DISPLAY_OPERATIVE);
}

void DisplayControl::setBrightness(BrightnessLevel level)
{
	switch (level)
	{
	case BRIGHTNESS_OFF:
		data[DISPLAY_FREEZED].brightness = 10;
		data[DISPLAY_FREEZED].backlight = false;
		data[DISPLAY_FREEZED].screensaver = false;
		data[DISPLAY_SCREENSAVER].brightness = 10;
		data[DISPLAY_SCREENSAVER].backlight = false;
		data[DISPLAY_SCREENSAVER].screensaver = false;
		break;

	case BRIGHTNESS_LOW:
		data[DISPLAY_FREEZED].brightness = 255;
		data[DISPLAY_FREEZED].backlight = true;
		data[DISPLAY_FREEZED].screensaver = false;
		data[DISPLAY_SCREENSAVER].brightness = 255;
		data[DISPLAY_SCREENSAVER].backlight = true;
		data[DISPLAY_SCREENSAVER].screensaver = true;
		break;

	case BRIGHTNESS_NORMAL:
		data[DISPLAY_FREEZED].brightness = 210;
		data[DISPLAY_FREEZED].backlight = true;
		data[DISPLAY_FREEZED].screensaver = false;
		data[DISPLAY_SCREENSAVER].brightness = 210;
		data[DISPLAY_SCREENSAVER].backlight = true;
		data[DISPLAY_SCREENSAVER].screensaver = true;
		break;

	case BRIGHTNESS_HIGH:
		data[DISPLAY_FREEZED].brightness = 50;
		data[DISPLAY_FREEZED].backlight = true;
		data[DISPLAY_FREEZED].screensaver = false;
		data[DISPLAY_SCREENSAVER].brightness = 50;
		data[DISPLAY_SCREENSAVER].backlight = true;
		data[DISPLAY_SCREENSAVER].screensaver = true;
		break;

	default:
		qFatal("Unknown level for brightness");
	}

	// Operative status has the same values for all levels
	data[DISPLAY_OPERATIVE].brightness = 10;
	data[DISPLAY_OPERATIVE].backlight = true;
	data[DISPLAY_OPERATIVE].screensaver = false;

#ifdef CONFIG_BTOUCH
	// TODO: to be changed on TouchX
	setCfgValue("brightness/level", level, DISPLAY);
#endif
	current_brightness = level;
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
DisplayControl bt_global::display;
