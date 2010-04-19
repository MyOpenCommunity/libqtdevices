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

#include <QStringList>


QDomNode getChildWithName(const QDomNode &parent, const QString &name)
{
	QDomNode n = parent.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName() == name)
			return n;

		n = n.nextSibling();
	}
	return QDomNode();
}

QDomElement getElement(const QDomNode &root, const QString &path)
{
	QStringList sl = path.split('/');
	QDomNode node = root;
	while (!sl.isEmpty())
	{
		QString str = sl.first();
		if (!node.isNull())
			node = getChildWithName(node, str);
		sl.pop_front();
	}
	return node.toElement();
}

QDomNode getChildWithId(const QDomNode &parent, const QRegExp &node_regexp, int id)
{
	return getChildWithId(parent, node_regexp, "id", id);
}

QDomNode getChildWithId(const QDomNode &parent, const QRegExp &node_regexp, const QString &idName, int id)
{
	QDomNode n = parent.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName().contains(node_regexp))
		{
			QDomNode child = n.firstChild();
			while (!child.isNull() && child.nodeName() != idName)
				child = child.nextSibling();

			if (!child.isNull() && child.toElement().text().toInt() == id)
				return n;
		}
		n = n.nextSibling();
	}
	return QDomNode();
}

QDomNode findXmlNode(const QDomNode &root, const QRegExp &node_regexp, const QString &id_name, int id, int& serial_number)
{
	QDomNode n = root.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName().contains(node_regexp))
		{
			QDomNode child = n.firstChild();
			while (!child.isNull() && child.nodeName() != id_name)
				child = child.nextSibling();

			if (!child.isNull() && child.toElement().text().toInt() == id)
				if (!--serial_number)
					return n;
		}
		if (n.hasChildNodes())
		{
			QDomNode res = findXmlNode(n, node_regexp, id_name, id, serial_number);
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

QString getTextChild(const QDomNode &parent, const QString &name)
{
	QDomNode n = getChildWithName(parent, name);
	if (n.isNull())
		return QString();
	return n.toElement().text();
}
