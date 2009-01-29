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
	// The dummy costructor.. see .cpp for details
	FontManager();

	enum Type
	{
		TITLE = 0,
		SUBTITLE = 1,
		TEXT = 2,
		SMALLTEXT = 3
	};

	const QFont& get(Type t);
	void loadFonts(QString font_file);

private:
	FontManager(const FontManager &);
	FontManager &operator=(const FontManager&);

	QHash<Type, QFont> font_list;
};


namespace bt_global { extern FontManager font; }

#endif // FONTMANAGER_H
