#include "hardware_functions.h"
#include "generic_functions.h"

#include <QFile>
#include <QScreen>
#include <QtDebug>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h> // sprintf


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

void setContrast(unsigned char c,bool b)
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
	if (b)
	{
		sprintf(contr,"%03d",c);
		setCfgValue("value", contr, CONTRASTO);
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

void setBrightnessLevel(int level)
{
	if (QFile::exists("/proc/sys/dev/btweb/brightness"))
	{
		int fd = open("/proc/sys/dev/btweb/brightness", O_WRONLY);
		if (fd >= 0)
		{
			writeValueToFd(fd, level);
			close(fd);
		}
	}
}

void setBacklightOn(bool b)
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

void setBacklight(bool b)
{
	char name[50];

	getName(name);
	if (!strncmp(name, "H4684_IP", strlen("H4684_IP")))
	{
		if (b)
			setBrightnessLevel(10);
		else
			setBrightnessLevel(210);

		if (b)
			setBacklightOn(true);
	}
	else
		setBacklightOn(b);
}

void setBeep(bool buzzer_enable, bool write_to_conf)
{
	const char *p = buzzer_enable ? "1" : "0";
	if (QFile::exists("/proc/sys/dev/btweb/buzzer_enable"))
	{
		FILE* fd = fopen("/proc/sys/dev/btweb/buzzer_enable", "w");
		if (fd >= 0)
		{
			fwrite(p, 1, 1, fd);
			fclose(fd);
		}
	}

	if (write_to_conf)
	{
		setCfgValue("value", p, SUONO);
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

void beep()
{
	beep(50);
}

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

void getAlarmVolumes(int index, int *volSveglia, uchar *sorgente, uchar *stazione)
{
	int eeprom;
	char chiave[6];

	qDebug() << "Reading alarm volume from nvram for index" << index;

	memset(chiave,'\000',sizeof(chiave));
	eeprom = open("/dev/nvram", O_RDWR | O_SYNC, 0666);

	if (eeprom == -1)
		return;

	lseek(eeprom, BASE_EEPROM+index*(AMPLI_NUM+KEY_LENGTH+SORG_PAR),SEEK_SET);
	read(eeprom, chiave, 5);

	if (strcmp(chiave,AL_KEY))
	{
		lseek(eeprom, BASE_EEPROM+index*(AMPLI_NUM+KEY_LENGTH+SORG_PAR), SEEK_SET);
		write(eeprom,AL_KEY,5);
		for (unsigned int idx = 0; idx < AMPLI_NUM; idx++)
		{
			write(eeprom,"\000",1);
			volSveglia[idx]=-1;
		}
	}
	else
	{
		int ploffete = BASE_EEPROM + index*(AMPLI_NUM+KEY_LENGTH+SORG_PAR) + KEY_LENGTH;
		lseek(eeprom,ploffete, SEEK_SET);
		for (unsigned int idx = 0; idx < AMPLI_NUM; idx++)
		{
			read(eeprom,&volSveglia[idx],1);
			volSveglia[idx]&=0x1F;
		}
		read(eeprom,&sorgente,1);
		read(eeprom,&stazione,1);
	}
	close(eeprom);
}

void setAlarmVolumes(int index, int *volSveglia, uchar sorgente, uchar stazione)
{
	int eeprom;
	eeprom = open("/dev/nvram", O_RDWR | O_SYNC, 0666);

	qDebug() << "Writing alarm volume to nvram for index" << index;

	if (eeprom == -1)
		return;

	lseek(eeprom,BASE_EEPROM + index*(AMPLI_NUM+KEY_LENGTH+SORG_PAR) + KEY_LENGTH, SEEK_SET);
	for (unsigned int idx = 0; idx < AMPLI_NUM; idx++)
		write(eeprom,&volSveglia[idx],1);
	write(eeprom,&sorgente,1);
	write(eeprom,&stazione,1);
	close(eeprom);
}
