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
	MESSAGES=27,
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
	MESSAGES=17,                                  /*!< Textual Messages from scs */
	LOAD_MANAGEMENT=18,                           /*!< Load management system */
	SCENARI=1000,                                 /*!< Scenarios managing */
	SCENARI_EVOLUTI=20,                           /*!< Advanced scenarios management */
	VIDEOCITOFONIA=10000,                         /*!< VideoDoorEntry system */
	IMPOSTAZIONI=29,                              /*!< Settings */
	MULTIMEDIA=16000,                             /*!< Multimedia system */
	DIFSON_MULTI=12000,                           /*!< Sound diffusion system (mono and multichannel) */
	SUPERVISIONE=-2,                              /*!< Supervision system */
	TERMOREG_MULTI_PLANT=-3,                      /*!< Thermoregulation system with one or more 4-zones plants */
	ENERGY_MANAGEMENT=-4,                         /*!< Energy management system */
	ENERGY_DATA=-5,                               /*!< Energy data system */
	FEED_READER=-6,                               /*!< Feed reader page */
};
#endif


/****************************************************************
 **  definizione dei file di immagini
 ****************************************************************/
/*!  \def ICON_CICLA
  The file name for \a changing icon*/
#define ICON_CICLA  (IMG_PATH "btncycle.png")
/*!  \def ICON_OK
  The file name for \a ok icon*/
#define ICON_OK  (IMG_PATH "btnok.png")
/*!  \def ICON_ON
  The file name for \a on icon*/
#define ICON_ON  (IMG_PATH "btnon.png")
/*!  \def ICON_OFF
  The file name for \a off icon*/
#define ICON_OFF  (IMG_PATH "btnoff.png")
/*!  \def ICON_IMPOSTA
  The file name for \a setting icon*/
#define ICON_IMPOSTA  (IMG_PATH "audiosetting.png")
/*!  \def ICON_INFO
  The file name for \a information icon*/
#define ICON_INFO  (IMG_PATH "btninfo.png")
/*!  \def ICON_FRECCIA_SU
  The file name for \a up-arrow icon*/
#define ICON_FRECCIA_SU  (IMG_PATH "arrup.png")
/*!  \def ICON_FRECCIA_GIU
  The file name for \a down-arrow icon*/
#define ICON_FRECCIA_GIU  (IMG_PATH "arrdw.png")
/*!  \def ICON_FRECCIA_SX
  The file name for \a left-arrow icon*/
#define ICON_FRECCIA_SX  (IMG_PATH "arrlf.png")
/*!  \def ICON_FRECCIA_DX
  The file name for \a right-arrow icon*/
#define ICON_FRECCIA_DX  (IMG_PATH "arrrg.png")
/*!  \def ICON_FFWD
  The file name for \a feed \a forward icon*/
#define ICON_FFWD  (IMG_PATH "btnforward.png")
/*!  \def ICON_REW
  The file name for \a rewind icon*/
#define ICON_REW  (IMG_PATH "btnbackward.png")
/*!  \def ICON_DIFFSON
  The file name for \a diffson icon*/
#define ICON_DIFFSON  (IMG_PATH "appdiffsmall.png")
/*!  \def ICON_SVEGLIA_ON
  The file name for \a alarm \a clock icon*/
#define ICON_SVEGLIA_ON  (IMG_PATH "svegliaon.png")
/*!  \def ICON_PIU
  The file name for \a plus icon*/
#define ICON_PIU  (IMG_PATH "btnplus.png")
/*!  \def ICON_MENO
  The file name for \a minus icon*/
#define ICON_MENO  (IMG_PATH "btnmin.png")

/*!  \def ICON_CANC
  The file name for \a cancellation icon*/
#define ICON_CANC  (IMG_PATH "btncanc.png")
/*!  \def ICON_VUOTO
  The file name for \a empty icon*/
#define ICON_VUOTO  (IMG_PATH "btnempty.png")
/*!  \def ICON_DEL
  The file name for \a delete icon*/
#define ICON_DEL  (IMG_PATH "btndel.png")
/*!  \def ICON_STOPNGO_APERTO
  The file name for \a open stopngo icon*/
#define ICON_STOPNGO_APERTO (IMG_PATH "S&G_Aperto.png")
/*!  \def ICON_STOPNGO_CHIUSO
  The file name for \a closed stopngo icon*/
#define ICON_STOPNGO_CHIUSO (IMG_PATH "S&G_Chiuso.png")
/*!  \def ICON_STOPNGO_BLOCCO
  The file name for \a locked stopngo icon*/
#define ICON_STOPNGO_BLOCCO (IMG_PATH "S&G_Aperto_Blocco.png")
/*!  \def ICON_STOPNGO_CORTOCIRCUITO
  The file name for \a short circuit stopngo icon*/
#define ICON_STOPNGO_CORTOCIRCUITO (IMG_PATH "S&G_Aperto_Cortocircuito.png")
/*!  \def ICON_STOPNGO_GUASTO_TERRA
  The file name for \a earth failure stopngo icon*/
#define ICON_STOPNGO_GUASTO_TERRA (IMG_PATH "S&G_Aperto_Guastoversoterra.png")
/*!  \def ICON_STOPNGO_SOVRATENSIONE
  The file name for \a out of Vmax stopngo icon*/
#define ICON_STOPNGO_SOVRATENSIONE (IMG_PATH "S&G_Aperto_Sovratensione.png")
/*!  \def ICON_STOPNGO_A_RIARMO
  The file name for \a stopngo enabled arm icon*/
#define ICON_STOPNGO_A_RIARMO (IMG_PATH "a_riarmo.png")
/*!  \def ICON_STOPNGO_D_RIARMO
  The file name for \a stopngo disabled arm icon*/
#define ICON_STOPNGO_D_RIARMO (IMG_PATH "d_riarmo.png")
/*!  \def ICON_STOPNGO_A_VERIFICA_IMPIANTO
  The file name for \a stopngo verify icon*/
#define ICON_STOPNGO_A_VERIFICA_IMPIANTO (IMG_PATH "a_verifica_impianto.png")
/*!  \def ICON_STOPNGO_D_VERIFICA_IMPIANTO
  The file name for \a stopngo disabled verify icon*/
#define ICON_STOPNGO_D_VERIFICA_IMPIANTO (IMG_PATH "d_verifica_impianto.png")
/*!  \def ICON_STOPNGO_A_AUTOTEST
  The file name for \a stopngo autotest icon*/
#define ICON_STOPNGO_A_AUTOTEST (IMG_PATH "a_autotest.png")
/*!  \def ICON_STOPNGO_D_AUTOTEST
  The file name for \a stopngo disabled autotest icon*/
#define ICON_STOPNGO_D_AUTOTEST (IMG_PATH "d_autotest.png")


/*! \enum ItemType
 *  This enum describes the various items implemented
 */
enum ItemType
{
	ATTUAT_AUTOM=0,                               /*!<  Automation actuator */
	DIMMER=1,                                     /*!<  Dimmer */
	ATTUAT_AUTOM_INT=2,                           /*!<  Interblocked automation actuator */
	VUOTO_3=3,                                    /*!<  Not used */
	SCENARIO=4,                                   /*!<  Scenario */
	GR_ATTUAT_AUTOM=5,                            /*!<  Automation actuator's group */
	GR_DIMMER=6,                                  /*!<  Dimmer's group */
	CARICO=7,                                     /*!<  Appliance */
	ATTUAT_AUTOM_INT_SIC=8,                       /*!<  Secure interblocked actuator */
	ATTUAT_AUTOM_TEMP=9,                          /*!<  Timed automation actuator */
	GR_ATTUAT_INT=10,                             /*!<  Interblocked actuator's group */
	ATTUAT_AUTOM_PULS=11,                         /*!<  Pulse automation actuator */
	ATTUAT_VCT_LS=12,                             /*!<  Stairlight video-doorentrysystem actuator */
	ATTUAT_VCT_SERR=13,                           /*!<  Lock video-doorentrysystem actuator */
	SET_DATA_ORA=14,                              /*!<  Time setting */
	VUOTO_15=15,                                  /*!<  Not used */
	SORGENTE_AUX=16,                              /*!<  Auxiliary sound source */
	SORGENTE_RADIO=17,                            /*!<  FM tuner */
	AMPLIFICATORE=18,                             /*!<  Amplifier */
	GR_AMPLIFICATORI=19,                          /*!<  Amplifier's group */
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
	MOD_SCENARI=29,                               /*!<  DIN scenario module's scenario */
	DATA=30,                                      /*!<  Date */
	TEMPERATURA=31,                               /*!<  Temperature */
	OROLOGIO=32,                                  /*!<  Clock */
	ALLARME=33,                                   /*!<  Alarm */
	CMDSPECIAL=34,                                /*!<  Special command */
	DIMMER_100=35,                                /*!<  New dimmer */
	ATTUAT_AUTOM_TEMP_NUOVO_N=36,                 /*!< Attuatore temp nuovo, N tempi */
	ATTUAT_AUTOM_TEMP_NUOVO_F=37,                 /*!< Attuatore temp nuovo, fisso */
	SCENARIO_EVOLUTO=38,                          /*!< Scenario evoluto */
	SCENARIO_SCHEDULATO=39,                       /*!< Scenario schedulato */
	AUTOM_CANC_ATTUAT_ILL=40,                     /*!< Automatismi-Cancello con attuatore illuminazione */
	AUTOM_CANC_ATTUAT_VC=41,                      /*!< Automatismi-Cancello con attuatore videocitofonia */
	PPT_STAT=42,                                  /*!< Automatismi-PPTStat */
	PPT_SCE=43,                                   /*!< Automatismi-PPTSce */
	GR_DIMMER100=44,                              /*!< Gruppo dimmer 100 livelli */
	SORG_RADIO=45,                                /*!< Sorgente radio per Diffusione Sonora Multicanale */
	SORG_AUX=46,                                  /*!< Sorgente aux per Diffusione Sonora Multicanale */
	AMBIENTE=47,                                  /*!< Ambiente diff son. multi */
	INSIEME_AMBIENTI=48,                          /*!< Ins. amb. diff. son. multi */
	POSTO_ESTERNO=49,                             /*!< Posto esterno */
	SORGENTE_MULTIM=50,                           /*!< Sorgente Multimediale per Diffusione Sonora */
	SORGENTE_MULTIM_MC=51,                        /*!< Sorgente Multimediale per Diffusione Sonora Multicanale */
#ifdef CONFIG_BTOUCH
	TERMO_99Z_PROBE=22,                           /*!< Probe controlled by 99 zones thermal regulator */
	TERMO_99Z_PROBE_FANCOIL=52,                   /*!< Probe controlled by 99 zones thermal regulator with fan-coil control */
	TERMO_4Z_PROBE=53,                            /*!< Probe controlled by 4 zones thermal regulator */
	TERMO_4Z_PROBE_FANCOIL=54,                    /*!< Probe controlled by 4 zones thermal regulator with fan-coil control */
	TERMO_NC_EXTPROBE=55,                         /*!< External not-controlled probe */
	TERMO_NC_PROBE=56,                            /*!< Not-controlled probe */
	TERMO_HOME_NC_EXTPROBE=57,                    /*!< Home page external not-controlled probe */
	TERMO_HOME_NC_PROBE=58,                       /*!< Home page not-controlled probe */
#else
	TERMO_99Z_PROBE=8012,                         /*!< Probe controlled by 99 zones thermal regulator */
	TERMO_99Z_PROBE_FANCOIL=8012,                 /*!< Probe controlled by 99 zones thermal regulator with fan-coil control */
	TERMO_4Z_PROBE=8042,                          /*!< Probe controlled by 4 zones thermal regulator */
	TERMO_4Z_PROBE_FANCOIL=8042,                  /*!< Probe controlled by 4 zones thermal regulator with fan-coil control */
	TERMO_NC_EXTPROBE=8021,                       /*!< External not-controlled probe */
	TERMO_NC_PROBE=8031,                          /*!< Not-controlled probe */
	TERMO_HOME_NC_EXTPROBE=207,                   /*!< Home page external not-controlled probe */
	TERMO_HOME_NC_PROBE=208,                      /*!< Home page not-controlled probe */
#endif
	CLASS_STOPNGO=59,                             /*!< StopnGo devices class */
	STOPNGO=60,                                   /*!< StopnGo device */
	STOPNGO_PLUS=61,                              /*!< StopnGo Plus device */
	STOPNGO_BTEST=62,                             /*!< StopnGo BTest device */
#ifdef CONFIG_BTOUCH
	TERMO_99Z=66,                                 /*!< 99 zones thermal regulator */
#else
	TERMO_99Z=8011,                               /*!< 99 zones thermal regulator */
#endif
	TERMO_4Z=68,                                  /*!< 4 zones thermal regulator */
	POWER_AMPLIFIER=69,                           /*!< Power amplifier*/
	ENERGY_TYPE=70,                               /*!< The type/interface of energy management */
	LANSETTINGS=72,                               /*!< LAN settings and information */
#ifdef CONFIG_BTOUCH
	AIR_SPLIT=74,                                 /*!< AIR conditioning single split (basic) */
	AIR_GENERAL=75,                               /*!< AIR conditioning general split (basic) */
	AIR_SPLIT_ADV=77,                             /*!< AIR conditioning single split (advanced) */
	AIR_GENERAL_ADV=78,                           /*!< AIR conditioning general split (advanced) */
#else
	AIR_SPLIT=4001,                               /*!< AIR conditioning single split (basic) */
	AIR_GENERAL=4002,                             /*!< AIR conditioning general split (basic) */
	AIR_SPLIT_ADV=4003,                           /*!< AIR conditioning single split (advanced) */
	AIR_GENERAL_ADV=4004,                         /*!< AIR conditioning general split (advanced) */
#endif
	LOAD_WITH_CU=80,                              /*!< Load with central unit */
	LOAD_WITHOUT_CU=81,                           /*!< Load without central unit */
	LOAD_DIAGNOSTIC=82,                           /*!< Load diagnostics */
};

#endif //MAIN_H


