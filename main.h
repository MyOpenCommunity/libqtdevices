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

/****************************************************************
** Default configurazione applicativo
****************************************************************/
/*!  \def MYPROCESSNAME
  Name of the process*/
#define MYPROCESSNAME              "BTouch"
/*!  \def MY_FILE_CFG_DEFAULT
     The default configuration file*/
#define MY_FILE_CFG_DEFAULT        "cfg/stack_open.xml"
/*!  \def MY_FILE_USER_CFG_DEFAULT
     The user-configurationFile name if not specified in stack_open.xml*/
#define MY_FILE_USER_CFG_DEFAULT   "cfg/conf.xml"
/*!  \def MY_PARSER_DEFAUL
     The default parser */
#define MY_PARSER_DEFAULT          "bin/bt_xmlparser"
/*!  \def MY_FILE_LOG_DEFAULT
     The logFile name if not specified in stack_open.xml*/
#define MY_FILE_LOG_DEFAULT        "log/BTouch.log"
/*!  \def VERBOSITY_LEVEL_DEFAULT
     The default verbosity level */
#define VERBOSITY_LEVEL_DEFAULT    0

#define XML_FILE_IN_DEFAULT        ".bto-to-xml"
#define XML_FILE_OUT_DEFAULT       ".xml-to-bto"
#define PATH_VAR_DEFAULT           "/var"


/*!  \def FILE_TEST1
     The file name to watch to generate a RED page*/
#define FILE_TEST1				"MODALITA_TEST1"
/*!  \def FILE_TEST2
     The file name to watch to generate a GREEN page*/
#define FILE_TEST2				"MODALITA_TEST2"
/*!  \def FILE_TEST3
     The file name to watch to generate a BLUE page*/
#define FILE_TEST3				"MODALITA_TEST3"
/*!  \def FILE_AGGIORNAMENTO
     The file name to watch to generate the \a configuration page*/
#define FILE_AGGIORNAMENTO	"MODALITA_AGGIORNAMENTO"
/*!  \def FILE_CHANGE_CONF
     The file to generate when changing the user-configurationFile to avoid beinf resetted from bt_processi*/
#define FILE_CHANGE_CONF	"BTOUCH_CHANGE_CONF"
/*!  \def FILE_WDT
     The file name to create for software watchdog*/
#define FILE_WDT				"/var/tmp/bticino/bt_wd/BTouch_qws"

/****************************************************************
**  definizione dimensioni schermo
****************************************************************/
/*!  \def MAX_WIDTH 
     Maximum width of the screen*/
#define MAX_WIDTH		240		
/*!  \def MAX_HEIGHT 
     Maximum height of the screen*/
#define MAX_HEIGHT	320
/*!  \def NUM_RIGHE 
     Number of lines shown on the screen*/
#define NUM_RIGHE		4

/****************************************************************
**  definizione dei sottomenù
****************************************************************/
   /*! \enum pagSecLiv
     This enum describes the various kind of pages*/
enum pagSecLiv{
  AUTOMAZIONE=1,				/*!< Automation system*/
  ILLUMINAZIONE=2,				/*!< Lighting system*/
  ANTIINTRUSIONE=3,			/*!< Anti-intrusion system*/
  CARICHI=4,					/*!< Appliances managing system*/
  TERMOREGOLAZIONE=5,		/*!< Thermoregulation system*/
  DIFSON=6,						/*!< Sound diffusion system*/
  SCENARI=7,					/*!< Scenarios managing*/	
  IMPOSTAZIONI=8,				/*!< Settings */
  BACK=9,             					/*!< Back button - placed here for convenience*/  
  SPECIAL=10,          				/*!< Special button - placed here for convenience*/
  SCHEDULAZIONI=11	  		/*!< Schedulations page*/
};



/****************************************************************
**  definizione dei file di immagini
****************************************************************/
/*!  \def ICON_CICLA
     The file name for \a changing icon*/
#define ICON_CICLA			"cfg/skin/btncycle.png"
/*!  \def ICON_OK
     The file name for \a ok icon*/
#define ICON_OK			"cfg/skin/btnok.png"
/*!  \def ICON_ON
     The file name for \a on icon*/
#define ICON_ON			"cfg/skin/btnon.png"
/*!  \def ICON_OFF
     The file name for \a off icon*/
#define ICON_OFF			"cfg/skin/btnoff.png"
/*!  \def ICON_IMPOSTA
     The file name for \a setting icon*/
#define ICON_IMPOSTA		"cfg/skin/audiosetting.png"
/*!  \def ICON_INFO
     The file name for \a information icon*/
#define ICON_INFO			"cfg/skin/btninfo.png"
/*!  \def ICON_FRECCIA_SU
     The file name for \a up-arrow icon*/
#define ICON_FRECCIA_SU	"cfg/skin/arrup.png"
/*!  \def ICON_FRECCIA_GIU
     The file name for \a down-arrow icon*/
#define ICON_FRECCIA_GIU	"cfg/skin/arrdw.png"
/*!  \def ICON_FRECCIA_SX
     The file name for \a left-arrow icon*/
#define ICON_FRECCIA_SX	"cfg/skin/arrlf.png"
/*!  \def ICON_FRECCIA_DX
     The file name for \a right-arrow icon*/
#define ICON_FRECCIA_DX	"cfg/skin/arrrg.png"
/*!  \def ICON_FFWD
     The file name for \a feed \a forward icon*/
#define ICON_FFWD			"cfg/skin/btnforward.png"
/*!  \def ICON_REW
     The file name for \a rewind icon*/
#define ICON_REW			"cfg/skin/btnbackward.png"
/*!  \def ICON_OROLOGIO
     The file name for \a clock icon*/
#define ICON_OROLOGIO		"cfg/skin/orologio.png"
/*!  \def ICON_CALENDARIO
     The file name for \a calendar icon*/
#define ICON_CALENDARIO	"cfg/skin/calendario.png"
/*!  \def ICON_SVEGLIA_ON
     The file name for \a alarm \a clock icon*/
#define ICON_SVEGLIA_ON 	"cfg/skin/svegliaon.png"
/*!  \def ICON_PIU
     The file name for \a plus icon*/
#define ICON_PIU			"cfg/skin/btnplus.png"
/*!  \def ICON_MENO
     The file name for \a minus icon*/
#define ICON_MENO			"cfg/skin/btnmin.png"
/*!  \def ICON_MANUAL_ON
     The file name for \a manual \a selected icon*/
#define ICON_MANUAL_ON	"cfg/skin/btnman.png"
/*!  \def ICON_MANUAL_OFF
     The file name for \a manual \a unselected icon*/
#define ICON_MANUAL_OFF	"cfg/skin/btnmanoff.png"
/*!  \def ICON_AUTO
     The file name for \a automatic \a selected icon*/
#define ICON_AUTO_ON		"cfg/skin/btnauto.png"
/*!  \def ICON_AUTO_OFF
     The file name for \a automatic \a unselected icon*/
#define ICON_AUTO_OFF	"cfg/skin/btnautooff.png"
/*!  \def ICON_MEM
     The file name for \a memorization icon*/
#define ICON_MEM			"cfg/skin/btnmem.png"
/*!  \def ICON_ZERO
     The file name for \a zero icon*/
#define ICON_ZERO			"cfg/skin/num0.png"
/*!  \def ICON_UNO
     The file name for \a one icon*/
#define ICON_UNO			"cfg/skin/num1.png"
/*!  \def ICON_DUE
     The file name for \a two icon*/
#define ICON_DUE			"cfg/skin/num2.png"
/*!  \def ICON_TRE
     The file name for \a three icon*/
#define ICON_TRE			"cfg/skin/num3.png"
/*!  \def ICON_QUATTRO
     The file name for \a for icon*/
#define ICON_QUATTRO		"cfg/skin/num4.png"
/*!  \def ICON_CINQUE
     The file name for \a five icon*/
#define ICON_CINQUE		"cfg/skin/num5.png"
/*!  \def ICON_SEI
     The file name for \a six icon*/
#define ICON_SEI			"cfg/skin/num6.png"
/*!  \def ICON_SETTE
     The file name for \a seven icon*/
#define ICON_SETTE			"cfg/skin/num7.png"
/*!  \def ICON_OTTO
     The file name for \a eight icon*/
#define ICON_OTTO			"cfg/skin/num8.png"
/*!  \def ICON_NOVE
     The file name for \a nine icon*/
#define ICON_NOVE			"cfg/skin/num9.png"
/*!  \def ICON_CANC
     The file name for \a cancellation icon*/
#define ICON_CANC			"cfg/skin/btncanc.png"
/*!  \def ICON_VUOTO
     The file name for \a empty icon*/
#define ICON_VUOTO		"cfg/skin/btnempty.png"
/*!  \def ICON_DEL
     The file name for \a delete icon*/
#define ICON_DEL			"cfg/skin/btndel.png"




/****************************************************************
**  definizione dei vari item
****************************************************************/
   /*! \enum bannerType
     This enum describes the various banner type implemented*/
enum  bannerType {
  ATTUAT_AUTOM=0,			/*!<  Automation actuator*/
  DIMMER=1,						/*!<  Dimmer*/
  ATTUAT_AUTOM_INT=2,		/*!<  Interblocked automation actuator*/
  VUOTO_3=3,					/*!<  Not used*/
  SCENARIO=4,					/*!<  Scenario*/
  GR_ATTUAT_AUTOM=5,		/*!<  Automation actuator's group*/
  GR_DIMMER=6,				/*!<  Dimmer's group*/
  CARICO=7,						/*!<  Appliance*/
  ATTUAT_AUTOM_INT_SIC=8,	/*!<  Secure interblocked actuator*/
  ATTUAT_AUTOM_TEMP=9,		/*!<  Timed automation actuator*/
  GR_ATTUAT_INT=10,			/*!<  Interblocked actuator's group*/
  ATTUAT_AUTOM_PULS=11,		/*!<  Pulse automation actuator*/
  ATTUAT_VCT_LS=12,			/*!<  Stairlight video-doorentrysystem actuator*/
  ATTUAT_VCT_SERR=13,		/*!<  Lock video-doorentrysystem actuator*/
  SET_DATA_ORA=14,			/*!<  Time setting*/
  VUOTO_15=15,					/*!<  Not used*/
  SORGENTE=16,				/*!<  Auxiliary sound source*/
  SORGENTE_RADIO=17,			/*!<  FM tuner*/
  AMPLIFICATORE=18,			/*!<  Amplifier*/
  GR_AMPLIFICATORI=19,		/*!<  Amplifier's group*/
  SET_SVEGLIA=20,				/*!<  AlarmClock setting*/
  CALIBRAZIONE=21,				/*!<  Calibration*/
  TERMO=22,					/*!<  Thermoregulation zone*/
  ZONANTINTRUS=23,			/*!<  Anti-intrusion zone*/
  IMPIANTINTRUS=24,			/*!<  Anti-intrusion system*/
  SUONO=25,					/*!<  Beep*/
  PROTEZIONE=26,				/*!<  Password's settings*/
  VERSIONE=27,					/*!<  Version*/
  CONTRASTO=28,				/*!<  Contrast*/
  MOD_SCENARI=29,       			/*!<  DIN scenario module's scenario*/
  DATA=30,                				/*!<  Date*/
  TEMPERATURA=31,       			/*!<  Temperature*/
  OROLOGIO=32,          			/*!<  Clock*/
  ALLARME=33,					/*!<  Alarm*/
  CMDSPECIAL=34	  			/*!<  Special command*/
};

/*! banTesti: text utilized during the initialization sequence when the menu is built up*/
const char banTesti[34][20] = {"ATTUAT_AUTOM","DIMMER","ATTUAT_AUTOM_INT","VUOTO","SCENARIO","GR_ATT_INT","GR_DIMMER","CARICO",\
                            "ATT_AUT_INT_SIC","ATT_AUT_TEMP","GR_ATT_INT","ATT_AUT_PULS","ATT_VCT_LS","ATT_VCT_SER","SET_DATA","VUOTO",\
                            "SORGENTE","SORG_RADIO","AMPLI", "GR_AMPLI","SET_SVEGLIA","CALIB","TERMO","ZONANTI","IMPANTI","SUONO","PROT","VERS",\
                            "CONTR","MOD_SCEN","DATA;TEMP","TIME","ALL","SPECIAL" };
/*! pagTesti: text utilized during the initialization sequence when the menu is built up*/
const char pagTesti[11][20] = {"AUTOMAZIONE","ILLUMINAZIONE","ANTINTRUSIONE","CARICHI","TERMOREG","DIFSON","SCENARI","IMPOSTAZ",\
                            "BACK","SPECIAL"," SCHEDULAZ" };
                
/*! \enum pulsType
  differentiate various type of pulse banner*/
enum pulsType{
                AUTOMAZ,	/*!< driving as a pulse an automation actuator*/
                VCT_SERR,	/*!< driving as a pulse a video-doorentrysystem actuator configured as "lock"*/
                VCT_LS		/*!< driving as a pulse a video-doorentrysystem actuator configured as "stairlight"*/
   };
/***************************************************************
  **definizioni di carattere generale
***************************************************************/
/*! \def MAX_PATH
  maximum number of characters describing a file path*/
#define MAX_PATH		50
/*! \def TIME_RIP_REGOLAZ
  time [ms] between two repetitionof a regulation command during the same pressure*/
#define TIME_RIP_REGOLAZ	500
/*! \def BEEP
  if not defined it's impossible to have a beep qhen pressing a button*/
#define BEEP
#endif //MAIN_H

