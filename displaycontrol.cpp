#include "displaycontrol.h"
#include "generic_functions.h" // setCfgValue, setBrightnessLevel, setBacklightOn

#include <assert.h>


DisplayControl::DisplayControl()
{
	forced_operative_mode = false;
}

void DisplayControl::_setBrightness(BrightnessLevel level)
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

	default:
		assert(!"Unknown level for brightness");
	}

	// Operative status has the same values for all levels
	data[DISPLAY_OPERATIVE].brightness = 10;
	data[DISPLAY_OPERATIVE].backlight = true;
	data[DISPLAY_OPERATIVE].screensaver = false;

	current_brightness = level;
}

void DisplayControl::setBrightness(BrightnessLevel level)
{
	_setBrightness(level);
	// TODO: dato che all'interno dell'item "DISPLAY" esiste un solo nodo level
	// allora non ci sono ambiguita'. In ogni caso sarebbe l'ideale poter scrivere
	// setCfgValue("brightness/level", level, DISPLAY) con significato analogo
	// a quello della getElement.
	setCfgValue("level", QString::number(level), DISPLAY);
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

void DisplayControl::setState(DisplayStatus status)
{
	if (!forced_operative_mode || forced_operative_mode && status == DISPLAY_OPERATIVE)
	{
		setBacklightOn(data[status].backlight);
		setBrightnessLevel(data[status].brightness);
	}
}

bool DisplayControl::screenSaverActive()
{
	return data[DISPLAY_SCREENSAVER].screensaver && !forced_operative_mode;
}

void DisplayControl::setScreenSaver(ScreenSaver::Type t)
{
	setCfgValue("type", QString::number(t), DISPLAY);
	current_screensaver = t;
}

ScreenSaver::Type DisplayControl::currentScreenSaver()
{
	return current_screensaver;
}


// The global definition of display
DisplayControl bt_global::display;
