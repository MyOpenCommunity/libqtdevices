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


/*!
	\brief Looks for a child node with the given name

	Returns the node child of "parent" with name "name".
	If the node is not found returns an invalid node.
 */
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

/**
	\brief Looks for a child element by path

	Returns the element located in "path" starting from "root".
	If the element is not found returns an invalid element.

	\note Path must omit the root node, have "/" as node separator and not have
	a trailing separator.
	(Example: "setup/generale/clock/dateformat")
 */
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

/*!
	\brief Looks for a child node by a regexp and "id" child node
	\overload QDomNode getChildWithId(const QDomNode &parent, const QRegExp &node_regexp, int id)

	Returns the node child of "parent" that match the "regexp" and has a child
	with id "id".
	If the node is not found returns an invalid node.
 */
QDomNode getChildWithId(const QDomNode &parent, const QRegExp &node_regexp, int id)
{
	return getChildWithId(parent, node_regexp, "id", id);
}

/*!
	\brief Looks for a child node by a regexp and a child node

	Returns the node child of "parent" that match the "regexp" and has a child
	with name "idName" and value "id".
	If the node is not found returns an invalid node.
 */
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

/*!
	\brief Looks for a node by parent and child.

	Returns a node that is descendent of the node "root" and have a child
	"tag_name" with a "value".
	If the node is not found returns an invalid node.
 */
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

/*!
	\brief Return a list of children of node 'parent' whose name start with 'name'.
 */
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

/*!
	\brief Looks for a text node child of "root".

	This function is a wrapper around getChildWithName.
	Returns the text of child if present, a null QString otherwise.
 */
QString getTextChild(const QDomNode &parent, const QString &name)
{
	QDomNode n = getChildWithName(parent, name);
	if (n.isNull())
		return QString();
	return n.toElement().text();
}

/*!
	\overload findXmlNode(const QDomNode &root, const QRegExp &node_regexp, int id, int& serial_number)
 */

/*!
	\overload findXmlNode(const QDomNode &root, const QRegExp &node_regexp, int id)
 */
