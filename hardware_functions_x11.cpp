#include "hardware_functions.h"
#include "main.h"

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

void setContrast(unsigned char c)
{
	contrast = c;
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

void setBeep(bool buzzer_enable)
{
	const char *p = buzzer_enable ? "1" : "0";

	buzzer_enabled = buzzer_enable;
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

static QProcess play_sound_process;
void playSound(const QString &wavFile)
{
	if (play_sound_process.state() != QProcess::NotRunning)
	{
		play_sound_process.terminate();
		play_sound_process.waitForFinished();
	}
	play_sound_process.start("mplayer", QStringList(wavFile));
}

void stopSound()
{
	play_sound_process.terminate();
}

void setVctVideoValue(const QString &command, const QString &value)
{
}

void initMultimedia()
{
}

void setVolume(VolumeType type, int value)
{
}
