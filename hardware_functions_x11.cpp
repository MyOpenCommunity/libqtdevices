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
static QDateTime lastPress = QDateTime::currentDateTime().addSecs(-3600);

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

namespace {
	struct VolumeData
	{
		int volSveglia[AMPLI_NUM];
		uchar sorgente, stazione;

		VolumeData()
		{
			memset(&volSveglia, 0, sizeof(volSveglia));
			sorgente = stazione = 0;
		}
	};

	QString toString(const VolumeData &d)
	{
		return QString("station=%1, source=%2").arg(d.stazione).arg(d.sorgente);
	}
}

QHash<int, VolumeData> volumes;

void getAlarmVolumes(int index, int *volSveglia, uchar *sorgente, uchar *stazione)
{
	VolumeData data = volumes[index];

	memcpy(volSveglia, data.volSveglia, AMPLI_NUM * sizeof(int));
	*sorgente = data.sorgente;
	*stazione = data.stazione;

	qDebug() << "Reading volume data" << toString(data);
}

void setAlarmVolumes(int index, int *volSveglia, uchar sorgente, uchar stazione)
{
	VolumeData data;

	memcpy(data.volSveglia, volSveglia, AMPLI_NUM * sizeof(int));
	data.sorgente = sorgente;
	data.stazione = stazione;

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

