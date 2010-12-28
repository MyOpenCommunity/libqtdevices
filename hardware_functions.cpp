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
#include "generic_functions.h" // smartExecute
#include "main.h"
#ifdef BT_HARDWARE_TS_10
#include "audiostatemachine.h"
#include "mediaplayer.h"
#endif

#include <QFile>
#include <QScreen>
#include <QtDebug>
#include <QProcess>
#include <QDateTime>
#include <QStringList>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h> // sprintf


// values for /bin/settrimmer
#define TFT_BRIGHTNESS    "1"
#define TFT_CONTRAST      "2"
#define TFT_COLOR         "3"
#define TFT_CONTRAST_VCT  "4"
#define TFT_COLOR_VCT     "5"
#define TFT_BRIGTH_VCT    "6"



#define E2_BASE 11360

#define KEY_LENGTH 5
#define AL_KEY "\125\252\125\252\125"
#define SORG_PAR 2


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

void setContrast(unsigned char c)
{
	char contr[4];

	if (QFile::exists("/proc/sys/dev/btweb/contrast"))
	{
		int fd = open("/proc/sys/dev/btweb/contrast", O_WRONLY);
		if (fd >= 0)
		{
			sprintf(contr,"%d",c);
			qDebug("setto il contrasto a : %d", c);
			write(fd, contr, 4);
			close(fd);
		}
	}
}

unsigned char getContrast()
{
	char contr[4];
	if (QFile::exists("/proc/sys/dev/btweb/contrast"))
	{
		int fd = open("/proc/sys/dev/btweb/contrast",O_RDONLY);
		if (fd >= 0)
		{
			read(fd, contr, 4);
			close(fd);
			return atoi(contr);
		}
	}
	return 0x00;
}

static void writeValueToFd(int fd, int value)
{
	// this has to be changed in qt4
	QByteArray str = QString::number(value).toLatin1();
	write(fd, str.constData(), str.length());
}

#ifdef BT_HARDWARE_TS_3_5
void setBrightnessLevel(int level)
{
	int hardware_level;
	switch (level)
	{
	case 1:
		hardware_level = 255;
		break;
	case 2:
		hardware_level = 240;
		break;
	case 3:
		hardware_level = 225;
		break;
	case 4:
		hardware_level = 210;
		break;
	case 5:
		hardware_level = 180;
		break;
	case 6:
		hardware_level = 140;
		break;
	case 7:
		hardware_level = 100;
		break;
	case 8:
		hardware_level = 50;
		break;
	case 9:
		hardware_level = 10;
		break;
	case 10:
		hardware_level = 0;
		break;
	default:
		return;
	};

	if (QFile::exists("/proc/sys/dev/btweb/brightness"))
	{
		int fd = open("/proc/sys/dev/btweb/brightness", O_WRONLY);
		if (fd >= 0)
		{
			writeValueToFd(fd, hardware_level);
			close(fd);
		}
	}
}
#else
void setBrightnessLevel(int level)
{
	int hardware_level;

	switch (level)
	{
	case 1:
		hardware_level = 13;
		break;
	case 2:
		hardware_level = 12;
		break;
	case 3:
		hardware_level = 10;
		break;
	case 4:
		hardware_level = 9;
		break;
	case 5:
		hardware_level = 7;
		break;
	case 6:
		hardware_level = 6;
		break;
	case 7:
		hardware_level = 4;
		break;
	case 8:
		hardware_level = 3;
		break;
	case 9:
		hardware_level = 1;
		break;
	case 10:
		hardware_level = 0;
		break;
	default:
		return;
	};
	smartExecute("/bin/settrimmer",
		QStringList() << TFT_BRIGHTNESS << QString::number(hardware_level));
}
#endif

void setBacklight(bool b)
{
	if (QFile::exists("/proc/sys/dev/btweb/backlight"))
	{
		int fd = open("/proc/sys/dev/btweb/backlight", O_WRONLY);
		if (fd >= 0)
		{
			writeValueToFd(fd, b);
			close(fd);
		}
	}

#ifdef BT_HARDWARE_TS_10
	if (b)
	{
		QStringList args_contrast;
		args_contrast << TFT_CONTRAST << "7";
		smartExecute("/bin/settrimmer", args_contrast);
	}
#endif
}


#ifdef BT_HARDWARE_TS_10

bool buzzer_enabled = false;

void setBeep(bool enable)
{
	// enter the BEEP_ON state to enable the buzzer
	if (enable)
		bt_global::audio_states->toState(AudioStates::BEEP_ON);
	// exit the BEEP_ON state to disable the buzzer
	else if (!enable && buzzer_enabled)
		bt_global::audio_states->removeState(AudioStates::BEEP_ON);
	buzzer_enabled = enable;
}

bool getBeep()
{
	return buzzer_enabled;
}

#else

void setBeep(bool enable)
{
	// TODO port the audio state machine to touch 3.5

	const char *p = enable ? "1" : "0";
	if (QFile::exists("/proc/sys/dev/btweb/buzzer_enable"))
	{
		FILE* fd = fopen("/proc/sys/dev/btweb/buzzer_enable", "w");
		if (fd >= 0)
		{
			fwrite(p, 1, 1, fd);
			fclose(fd);
		}
	}
}

bool getBeep()
{
	unsigned char c;
	if (QFile::exists("/proc/sys/dev/btweb/buzzer_enable"))
	{
		FILE *fd = fopen("/proc/sys/dev/btweb/buzzer_enable",  "r");
		if (fd >= 0)
		{
			fread (&c, 1, 1, fd);
			fclose(fd);
			return c != '0';
		}
	}
	return false;
}

#endif

bool getBacklight()
{
	char c[4];
	char name[50];
	int fd;

	getName(name);
	if (!strncmp(name, "H4684_IP", strlen("H4684_IP")))
	{
		if (QFile::exists("/proc/sys/dev/btweb/brightness"))
		{
			fd = open("/proc/sys/dev/btweb/brightness", O_RDONLY);
			if (fd >= 0)
			{
				read(fd, c, 4);
				close(fd);
				if (atoi(c) < 100)
					return true;
			}
		}
	}
	else
	{
		if (QFile::exists("/proc/sys/dev/btweb/backlight"))
		{
			fd = open("/proc/sys/dev/btweb/backlight", O_RDONLY);
			if (fd >= 0)
			{
				read (fd, c, 1);
				close(fd);
				if (c[0] != '0')
					return true;
			}
		}
	}
	return false;
}


void setOrientation(QString orientation)
{
	if (QFile::exists("/proc/sys/dev/btweb/upsidedown"))
	{
		int fd = open("/proc/sys/dev/btweb/upsidedown", O_WRONLY);
		if (fd >= 0)
		{
			write(fd, orientation.toAscii().constData(), 1);
			close(fd);
		}
	}
}

void beep(int t)
{
#ifdef BT_HARDWARE_TS_3_5
	if (QFile::exists("/proc/sys/dev/btweb/buzzer"))
	{
		int fd = open("/proc/sys/dev/btweb/buzzer", O_WRONLY);
		if (fd >= 0)
		{
			char te[10];
			sprintf(te, "4000 %d", t);
			write(fd, te, strlen(te));
			close(fd);
		}
	}
#else // BT_HARDWARE_TS_10
	if (buzzer_enabled && bt_global::audio_states->currentState() == AudioStates::BEEP_ON && QFile::exists(SOUND_PATH "beep.wav"))
		bt_global::sound->play(SOUND_PATH "beep.wav");
#endif
}

void beep()
{
	beep(50);
}

#ifdef BT_HARDWARE_TS_3_5

unsigned long getTimePress()
{
	unsigned long t = 0;
	char time[50];

	memset(time,'\000',sizeof(time));
	if (QFile::exists("/proc/sys/dev/btweb/touch_ago"))
	{
		int fd = open("/proc/sys/dev/btweb/touch_ago", O_RDONLY);

		read(fd, time, 6);
		close(fd);
		t = atol(time);
	}
	return t;
}

#else

static QDateTime lastPress = QDateTime::currentDateTime().addSecs(-3600);

void setTimePress(const QDateTime &press)
{
	lastPress = press;
}

unsigned long getTimePress()
{
	return lastPress.secsTo(QDateTime::currentDateTime());
}

#endif

void rearmWDT()
{
	if (!QFile::exists(FILE_WDT))
	{
		int fd = creat(FILE_WDT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
		if (fd != -1)
		{
			close(fd);
		}
	}
}

void getName(char *name)
{
	memset(name, '\0', sizeof(name));
	if (QFile::exists("/proc/sys/dev/btweb/name"))
	{
		int fd = open("/proc/sys/dev/btweb/name", O_RDONLY);
		if (fd >= 0)
		{
			read(fd, name, 50);
			close(fd);
		}
	}
}

static inline int alarmOffset(int index)
{
	return E2_BASE + index * (AMPLI_NUM + KEY_LENGTH + SORG_PAR);
}

void getAlarmVolumes(int index, int *volSveglia, uchar *sorgente, uchar *stazione)
{
	qDebug() << "Reading alarm volume from e2 for index" << index;

	memset(volSveglia, 0, sizeof(int) * AMPLI_NUM);
	*sorgente = *stazione = 0;

	int eeprom = open(DEV_E2, O_RDWR | O_SYNC, 0666);
	if (eeprom == -1)
		return;

	// read the alarm key, to check if this E2 offset contains alarm data
	char keys[5] = {0, 0, 0, 0, 0};
	lseek(eeprom, alarmOffset(index), SEEK_SET);
	read(eeprom, keys, 5);

	if (memcmp(keys, AL_KEY, 5) != 0)
	{
		// no alarm data: initialize the E2 offset
		lseek(eeprom, alarmOffset(index), SEEK_SET);
		write(eeprom, AL_KEY, 5);

		for (unsigned int idx = 0; idx < AMPLI_NUM; idx++)
		{
			write(eeprom, "\000", 1);
			volSveglia[idx] = -1;
		}
	}
	else
	{
		// read the alarm data from E2
		lseek(eeprom, alarmOffset(index) + KEY_LENGTH, SEEK_SET);

		for (unsigned int idx = 0; idx < AMPLI_NUM; idx++)
		{
			read(eeprom, &volSveglia[idx], 1);

			if (volSveglia[idx] == 0xff)
				volSveglia[idx] = -1;
			else
				volSveglia[idx] &= 0x1F;
		}

		read(eeprom, sorgente, 1);
		read(eeprom, stazione, 1);
	}

	close(eeprom);
}

void setAlarmVolumes(int index, int *volSveglia, uchar sorgente, uchar stazione)
{
	int eeprom = open(DEV_E2, O_RDWR | O_SYNC, 0666);

	qDebug() << "Writing alarm volume to e2 for index" << index;

	if (eeprom == -1)
		return;

	lseek(eeprom, alarmOffset(index) + KEY_LENGTH, SEEK_SET);

	for (unsigned int idx = 0; idx < AMPLI_NUM; idx++)
		write(eeprom, &volSveglia[idx], 1);

	write(eeprom, &sorgente, 1);
	write(eeprom, &stazione, 1);
	close(eeprom);
}

void setVctVideoValue(const QString &command, const QString &value)
{
#ifdef BT_HARDWARE_TS_10
	QProcess::startDetached("/home/bticino/bin/set_videocom",
		QStringList() << command << value);
#endif
}

void initMultimedia()
{
#ifdef BT_HARDWARE_TS_10
	smartExecute("/bin/init_audio_system");
	smartExecute("/bin/init_video_system");
#endif
}

void usbHotplug()
{
#ifdef BT_HARDWARE_TS_10
	smartExecute("/sbin/hotplug add 1");
#endif
}

QPair <QString, QStringList> getAudioCmdLine(const QString &audio_path)
{
	return qMakePair(QString("/bin/sox"), QStringList() << "-w" << "-c" << "2"
		<< "-s" << "-t" << "wav" << audio_path << "-t" << "ossdsp" << "/dev/dsp1");
}

void dumpSystemMemory()
{
#ifdef BT_HARDWARE_TS_10
	QFile f("/proc/meminfo");
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	qDebug() << "----------------------- SYSTEM MEMORY -----------------------";
	// We are interested in the first two lines
	qDebug() << f.readLine().constData();
	qDebug() << f.readLine().constData();
	qDebug() << "-------------------------------------------------------------";
#endif
}

