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


#include "fontmanager.h"
#include "xml_functions.h" // getChildren, getTextChild

#include <QString>
#include <QDebug>
#include <QFile>
#include <QFontInfo>


namespace
{
	// Return the int value of an item that is actually an unsigned, so the value
	// -1 means that the child is not found.
	inline int getIntChild(const QDomNode &config_node, QString tagname)
	{
		QString text = getTextChild(config_node, tagname);
		if (text.isNull())
			return -1;

		return text.toInt();
	}
}

FontManager::FontManager(QString font_file)
{
	qDebug("Font file: %s", qPrintable(font_file));
	if (QFile::exists(font_file))
	{
		QFile fh(font_file);
		QDomDocument qdom;
		if (qdom.setContent(&fh))
		{
			foreach (const QDomNode &font_node, getChildren(qdom.documentElement(), "font"))
			{
				// Required fields for a font are: type, family, size, weight
				// The field descr is used for debugging and to make the xml
				// conf file more readable.
				int type = getIntChild(font_node, "type");
				QString family = getTextChild(font_node, "family");
				int size = getIntChild(font_node, "size");
				int weight = getIntChild(font_node, "weight");
				if (type == -1 || size == -1 || weight == -1)
				{
					qWarning("Missing some fields for font \"%s\" [%d]", qPrintable(getTextChild(font_node, "descr")), type);
					continue;
				}
				QFont f;
				f.setFamily(family);
				f.setPointSize(size);
				f.setWeight(weight);
#if DEBUG
				qDebug() << qPrintable(QString("Font [requested] -> Family:%1 Point:%2 Pixel:%3 Weight:%4").arg(family).
										arg(size).arg(f.pixelSize()).arg(weight));

				QFontInfo fi(f);
				qDebug() << qPrintable(QString("Font [obtained]  -> Family:%1 Point:%2 Pixel:%3 Weight:%4").arg(fi.family()).arg(fi.pointSize()).
										arg(fi.pixelSize()).arg(fi.weight()));
#endif
				font_list[static_cast<Type>(type)] = f;
			}
		}
	}
}

const QFont& FontManager::get(Type t)
{
	if (!font_list.contains(t))
		qFatal("Font of type %d not load!", t);

	return font_list[t];
}


// The global definition of font manager
FontManager *bt_global::font = 0;

