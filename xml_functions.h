#ifndef XML_FUNCTIONS_H
#define XML_FUNCTIONS_H

#include <QDomNode>
#include <QRegExp>
#include <QString>

/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This file contains a collection of function designed to extract values from an xml file.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date November 2008
 */

/// Return the node child of 'parent' with name 'name'.
QDomNode getChildWithName(const QDomNode &parent, const QString &name);

/**
 * Finds a node in a DOM tree with the path given as a parameter.
 * \param root the node root of the tree.
 * \param path The path in the xml tree that leads to the node. The path must omit the root node
 * (example: node "configuratore" in conf.xml) and must separate the nodes with a '/'. The last node
 * must not be followed by a '/'.
 * Example: "setup/generale/clock/dateformat"
 * \return A Null node if the path is not found, a valid node otherwise.
 */
QDomElement getElement(const QDomNode &root, const QString &path);

/// Return the node child of 'parent' that match the 'regexp' and have an attribute with id 'id'.
QDomNode getChildWithId(const QDomNode &parent, const QRegExp &node_regexp, int id);

/// Return the node child of 'parent' that match the 'regexp' and has a child with name idName and value=id.
QDomNode getChildWithId(const QDomNode &parent, const QRegExp &node_regexp, const QString &idName, int id);

/// A wrapper function around getChildWithName. Return the text of child if present, a null QString otherwise.
QString getTextChild(const QDomNode &parent, const QString &name);

/// Return a list of children of node 'parent' whose name start with 'name'.
QList<QDomNode> getChildren(const QDomNode &parent, const QString &name);

/**
  * Find a node in the DOM tree that is descendent of the node 'parent'.
  * \param root the node root of the tree
  * \param node_regexp the regular expression that has to be matched with the target node
  * \param id the id of the target node
  * \param the serial number of the target node.
  * \return A Null node if the node is not found, the target node ortherwise.
  */
QDomNode findXmlNode(const QDomNode &root, const QRegExp &node_regexp, int id, int& serial_number);

// A convenient overload for the above function
inline QDomNode findXmlNode(const QDomNode &root, const QRegExp &node_regexp, int id)
{
	int serial_number = 1;
	return findXmlNode(root, node_regexp, id, serial_number);
}

#endif
