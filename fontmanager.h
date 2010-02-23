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


#ifndef FONTMANAGER_H
#define FONTMANAGER_H

class QString;

#include <QFont>
#include <QHash>

/**
 * \class FontManager
 *
 * This class load the list of font from an xml file. It is designed to use as
 * global object, through the "get" method used to obtain a font of "Type" category.
 */
class FontManager
{
public:
	FontManager(QString font_file);

	enum Type
	{
		TITLE = 0,
		SUBTITLE = 1,
		TEXT = 2,
		SMALLTEXT = 3,
	};

	const QFont& get(Type t);

private:
	FontManager(const FontManager &);
	FontManager &operator=(const FontManager&);

	QHash<Type, QFont> font_list;
};


namespace bt_global { extern FontManager *font; }

#endif // FONTMANAGER_H
