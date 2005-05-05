/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** items.h
**
**definizioni dei vari items implementati
**
****************************************************************/
#include <qwidget.h>

#include "banner.h"
#include "main.h"
#include <qptrlist.h>

/*****************************************************************
**Dimmer
****************************************************************/

#ifndef  DIMMER_H
#define DIMMER_H

#include "bannregolaz.h"


class dimmer : public bannRegolaz
{
    Q_OBJECT
public:
     dimmer( QWidget *, const char *,char*,char*,char*,char*,char*);     
     void inizializza();
public slots:
    void gestFrame(char*);
    void Accendi();
    void Spegni();
    void Aumenta();
    void Diminuisci();
signals:
   // void sendFrame(char *);          
private:
     char value;
     char pul;
     char gruppi[4];
};

#endif //DIMMER_H

/*****************************************************************
**attuatore automazione
****************************************************************/

#ifndef ATTUAT_AUTOM_H
#define ATTUAT_AUTOM_H

#include "bannonoff.h"

class attuatAutom : public bannOnOff
{
    Q_OBJECT
public:
     attuatAutom( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0);   
          void inizializza();
public slots:
     void gestFrame(char*);
     void Attiva();
     void Disattiva();
     char* getChi();
signals:
  //  void sendFrame(char *);      
private:    
     char gruppi[4];    
};


#endif //ATTUAT_AUTOM_H

/*****************************************************************
**gruppo automazione
****************************************************************/

#ifndef GR_ATTUAT_AUTOM_H
#define GR_ATTUAT_AUTOM_H

#include "bannonoff.h"

class grAttuatAutom : public bannOnOff
{
    Q_OBJECT
public:
     grAttuatAutom  ( QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Icon=NULL,int periodo=0,int numFrame=0);       
     void setAddress(void*);	
//      void inizializza();
public slots:
     void Attiva();
     void Disattiva();
signals:
  //  void sendFrame(char *);           
private:
    QPtrList<char> elencoDisp;
   
};


#endif //GR_ATTUAT_AUTOM_H

/*****************************************************************
**gruppo dimmer
****************************************************************/

#ifndef GR_DIMMER_H
#define GR_DIMMER_H

#include "bannregolaz.h"

class grDimmer : public bannRegolaz
{
    Q_OBJECT
public:
     grDimmer  ( QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Iconsx=NULL,char*Icondx=NULL,int periodo=0,int numFrame=0);       
     void setAddress(void*);	    
     void inizializza();
public slots:
     void Attiva();
     void Disattiva();
     void Aumenta();
     void Diminuisci();
signals:
 //   void sendFrame(char *);           
private:
    QPtrList<char> elencoDisp;
   
};

#endif //GR_DIMMER_H

/*****************************************************************
**scenario
****************************************************************/

#ifndef SCENARIO_H
#define SCENARIO_H

#include "bannondx.h"

class scenario : public bannOnSx 
{
    Q_OBJECT
public:
     scenario( sottoMenu  *, const char * ,char*,char*);
     void inizializza();
signals:
  //  void sendFrame(char *);           
public slots:
     void Attiva();
private:

};


#endif //SCENARIO_H

/*****************************************************************
**carico
****************************************************************/

#ifndef CARICO_H
#define CARICO_H

#include "bannondx.h"

class carico : public bannOnSx 
{
    Q_OBJECT
public:
     carico(sottoMenu  *, const char * ,char*,char*);     
     void inizializza();
public slots:
     void gestFrame(char*);
     void Attiva();
signals:
 //   void sendFrame(char *);      
private:

};


#endif //CARICO_H

/*****************************************************************
**attuatore automazione interbloccato
****************************************************************/

#ifndef ATTUAT_AUTOM_INT_H
#define ATTUAT_AUTOM_INT_H

#include "bannonoff.h"

class attuatAutomInt : public bannOnOff
{
    Q_OBJECT
public:
     attuatAutomInt ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0);     
       void inizializza();   
   
public slots:
     void gestFrame(char*);
     void analizzaUp();
     void analizzaDown();
signals:
 //   void sendFrame(char *);           
private:
     char uprunning,dorunning;  
     char nomeFile1[MAX_PATH],nomeFile2[MAX_PATH],nomeFile3[MAX_PATH];
};


#endif //ATTUAT_AUTOM_INT_H


/*****************************************************************
**attuatore automazione interbloccato sicuro
****************************************************************/

#ifndef ATTUAT_AUTOM_INT_SIC_H
#define ATTUAT_AUTOM_INT_SIC_H

#include "bannonoff.h"

class attuatAutomIntSic : public bannOnOff
{
    Q_OBJECT
public:
     attuatAutomIntSic ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0); 
     void inizializza();
public slots:
     void gestFrame(char*);
     void doPres();
     void upPres();
     void doRil();
     void upRil();
signals:
 //   void sendFrame(char *);           
private:
     char uprunning,dorunning;
     char nomeFile1[MAX_PATH],nomeFile2[MAX_PATH],nomeFile3[MAX_PATH];
};


#endif //ATTUAT_AUTOM_INT_SIC_H


/*****************************************************************
**attuatore automazione temporizzato
****************************************************************/

#ifndef ATTUAT_AUTOM_TEMP_H
#define ATTUAT_AUTOM_TEMP_H

static const char* tempi[]={"1'","2'","3'","4'","5'","15'"};//,"30''","0,5''"} ;
     
#include "bannonoff2scr.h"

class attuatAutomTemp : public bannOnOff2scr
{
    Q_OBJECT
public:
     attuatAutomTemp ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0);
     void inizializza();
public slots:
     void gestFrame(char*);
     void Attiva();
     void CiclaTempo();
signals:
  //  void sendFrame(char *);           
private:
     uchar cntTempi;   
};


#endif //ATTUAT_AUTOM_TEMP_H


/*****************************************************************
**gruppo automazione
****************************************************************/

#ifndef GR_ATTUAT_INT_H
#define GR_ATTUAT_INT_H

#include "bann3but.h"

class grAttuatInt : public bann3But
{
    Q_OBJECT
public:
     grAttuatInt(QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Icon=NULL,int periodo=0,int numFrame=0); 

     void setAddress(void*);	    
     void inizializza();
public slots:
     void Alza();
     void Abbassa();
     void Ferma();
signals:
//    void sendFrame(char *);           
private:
    QPtrList<char> elencoDisp;
 };


#endif //GR_ATTUAT_INT_H


/*****************************************************************
**attuatore a pulsante (automaz e vct)
****************************************************************/

#ifndef ATTUAT_PULS_H
#define ATTUAT_PULS_H

#include "bannpuls.h"

class attuatPuls : public bannPuls
{
    Q_OBJECT
public:
     attuatPuls( QWidget *parent=0, const char *name=NULL ,char*indirizzi=NULL,char* IconaSx=NULL,/*char* IconaDx=NULL,*/char*IconActive=NULL,char tipo=0,int periodo=0,int numFrame=0); 
     void inizializza();
public slots:
     void Attiva();
     void Disattiva();
signals:
 //   void sendFrame(char *);           
private:
     char type;
};



#endif //ATTUAT_PULS_H


/*****************************************************************
**Amplificatore
****************************************************************/

#ifndef  AMPLIFICATORE_H
#define AMPLIFICATORE_H

#include "bannregolaz.h"


class amplificatore : public bannRegolaz
{
    Q_OBJECT
public:
     amplificatore( QWidget *, const char *,char*,char*,char*,char*,char*);          
     void inizializza();
public slots:
    void gestFrame(char*);
    void Accendi();
    void Spegni();
    void Aumenta();
    void Diminuisci();
signals:
//    void sendFrame(char *);          
private:
     char value;
};

#endif //AMPLIFICATORE_H

/*****************************************************************
**gruppo amplificatori
****************************************************************/

#ifndef GR_AMPLI_H
#define GR_AMPLI_H

#include "bannregolaz.h"

class grAmplificatori : public bannRegolaz
{
    Q_OBJECT
public:
     grAmplificatori  ( QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Iconsx=NULL,char*Icondx=NULL,int periodo=0,int numFrame=0);       
     void inizializza();
     void setAddress(void*);	    
public slots:
     void Attiva();
     void Disattiva();
     void Aumenta();
     void Diminuisci();
signals:
 //   void sendFrame(char *);           
private:
    QPtrList<char> elencoDisp;
    
};

#endif //GR_AMPLI_H

/*****************************************************************
**Sorgente_Radio
****************************************************************/

#ifndef  SORG_RADIO_H
#define SORG_RADIO_H

#include "bannciclaz.h"
#include "radio.h"

class banradio : public bannCiclaz
{
        Q_OBJECT
public:
    banradio( QWidget *parent,const char *name,char* indirizzo );
   void 	inizializza();
   void 	setBGColor(int, int , int );
   void 	setFGColor(int , int , int );		
   void 	setBGColor(QColor );
   void 	setFGColor(QColor );	
public slots:
     void 	gestFrame(char*);
    void	show();
    void 	ciclaSorg();
    void 	decBrano();
    void 	aumBrano();
    void 	SetText(const char *);
    void 	aumFreqAuto();
    void	decFreqAuto();
    void	aumFreqMan();
    void	decFreqMan();
    void 	changeStaz();
    void	memoStaz(uchar);
    void	richFreq();
   void 	stopRDS();
   void	startRDS();    
signals:
   // void 	sendFrame(char *);          
private:    
    radio*	 myRadio;    
};

#endif //SORGENTE_RADIO_H

/*****************************************************************
**Sorgente
****************************************************************/

#ifndef  SORGENTE_H
#define SORGENTE_H

#include "bannciclaz.h"

class sorgente : public bannCiclaz
{
        Q_OBJECT
public:
    sorgente( QWidget *parent,const char *name,char* indirizzo );
   void inizializza();
public slots:
     void gestFrame(char*);
    void ciclaSorg();
    void decBrano();
    void aumBrano();
signals:
  //  void sendFrame(char *);          
};

#endif //SORGENTE_H


/*****************************************************************
**Zona Termoregolazione
****************************************************************/

#ifndef  THERMO_H
#define THERMO_H

#include "banntermo.h"

class termoPage : public bannTermo
{
        Q_OBJECT
public:
    termoPage ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaMeno=NULL,char* IconaPiu=NULL,char*IconaMansx=NULL,char*IconaAuto=NULL, QColor 	SecondForeground=QColor(0,0,0));      
   void 	inizializza();
   char* 	getChi();
public slots:
     void 	gestFrame(char*);
     void	aumSetpoint();
     void	decSetpoint();
     void	autoMan();
     void	show();
     void	hide();
signals:
  // void 	sendFrame(char *);          
private:     
    uchar  statoSonda; 
};


enum statisonda{
                                 S_AUTO,
		 S_MAN,
		 S_ANTIGELO,
		 S_OFF,
		     };
#endif //THERMO_H

/*****************************************************************
**Zona Antiintrusione
****************************************************************/	
#ifndef ZONA_ANTI_H
#define ZONA_ANTI_H

#include "bannonoff.h"

class zonaAnti : public bannOnOff
{
    Q_OBJECT
public:
     zonaAnti( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL/*,char* IconaSx=NULL,char* IconaDx=NULL*/,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0);   
          void inizializza();
public slots:
     void gestFrame(char*);
     char* getChi();
signals:
private:    
};


#endif //ZONA_ANTI

/*****************************************************************
**Impianto Antiintrusione
****************************************************************/	
#ifndef IMP_ANTI_H
#define IMP_ANTI_H

#include "bann2butlab.h"
#include "tastiera.h"

class impAnti : public  bann2butLab
{
    Q_OBJECT
public:
     impAnti( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0);   
          void inizializza();
public slots:
     void gestFrame(char*);
     char* getChi();
     void Inserisci();
     void Disinserisci();
     void Insert(char*);
     void DeInsert(char*);
signals:
    // void CodaAllarmi();
     void impiantoInserito();
private:    
     tastiera* tasti;
};


#endif //IMP_ANTI_H

/*****************************************************************
**Allarme
****************************************************************/	
#ifndef ALLARME_H
#define ALLARME_H

#include "bannondx.h"

class allarme : public  bannOnDx
{
    Q_OBJECT
public:
     allarme( sottoMenu  *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL);   
public slots:
    void muoio();	
signals:
private:    
};


#endif //ALLARME_H

/*****************************************************************
**Gestione Modulo scenari
****************************************************************/	
#ifndef MOD_SCEN_H
#define MOD_SCEN_H

#include "bann4taslab.h"

class gesModScen : public  bann4tasLab
{
    Q_OBJECT
public:
     gesModScen( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* Ico1=NULL,char* Ico2=NULL,char* Ico3=NULL,char* Ico4=NULL,\
		 char* Ico5=NULL, char* Ico6=NULL, char* Ico7=NULL);   
public slots:
    void attivaScenario();	
    void enterInfo();
    void exitInfo();
    void startProgScen();
    void stopProgScen();
    void cancScen();
    void gestFrame(char*);
    void inizializza();
signals:
private:    
    char iconOn[50];
    char iconStop[50];       
    char iconInfo[50];
    char iconNoInfo[50];       
    char cosa[10];
    char dove[10];
    unsigned char sendInProgr;
};

#endif //MOD_SCEN_H
