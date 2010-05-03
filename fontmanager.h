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

	// TODO: need to split SUBTITLE and SMALLTEXT
	//
	// TITLE: title of the page (used for all TouchX pages, and for a couple of BTouch pages)
	// SUBTITLE: AUX source, load deactivation time, external/non-controlled probe
	//           RDS radio, version page, load management current consumption,
	//           load management "force on" confirmation page, energy management date selection
	// TEXT: default text size
	// BANNERTEXT: label text for banner labels aligned horizontally with the icons
	// BANNERDESCRIPTION: descriptive text displayed under a banner (or an icon)
	// SMALLTEXT: energy graph, energy table date, energy view date, lan settings text,
	//            version page, RDS radio
	// HOMEPAGEWIDGET: date/time/temperature display in home page
	// PROBE_TEMPERATURE: current temperature of a thermal probe
	// PROBE_SETPOINT: set point temperature of a thermal probe
	// REGULATOR_TEMPERATURE: current temperature of a thermal regulator
	// REGULATOR_DESCRIPTION: description of a thermal regulator
	// AIRCONDITIONING_TEMPERATURE: temperature measured by a split
	// PLAYER_TITLE: song title (first line)
	// PLAYER_AUTHOR: song author/info (second line)
	// PLAYER_INFO: current track and elapsed time
	// AUDIO_SOURCE_TEXT: source description text for aux/multimedia/radio banner
	// AUDIO_SOURCE_DESCRIPTION: descriptive text, displayed under the source description
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
		AIRCONDITIONING_TEMPERATURE = 1,
		// TODO, check if it is correct
		AUDIO_SOURCE_TEXT = 1,
		AUDIO_SOURCE_DESCRIPTION = 1,
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
		AIRCONDITIONING_TEMPERATURE = 0,
		// date/time edit
		DATE_TIME = 0,
		// audio source text
		AUDIO_SOURCE_TEXT = BANNERTEXT,
		AUDIO_SOURCE_DESCRIPTION = BANNERDESCRIPTION,
		// rds radio
		RADIO_NAME = TITLE,
		RADIO_MEMORY_NUMBER = SMALLTEXT,
		RADIO_STATION = SMALLTEXT,
		RADIO_ENVIRONMENT = TITLE,
		// audio player
		PLAYER_TITLE = HOMEPAGEWIDGET,
		PLAYER_AUTHOR = TITLE,
		PLAYER_INFO = BANNERDESCRIPTION,
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
