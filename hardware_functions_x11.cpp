#include "hardware_functions.h"
#include "generic_functions.h"

#include <QScreen>
#include <QApplication>
#include <QProcess>
#include <QDateTime>

#include <stdio.h> // sprintf


static bool buzzer_enabled = false;
static bool backlight = false;
static unsigned char contrast = 0;
static QDateTime lastPress = QDateTime::currentDateTime();


int maxWidth()
{
	static int width = 0;
	if (!width)
		width = QScreen::instance()->width();
	return width;
}

int maxHeight()
{
	static int height = 0;
	if (!height)
		height = QScreen::instance()->height();
	return height;
}

HardwareType hardwareType()
{
	if (maxWidth() == 800)
		return TOUCH_X;

	return BTOUCH;
}

void setContrast(unsigned char c,bool b)
{
	contrast = c;

	if (b)
	{
		char contr[4];
		sprintf(contr,"%03d",c);
		setCfgValue("value", contr, CONTRASTO);
	}
}

unsigned char getContrast()
{
	return contrast;
}

void setBrightnessLevel(int level)
{
	// do nothing
}

void setBacklightOn(bool b)
{
	backlight = b;
}

void setBacklight(bool b)
{
	setBacklightOn(b);
}

void setBeep(bool buzzer_enable, bool write_to_conf)
{
	const char *p = buzzer_enable ? "1" : "0";

	buzzer_enabled = buzzer_enable;

	if (write_to_conf)
	{
#ifdef CONFIG_BTOUCH
		setCfgValue("value", p, SUONO);
#else
		setCfgValue("enabled", p, BEEP_ICON);
#endif
	}
}

bool getBeep()
{
	return buzzer_enabled;
}

bool getBacklight()
{
	return backlight;
}

void setOrientation(QString orientation)
{
	// do nothing
}

void beep(int t)
{
	if (buzzer_enabled)
#ifdef LAYOUT_TOUCHX
		playSound(SOUND_PATH "beep.wav");
#else
		QProcess::execute("beep");
#endif
}

void beep()
{
	beep(50);
}

void setTimePress(const QDateTime &press)
{
	lastPress = press;
}

unsigned long getTimePress()
{
	return lastPress.secsTo(QDateTime::currentDateTime());
}

void rearmWDT()
{
	// do nothing
}

void getName(char *name)
{
	name[0] = 0;
}

void getAlarmVolumes(int index, int *volSveglia, uchar *sorgente, uchar *stazione)
{
	// do nothing
}

void setAlarmVolumes(int index, int *volSveglia, uchar sorgente, uchar stazione)
{
	// do nothing
}

void playSound(const QString &wavFile)
{
	QProcess::startDetached("mplayer", QStringList(wavFile));
}

void setVctVideoValue(const QString &command, const QString &value)
{
}
