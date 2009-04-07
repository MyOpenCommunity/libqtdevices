#include "generic_functions.h"
#include "xml_functions.h"

#include <QMapIterator>
#include <QTextStream>
#include <QDateTime>
#include <QDomElement>
#include <QDomNode>
#include <QWidget>
#include <QPixmap>
#include <QRegExp>
#include <QDebug>
#include <QDir>

#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

QString createMsgOpen(QString who, QString what, QString where)
{
	return '*' + who + '*' + what + '*' + where + "##";
}

QString createRequestOpen(QString who, QString what, QString where)
{
	return QString("*#%1*%2*%3##").arg(who).arg(where).arg(what);
}

QString createWriteRequestOpen(QString who, QString what, QString where)
{
	return QString("*#%1*%2*#%3##").arg(who).arg(where).arg(what);
}

QString createStatusRequestOpen(QString who, QString where)
{
	return QString("*#%1*%2##").arg(who).arg(where);
}

// TODO: unificare queste tre funzioni praticamente identiche!
QString getPressName(QString name)
{
	int pos = name.indexOf(".");
	if (pos != -1)
		return name.left(pos) + "p" + name.mid(pos);

	return QString();
}

QString getZoneName(QString name, QString zone)
{
	int pos = name.indexOf(".");
	if (pos != -1)
		return name.left(pos) + zone.at(1) + name.mid(pos);

	return QString();
}

QString getAmbName(QString name, QString amb)
{
	int pos = name.indexOf(".");
	if (pos != -1)
		return name.left(pos) + amb.at(0) + name.mid(pos);

	return QString();
}

/**
 * Changes a value in conf.xml file atomically.
 * It works on a temporary file and then moves that file on conf.xml with a call to ::rename().
 */
bool setCfgValue(QMap<QString, QString> data, int item_id, int serial_number, const QString &filename)
{
	QFile config_file(filename);
	if (!config_file.open(QIODevice::ReadOnly))
		return false;

	const QString tmp_filename = "cfg/appoggio.xml";
	if (QFile::exists(tmp_filename))
		QFile::remove(tmp_filename);

	QFile tmp_file(tmp_filename);
	if (!tmp_file.open(QIODevice::WriteOnly))
		return false;

	QDomDocument doc("config_document");
	if (!doc.setContent(&config_file))
	{
		config_file.close();
		return false;
	}
	config_file.close();

	QDomNode n = findXmlNode(doc, QRegExp(".*"), item_id, serial_number);
	assert(!n.isNull() && qPrintable(QString("No object found with id %1").arg(item_id)));

	QMapIterator<QString, QString> it(data);
	while (it.hasNext())
	{
		it.next();
		QDomElement el = getElement(n, it.key());
		assert(!el.isNull() && qPrintable(QString("No element found: %1").arg(it.key())));
		// To replace the text of the element
		el.replaceChild(doc.createTextNode(it.value()), el.firstChild());
	}

	// Use a text stream to handle unicode properly
	QTextStream tmp_stream(&tmp_file);
	tmp_stream << doc.toString(0);
	tmp_stream.flush();
	tmp_file.close();

	// QDir::rename fails if destination file exists so we use rename system call
	if (!::rename(qPrintable(tmp_filename), qPrintable(filename)))
		return true;

	// Write an empty file to warn other process that the configuration file has changed.
	int fd = open(FILE_CHANGE_CONF, O_CREAT, 0666);
	if (fd >= 0)
		close(fd);

	return false;
}

bool setCfgValue(QString field, QString value, int item_id, int num_item, const QString &filename)
{
	QMap<QString, QString> m;
	m[field] = value;
	return setCfgValue(m, item_id, num_item, filename);
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
	if (! QFile::exists(FILE_WDT))
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

