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
	AMPLIFIER_ADDRESS,
	SOURCE_ADDRESS,
	INIT_COMPLETE
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

#ifndef CONFIG_BTOUCH
QDomNode getPageNodeFromPageId(int pageid);
QDomNode getPageNodeFromChildNode(QDomNode node, QString child_name);
QDomNode getHomepageNode();
#endif

// See getElement
QDomElement getConfElement(QString path);

void resetTimer(int signo);

/*! \def MY_FILE_CFG_DEFAULT
 *  The default configuration file
 */
#define MY_FILE_CFG_DEFAULT        "cfg/stack_open.xml"
/*! \def MY_FILE_USER_CFG_DEFAULT
 *  The user-configurationFile name if not specified in stack_open.xml
 */
#define MY_FILE_USER_CFG_DEFAULT   "cfg/conf.xml"
/*! \def MY_PARSER_DEFAUL
 *  The default parser
 */
#define MY_PARSER_DEFAULT          "bin/bt_xmlparser"
/*! \def MY_FILE_LOG_DEFAULT
 *  The logFile name if not specified in stack_open.xml
 */
#define MY_FILE_LOG_DEFAULT        "log/BTouch.log"
/*! \def IMG_PATH
 *  Define path for Icons
 */
#define IMG_PATH                    "cfg/extra/1/skin/"
/*! \def SOUND_PATH
 *  Define path for Sounds
 */
#define SOUND_PATH                    "cfg/extra/10/"
/*! \def EXTRA_FILE
 *  Define xml file that contains extras (for istance fonts color and fonts background)
 */
#define EXTRA_FILE                  "cfg/extra/1/extra.xml"
/*! \def SKIN_FILE
 *  Define the css file that contains the styles for the application
 */
#define SKIN_FILE                  "cfg/extra/1/skin.xml"
/*! \def VERBOSITY_LEVEL_DEFAULT
 * The default verbosity level
 */
/*! \def RINGTONE_FILE
 *  Define the css file that contains the ringtones
 */
#define RINGTONE_FILE                  "cfg/extra/2/ringtones.xml"

#define MY_FILE_CFG_FONT        "cfg/extra/3/font_%1.xml"
/*! \def MY_FILE_CFG_FONT
 *  The font configuration file
 */
#define VERBOSITY_LEVEL_DEFAULT    0
/*! \def VERBOSITY_LEVEL_DEFAUL
 *  It defines the starting path for mediaserver
 */
#ifndef MEDIASERVER_PATH
#define MEDIASERVER_PATH            "/home/bticino/mediaserver/"
#endif
/*! \def FILE_CHANGE_CONF
 *  The file to generate when changing the user-configurationFile to avoid beinf resetted from bt_processi
 */
#define FILE_CHANGE_CONF           "BTOUCH_CHANGE_CONF"
/*! \def FILE_WDT
 *  The file name to create for software watchdog
 */
#define FILE_WDT                   "/var/tmp/bticino/bt_wd/BTouch_qws"

#define LANGUAGE_FILE_TMPL        "cfg/extra/2/btouch_%s.qm"
/*! \def LANGUAGE_FILE_TMPL
 *  The template path to find the language file.
 */
#define DEFAULT_LANGUAGE          "en"
/*! \def DEFAULT_LANGUAGE
 *  Default language used in BTouch
 */


/*! \def MAX_WIDTH
 *  Maximum width of the screen
 */
#define MAX_WIDTH                  240
/*! \def MAX_HEIGHT
 *  Maximum height of the screen
 */
#define MAX_HEIGHT                 320
/*! \def NUM_RIGHE
 *  Number of lines shown on the screen
 */
#define NUM_RIGHE                  4

/// The simbol of temperature degrees
#define TEMP_DEGREES "\272"


#ifdef CONFIG_BTOUCH
enum Section
{
	NO_SECTION=0,
	AUTOMAZIONE=1,                                /*!< Automation system */
	ILLUMINAZIONE=2,                              /*!< Lighting system */
	ANTIINTRUSIONE=3,                             /*!< Anti-intrusion system */
	CARICHI=4,                                    /*!< Appliances managing system */
	TERMOREGOLAZIONE=5,                           /*!< Thermoregulation system */
	DIFSON=6,                                     /*!< Sound diffusion system */
	SCENARI=7,                                    /*!< Scenarios managing */
	IMPOSTAZIONI=8,                               /*!< Settings */
	BACK=9,                                       /*!< Back button - placed here for convenience */
	SPECIAL=10,                                   /*!< Special button - placed here for convenience */
	VIDEOCITOFONIA=11,
	SCENARI_EVOLUTI=12,                           /*!< Advanced scenarios management */
	DIFSON_MULTI=13,                              /*!< Multichannel sound diffusion system */
	SUPERVISIONE=14,                              /*!< Supervision system */
	TERMOREG_MULTI_PLANT=15,                      /*!< Thermoregulation system with one or more 4-zones plants */
	ENERGY_MANAGEMENT=16,                         /*!< Energy management system */
	ENERGY_DATA=17,                               /*!< Energy data system */
	LOAD_MANAGEMENT=18,                           /*!< Load management system */
	FEED_READER=99,                               /*!< Feed reader page */
	/* Added to avoid compile problems... */
	MULTIMEDIA=26,
	MESSAGES=27,\
	AIR_CONDITIONING=-1
};
#else
enum Section
{
	NO_SECTION=0,
	AUTOMAZIONE=3000,                             /*!< Automation system */
	ILLUMINAZIONE=2000,                           /*!< Lighting system */
	ANTIINTRUSIONE=13000,                         /*!< Anti-intrusion system */
	CARICHI=4,                                    /*!< Appliances managing system */
	TERMOREGOLAZIONE=8000,                        /*!< Thermoregulation system */
	BACK=9,                                       /*!< Back button - placed here for convenience */
	SPECIAL=10,                                   /*!< Special button - placed here for convenience */
	MESSAGES=17000,                               /*!< Textual Messages from scs */
	LOAD_MANAGEMENT=18000,                        /*!< Load management system */
	SCENARI=1000,                                 /*!< Scenarios managing */
	SCENARI_EVOLUTI=9000,                         /*!< Advanced scenarios management */
	VIDEOCITOFONIA=10000,                         /*!< VideoDoorEntry system */
	IMPOSTAZIONI=29,                              /*!< Settings */
	MULTIMEDIA=16000,                             /*!< Multimedia system */
	DIFSON_MONO=11000,                            /*!< Sound diffusion system (monochannel) */
	DIFSON_MULTI=12000,                           /*!< Sound diffusion system (multichannel) */
	SUPERVISIONE=6000,                            /*!< Supervision system */
	TERMOREG_MULTI_PLANT=-3,                      /*!< Thermoregulation system with one or more 4-zones plants */
	ENERGY_MANAGEMENT=15000,                      /*!< Energy management system */
	ENERGY_DATA=7000,                             /*!< Energy data system */
	FEED_READER=-6,                               /*!< Feed reader page */
	AIR_CONDITIONING=4000,                        /*!< Air conditioning */
};
#endif


/*! \enum ItemType
 *  This enum describes the various items implemented
 */
enum ItemType
{
	CARICO=7,                                     /*!<  Appliance */
	SET_DATA_ORA=14,                              /*!<  Time setting */
	SET_SVEGLIA=20,                               /*!<  AlarmClock setting */
	SET_SVEGLIA_SINGLEPAGE=14201,                 /*!<  AlarmClock setting with state icon */
	DISPLAY=21,                                   /*!<  Display */
#ifdef CONFIG_BTOUCH
	ZONANTINTRUS=23,                              /*!<  Anti-intrusion zone */
	IMPIANTINTRUS=24,                             /*!<  Anti-intrusion system */
#endif
	SUONO=25,                                     /*!<  Beep */
	PROTEZIONE=26,                                /*!<  Password's settings */
	VERSIONE=27,                                  /*!<  Version */
	CONTRASTO=28,                                 /*!<  Contrast */
	DATA=30,                                      /*!<  Date */
	TEMPERATURA=31,                               /*!<  Temperature */
	OROLOGIO=32,                                  /*!<  Clock */
	POSTO_ESTERNO=49,                             /*!< Posto esterno */
	LANSETTINGS=72,                               /*!< LAN settings and information */
	// for SpecialPage, HeaderWidget
#ifdef CONFIG_BTOUCH
	TERMO_HOME_NC_EXTPROBE=57,                    /*!< Home page external not-controlled probe */
	TERMO_HOME_NC_PROBE=58,                       /*!< Home page not-controlled probe */
#else
	TERMO_HOME_NC_EXTPROBE=207,                   /*!< Home page external not-controlled probe */
	TERMO_HOME_NC_PROBE=208,                      /*!< Home page not-controlled probe */
#endif
};

#endif //MAIN_H


