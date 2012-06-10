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
#include <QStringList>
#include <QDebug>


bool saveXml(const QDomDocument &document, const QString &filename)
{
	// TODO: write in a temporary file and move it at the end replacing the
	// old configuration file.
	QFile fh(filename);
	if (!fh.open(QIODevice::WriteOnly))
		return false;

	QTextStream stream(&fh);
	stream.setCodec("UTF-8");
	stream << document.toString(4);
	stream.flush();
	fh.close();
	return true;
}

bool setAttribute(QDomNode &n, const QString &attr, const QString &value)
{
	QDomAttr a = n.toElement().attributeNode(attr);
	if (a.isNull())
		return false;

	a.setValue(value);
	return true;
}

QString getAttribute(const QDomNode &n, const QString &attr, const QString &def)
{
	QDomNode attribute = n.attributes().namedItem(attr);
	if (attribute.isNull())
	{
		if (def.isEmpty())
			qWarning() << "Attribute" << attr << "not present for node" << n.nodeName();
		else
			qWarning() << "Using default value" << def << "for node" << n.nodeName();
		return def;
	}

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
			qWarning() << "Error converting attribute" << attr << "of node" << n.nodeName() << "to int";
		else
			qWarning() << "Using default value" << def << "for node" << n.nodeName();
		return def;
	}
	return val;
}

QTime getTimeAttribute(const QDomNode &n, const QString &attr, QTime def)
{
	QString a = getAttribute(n, attr);
	if (a.isEmpty())
	{
		// def.toString() is required when compiling with -DQT_NO_DEBUG_OUTPUT
		qWarning() << "Using default value" << def.toString() << "for node" << n.nodeName();
		return def;
	}
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

