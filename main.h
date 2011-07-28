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


#ifndef MAIN_H
#define MAIN_H

#include <QHash>

class QDomNode;
class QDomElement;
class QString;

/**
 * The following enum defines the keys of the global configuration.
 */
enum GlobalFields
{
	LANGUAGE,
	TEMPERATURE_SCALE,
	DATE_FORMAT,
	MODEL,
	NAME,
	PI_ADDRESS,
	PI_MODE,
	GUARD_UNIT_ADDRESS,
	AMPLIFIER_ADDRESS,
	SOURCE_ADDRESS,
	TS_NUMBER,
	INIT_COMPLETE,
};

namespace bt_global { extern QHash<GlobalFields, QString> *config; }

enum TemperatureScale
{
	CELSIUS = 0,
	FAHRENHEIT,
};

enum DateFormat
{
	EUROPEAN_DATE = 0,   // dd.mm.yy
	USA_DATE = 1,        // mm.dd.yy
	YEAR_FIRST = 2,      // yy.mm.dd
};

QDomNode getPageNode(int id);

#ifndef CONFIG_TS_3_5
QDomNode getPageNodeFromPageId(int pageid);
QDomNode getPageNodeFromChildNode(QDomNode node, QString child_name);
QDomNode getHomepageNode();
#endif

// See getElement
QDomElement getConfElement(QString path);

#ifdef BT_HARDWARE_DM365
// The default configuration file for BTouch and other processes
#define MY_FILE_CFG_DEFAULT "/var/tmp/stack_open.xml"

// The configuration file for BTouch
#define MY_FILE_USER_CFG_DEFAULT "/var/tmp/conf.xml"

// The log filename if not specified in stack_open.xml
#define MY_FILE_LOG_DEFAULT "/var/tmp/BTouch.log"

// The file to generate when the configuration is successfully loaded
#define FILE_CONF_LOADED "/var/tmp/flags/BTouch_checkconfok"
#else
// The default configuration file for BTouch and other processes
#define MY_FILE_CFG_DEFAULT "cfg/stack_open.xml"

// The configuration file for BTouch
#define MY_FILE_USER_CFG_DEFAULT "cfg/conf.xml"

// The default xml parser (not used by BTouch)
#define MY_PARSER_DEFAULT "bin/bt_xmlparser"

// The log filename if not specified in stack_open.xml
#define MY_FILE_LOG_DEFAULT "log/BTouch.log"

// The file to generate when the configuration is successfully loaded
#define FILE_CONF_LOADED "BTouch_checkconfok"
#endif

// The base path for the images
#ifndef IMG_PATH
#define IMG_PATH "cfg/extra/1/skin/"
#endif

// The path for sounds
#define SOUND_PATH "cfg/extra/10/"

// The css file that contains the styles for the application
#define SKIN_FILE "cfg/extra/1/skin.xml"

// The default verbosity level
#define VERBOSITY_LEVEL_DEFAULT 0

// The xml file that contains the ringtones
#define RINGTONE_FILE "cfg/extra/5/ringtones.xml"

// The font configuration file
#define MY_FILE_CFG_FONT "cfg/extra/3/font_%1.xml"

// The file to generate when changing the configuration file to avoid being resetted from bt_processi
#define FILE_CHANGE_CONF "BTOUCH_CHANGE_CONF"

// The file name to create for software watchdog
#define FILE_WDT "/var/tmp/bticino/bt_wd/BTouch_qws"

// The template path to find the language file.
#define LANGUAGE_FILE_TMPL "cfg/extra/2/btouch_%s.qm"

// The default language used in the GUI
#define DEFAULT_LANGUAGE "en"

// The simbol of temperature degrees
#define TEMP_DEGREES "\272"

// The delay to use in the starting phase to avoid bottlenecks in the scs bus.
#define TS_NUMBER_FRAME_DELAY 500

enum Section
{
	NO_SECTION = 0,
	AUTOMATION = 3000,                              /*!< Automation system */
	LIGHTING = 2000,                                /*!< Lighting system */
	ANTIINTRUSION = 13000,                          /*!< Anti-intrusion system */
	LOADS = 5000,                                   /*!< Appliances managing system */
	THERMALREGULATION = 8000,                       /*!< Thermoregulation system */
	SPECIAL = 10,                                   /*!< Special button - placed here for convenience */
	MESSAGES = 17000,                               /*!< Textual Messages from scs */
	LOAD_MANAGEMENT = 18000,                        /*!< Load management system */
	SCENARIOS = 1000,                               /*!< Scenarios managing */
	EVOLVED_SCENARIOS = 9000,                       /*!< Evolved scenarios management */
	VIDEODOORENTRY = 10000,                         /*!< VideoDoorEntry system */
	SETTINGS = 14000,                               /*!< Settings */
	MULTIMEDIA = 16000,                             /*!< Multimedia system */
	SOUNDDIFFUSION = 11000,                         /*!< Sound diffusion system (monochannel) */
	SOUNDDIFFUSION_MULTI = 12000,                   /*!< Sound diffusion system (multichannel) */
	SUPERVISION = 6000,                             /*!< Supervision system */
	THERMALREGULATION_MULTI = -3,                   /*!< Thermoregulation system with one or more 4-zones plants */
	ENERGY_MANAGEMENT = 15000,                      /*!< Energy management system */
	ENERGY_DATA = 7000,                             /*!< Energy data system */

#ifdef BUILD_EXAMPLES
	TCP_BANNER_TEST = 55555,
#endif
};

// The items used in various part of the code: homepage, special page (ts3) or headerwidget (ts10)
enum GlobalItems
{
	ITEM_TIME = 205,                  /*!<  Clock */
	ITEM_DATE = 206,                  /*!<  Date */
	ITEM_TEMPERATURE_EXTPROBE = 208,  /*!< External not-controlled probe */
	ITEM_TEMPERATURE_PROBE = 207,     /*!< Not-controlled probe */

};

#endif //MAIN_H


