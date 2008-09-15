/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** main.h
 **
 **definizioni di carattere generale
 **
 ****************************************************************/
#ifndef MAIN_H
#define MAIN_H

#include "icondispatcher.h"

#include <qdom.h>

QDomNode getPageNode(int id);
QDomNode getChildWithId(QDomNode parent, const QRegExp &node_regexp, int id);
QString getLanguage();

// FIXME: next time a global function is needed, create an object that stores global configuration
enum TemperatureScale
{
	CELSIUS = 0,
	FAHRENHEIT,
	NONE,
};
TemperatureScale readTemperatureScale();
void readExtraConf(QColor **bg, QColor **fg1, QColor **fg2);

/****************************************************************
 ** ICONS LIBRARY
 ****************************************************************/
/**
 * Icons is a vector with pointers to QPixmap.
 * When an icon is needed, iconsLibrary is asked to create
 * a QPixmap and return the pointer. This pointer is stored
 * in Icons, instead to manually create a QPixmap and store the
 * pointer. In this way iconsLibrary do not waste memory.
 *
 * For instance if we have
 *
 * if(!Icon[0])
 * Icon[0] = new QPixmap();
 * Icon[0]->load(actuallcon);
 *
 * we instead write
 *
 * if(!Icon[0]) Icon[0] = iconsLibrary.getIcon(actuallcon)
 *
 * NOW iconsLibrary has its own destructor for icons
 * no need to destroy them in banner
 */
/// We use a global object to handle icons_library because different classes need icons.
extern IconDispatcher icons_library;

/// Global application config built using QDom
extern QDomDocument qdom_appconfig;


/****************************************************************
 ** Default configurazione applicativo
 ****************************************************************/
/*! \def MYPROCESSNAME
 *  Name of the process
 */
#define MYPROCESSNAME              "BTouch"
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
/*! \def EXTRA_FILE
 *  Define xml file that contains extras (for istance fonts color and fonts background)
 */
#define EXTRA_FILE                  "cfg/extra/1/extra.xml"
/*! \def VERBOSITY_LEVEL_DEFAULT
 * The default verbosity level
 */
#define MY_FILE_CFG_FONT        "cfg/extra/3/font_%s.xml"
/*! \def MY_FILE_CFG_FONT
 *  The font configuration file
 */
#define VERBOSITY_LEVEL_DEFAULT    0
/*! \def VERBOSITY_LEVEL_DEFAUL
 *  It defines the starting path for mediaserver
 */
#define MEDIASERVER_PATH            "/home/bticino/mediaserver/"
/*! \def XML_FILE_IN_DEFAULT
 */
#define XML_FILE_IN_DEFAULT        ".bto-to-xml"
/*! \def XML_FILE_OUT_DEFAULT
 */
#define XML_FILE_OUT_DEFAULT       ".xml-to-bto"
/*! \def PATH_VAR_DEFAULT
 */
#define PATH_VAR_DEFAULT           "/var"
/*!  \def FILE_TEST1
 * The file name to watch to generate a RED page
 */
#define FILE_TEST1                 "MODALITA_TEST1"
/*! \def FILE_TEST2
 *  The file name to watch to generate a GREEN page
 */
#define FILE_TEST2                 "MODALITA_TEST2"
/*! \def FILE_TEST3
 *  The file name to watch to generate a BLUE page
 */
#define FILE_TEST3                 "MODALITA_TEST3"
/*! \def FILE_AGGIORNAMENTO
 *  The file name to watch to generate the \a configuration page
 */
#define FILE_AGGIORNAMENTO	   "MODALITA_AGGIORNAMENTO"
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

/****************************************************************
 **  definizione dimensioni schermo
 ****************************************************************/
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

/****************************************************************
 **  definizione dei sottomenù
 ****************************************************************/
/*! \enum pagSecLiv
  This enum describes the various kind of pages*/
enum pagSecLiv{
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
	//SCHEDULAZIONI=11,                           /*!< Schedulations page*/
	VIDEOCITOFONIA=11,
	SCENARI_EVOLUTI=12,                           /*!< Advanced scenarios management */
	DIFSON_MULTI=13,                              /*!< Multichannel sound diffusion system */
	SUPERVISIONE=14,                              /*!< Supervision system */
	TERMOREG_MULTI_PLANT=15                       /*!< Thermoregulation system with one or more 4-zones plants */
};


/****************************************************************
 **  definizione dei file di immagini
 ****************************************************************/
/*!  \def ICON_CICLA
  The file name for \a changing icon*/
#define ICON_CICLA			(IMG_PATH "btncycle.png")
/*!  \def ICON_OK
  The file name for \a ok icon*/
#define ICON_OK				(IMG_PATH "btnok.png")
/*!  \def ICON_ON
  The file name for \a on icon*/
#define ICON_ON				(IMG_PATH "btnon.png")
/*!  \def ICON_OFF
  The file name for \a off icon*/
#define ICON_OFF			(IMG_PATH "btnoff.png")
/*!  \def ICON_IMPOSTA
  The file name for \a setting icon*/
#define ICON_IMPOSTA		(IMG_PATH "audiosetting.png")
/*!  \def ICON_INFO
  The file name for \a information icon*/
#define ICON_INFO			(IMG_PATH "btninfo.png")
/*!  \def ICON_FRECCIA_SU
  The file name for \a up-arrow icon*/
#define ICON_FRECCIA_SU		(IMG_PATH "arrup.png")
/*!  \def ICON_FRECCIA_GIU
  The file name for \a down-arrow icon*/
#define ICON_FRECCIA_GIU	(IMG_PATH "arrdw.png")
/*!  \def ICON_FRECCIA_SX
  The file name for \a left-arrow icon*/
#define ICON_FRECCIA_SX		(IMG_PATH "arrlf.png")
/*!  \def ICON_FRECCIA_DX
  The file name for \a right-arrow icon*/
#define ICON_FRECCIA_DX		(IMG_PATH "arrrg.png")
/*!  \def ICON_FFWD
  The file name for \a feed \a forward icon*/
#define ICON_FFWD			(IMG_PATH "btnforward.png")
/*!  \def ICON_REW
  The file name for \a rewind icon*/
#define ICON_REW			(IMG_PATH "btnbackward.png")
/*!  \def ICON_DIFFSON
  The file name for \a diffson icon*/
#define ICON_DIFFSON			(IMG_PATH "appdiffsmall.png")
/*!  \def ICON_OROLOGIO
  The file name for \a clock icon*/
#define ICON_OROLOGIO		(IMG_PATH "orologio.png")
/*!  \def ICON_CALENDARIO
  The file name for \a calendar icon*/
#define ICON_CALENDARIO		(IMG_PATH "calendario.png")
/*!  \def ICON_SVEGLIA_ON
  The file name for \a alarm \a clock icon*/
#define ICON_SVEGLIA_ON 	(IMG_PATH "svegliaon.png")
/*!  \def ICON_PIU
  The file name for \a plus icon*/
#define ICON_PIU			(IMG_PATH "btnplus.png")
/*!  \def ICON_MENO
  The file name for \a minus icon*/
#define ICON_MENO			(IMG_PATH "btnmin.png")
/*!  \def ICON_MANUAL_ON
  The file name for \a manual \a selected icon*/
#define ICON_MANUAL_ON		(IMG_PATH "btnman.png")
/*!  \def ICON_MANUAL_OFF
  The file name for \a manual \a unselected icon*/
#define ICON_MANUAL_OFF		(IMG_PATH "btnmanoff.png")
/*!  \def ICON_AUTO
  The file name for \a automatic \a selected icon*/
#define ICON_AUTO_ON		(IMG_PATH "btnauto.png")
/*!  \def ICON_AUTO_OFF
  The file name for \a automatic \a unselected icon*/
#define ICON_AUTO_OFF		(IMG_PATH "btnautooff.png")
/*!  \def ICON_MEM
  The file name for \a memorization icon*/
#define ICON_MEM			(IMG_PATH "btnmem.png")
/*!  \def ICON_ZERO
  The file name for \a zero icon*/
#define ICON_ZERO			(IMG_PATH "num0.png")
/*!  \def ICON_UNO
  The file name for \a one icon*/
#define ICON_UNO			(IMG_PATH "num1.png")
/*!  \def ICON_DUE
  The file name for \a two icon*/
#define ICON_DUE			(IMG_PATH "num2.png")
/*!  \def ICON_TRE
  The file name for \a three icon*/
#define ICON_TRE			(IMG_PATH "num3.png")
/*!  \def ICON_QUATTRO
  The file name for \a for icon*/
#define ICON_QUATTRO		(IMG_PATH "num4.png")
/*!  \def ICON_CINQUE
  The file name for \a five icon*/
#define ICON_CINQUE			(IMG_PATH "num5.png")
/*!  \def ICON_SEI
  The file name for \a six icon*/
#define ICON_SEI			(IMG_PATH "num6.png")
/*!  \def ICON_SETTE
  The file name for \a seven icon*/
#define ICON_SETTE			(IMG_PATH "num7.png")
/*!  \def ICON_OTTO
  The file name for \a eight icon*/
#define ICON_OTTO			(IMG_PATH "num8.png")
/*!  \def ICON_NOVE
  The file name for \a nine icon*/
#define ICON_NOVE			(IMG_PATH "num9.png")
/*!  \def ICON_CANC
  The file name for \a cancellation icon*/
#define ICON_CANC			(IMG_PATH "btncanc.png")
/*!  \def ICON_VUOTO
  The file name for \a empty icon*/
#define ICON_VUOTO			(IMG_PATH "btnempty.png")
/*!  \def ICON_DEL
  The file name for \a delete icon*/
#define ICON_DEL			(IMG_PATH "btndel.png")
/*!  \def ICON_SUPERVISIONE
  The file name for \a plant supervision icon*/
#define ICON_SUPERVISIONE (IMG_PATH "supervisione_impianto.png")
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



/****************************************************************
 **  definizione dei vari item
 ****************************************************************/
/*! \enum bannerType
 *  This enum describes the various banner type implemented
 */
enum  bannerType {
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
	CALIBRAZIONE=21,                              /*!<  Calibration */
	ZONANTINTRUS=23,                              /*!<  Anti-intrusion zone */
	IMPIANTINTRUS=24,                             /*!<  Anti-intrusion system */
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
	GR_DIMMER100=44,                              /*!< Gruppo dimmer 100 livelli */
	SORG_RADIO=45,                                /*!< Sorgente radio per Diffusione Sonora Multicanale */
	SORG_AUX=46,                                  /*!< Sorgente aux per Diffusione Sonora Multicanale */
	AMBIENTE=47,                                  /*!< Ambiente diff son. multi */
	INSIEME_AMBIENTI=48,                          /*!< Ins. amb. diff. son. multi */
	POSTO_ESTERNO=49,                             /*!< Posto esterno */
	SORGENTE_MULTIM=50,                           /*!< Sorgente Multimediale per Diffusione Sonora */
	SORGENTE_MULTIM_MC=51,                        /*!< Sorgente Multimediale per Diffusione Sonora Multicanale */
	TERMO_99Z_PROBE=22,                           /*!< Probe controlled by 99 zones thermal regulator */
	TERMO_99Z_PROBE_FANCOIL=52,                   /*!< Probe controlled by 99 zones thermal regulator with fan-coil control */
	TERMO_4Z_PROBE=53,                            /*!< Probe controlled by 4 zones thermal regulator */
	TERMO_4Z_PROBE_FANCOIL=54,                    /*!< Probe controlled by 4 zones thermal regulator with fan-coil control */
	TERMO_NC_EXTPROBE=55,                         /*!< External not-controlled probe */
	TERMO_NC_PROBE=56,                            /*!< Not-controlled probe */
        TERMO_HOME_NC_EXTPROBE=57,                    /*!< Home page external not-controlled probe */
        TERMO_HOME_NC_PROBE=58,                       /*!< Home page not-controlled probe */
	CLASS_STOPNGO=59,                             /*!< StopnGo devices class */
	STOPNGO=60,                                   /*!< StopnGo device */
	STOPNGO_PLUS=61,                              /*!< StopnGo Plus device */
	STOPNGO_BTEST=62,                             /*!< StopnGo BTest device */
	TERMO_99Z=66,                                 /*!< 99 zones thermal regulator */
	TERMO_4Z=68,                                  /*!< 4 zones thermal regulator */
	POWER_AMPLIFIER=69,                           /*!< Power amplifier*/
	POWER_AMPLIFIER_PRESET=70,
	POWER_AMPLIFIER_TREBLE=71,
	POWER_AMPLIFIER_BASS=72,
	POWER_AMPLIFIER_BALANCE=73,
	POWER_AMPLIFIER_LOUD=74
};

/*! \enum pulsType
 * differentiate various type of pulse banner
 */
enum pulsType
{
	AUTOMAZ,                   /*!< driving as a pulse an automation actuator */
	VCT_SERR,                  /*!< driving as a pulse a video-doorentrysystem actuator configured as "lock" */
	VCT_LS,                    /*!< driving as a pulse a video-doorentrysystem actuator configured as "stairlight" */
};


/***************************************************************
 **definizioni di carattere generale
 ***************************************************************/
/*! \def MAX_PATH
 * maximum number of characters describing a file path
 */
#define MAX_PATH 50
/*! \def TIME_RIP_REGOLAZ  time [ms] between two repetitionof a regulation command during the same pressure
 */
#define TIME_RIP_REGOLAZ 500
/*! \def BEEP
 * if not defined it's impossible to have a beep when pressing a button
 */
#define BEEP

/// The simbol of temperature degrees
#define TEMP_DEGREES "\272"


#endif //MAIN_H


