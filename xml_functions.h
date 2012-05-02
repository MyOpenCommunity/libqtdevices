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

/*!
	Return the value of the attribute \a attr in node \a n.

	If \a attr is not present or invalid, \a def is returned instead.
*/
QString getAttribute(const QDomNode &n, const QString &attr, const QString &def=QString());

/*!
	Return the int value of the attribute \a attr.
	A warning is printed if the attribute is not an integer number.
*/
int getIntAttribute(const QDomNode &n, const QString &attr);

/*!
	\ingroup Core
	\brief Looks for a child node with the given name

	Returns the node child of \a parent with name \a name.
	If the node is not found returns an null node.
*/
inline QDomNode getChildWithName(const QDomNode &parent, const QString &name)
{
	return parent.namedItem(name);
}

/*!
	\ingroup Core
	\brief Looks for a child element by path.

	Returns the element located in \a path starting from \a root.
	If the element is not found returns an invalid element.

	\note Path must omit the root node, have "/" as node separator and not have
	a trailing separator.
	(Example: "setup/generale/clock/dateformat")
*/
QDomElement getElement(const QDomNode &root, const QString &path);

/*!
	\ingroup Core
	\brief Looks for a child node by a regexp and \a id child node.

	Returns the node child of \a parent that match the \a regexp and has a child
	with \a id.
	If the node is not found returns an invalid node.
*/
QDomNode getChildWithId(const QDomNode &parent, const QRegExp &node_regexp, int id);

/*!
	\ingroup Core
	\brief Looks for a child node by a regexp and a child node.

	Returns the node child of \a parent that match the \a regexp and has a child
	with name \a idName and value \a id.
	If the node is not found returns an invalid node.
*/
QDomNode getChildWithId(const QDomNode &parent, const QRegExp &node_regexp, const QString &idName, int id);

/*!
	\ingroup Core
	\brief Looks for a text node child of \a root.

	This function is a wrapper around getChildWithName().
	Returns the text of child if present, a null QString otherwise.
*/
QString getTextChild(const QDomNode &parent, const QString &name);

/*!
	\ingroup Core
	\brief Return a list of children of node \a parent whose name start with \a name.
*/
QList<QDomNode> getChildren(const QDomNode &parent, const QString &name);

/*!
	\ingroup Core
	\brief Return a list of children of node \a parent whose name is \a name.
*/
QList<QDomNode> getChildrenExact(const QDomNode &parent, const QString &name);

/*!
	\ingroup Core
	\brief Looks for a node by parent and child.

	Returns a node that is descendent of the node \a root and have a child
	\a tag_name with a \a value.
	If the node is not found returns an invalid node.
*/
QDomNode findXmlNode(const QDomNode &root, const QRegExp &node_regexp, const QString &tag_name, int id, int& serial_number);

/*!
	\ingroup Core
	\brief Looks for a node by parent and child.

	This is an overloaded member function, provided for convenience.
*/
inline QDomNode findXmlNode(const QDomNode &root, const QRegExp &node_regexp, const QString &tag_name, int id)
{
	int serial_number = 1;
	return findXmlNode(root, node_regexp, tag_name, id, serial_number);
}

/*!
	\ingroup Core
	\brief Looks for a node by parent and child.

	This is an overloaded member function, provided for convenience.
*/
inline QDomNode findXmlNode(const QDomNode &root, const QRegExp &node_regexp, int id, int& serial_number)
{
	return findXmlNode(root, node_regexp, "id", id, serial_number);
}

/*!
	\ingroup Core
	\brief Looks for a node by parent and child.

	This is an overloaded member function, provided for convenience.
*/
inline QDomNode findXmlNode(const QDomNode &root, const QRegExp &node_regexp, int id)
{
	int serial_number = 1;
	return findXmlNode(root, node_regexp, "id", id, serial_number);
}

#endif
