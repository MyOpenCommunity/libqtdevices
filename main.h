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
**  definizione dimensioni schermo
****************************************************************/
#define MAX_WIDTH		240
#define MAX_HEIGHT		320
#define NUM_RIGHE		4

/****************************************************************
**  definizione dei sottomenù
****************************************************************/
enum pagSecLiv{    
                                 USCITA,
		AUTOMAZIONE,
		ILLUMINAZIONE,
		ANTIINTRUSIONE,
		CARICHI,
		TERMOREGOLAZIONE,
		DIFSON,
		SCENARI,
		IMPOSTAZIONI
	    };


/****************************************************************
**  definizione dei file di immagini
****************************************************************/
#define ICON_USCITA_80	"cfg/skin/uscita.png"
#define ICON_ILLUMINAZ_80	"cfg/skin/appIllum.png"
#define ICON_ANTIINTRUS_80 	"cfg/skin/appAnt.png"
#define ICON_CARICHI_80 	"cfg/skin/appCCar.png"
#define ICON_AUTOMAZIONE_80 "cfg/skin/appMov.png"
#define ICON_IMPOSTAZIONI_80 	"cfg/skin/appConf.png"
#define ICON_TERMOREGOL_80 	"cfg/skin/appTerm.png"
#define ICON_SCENARI_80 	"cfg/skin/appScen.png"
#define ICON_DIFSON_80 	"cfg/skin/appDiff.png"

#define ICON_LUCE_ACCESA 	"cfg/skin//lampOn.png"
#define ICON_LUCE_SPENTA 	"cfg/skin//lampOff.png"
#define ICON_LUCI	 	"cfg/skin//lampGrpOn.png"

#define ICON_LUCE_ACCESA2 	"cfg/skin//lampTime.png"
#define ICON_LUCE_SPENTA2 	"cfg/skin//lampTimeOff.png"

#define ICON_RUBINETTO_ACCESO2 	"cfg/skin//idroTime.png"
#define ICON_RUBINETTO_SPENTO2 	"cfg/skin//idroTimeOff.png"

#define ICON_TENDA 		"cfg/skin//tenda.png"
#define ICON_CANCELLO	"cfg/skin//canc.png"
#define ICON_STOP_MOV	"cfg/skin//stopMov.png"
#define ICON_STOP_MOVS	"cfg/skin//stopMovs.png"

#define ICON_ON_60		"cfg/skin//btnOn.png"
#define ICON_OFF_60		"cfg/skin//btnOff.png"
#define ICON_ON_80		"cfg/skin//btnOn.png"
#define ICON_OFF_80		"cfg/skin//btnOff.png"
#define ICON_STOP_60		"cfg/skin//stopMovs.png"
#define ICON_UP_60		"cfg/skin//btnOpen.png"
#define ICON_DOWN_60		"cfg/skin//btnClose.png"
#define ICON_UP_SIC_60	"cfg/skin//btnOpenSic.png"
#define ICON_DOWN_SIC_60	"cfg/skin//btnCloseSic.png"
#define ICON_CICLA_60		"cfg/skin//audioInChg.png"
#define ICON_OK_60		"cfg/skin//btnOk.png"
#define ICON_OK_80		"cfg/skin//btnOk.png"
#define ICON_IMPOSTA_60	"cfg/skin//audioSetting.png"
#define ICON_INFO		"cfg/skin//btnInfo.png"

#define ICON_FRECCIA_SU	"cfg/skin//arrUp.png"
#define ICON_FRECCIA_GIU	"cfg/skin//arrDw.png"
#define ICON_FRECCIA_SX	"cfg/skin//arrLf.png"
#define ICON_FRECCIA_DX	"cfg/skin//arrRg.png"

#define ICON_DIMMER_ACC	"cfg/skin//dimOnL.png"
#define ICON_DIMMER_SPE	"cfg/skin//dimOff.png"
#define ICON_DIMMERS_SX	"cfg/skin//dimGrpOnSx.png"
#define ICON_DIMMERS_DX	"cfg/skin//dimGrpOnDx.png"
#define ICON_AMPLI_ACC	"cfg/skin//amplOnL.png"
#define ICON_AMPLI_SPE	"cfg/skin//amplOffL.png"
#define ICON_AMPLIS_DX	"cfg/skin//amplGrpL5OnDx.png"
#define ICON_AMPLIS_SX	"cfg/skin//amplGrpL5OnSx.png"

#define ICON_FFWD_60		"cfg/skin//btnForward.png"
#define ICON_REW_60		"cfg/skin//btnBackward.png"

#define ICON_VENTIL_ACC	"cfg/skin//ventOn.png"
#define ICON_VENTIL_SPE	"cfg/skin//ventOff.png"

#define ICON_KEY_60		"cfg/skin//funcAutom.png"
#define ICON_OROLOGIO		"cfg/skin//orologio.png"
#define ICON_SVEGLIA_ON 	"cfg/skin//svegliaON.png"
#define ICON_SVEGLIA_OFF 	"cfg/skin//svegliaOFF.png"
#define ICON_SVEGLIA_ON_60 	"cfg/skin//svegliaON_60.png"
#define ICON_SVEGLIA_OFF_60 	"cfg/skin//svegliaOFF_60.png"

#define ICON_PIU		"cfg/skin//btnPlus.png"
#define ICON_MENO		"cfg/skin//btnMin.png"
#define ICON_MANUAL_ON	"cfg/skin//btnMan.png"
#define ICON_MANUAL_OFF	"cfg/skin//btnManOff.png"
#define ICON_AUTO_ON		"cfg/skin//btnAuto.png"
#define ICON_AUTO_OFF	"cfg/skin//btnAutoOff.png"
#define ICON_MEM		"cfg/skin//memo.png"

#define ICON_ZERO		"cfg/skin//num0.png"
#define ICON_UNO		"cfg/skin//num1.png"
#define ICON_DUE		"cfg/skin//num2.png"
#define ICON_TRE		"cfg/skin//num3.png"
#define ICON_QUATTRO		"cfg/skin//num4.png"
#define ICON_CINQUE		"cfg/skin//num5.png"
#define ICON_SEI		"cfg/skin//num6.png"
#define ICON_SETTE		"cfg/skin//num7.png"
#define ICON_OTTO		"cfg/skin//num8.png"
#define ICON_NOVE		"cfg/skin//num9.png"
#define ICON_CANC		"cfg/skin//btnCanc.png"

#define ICON_SONDAOFF	"cfg/skin//sondaAntigelo.png"
#define ICON_SONDAANTI	"cfg/skin//sondaOFF.png"

#define ICON_ZONA_PARZ	"cfg/skin/zona.png"
#define ICON_ZONA_NONPARZ	"cfg/skin/zonaP.png"
#define ICON_IMP_INS		"cfg/skin/antifurtoIns.png"
#define ICON_IMP_DIS		"cfg/skin/antifurtoDis.png"

#define ICON_VUOTO		"cfg/skin//btnEmpty.png"

/****************************************************************
**  definizione del background
****************************************************************/
#define	BG_R		77 
#define	BG_G		75
#define	BG_B		100

/****************************************************************
**  definizione dei vari item
****************************************************************/
enum  bannerType {
	ATTUAT_AUTOM,
	DIMMER,
	ATTUAT_AUTOM_INT,
	SCENARIO,
	GR_ATTUAT_AUTOM, 
	GR_DIMMER,
	CARICO,
	ATTUAT_AUTOM_INT_SIC,
	ATTUAT_AUTOM_TEMP,
	GR_ATTUAT_INT,
	ATTUAT_AUTOM_PULS,
	ATTUAT_VCT_LS,
	ATTUAT_VCT_SERR,
	SET_DATA_ORA,
	SORGENTE,
	SORGENTE_RADIO,	
	AMPLIFICATORE,
	GR_AMPLIFICATORI,
	SET_SVEGLIA,
	CALIBRAZIONE,
	TERMO,
	ZONANTINTRUS,
	IMPIANTINTRUS,
	SUONO,
	PROTEZIONE,
	VERSIONE,
	CONTRASTO
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

