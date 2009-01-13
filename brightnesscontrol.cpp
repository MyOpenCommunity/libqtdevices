#include "brightnesscontrol.h"
#include "generic_functions.h" // setCfgValue

#include <assert.h>


BrightnessControl *BrightnessControl::_instance = 0;


BrightnessControl::BrightnessControl()
{
}

void BrightnessControl::setLevel(BrightnessLevel level)
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

	// TODO: dato che all'interno dell'item "DISPLAY" esiste un solo nodo level
	// allora non ci sono ambiguita'. In ogni caso sarebbe l'ideale poter scrivere
	// setCfgValue("brightness/level", level, DISPLAY) con significato analogo
	// a quello della getElement.
	setCfgValue("level", QString::number(level), DISPLAY);
	current_level = level;
}

BrightnessLevel BrightnessControl::currentLevel()
{
	return current_level;
}

void BrightnessControl::setState(DisplayStatus status)
{
	setBacklightOn(data[status].backlight);
	setBrightnessLevel(data[status].brightness);
}

BrightnessControl *BrightnessControl::instance()
{
	if (!_instance)
		_instance = new BrightnessControl();
	return _instance;
}
