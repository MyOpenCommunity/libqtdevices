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


#ifndef XML_FUNCTIONS_H
#define XML_FUNCTIONS_H

#include <QDomNode>
#include <QRegExp>
#include <QString>


QDomNode getChildWithName(const QDomNode &parent, const QString &name);

QDomElement getElement(const QDomNode &root, const QString &path);

QDomNode getChildWithId(const QDomNode &parent, const QRegExp &node_regexp, int id);

QDomNode getChildWithId(const QDomNode &parent, const QRegExp &node_regexp, const QString &idName, int id);

QString getTextChild(const QDomNode &parent, const QString &name);

QList<QDomNode> getChildren(const QDomNode &parent, const QString &name);

QDomNode findXmlNode(const QDomNode &root, const QRegExp &node_regexp, const QString &tag_name, int id, int& serial_number);

inline QDomNode findXmlNode(const QDomNode &root, const QRegExp &node_regexp, const QString &tag_name, int id)
{
	int serial_number = 1;
	return findXmlNode(root, node_regexp, tag_name, id, serial_number);
}

// Some convenient overloads for the above function
inline QDomNode findXmlNode(const QDomNode &root, const QRegExp &node_regexp, int id, int& serial_number)
{
	return findXmlNode(root, node_regexp, "id", id, serial_number);
}

inline QDomNode findXmlNode(const QDomNode &root, const QRegExp &node_regexp, int id)
{
	int serial_number = 1;
	return findXmlNode(root, node_regexp, "id", id, serial_number);
}

#endif
