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
	DISPLAY_SCREENSAVER
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

	// Set the brightness. The difference between this method and the one without
	// underscore is that the other also store the brightness level in the configuration
	// file. So, use this method only in inizialization.
	void _setBrightness(BrightnessLevel level);
};

namespace bt_global { extern DisplayControl display; }

#endif //DISPLAYCONTROL_H
