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
QDomNode getChildWithName(QDomNode parent, QString name);

/**
 * Finds a node in a DOM tree with the path given as a parameter.
 * \param root the node root of the tree.
 * \param path The path in the xml tree that leads to the node. The path must omit the root node
 * (example: node "configuratore" in conf.xml) and must separate the nodes with a '/'. The last node
 * must not be followed by a '/'.
 * Example: "setup/generale/clock/dateformat"
 * \return A Null node if the path is not found, a valid node otherwise.
 */
QDomElement getElement(QDomNode root, QString path);

/// Return the node child of 'parent' that match the 'regexp' and have an attribute with id 'id'.
QDomNode getChildWithId(QDomNode parent, const QRegExp &node_regexp, int id);

#endif
