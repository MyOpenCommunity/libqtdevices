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
#include "mediaplayer.h" // bt_global::sound
#include "main.h"

#ifdef Q_WS_QWS
#include <QScreen>
#endif

#include <QApplication>
#include <QProcess>
#include <QDateTime>
#include <QtDebug>

#include <stdio.h> // sprintf


static bool buzzer_enabled = false;
static bool backlight = false;
static unsigned char contrast = 0;
static QDateTime last_press = QDateTime::currentDateTime();

namespace
{
	struct VolumeData
	{
		int alarm_volumes[AMPLI_NUM];
		uchar source, station;

		VolumeData()
		{
			memset(&alarm_volumes, 0, sizeof(alarm_volumes));
			source = source = 0;
		}
	};

	QString toString(const VolumeData &d)
	{
		return QString("station=%1, source=%2").arg(d.station).arg(d.source);
	}
}


int maxWidth()
{
	static int width = 0;
	if (!width)
	{
#ifdef Q_WS_QWS
		width = QScreen::instance()->width();
#else
		width = 800;
#endif
	}
	return width;
}

int maxHeight()
{
	static int height = 0;
	if (!height)
	{
#ifdef Q_WS_QWS
		height = QScreen::instance()->height();
#else
		height = 480;
#endif
	}
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

void setBacklight(bool b)
{
	backlight = b;
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
#ifdef LAYOUT_TS_10
		bt_global::sound->play(SOUND_PATH "beep.wav");
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
	last_press = press;
}

unsigned long getTimePress()
{
	return last_press.secsTo(QDateTime::currentDateTime());
}

void rearmWDT()
{
	// do nothing
}

void getName(char *name)
{
	name[0] = 0;
}



QHash<int, VolumeData> volumes;

void getAlarmVolumes(int index, int *alarm_volumes, uchar *source, uchar *station)
{
	VolumeData data = volumes[index];

	memcpy(alarm_volumes, data.alarm_volumes, AMPLI_NUM * sizeof(int));
	*source = data.source;
	*station = data.station;

	qDebug() << "Reading volume data" << toString(data);
}

void setAlarmVolumes(int index, int *alarm_volumes, uchar source, uchar station)
{
	VolumeData data;

	memcpy(data.alarm_volumes, alarm_volumes, AMPLI_NUM * sizeof(int));
	data.source = source;
	data.station = station;

	qDebug() << "Saving volume data" << toString(data);

	volumes[index] = data;
}


void setVctVideoValue(const QString &command, const QString &value)
{
}

void initMultimedia()
{
}

void usbHotplug()
{

}

QPair <QString, QStringList> getAudioCmdLine(const QString &audio_path)
{
	return qMakePair(QString("mplayer"), QStringList(audio_path));
}

void dumpSystemMemory()
{
}

int getMemFree()
{
	return -1;
}

void createFlagFile(QString filename)
{
	// On X11 there are no openserver stack processes active, so we have nothing to
	// notify.
	Q_UNUSED(filename);
}

