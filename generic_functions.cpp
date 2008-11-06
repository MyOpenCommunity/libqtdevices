/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**genericFunz.cpp
**
**funzioni generiche usate qua e là
**
****************************************************************/

#include "generic_functions.h"
#include "openclient.h"
#include "main.h"
#include "btmain.h"

#include <QDir>
#include <QWidget>
#include <QPixmap>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

#include <fcntl.h>
#include <unistd.h>

#define CONFILENAME	"cfg/conf.xml"


QString getPressName(QString name)
{
	int pos = name.indexOf(".");
	if (pos != -1)
		return name.left(pos) + "p" + name.mid(pos);

	return QString();
}

void getZoneName(char* name, char* pressName,char*zona, char len)
{
	memset(pressName,'\000',len);
	if (strstr(name,"."))
	{
		strncpy(pressName,name,strstr(name,".")-name);
		strncat(pressName,&zona[1],1);
		strcat(pressName,strstr(name,"."));
	}
}

QString getAmbName(QString name, QString amb)
{
	// TODO:  verificare che faccia la stessa cosa della versione c!
	int pos = name.indexOf(".");
	if (pos != -1)
		return name.left(pos) + amb.at(0) + name.mid(pos);

	return QString();
}

void getAmbName(char *name, char *out, char *amb, char len)
{
	memset(out,'\000',len);
	if (strstr(name,"."))
	{
		strncpy(out,name,strstr(name,".")-name);
		strncat(out, amb, 1);
		strcat(out,strstr(name,"."));
	}
}

bool setCfgValue(QString file, int id, QString campo, QString valore, int serNumId)
{
	int count;
	QString tmp_file = "cfg/appoggio.xml";

	comChConf();
	count = 1;
	if (QFile::exists(tmp_file))
		QFile::remove(tmp_file);

	QFile *fil1 = new QFile(file);
	QFile *fil2 = new QFile(tmp_file);
	if (!fil1->open(QIODevice::WriteOnly | QIODevice::ReadOnly))
		return false;

	if (!fil2->open(QIODevice::WriteOnly))
	{
		fil1->close();
		return false;
	}

	QTextStream t1(fil1);
	QTextStream t2(fil2);

	QString app1 = QString("<id>%1</id>").arg(id);

	while (true)
	{
		QString Line = t1.readLine().append('\n');
		if (Line == "\n")
			break;

		t2 << Line;

		if (Line.contains(app1))
		{
			if  (count == serNumId)
			{
				QString app2 = QString("<%1>").arg(campo);
				while (true)
				{
					Line = t1.readLine().append('\n');
					if (Line == "\n")
						break;

					if (!Line.contains(app2))
						t2 << Line;
					else
						break;
				}

				Line = QString("<%1>%2</%3>\n").arg(campo).arg(valore).arg(campo);
				t2 << Line;
				while (true)
				{
					Line = t1.readLine().append('\n');
					if (Line == "\n")
						break;
					t2 << Line;
				}
				t2.flush();
				fil2->flush();
				fil1->close();
				fil2->close();
				QDir::current().remove(file);
				QDir::current().rename(tmp_file, file);
				return true;
			}
			else
				count++;
		}
	}

	fil1->close();
	fil2->close();
	return false;
}

bool setCfgValue(int id, const char* campo, const char* valore)
{
	return setCfgValue(id, campo, valore,1);
}

bool setCfgValue(int id, const char* campo, const char* valore,int serNumId)
{
	return setCfgValue(CONFILENAME, id, campo, valore, serNumId);
}

bool copyFile(char* orig, char* dest)
{
	QFile *filIN = new QFile(orig);
	if (!filIN->open(QIODevice::ReadOnly))
		return false;

	if (QFile::exists(dest))
		QFile::remove(dest);

	QFile *filOUT = new QFile(dest);

	if (!filOUT->open(QIODevice::WriteOnly))
		return false;

	QDataStream tIN(filIN);
	QDataStream tOUT(filOUT);
	unsigned char i;
	do
	{
		tIN >> i;
		tOUT << i;
	}
	while(!tIN.atEnd());

	filIN->close();
	filOUT->close();
	return true;
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
		setCfgValue(CONTRASTO, "value",contr);
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
		setCfgValue(SUONO, "value", p);
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


void setOrientation(char* o)
{
	if (QFile::exists("/proc/sys/dev/btweb/upsidedown"))
	{
		int fd = open("/proc/sys/dev/btweb/upsidedown", O_WRONLY);
		if (fd >= 0)
		{
			write(fd, o, 1);
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
	if (! QFile::exists(FILE_WDT))
	{

		int fd = creat(FILE_WDT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
		if (fd != -1)
		{
			close(fd);
		}
	}
}

void  comChConf()
{
	int fd = open(FILE_CHANGE_CONF, O_CREAT, 0666);
	if (fd >= 0)
	{
		close(fd);
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

extern BtMain *BTouch;

void resetTimer(int signo)
{
	qDebug("resetTimer()");
	BTouch->resetTimer();
}

void grabScreen(void* pWidget)
{
	char filename[1024];
	qDebug("=============== I'm going to grab the screen ===============");

	if (!QDir::current().exists(QString("grabs")))
	{
		qDebug("=============== Create the grabs directory ===============");
		QDir::current().mkdir(QString("grabs"));
	}

	QPixmap grabbedScreen = QPixmap::grabWidget((QWidget*)pWidget, 0, 0, -1, -1);
	sprintf(filename, "%.2d:%.2d:%.2d,%.1d",QTime::currentTime().hour() ,QTime::currentTime().minute(), QTime::currentTime().second(),QTime::currentTime().msec()/100);
	QString fn(filename);
	if (grabbedScreen.save(fn.prepend("grabs/"), "PNG"))
		qDebug("=============== Screen grabbed ===============");
	else
		qDebug("=============== Screen grab error ===============");
}
