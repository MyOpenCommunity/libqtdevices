/**
 * \file
 * <!--
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This file contains the class used to manage the font
 *
 * \author Gianni Valdambrini <aleister@develer.com>
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
		HOMEPAGEWIDGET = 6
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
		HOMEPAGEWIDGET = 6
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
