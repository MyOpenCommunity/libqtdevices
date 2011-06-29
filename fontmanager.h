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


/*!
	\ingroup Core
	\brief The global manager of the fonts.

	This class uses an xml file to load, for each value of the enumeration 
	FontManager::Type, the font associated.
	Use the method get() to retrieve the font for the given type.

	The mapping between the values in FontManager::Type and actual font types/sizes
	is defined in XML font mapping files (one per language):
	\verbatim
<fontconfig>
  <font>
    <type>0</type>
    <descr>TITLE</descr>
    <family>arialuni</family>
    <size>40</size>
    <weight>75</weight>
  </font>
  ...
</fontconfig>
	\endverbatim

	For every font definition in FontManager::Type there must be a
	matching \c \<font> entry with the same \c \<type>.  The other
	attributes describe the font associated with the entry.

	\c \<family> is the name of the font family; \c \<size> is the
	size in points (1/72th of an inch); \c \<weight> can have any value
	from 0 to 99, but there are some special values: 50 is for a normal
	font weight, 75 for bold and 25 for a light font.
*/
class FontManager
{
public:
	FontManager(QString font_file);

#ifdef LAYOUT_TS_3_5
	enum Type
	{
		TITLE = 0,
		SUBTITLE = 1,
		TEXT = 2,
		BANNERTEXT = 2,
		BANNERDESCRIPTION = 2,
		SMALLTEXT = 3,
		HOMEPAGEWIDGET = 2,
		// thermal regulation
		PROBE_TEMPERATURE = 0,
		PROBE_SETPOINT = 1,
		EXTERNAL_PROBE = 1,
		REGULATOR_TEMPERATURE = 1,
		REGULATOR_DESCRIPTION = 1,
		AIRCONDITIONING_TEMPERATURE = 1,
		AUDIO_SOURCE_TEXT = 2,
		AUDIO_SOURCE_DESCRIPTION = 2,
		// rds radio
		RADIO_NAME = SUBTITLE,
		RADIO_MEMORY_NUMBER = TEXT,
		RADIO_STATION = TITLE,
		RADIO_ENVIRONMENT = TITLE,

		// audio player
		PLAYER_RADIO_TITLE = TEXT,          /*!< radio ip title */
		PLAYER_RADIO_URL = TEXT,            /*!< radio ip url */
		PLAYER_INFO = TEXT,                 /*!< current track and elapsed time */
	};
#else
	/*!
		\brief Font Types.
	*/
	enum Type
	{
		TITLE = 0,                                        /*!< title of the page (used for all TS 10'' pages, and for a couple of TS 3.5'' pages) */
		SUBTITLE = 0,                                     /*!< load deactivation time, version page, load management current consumption,
		                                                       load management "force on" confirmation page, energy management date selection */
		TEXT = 2,                                         /*!< default text size */
		BANNERTEXT = 4,                                   /*!< label text for banner labels aligned horizontally with the icons */
		BANNERDESCRIPTION = 5,                            /*!< descriptive text displayed under a banner (or an icon) */
		SMALLTEXT = 2,                                    /*!< energy graph, energy table date, energy view date, lan settings text,version page, RDS radio */
		HOMEPAGEWIDGET = 6,                               /*!< date/time/temperature display in home page */
		// thermal regulation
		PROBE_TEMPERATURE = 7,                            /*!< current temperature of a thermal probe */
		PROBE_SETPOINT = 8,                               /*!< set point temperature of a thermal probe */
		EXTERNAL_PROBE = 2,                               /*!< description and temperature for external/non controlled probes */
		REGULATOR_TEMPERATURE = 8,                        /*!< current temperature of a thermal regulator */
		REGULATOR_DESCRIPTION = 8,                        /*!< description of a thermal regulator */
		AIRCONDITIONING_TEMPERATURE = 0,                  /*!< temperature measured by a split */
		// date/time edit
		DATE_TIME = 0,                                    /*!< labels in the date or time edit */
		// audio source text
		AUDIO_SOURCE_TEXT = BANNERTEXT,                   /*!< source description text for aux/multimedia/radio banner */
		AUDIO_SOURCE_DESCRIPTION = BANNERDESCRIPTION,     /*!< descriptive text, displayed under the source description */
		// rds radio
		RADIO_NAME = TITLE,                               /*!< the name of the radio */
		RADIO_MEMORY_NUMBER = 3,                          /*!< the number of the radio station */
		RADIO_STATION = 3,                                /*!< the frequency of the radio station */
		// audio player
		PLAYER_TITLE = HOMEPAGEWIDGET,                   /*!< song title (first line) */
		PLAYER_AUTHOR = TITLE,                           /*!< song author/info (second line) */
		PLAYER_INFO = BANNERDESCRIPTION,                 /*!< current track and elapsed time */
	};
#endif

	/*!
		\brief Return a reference to the font to use for the given \a t.
	*/
	const QFont& get(Type t);

private:
	FontManager(const FontManager &);
	FontManager &operator=(const FontManager&);

	QHash<Type, QFont> font_list;
};


namespace bt_global { extern FontManager *font; }

#endif // FONTMANAGER_H
