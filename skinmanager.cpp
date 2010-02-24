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

QString SkinManager::getStyle()
{
	return style;
}

void SkinManager::addToContext(int cid)
{
	Q_ASSERT_X(cid >=0, "SkinManager::addToContext", "Invalid cid context");
	cid_lookup_list.append(cid);
}

void SkinManager::removeFromContext()
{
	cid_lookup_list.removeLast();
}

bool SkinManager::hasContext()
{
	return !cid_lookup_list.isEmpty();
}

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

	qWarning("SkinManager: No image %s found", qPrintable(name));
	return QString("");
}

SkinManager::CidState SkinManager::getCidState()
{
	return cid_lookup_list;
}

void SkinManager::setCidState(const CidState &state)
{
	cid_lookup_list = state;
}


SkinContext::SkinContext(int cid)
{
	Q_ASSERT_X(bt_global::skin, "SkinContext::SkinContext", "SkinManager not yet built!");
	bt_global::skin->addToContext(cid);
}

SkinContext::~SkinContext()
{
	bt_global::skin->removeFromContext();
}


// The global definition of skin manager pointer
SkinManager *bt_global::skin = 0;
