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
  SPECIAL=10          // tipo di bottone special
};



/****************************************************************
**  definizione dei file di immagini
****************************************************************/

#define ICON_CICLA		"cfg/skin/btncycle.png"
#define ICON_OK		"cfg/skin/btnok.png"
#define ICON_ON		"cfg/skin/btnon.png"
#define ICON_OFF		"cfg/skin/btnoff.png"
#define ICON_IMPOSTA		"cfg/skin/audiosetting.png"

#define ICON_INFO		"cfg/skin/btninfo.png"

#define ICON_FRECCIA_SU	"cfg/skin/arrup.png"
#define ICON_FRECCIA_GIU	"cfg/skin/arrdw.png"
#define ICON_FRECCIA_SX	"cfg/skin/arrlf.png"
#define ICON_FRECCIA_DX	"cfg/skin/arrrg.png"

#define ICON_FFWD		"cfg/skin/btnforward.png"
#define ICON_REW		"cfg/skin/btnbackward.png"

#define ICON_OROLOGIO		"cfg/skin/orologio.png"
#define ICON_CALENDARIO	"cfg/skin/calendario.png"

#define ICON_SVEGLIA_ON 	"cfg/skin/svegliaon.png"

#define ICON_PIU		"cfg/skin/btnplus.png"
#define ICON_MENO		"cfg/skin/btnmin.png"

#define ICON_MANUAL_ON	"cfg/skin/btnman.png"
#define ICON_MANUAL_OFF	"cfg/skin/btnmanoff.png"
#define ICON_AUTO_ON		"cfg/skin/btnauto.png"
#define ICON_AUTO_OFF	"cfg/skin/btnautooff.png"
#define ICON_MEM		"cfg/skin/btnmem.png"

#define ICON_ZERO		"cfg/skin/num0.png"
#define ICON_UNO		"cfg/skin/num1.png"
#define ICON_DUE		"cfg/skin/num2.png"
#define ICON_TRE		"cfg/skin/num3.png"
#define ICON_QUATTRO		"cfg/skin/num4.png"
#define ICON_CINQUE		"cfg/skin/num5.png"
#define ICON_SEI		"cfg/skin/num6.png"
#define ICON_SETTE		"cfg/skin/num7.png"
#define ICON_OTTO		"cfg/skin/num8.png"
#define ICON_NOVE		"cfg/skin/num9.png"
#define ICON_CANC		"cfg/skin/btncanc.png"

#define ICON_SONDAOFF	"cfg/skin/sondaantigelo.png"
#define ICON_SONDAANTI	"cfg/skin/sondaoff.png"

#define ICON_VUOTO		"cfg/skin/btnempty.png"


/*
  #define ICON_DIMMER_ACC	"cfg/skin/dimonl.png"
#define ICON_DIMMER_SPE	"cfg/skin/dimoff.png"
#define ICON_DIMMERS_SX	"cfg/skin/dimgrponsx.png"
#define ICON_DIMMERS_DX	"cfg/skin/dimgrpondx.png"
#define ICON_AMPLI_ACC	"cfg/skin/amplonl.png"
#define ICON_AMPLI_SPE	"cfg/skin/amploffl.png"
#define ICON_AMPLIS_DX	"cfg/skin/amplgrpl5ondx.png"
#define ICON_AMPLIS_SX	"cfg/skin/amplgrpl5onsx.png"
  #define ICON_USCITA_80	"cfg/skin/uscita.png"
#define ICON_ILLUMINAZ_80	"cfg/skin/appIllum.png"
#define ICON_ANTIINTRUS_80 	"cfg/skin/appant.png"
#define ICON_CARICHI_80 	"cfg/skin/appccar.png"
#define ICON_AUTOMAZIONE_80 "cfg/skin/appmov.png"
#define ICON_IMPOSTAZIONI_80 	"cfg/skin/appconf.png"
#define ICON_TERMOREGOL_80 	"cfg/skin/appterm.png"
#define ICON_SCENARI_80 	"cfg/skin/appscen.png"
#define ICON_DIFSON_80 	"cfg/skin/appdiff.png"

#define ICON_LUCE_ACCESA 	"cfg/skin/lampon.png"
#define ICON_LUCE_SPENTA 	"cfg/skin/lampoff.png"
#define ICON_LUCI	 	"cfg/skin/lampgrpon.png"

#define ICON_LUCE_ACCESA2 	"cfg/skin/lamptime.png"
#define ICON_LUCE_SPENTA2 	"cfg/skin/lamptimeoff.png"

#define ICON_RUBINETTO_ACCESO2 	"cfg/skin/idrotime.png"
#define ICON_RUBINETTO_SPENTO2 	"cfg/skin/idrotimeoff.png"

#define ICON_TENDA 		"cfg/skin/tenda.png"
#define ICON_CANCELLO	"cfg/skin/canc.png"
#define ICON_STOP_MOV	"cfg/skin/stopmov.png"
#define ICON_STOP_MOVS	"cfg/skin/stopmovs.png"

#define ICON_ON_60		"cfg/skin/btnon.png"
#define ICON_OFF_60		"cfg/skin/btnoff.png"
#define ICON_ON_80		"cfg/skin/btnon.png"
#define ICON_OFF_80		"cfg/skin/btnoff.png"
#define ICON_STOP_60		"cfg/skin/btnstop.png"
//"cfg/skin/stopmovs.png"
#define ICON_UP_60		"cfg/skin/btnopen.png"
#define ICON_DOWN_60		"cfg/skin/btnclose.png"
#define ICON_UP_SIC_60	"cfg/skin/btnopensic.png"
#define ICON_DOWN_SIC_60	"cfg/skin/btnclosesic.png"
#define ICON_CICLA_60		"cfg/skin/audioinchg.png"
  #define ICON_OK_80		"cfg/skin/btnok.png"
#define ICON_IMPOSTA_60	"cfg/skin/audiosetting.png"
  #define ICON_INFO_60		"cfg/skin/btninfo60.png"
  #define ICON_VENTIL_ACC	"cfg/skin/venton.png"
#define ICON_VENTIL_SPE	"cfg/skin/ventoff.png"

#define ICON_KEY_60		"cfg/skin/funcautom.png"
  
#define ICON_ZONA_PARZ	"cfg/skin/zona.png"
#define ICON_ZONA_NONPARZ	"cfg/skin/zonap.png"
#define ICON_IMP_INS		"cfg/skin/antifurtoins.png"
#define ICON_IMP_DIS		"cfg/skin/antifurtodis.png"
                                                                                                                                                                                                    
#define ICON_PWD_ON             "cfg/skin/pswon.png"
#define ICON_PWD_OFF            "cfg/skin/pswoff.png"

#define ICON_SCEN                            "cfg/skin/scen.png"
#define ICON_PROG_SCEN	  "cfg/skin/progscen.png"
#define ICON_INFO_SCEN	  "cfg/skin/setscen.png"
#define ICON_STOP_PROG_SCEN "cfg/skin/btnstop.png"
#define ICON_DEL_SCEN	  "cfg/skin/delscen.png"

#define ICON_SVEGLIA_OFF 	"cfg/skin/svegliaoff.png"
#define ICON_SVEGLIA_ON_60 	"cfg/skin/svegliaon_60.png"
#define ICON_SVEGLIA_OFF_60 	"cfg/skin/svegliaoff_60.png"
*/


/****************************************************************
**  definizione del background
****************************************************************/
/*#define	BG_R		77 
#define	BG_G		75
#define	BG_B		100
*/
/*#define	BG_R		150 
#define	BG_G		180
#define	BG_B		220*/

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

