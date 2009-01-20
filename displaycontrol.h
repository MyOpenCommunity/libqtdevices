#ifndef DISPLAYCONTROL_H
#define DISPLAYCONTROL_H

#include <QMap>

enum BrightnessLevel
{
	BRIGHTNESS_OFF,
	BRIGHTNESS_LOW,
	BRIGHTNESS_NORMAL
};

enum DisplayStatus
{
	DISPLAY_OPERATIVE,
	DISPLAY_FREEZED,
	DISPLAY_SCREENSAVER
};


class DisplayControl
{
public:
	void setState(DisplayStatus status);
	BrightnessLevel currentBrightness();
	void setBrightness(BrightnessLevel level);
	bool screenSaverActive();

private:
	struct DisplayData
	{
		int brightness;
		bool backlight;
		bool screensaver;
	};

	QMap<DisplayStatus, DisplayData> data;
	BrightnessLevel current_brightness;
};

namespace bt_global { extern DisplayControl display; }

#endif //DISPLAYCONTROL_H
