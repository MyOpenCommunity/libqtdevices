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
#define MYPROCESSNAME              "BTouch"
#define MY_FILE_CFG_DEFAULT        "cfg/stack_open.xml"
#define MY_FILE_USER_CFG_DEFAULT   "cfg/conf.xml"
#define MY_PARSER_DEFAULT          "bin/bt_xmlparser"
#define MY_FILE_LOG_DEFAULT        "log/BTouch.log"
#define VERBOSITY_LEVEL_DEFAULT    0
// Parsing XML
#define XML_FILE_IN_DEFAULT        ".bto-to-xml"
#define XML_FILE_OUT_DEFAULT       ".xml-to-bto"
#define PATH_VAR_DEFAULT           "/var"



#define FILE_TEST1				"MODALITA_TEST1"
#define FILE_TEST2				"MODALITA_TEST2"
#define FILE_TEST3				"MODALITA_TEST3"
#define FILE_TEST4				"MODALITA_TEST4"
#define FILE_AGGIORNAMENTO	"MODALITA_AGGIORNAMENTO"
#define FILE_CHANGE_CONF	"BTOUCH_CHANGE_CONF"

/****************************************************************
**  definizione dimensioni schermo
****************************************************************/
#define MAX_WIDTH		240
#define MAX_HEIGHT		320
#define NUM_RIGHE		4

/****************************************************************
**  definizione dei sottomenù
****************************************************************/
enum pagSecLiv{
  AUTOMAZIONE=1,
  ILLUMINAZIONE=2,
  ANTIINTRUSIONE=3,
  CARICHI=4,
  TERMOREGOLAZIONE=5,
  DIFSON=6,
  SCENARI=7,
  IMPOSTAZIONI=8,
  BACK=9,             // tipo di bottone per tornare
  SPECIAL=10,          // tipo di bottone special
  SCHEDULAZIONI=11	  
};



/****************************************************************
**  definizione dei file di immagini
****************************************************************/

#define ICON_CICLA			"cfg/skin/btncycle.png"
#define ICON_OK			"cfg/skin/btnok.png"
#define ICON_ON			"cfg/skin/btnon.png"
#define ICON_OFF			"cfg/skin/btnoff.png"
#define ICON_IMPOSTA		"cfg/skin/audiosetting.png"

#define ICON_INFO			"cfg/skin/btninfo.png"

#define ICON_FRECCIA_SU	"cfg/skin/arrup.png"
#define ICON_FRECCIA_GIU	"cfg/skin/arrdw.png"
#define ICON_FRECCIA_SX	"cfg/skin/arrlf.png"
#define ICON_FRECCIA_DX	"cfg/skin/arrrg.png"

#define ICON_FFWD			"cfg/skin/btnforward.png"
#define ICON_REW			"cfg/skin/btnbackward.png"

#define ICON_OROLOGIO		"cfg/skin/orologio.png"
#define ICON_CALENDARIO	"cfg/skin/calendario.png"

#define ICON_SVEGLIA_ON 	"cfg/skin/svegliaon.png"

#define ICON_PIU			"cfg/skin/btnplus.png"
#define ICON_MENO			"cfg/skin/btnmin.png"

#define ICON_MANUAL_ON	"cfg/skin/btnman.png"
#define ICON_MANUAL_OFF	"cfg/skin/btnmanoff.png"
#define ICON_AUTO_ON		"cfg/skin/btnauto.png"
#define ICON_AUTO_OFF	"cfg/skin/btnautooff.png"
#define ICON_MEM			"cfg/skin/btnmem.png"

#define ICON_ZERO			"cfg/skin/num0.png"
#define ICON_UNO			"cfg/skin/num1.png"
#define ICON_DUE			"cfg/skin/num2.png"
#define ICON_TRE			"cfg/skin/num3.png"
#define ICON_QUATTRO		"cfg/skin/num4.png"
#define ICON_CINQUE		"cfg/skin/num5.png"
#define ICON_SEI			"cfg/skin/num6.png"
#define ICON_SETTE			"cfg/skin/num7.png"
#define ICON_OTTO			"cfg/skin/num8.png"
#define ICON_NOVE			"cfg/skin/num9.png"
#define ICON_CANC			"cfg/skin/btncanc.png"

#define ICON_VUOTO		"cfg/skin/btnempty.png"
#define ICON_DEL			"cfg/skin/btndel.png"




/****************************************************************
**  definizione dei vari item
****************************************************************/
enum  bannerType {
  ATTUAT_AUTOM=0,
  DIMMER=1,
  ATTUAT_AUTOM_INT=2,
  VUOTO_3=3,
  SCENARIO=4,
  GR_ATTUAT_AUTOM=5,
  GR_DIMMER=6,
  CARICO=7,
  ATTUAT_AUTOM_INT_SIC=8,
  ATTUAT_AUTOM_TEMP=9,
  GR_ATTUAT_INT=10,
  ATTUAT_AUTOM_PULS=11,
  ATTUAT_VCT_LS=12,
  ATTUAT_VCT_SERR=13,
  SET_DATA_ORA=14,
  VUOTO_15=15,
  SORGENTE=16,
  SORGENTE_RADIO=17,
  AMPLIFICATORE=18,
  GR_AMPLIFICATORI=19,
  SET_SVEGLIA=20,
  CALIBRAZIONE=21,
  TERMO=22,
  ZONANTINTRUS=23,
  IMPIANTINTRUS=24,
  SUONO=25,
  PROTEZIONE=26,
  VERSIONE=27,
  CONTRASTO=28,
  MOD_SCENARI=29,       // programmazione modulo scenari
  DATA=30,              // item special visualizza data 
  TEMPERATURA=31,       // item special visualizza temperatura
  OROLOGIO=32,          // item special visualizza ora
  ALLARME=33,
  CMDSPECIAL=34	  
};


      
enum pulsType{
                AUTOMAZ,
	VCT_SERR,
	VCT_LS
   };
/***************************************************************
  **definizioni di carattere generale
***************************************************************/
#define MAX_PATH		50
#define TIME_RIP_REGOLAZ	500
#define BEEP
#endif //MAIN_H

