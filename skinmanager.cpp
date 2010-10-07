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


#include "skinmanager.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "main.h" // IMG_PATH

#include <QDomNode>
#include <QDebug>
#include <QFile>


/*!
	\class SkinManager

	\brief A global class to load and get stylesheet and images path defined in a skin
	xml file.

	To get images path, you often should set (adding or removing cid) the
	\em explicit context using addToContext() / removeFromContext() or the helper
	class SkinContext before getting them. An \em implicit context, which can be
	used to retrieve common images, is always set.
	You can also check if a tagname is defined using the exists() method, and
	save/restore the current context using getCidState() and setCidState().

	For the stylesheet the method getStyle() is provided to get the string that
	contains the global style to apply.

	This class is designed to use as a global object, through the bt_global
	namespace.
*/

/*!
	\typedef SkinManager::CidState

	Synonym for QList<int>
*/


/*!
	\brief %Loads the stylesheet and tagnames from the skin configuration file.
*/
SkinManager::SkinManager(QString filename)
{
	if (QFile::exists(filename))
	{
		QFile file(filename);
		QDomDocument qdom;
		if (qdom.setContent(&file))
		{
			QDomNode root = qdom.documentElement();
			style = getTextChild(root, "css");
			foreach (const QDomNode &item, getChildren(root, "item"))
			{
				int cid = getTextChild(item, "cid").toInt();
				images[cid] = QHash<QString, QString>();
				foreach (const QDomNode &img, getChildren(item, "img_"))
				{
					QDomElement el = img.toElement();
					images[cid][el.tagName().mid(4)] = el.text();
				}
			}
			QDomNode common = getChildWithName(root, "common");
			foreach (const QDomNode &img, getChildren(common, "img_"))
			{
				QDomElement el = img.toElement();
				images[-1][el.tagName().mid(4)] = el.text(); // we use -1 as special case
			}
		}
		else
			qWarning("SkinManager: the skin file called %s is not readable or a valid xml file",
				qPrintable(filename));
	}
	else
		qWarning("SkinManager: no skin file called %s", qPrintable(filename));
}

/*!
	Returns the stylesheet reads from the skin xml file.
*/
QString SkinManager::getStyle()
{
	return style;
}

/*!
	\brief Add a cid to the skin context.

	Most of the time, you have no need to use this function directly, because
	you can use the wrapper SkinContext.
*/
void SkinManager::addToContext(int cid)
{
	Q_ASSERT_X(cid >=0, "SkinManager::addToContext", "Invalid cid context");
	cid_lookup_list.append(cid);
}

/*!
	\brief Remove a cid to the skin context.

	\sa addToContext
*/
void SkinManager::removeFromContext()
{
	cid_lookup_list.removeLast();
}

/*!
	\brief Check if an explicit context is set.
*/
bool SkinManager::hasContext()
{
	return !cid_lookup_list.isEmpty();
}

/*!
	\brief Check if a \a name is defined in the skin xml file.
*/
bool SkinManager::exists(QString name)
{
	for (int i = cid_lookup_list.size() - 1; i >= 0; --i)
	{
		int cid = cid_lookup_list[i];
		if (images[cid].contains(name))
			return true;
	}

	if (images[-1].contains(name))
		return true;

	return false;
}

/*!
	\brief Returns the full image path.

	\a name is the tagname used in the code and also specified in the skin xml
	file withouth the prefix 'img_'. If \a name is not found an empty string is
	returned.
*/
QString SkinManager::getImage(QString name)
{
	for (int i = cid_lookup_list.size() - 1; i >= 0; --i)
	{
		int cid = cid_lookup_list[i];
		if (images[cid].contains(name))
			return IMG_PATH + images[cid][name];
	}

	if (images[-1].contains(name))
		return IMG_PATH + images[-1][name];

	qWarning() << "SkinManager: No image" << name << "found, current context: " << cid_lookup_list;
	return QString("");
}

/*!
	\brief Return the whole skin context.
*/
SkinManager::CidState SkinManager::getCidState()
{
	return cid_lookup_list;
}

/*!
	\brief Set the whole skin context.
*/
void SkinManager::setCidState(const CidState &state)
{
	cid_lookup_list = state;
}


/*!
	\class SkinContext

	\brief An helper class for SkinManager.

	A simply wrapper around the SkinManager::addToContext() and
	SkinManager::removeFromContext(), useful to avoid the explicit remove calls
	from the context.
*/


/*!
	\brief Create the object and add the \x cid to the context
*/
SkinContext::SkinContext(int cid)
{
	Q_ASSERT_X(bt_global::skin, "SkinContext::SkinContext", "SkinManager not yet built!");
	bt_global::skin->addToContext(cid);
}

/*!
	\brief Destroy the object and remove the previously added cid from the context.
*/
SkinContext::~SkinContext()
{
	bt_global::skin->removeFromContext();
}


// The global definition of skin manager pointer
SkinManager *bt_global::skin = 0;

