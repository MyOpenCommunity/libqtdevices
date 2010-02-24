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

	// TEXT: default text size
	// BANNERTEXT: label text for banner labels aligned horizontally with the icons
	// BANNERDESCRIPTION: descriptive text displayed under a banner
	// HOMEPAGEWIDGET: date/time/temperature display in home page
#ifdef LAYOUT_BTOUCH
	enum Type
	{
		TITLE = 0,
		SUBTITLE = 1,
		TEXT = 2,
		BANNERTEXT = 2,
		BANNERDESCRIPTION = 2,
		SMALLTEXT = 3,
		HOMEPAGEWIDGET = 6,
		// thermal regulation
		PROBE_TEMPERATURE = 0,
		PROBE_SETPOINT = 1,
		REGULATOR_TEMPERATURE = 1,
		REGULATOR_DESCRIPTION = 1,
	};
#else
	enum Type
	{
		TITLE = 0,
		SUBTITLE = 0, // TBD, same as TITLE for now
		TEXT = 2,
		BANNERTEXT = 4,
		BANNERDESCRIPTION = 5,
		SMALLTEXT = 2, // TBD, same as TEXT for now
		HOMEPAGEWIDGET = 6,
		// thermal regulation
		PROBE_TEMPERATURE = 7,
		PROBE_SETPOINT = 8,
		REGULATOR_TEMPERATURE = 8,
		REGULATOR_DESCRIPTION = 8,
		// date/time edit
		DATE_TIME = 0,
	};
#endif

	const QFont& get(Type t);

private:
	FontManager(const FontManager &);
	FontManager &operator=(const FontManager&);

	QHash<Type, QFont> font_list;
};


namespace bt_global { extern FontManager *font; }

#endif // FONTMANAGER_H
