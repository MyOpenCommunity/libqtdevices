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
#ifdef BT_HARDWARE_PXA270
#include "audiostatemachine.h"
#endif
#if defined(BT_HARDWARE_PXA270) || defined(BT_HARDWARE_DM365)
#include "mediaplayer.h" // bt_global::sound
#endif

#include <QFile>
#include <QScreen>
#include <QtDebug>
#include <QProcess>
#include <QDateTime>
#include <QStringList>
#include <QByteArray>
#include <QList>

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
#ifdef BT_HARDWARE_DM365
	const char *FILE_CONTRAST = "/sys/class/lcd/tm035kbh02/contrast";
#else
	const char *FILE_CONTRAST = "/proc/sys/dev/btweb/contrast";
#endif
	char contr[4];

	if (QFile::exists(FILE_CONTRAST))
	{
		int fd = open(FILE_CONTRAST, O_WRONLY);
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
#ifdef BT_HARDWARE_DM365
	const char *FILE_CONTRAST = "/sys/class/lcd/tm035kbh02/contrast";
#else
	const char *FILE_CONTRAST = "/proc/sys/dev/btweb/contrast";
#endif
	char contr[4];

	if (QFile::exists(FILE_CONTRAST))
	{
		int fd = open(FILE_CONTRAST, O_RDONLY);
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

#ifdef BT_HARDWARE_PXA255
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
#elif defined(BT_HARDWARE_DM365)
void setBrightnessLevel(int level)
{
	int hardware_level;
	switch (level)
	{
	case 1:
		hardware_level = 0;
		break;
	case 2:
		hardware_level = 10;
		break;
	case 3:
		hardware_level = 50;
		break;
	case 4:
		hardware_level = 100;
		break;
	case 5:
		hardware_level = 140;
		break;
	case 6:
		hardware_level = 140;
		break;
	case 7:
		hardware_level = 210;
		break;
	case 8:
		hardware_level = 225;
		break;
	case 9:
		hardware_level = 240;
		break;
	case 10:
		hardware_level = 255;
		break;
	default:
		return;
	};

	if (QFile::exists("/sys/class/backlight/dingo_backlight/brightness"))
	{
		int fd = open("/sys/class/backlight/dingo_backlight/brightness", O_WRONLY);
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
	char name[50];
	int fd;

	getName(name);
	if (!strncmp(name, "DINGO", strlen("DINGO")))
	{
		if (QFile::exists("/sys/class/backlight/dingo_backlight/bl_power"))
		{
			fd = open("/sys/class/backlight/dingo_backlight/bl_power", O_WRONLY);
			if (fd >= 0)
			{
				writeValueToFd(fd, !b);
				close(fd);
			}
		}
	}
	else
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
	}

#ifdef BT_HARDWARE_PXA270
	if (b)
	{
		QStringList args_contrast;
		args_contrast << TFT_CONTRAST << "7";
		smartExecute("/bin/settrimmer", args_contrast);
	}
#endif
}


#if defined(BT_HARDWARE_PXA270) || defined(BT_HARDWARE_DM365)

bool buzzer_enabled = false;

void setBeep(bool enable)
{
#ifdef BT_HARDWARE_PXA270
	// enter the BEEP_ON state to enable the buzzer
	if (enable)
		bt_global::audio_states->toState(AudioStates::BEEP_ON);
	// exit the BEEP_ON state to disable the buzzer
	else if (!enable && buzzer_enabled)
		bt_global::audio_states->removeState(AudioStates::BEEP_ON);
#endif
	buzzer_enabled = enable;
}

bool getBeep()
{
	return buzzer_enabled;
}

void beep(int t)
{
#ifdef BT_HARDWARE_PXA270
	if (bt_global::audio_states->currentState() != AudioStates::BEEP_ON)
		return;
#endif
	if (buzzer_enabled && QFile::exists(SOUND_PATH "beep.wav"))
		bt_global::sound->play(SOUND_PATH "beep.wav");
}

#else // BT_HARDWARE_PXA255

void setBeep(bool enable)
{
	const char *p = enable ? "1" : "0";
	if (QFile::exists("/proc/sys/dev/btweb/buzzer_enable"))
	{
		FILE* fd = fopen("/proc/sys/dev/btweb/buzzer_enable", "w");
		if (fd != NULL)
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
		if (fd != NULL)
		{
			fread (&c, 1, 1, fd);
			fclose(fd);
			return c != '0';
		}
	}
	return false;
}

void beep(int t)
{
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
}

#endif


void beep()
{
	beep(50);
}

bool getBacklight()
{
	char c[4];
	char name[50];
	int fd;

	getName(name);
	if (!strncmp(name, "DINGO", strlen("DINGO")))
	{
		if (QFile::exists("/sys/class/backlight/dingo_backlight/bl_power"))
		{
			fd = open("/sys/class/backlight/dingo_backlight/bl_power", O_RDONLY);
			if (fd >= 0)
			{
				read (fd, c, 1);
				close(fd);
				if (c[0] == '0')
					return true;
			}
		}
	}
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

#if defined(BT_HARDWARE_X11) || defined(BT_HARDWARE_PXA270) || defined(BT_HARDWARE_DM365)

static QDateTime last_press = QDateTime::currentDateTime();

void setTimePress(const QDateTime &press)
{
	last_press = press;
}

unsigned long getTimePress()
{
	return last_press.secsTo(QDateTime::currentDateTime());
}

#else // BT_HARDWARE_PXA255

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
#ifdef BT_HARDWARE_DM365
	const char *FILE_NAME = "/sys/class/hwmon/hwmon0/device/in2_input";
#else
	const char *FILE_NAME = "/proc/sys/dev/btweb/name";
#endif

	memset(name, '\0', sizeof(name));
	if (QFile::exists(FILE_NAME))
	{
		int fd = open(FILE_NAME, O_RDONLY);
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

void getAlarmVolumes(int index, int *alarm_volumes, uchar *source, uchar *station)
{
	qDebug() << "Reading alarm volume from e2 for index" << index;

	memset(alarm_volumes, 0, sizeof(int) * AMPLI_NUM);
	*source = *station = 0;

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
			alarm_volumes[idx] = -1;
		}
	}
	else
	{
		// read the alarm data from E2
		lseek(eeprom, alarmOffset(index) + KEY_LENGTH, SEEK_SET);

		for (unsigned int idx = 0; idx < AMPLI_NUM; idx++)
		{
			read(eeprom, &alarm_volumes[idx], 1);

			if (alarm_volumes[idx] == 0xff)
				alarm_volumes[idx] = -1;
			else
				alarm_volumes[idx] &= 0x1F;
		}

		read(eeprom, source, 1);
		read(eeprom, station, 1);
	}

	close(eeprom);
}

void setAlarmVolumes(int index, int *alarm_volumes, uchar source, uchar station)
{
	int eeprom = open(DEV_E2, O_RDWR | O_SYNC, 0666);

	qDebug() << "Writing alarm volume to e2 for index" << index;

	if (eeprom == -1)
		return;

	lseek(eeprom, alarmOffset(index) + KEY_LENGTH, SEEK_SET);

	for (unsigned int idx = 0; idx < AMPLI_NUM; idx++)
		write(eeprom, &alarm_volumes[idx], 1);

	write(eeprom, &source, 1);
	write(eeprom, &station, 1);
	close(eeprom);
}

void setVctVideoValue(const QString &command, const QString &value)
{
#ifdef BT_HARDWARE_PXA270
	QProcess::startDetached("/home/bticino/bin/set_videocom",
		QStringList() << command << value);
#endif
}

void initMultimedia()
{
#ifdef BT_HARDWARE_PXA270
	smartExecute("/bin/init_audio_system");
	smartExecute("/bin/init_video_system");
#endif
}

void usbHotplug()
{
#ifdef BT_HARDWARE_PXA270
	system ("/sbin/hotplug add 3");
#endif
}

QPair <QString, QStringList> getAudioCmdLine(const QString &audio_path)
{
#ifdef BT_HARDWARE_DM365
	return qMakePair(QString("aplay"), QStringList() << audio_path << "-D" << "hw:0");
#else
	return qMakePair(QString("/bin/sox"), QStringList() << "-w" << "-c" << "2"
		<< "-s" << "-t" << "wav" << audio_path << "-t" << "ossdsp" << "/dev/dsp1");
#endif
}

void dumpSystemMemory()
{
#ifdef BT_HARDWARE_PXA270
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

int getMemFree()
{
#ifndef BT_HARDWARE_PXA270
	return -1;
#endif

	QFile f("/proc/meminfo");
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
		return -1;

	QHash<QString, int> data;
	QByteArray line = f.readLine();
	while (!line.isEmpty())
	{
		line.chop(3); // erase the ' kB' part of the string
		QList<QByteArray> l = line.trimmed().split(':');
		data[l[0].trimmed()] = l[1].trimmed().toInt();

		line = f.readLine();
	}

	return data["MemFree"] + data["Cached"];
}

void createFlagFile(QString filename)
{
	// We don't care about UTF8, the flag files are used for internal purpose.
	int fd = open(qPrintable(filename), O_CREAT, 0666);
	if (fd >= 0)
		close(fd);
}

void activateLocalSource()
{
#ifdef BT_HARDWARE_DM365
	system("echo 1 > /sys/class/gpio/gpio35/value");
#elif defined (BT_HARDWARE_PXA255)
	system("/bin/audio_on.tcl");
#else // BT_HARDWARE_PXA270
	smartExecute("/bin/rca2_on");
#endif
}

void deactivateLocalSource()
{
#ifdef BT_HARDWARE_DM365
	system("echo 0 > /sys/class/gpio/gpio35/value");
#elif defined (BT_HARDWARE_PXA255)
	system("/bin/audio_off.tcl");
#else // BT_HARDWARE_PXA270
	smartExecute("/bin/rca2_off");
#endif
}


