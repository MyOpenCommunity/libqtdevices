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


#include "hardware_functions.h"
#include "main.h"

#ifdef Q_WS_QWS
#include <QScreen>
#endif

#include <QApplication>
#include <QProcess>
#include <QDateTime>

#include <stdio.h> // sprintf


static bool buzzer_enabled = false;
static bool backlight = false;
static unsigned char contrast = 0;
static QDateTime lastPress = QDateTime::currentDateTime().addSecs(-3600);

int maxWidth()
{
	static int width = 0;
#ifdef Q_WS_QWS
	if (!width)
		width = QScreen::instance()->width();
#endif
	return width;
}

int maxHeight()
{
	static int height = 0;
#ifdef Q_WS_QWS
	if (!height)
		height = QScreen::instance()->height();
#endif
	return height;
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

Q_GLOBAL_STATIC(QProcess, play_sound_process);
void playSound(const QString &wavFile)
{
	if (play_sound_process()->state() != QProcess::NotRunning)
	{
		play_sound_process()->terminate();
		play_sound_process()->waitForFinished();
	}
	play_sound_process()->start("mplayer", QStringList(wavFile));
}

void stopSound()
{
	play_sound_process()->terminate();
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

void initScreen()
{
}
