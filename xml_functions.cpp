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


#include "xml_functions.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QDebug>

#include <sys/file.h>
#include <errno.h>
#include <unistd.h>

#include <lock_file.h>

#define LOCK_RETRY 100
#define LOCK_TIMEOUT 22000 /* micro seconds */

bool saveXml(const QDomDocument &document, const QString &filename)
{
	QString real_path = filename;
#if defined(BT_HARDWARE_X11)
	// write to symlink destination
	while (QFileInfo(real_path).isSymLink())
	{
		Q_ASSERT_X(QFileInfo(real_path).exists(), "saveXml", "Dangling symlink");
		real_path = QFileInfo(real_path).symLinkTarget();
	}
#endif

	QFile tmp_file(QFileInfo(real_path).dir().absoluteFilePath("appoggio.xml"));

	if (!tmp_file.open(QIODevice::WriteOnly))
		return false;

	int fd = tmp_file.handle();
	qDebug() << "Xml_Functions === MY file descriptor: " << fd;

	Lock_File lock_tmpFile(fd);
	int lock = lock_tmpFile.lockFile();
	int count = 0;
	while((lock < 0) && (count < LOCK_RETRY))
	{
		lock = lock_tmpFile.lockFile();
		qDebug() << "Xml_Functions === il file " << tmp_file.fileName() << " is locked, error: " << strerror(errno);
		count++;
		usleep(LOCK_TIMEOUT);
	}
	if (lock == 0)
	{
		qDebug() << "Xml_Functions === il file " << tmp_file.fileName() << " is free";
		int err = ftruncate(fd, 0);
		if (err < 0)
		qDebug() << "Xml_Functions === Error file closed: " << strerror(errno);
	}
	else
	{
		qDebug() << "Xml_Functions === il file " << tmp_file.fileName() << " is locked, error: " << strerror(errno);
		tmp_file.close();
		return false;
	}

	QTextStream stream(&tmp_file);
	stream.setCodec("UTF-8");
	stream << document.toString(4);
	stream.flush();
	lock_tmpFile.unlockFile();
	tmp_file.close();

	QFile xmlFile(real_path);
	if (!xmlFile.open(QIODevice::ReadOnly))
	{
		if (!xmlFile.open(QIODevice::ReadWrite))
			return false;
	}
	int realFD = xmlFile.handle();
	qDebug() << "Xml_Functions === MY file descriptor: " << fd;

	Lock_File lock_realFile(realFD);
	lock = lock_realFile.lockFile();
	count = 0;
	while((lock < 0) && (count < LOCK_RETRY))
	{
		lock = lock_realFile.lockFile();
		qDebug() << "Xml_Functions === il file " << xmlFile.fileName() << " is locked, error: " << strerror(errno);
		count++;
		usleep(LOCK_TIMEOUT);
	}
	if (lock == 0)
	{
		qDebug() << "Xml_Functions === il file " << xmlFile.fileName() << " is free";
	}
	else
	{
		qDebug() << "Xml_Functions === il file " << xmlFile.fileName() << " is locked, error: " << strerror(errno);
		return false;
	}


	bool rtn = ::rename(qPrintable(tmp_file.fileName()), qPrintable(real_path)) == 0;
	if (!rtn)
		perror("");

	lock_realFile.lockFile();
	xmlFile.close();
	return rtn;
}

void setAttribute(QDomNode &n, const QString &attr, const QString &value)
{
	QDomAttr a = n.toElement().attributeNode(attr);
	if (a.isNull())
	{
		a = n.ownerDocument().createAttribute(attr);
		n.toElement().setAttributeNode(a);
	}

	a.setValue(value);
}

QString getAttribute(const QDomNode &n, const QString &attr, const QString &def)
{
	QDomNode attribute = n.attributes().namedItem(attr);
	if (attribute.isNull())
		return def;

	if (!attribute.isAttr())
	{
		qWarning() << "Attribute" << attr << "is not a QDomAttr";
		return def;
	}

	return attribute.toAttr().value();
}

int getIntAttribute(const QDomNode &n, const QString &attr, int def)
{
	QString a = getAttribute(n, attr);
	bool ok;
	int val = a.toInt(&ok);
	if (!ok)
	{
		if (def == -1)
			qWarning() << "Error converting attribute" << attr << "of node" << n.nodeName() <<
				"at line" << n.lineNumber() << "to int";
		return def;
	}
	return val;
}

double getDoubleAttribute(const QDomNode &n, const QString &attr, double def)
{
	QString a = getAttribute(n, attr);
	bool ok;
	double val = a.replace(",", ".").toDouble(&ok);
	if (!ok)
	{
		if (def == -1)
			qWarning() << "Error converting attribute" << attr << "of node" << n.nodeName() <<
				"at line" << n.lineNumber() << "to double";
		return def;
	}
	return val;
}

QTime getTimeAttribute(const QDomNode &n, const QString &attr, QTime def)
{
	QString a = getAttribute(n, attr);
	if (a.isEmpty())
		return def;
	QStringList hms = a.split(":");
	int hours, minutes, seconds;
	bool ok;
	if (hms.length() == 0)
	{
		qWarning() << "Attribute" << attr << "not in right format";
		hours = 0;
	}
	else
		hours = hms[0].toInt(&ok);
	if (hms.length() <= 1)
	{
		qWarning() << "Attribute" << attr << "not in right format, minutes missing";
		minutes = 0;
	}
    else
		minutes = hms[1].toInt(&ok);
	if (hms.length() <= 2)
	{
		qWarning() << "Attribute" << attr << "not in right format, seconds missing";
		seconds = 0;
	}
	else
		seconds = hms[2].toInt(&ok);
	return QTime(hours, minutes, seconds);
}

QDomElement getElement(const QDomNode &root, const QString &path)
{
	QStringList sl = path.split('/');
	QDomNode node = root;
	while (!sl.isEmpty())
	{
		QString str = sl.first();
		if (!node.isNull())
			node = node.namedItem(str);
		sl.pop_front();
	}
	return node.toElement();
}

QDomNode getChildWithId(const QDomNode &parent, const QRegExp &node_regexp, int id)
{
	return getChildWithId(parent, node_regexp, "id", id);
}

QDomNode getChildWithId(const QDomNode &parent, const QRegExp &node_regexp, const QString &id_name, int id)
{
	QDomNode n = parent.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && node_regexp.exactMatch(n.nodeName()))
		{
			QDomNode child = n.firstChild();
			while (!child.isNull() && child.nodeName() != id_name)
				child = child.nextSibling();

			if (!child.isNull() && child.toElement().text().toInt() == id)
				return n;
		}
		n = n.nextSibling();
	}
	return QDomNode();
}

QDomNode findXmlNode(const QDomNode &root, const QRegExp &node_regexp, const QString &tag_name, int value, int& serial_number)
{
	QDomNode n = root.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName().contains(node_regexp))
		{
			QDomNode child = n.firstChild();
			while (!child.isNull() && child.nodeName() != tag_name)
				child = child.nextSibling();

			if (!child.isNull() && child.toElement().text().toInt() == value)
				if (!--serial_number)
					return n;
		}
		if (n.hasChildNodes())
		{
			QDomNode res = findXmlNode(n, node_regexp, tag_name, value, serial_number);
			if (!res.isNull())
				return res;
		}
		n = n.nextSibling();
	}
	return QDomNode();
}

QList<QDomNode> getChildren(const QDomNode &parent, const QString &name)
{
	QList<QDomNode> l;
	QDomNode n = parent.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName().startsWith(name))
			l.append(n);

		n = n.nextSibling();
	}
	
	return l;
}

QList<QDomNode> getChildrenExact(const QDomNode &parent, const QString &name)
{
	QList<QDomNode> l;
	QDomNode n = parent.firstChild();

	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName() == name)
			l.append(n);

		n = n.nextSibling();
	}

	return l;
}

QString getTextChild(const QDomNode &parent, const QString &name)
{
	QDomNode n = parent.namedItem(name);
	if (n.isNull())
		return QString();
	return n.toElement().text();
}

void setTextChild(QDomNode &parent, const QString &name, const QString &value)
{
	QDomNode n = parent.namedItem(name);
	if (n.isNull())
	{
		n = parent.ownerDocument().createElement(name);
		parent.appendChild(n);
	}

	QDomNode text = parent.ownerDocument().createTextNode(value);
	QDomNodeList childs = n.childNodes();

	for (int i = 0; i < childs.count(); ++i)
		n.removeChild(childs.at(i));

	n.appendChild(text);
}
