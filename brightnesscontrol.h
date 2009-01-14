#ifndef BRIGHTNESSCONTROL_H
#define BRIGHTNESSCONTROL_H

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


class BrightnessControl
{
public:
	void setState(DisplayStatus status);
	BrightnessLevel currentLevel();
	void setLevel(BrightnessLevel level);

private:
	struct DisplayData
	{
		int brightness;
		bool backlight;
		bool screensaver;
	};

	QMap<DisplayStatus, DisplayData> data;
	BrightnessLevel current_level;
	static BrightnessControl *_instance;
};

namespace bt_global { extern BrightnessControl brightness; }

#endif //BRIGHTNESSCONTROL_H
