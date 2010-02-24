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


#include "generic_functions.h"
#include "xml_functions.h"

#include <QMapIterator>
#include <QTextStream>
#include <QDomElement>
#include <QtGlobal> // Q_ASSERT_X
#include <QDomNode>
#include <QWidget>
#include <QRegExp>
#include <QDebug>
#include <QDir>

#include <fcntl.h>


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


QString getBostikName(const QString &name, const QString &suffix)
{
	int pos = name.indexOf(".");
	if (pos != -1)
		return name.left(pos) + suffix + name.mid(pos);

	return QString();
}

QString getPressName(QString name)
{
	return getBostikName(name, "p");
}

QString getZoneName(QString name, QString zone)
{
	return getBostikName(name, zone.at(1));
}

QString getAmbName(QString name, QString amb)
{
	return getBostikName(name, amb.at(0));
}

/**
 * Changes a value in conf.xml file atomically.
 * It works on a temporary file and then moves that file on conf.xml with a call to ::rename().
 */
bool prepareWriteCfgFile(QDomDocument &doc, const QString &filename)
{
	QFile config_file(filename);
	if (!config_file.open(QIODevice::ReadOnly))
		return false;

	if (!doc.setContent(&config_file))
	{
		config_file.close();
		return false;
	}
	config_file.close();

	return true;
}

bool writeCfgFile(const QDomDocument &doc, const QString &filename)
{
	const QString tmp_filename = "cfg/appoggio.xml";
	if (QFile::exists(tmp_filename))
		QFile::remove(tmp_filename);

	QFile tmp_file(tmp_filename);
	if (!tmp_file.open(QIODevice::WriteOnly))
		return false;

	// Use a text stream to handle unicode properly
	QTextStream tmp_stream(&tmp_file);
	tmp_stream.setCodec("UTF-8");
	QString xml = doc.toString(0);
	// Other processes don't support empty tags on xml, so we replace it with tags
	// with empty content.
	QRegExp empty_tag_reg("<([^>]+)/>");
	empty_tag_reg.setMinimal(true);
	tmp_stream << xml.replace(empty_tag_reg, "<\\1></\\1>");
	tmp_stream.flush();
	tmp_file.close();

	// QDir::rename fails if destination file exists so we use rename system call
	if (!::rename(qPrintable(tmp_file.fileName()), qPrintable(filename)))
	{
		// Write an empty file to warn other process that the configuration file has changed.
		int fd = open(FILE_CHANGE_CONF, O_CREAT, 0666);
		if (fd >= 0)
			close(fd);

		return true;
	}

	return false;
}

/**
 * Changes a value in conf.xml file atomically.
 * It works on a temporary file and then moves that file on conf.xml with a call to ::rename().
 */
#ifdef CONFIG_BTOUCH
bool setCfgValue(QMap<QString, QString> data, int item_id, int serial_number, const QString &filename)
#else
bool setCfgValue(QMap<QString, QString> data, int item_id, const QString &filename)
#endif
{
	if (!bt_global::config.contains(INIT_COMPLETE))
	{
		qDebug() << "Not writing to configuration during init";

		return true;
	}

	QDomDocument doc("config_document");
	if (!prepareWriteCfgFile(doc, filename))
		return false;

#ifdef CONFIG_BTOUCH
	QDomNode n = findXmlNode(doc, QRegExp(".*"), item_id, serial_number);
#else
	QDomElement gui = getElement(doc.documentElement(), "gui");
	QDomNode n;

	foreach (const QDomNode &page, getChildren(gui, "page"))
	{
		n = getChildWithId(page, QRegExp("item"), "itemID", item_id);
		if (!n.isNull())
			break;
	}
#endif
	Q_ASSERT_X(!n.isNull(), "setCfgValue", qPrintable(QString("No object found with id %1").arg(item_id)));

	// TODO maybe refactor & move to xml_functions.cpp/h
	QMapIterator<QString, QString> it(data);
	while (it.hasNext())
	{
		it.next();
		QDomElement el = getElement(n, it.key());
		Q_ASSERT_X(!el.isNull(), "setCfgValue", qPrintable(QString("No element found: %1").arg(it.key())));
		// To replace the text of the element
		el.replaceChild(doc.createTextNode(it.value()), el.firstChild());
	}

	return writeCfgFile(doc, filename);
}

#ifdef CONFIG_BTOUCH

// TODO rewrite setCfgValue using setGlobalCfgValue when removing CONFIG_BTOUCH
bool setGlobalCfgValue(QMap<QString, QString> data, const QString &id_name, int id_value, const QString &filename)
{
	if (!bt_global::config.contains(INIT_COMPLETE))
	{
		qDebug() << "Not writing to configuration during init";

		return true;
	}

	QDomDocument doc("config_document");
	if (!prepareWriteCfgFile(doc, filename))
		return false;

	int serial_number = 1; // dummy
	QDomNode n = findXmlNode(doc, QRegExp(".*"), id_name, id_value, serial_number);
	Q_ASSERT_X(!n.isNull(), "setCfgValue", qPrintable(QString("No object found with id %1").arg(id_value)));

	// TODO maybe refactor & move to xml_functions.cpp/h
	QMapIterator<QString, QString> it(data);
	while (it.hasNext())
	{
		it.next();
		QDomElement el = getElement(n, it.key());
		Q_ASSERT_X(!el.isNull(), "setGlobalCfgValue", qPrintable(QString("No element found: %1").arg(it.key())));
		// To replace the text of the element
		el.replaceChild(doc.createTextNode(it.value()), el.firstChild());
	}

	return writeCfgFile(doc, filename);
}

#endif

#ifdef CONFIG_BTOUCH

bool setCfgValue(QString field, QString value, int item_id, int num_item, const QString &filename)
{
	QMap<QString, QString> m;
	m[field] = value;
	return setCfgValue(m, item_id, num_item, filename);
}

bool setCfgValue(QString field, int value, int item_id, int num_item, const QString &filename)
{
	QMap<QString, QString> m;
	m[field] = QString::number(value);
	return setCfgValue(m, item_id, num_item, filename);
}

#else

bool setCfgValue(QString field, QString value, int item_id, const QString &filename)
{
	QMap<QString, QString> m;
	m[field] = value;
	return setCfgValue(m, item_id, filename);
}

bool setCfgValue(QString field, int value, int item_id, const QString &filename)
{
	QMap<QString, QString> m;
	m[field] = QString::number(value);
	return setCfgValue(m, item_id, filename);
}

#endif

int trasformaVol(int vol)
{
	if (vol < 0)
		return -1;
	if (vol <= 3)
		return 1;
	if (vol <= 7)
		return 2;
	if (vol <= 11)
		return 3;
	if (vol <= 14)
		return 4;
	if (vol <= 17)
		return 5;
	if (vol <= 20)
		return 6;
	if (vol <= 23)
		return 7;
	if (vol <= 27)
		return 8;
	if (vol <= 31)
		return 9;
	return -1;
}
