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
#include "openclient.h"
#include "banner.h"
#include "main.h"
#include <qptrlist.h>

/*****************************************************************
**Dimmer
****************************************************************/

#ifndef  DIMMER_H
#define DIMMER_H

#include "bannregolaz.h"
#include "device.h"
#include "frame_interpreter.h"

/*!
  \class dimmer
  \brief This is the dimmer-banner class.
  
  \author Davide
  \date lug 2005
*/  
class dimmer : public bannRegolaz
{
    Q_OBJECT
public:
     dimmer( QWidget *, const char *,char*,char*,char*,char*,char*,char*);     
     virtual void inizializza();
     void Draw();
public slots:
      //virtual void gestFrame(char*);
     virtual void status_changed(QPtrList<device_status>);
private slots:
    virtual void Accendi();
    virtual void Spegni();
    virtual void Aumenta();
    virtual void Diminuisci();
signals:
   // void sendFrame(char *);          
    void frame_available(char *);
protected:
    //char value;
    char pul;
    char gruppi[4];
    device *dev;
};

#endif //DIMMER_H

/*****************************************************************
**Dimmer
****************************************************************/

#ifndef  DIMMER100_H
#define DIMMER100_H

#include "bannregolaz.h"

/*!
  \class dimmer 100 livelli
  \brief This is the 100 lev dimmer-banner class.
  
  \author Ciminaghi
  \date Mar 2006
*/  
class dimmer100 : public dimmer
{
    Q_OBJECT ;
 public:
    dimmer100( QWidget *, const char *,char*,char*,char*,char*,char*,char*,
	       int, int); 
    void inizializza();
    //void Draw();
#if 0
    public slots:
	void gestFrame(char*);
#endif
    void status_changed(QPtrList<device_status>);
    private slots:
	void Accendi();
	void Spegni();
	void Aumenta();
	void Diminuisci();
 signals:
// void sendFrame(char *);          
 private:
    /*!
      \brief decode msg code, lev and speed from open message
      returns true if msg_open is a message for a new dimmer, 
      false otherwise
    */
    bool decCLV(openwebnet&, char& code, char& lev, char& speed,
		char& h, char& m, char& s); 
    int softstart, softstop;
    int last_on_lev;
    int speed;
};

#endif //DIMMER100_H

/*****************************************************************
**attuatore automazione
****************************************************************/

#ifndef ATTUAT_AUTOM_H
#define ATTUAT_AUTOM_H

#include "bannonoff.h"
/*!
  \class attuatAutom
  \brief This is the \a automation \a actuator-banner class.
  
  This class is used to represent both lightings and various kind of automation such as fan and irrigation.
  \author Davide
  \date lug 2005
*/  
class attuatAutom : public bannOnOff
{
    Q_OBJECT
public:
    attuatAutom( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0); 
    void inizializza();
public slots:
	//void gestFrame(char*);
 void status_changed(QPtrList<device_status>); 
private slots:
     void Attiva();
     void Disattiva();
     char* getChi();
signals:
  //  void sendFrame(char *);      
private:    
     char gruppi[4];    
     device *dev;
};


#endif //ATTUAT_AUTOM_H

/*****************************************************************
**gruppo automazione
****************************************************************/

#ifndef GR_ATTUAT_AUTOM_H
#define GR_ATTUAT_AUTOM_H

#include "bannonoff.h"

/*!
  \class grAttuatAutom
  \brief This class is made to control a number of automation actouators.
  
  It behaves essentially like attuatAutom but it doesn't represent the actuators state since different actuators can have different states.
  \author Davide
  \date lug 2005
*/  

class grAttuatAutom : public bannOnOff
{
    Q_OBJECT
public:
     grAttuatAutom  ( QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Icon=NULL,int periodo=0,int numFrame=0);       
/*! \brief This method is used to add an address list of the objects contained int he group managed by this class*/     
     void setAddress(void*);	
//      void inizializza();
private slots:
     void Attiva();
     void Disattiva();
signals:
  //  void sendFrame(char *);           
private:
    QPtrList<QString> elencoDisp;
   
};


#endif //GR_ATTUAT_AUTOM_H

/*****************************************************************
**gruppo dimmer
****************************************************************/

#ifndef GR_DIMMER_H
#define GR_DIMMER_H

#include "bannregolaz.h"
/*!
  \class grDimmer
  \brief This class is made to control a number of dimmers.
  
  It behaves essentially like \a dimmer even if it doesn't represent the dimmer's state since different dimmers can have different states.
  \author Davide
  \date lug 2005
*/  
class grDimmer : public bannRegolaz
{
    Q_OBJECT
public:
     grDimmer  ( QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Iconsx=NULL,char*Icondx=NULL,int periodo=0,int numFrame=0);       
/*! \brief This method is used to add an address list of the objects contained int he group managed by this class*/     
     void setAddress(void*);	    
     void inizializza();
private slots:
     virtual void Attiva();
     virtual void Disattiva();
     virtual void Aumenta();
     virtual void Diminuisci();
signals:
 //   void sendFrame(char *);           
protected:
    QPtrList<QString> elencoDisp;
   
};

#endif //GR_DIMMER_H

/*****************************************************************
**gruppo dimmer 100 livelli
****************************************************************/

#ifndef GR_DIMMER100_H
#define GR_DIMMER100_H

#include "bannregolaz.h"
/*!
  \class grDimmer100
  \brief This class is made to control a number of 100 levels dimmers
  
  It behaves essentially like \a dimmer group
  \author Davide
  \date Jun 2006
*/  
class grDimmer100 : public grDimmer
{
    Q_OBJECT
public:
     grDimmer100  ( QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Iconsx=NULL,char*Icondx=NULL,int periodo=0,int numFrame=0, QValueList<int> sstart = QValueList<int>(), QValueList<int> sstop = QValueList<int>());
private slots:
     void Attiva();
     void Disattiva();
     void Aumenta();
     void Diminuisci();
signals:
 //   void sendFrame(char *);           
 private:
     QValueList<int> soft_start;
     QValueList<int> soft_stop;
};

#endif //GR_DIMMER100_H

/*****************************************************************
**scenario
****************************************************************/

#ifndef SCENARIO_H
#define SCENARIO_H

#include "bannondx.h"
/*!
  \class scenario
  \brief This class is made to control a scenario of a \a scenario \a unit.
  
  \author Davide
  \date lug 2005
*/  
class scenario : public bannOnSx 
{
    Q_OBJECT
public:
     scenario( sottoMenu  *, const char * ,char*,char*);
     void inizializza();
signals:
  //  void sendFrame(char *);           
private slots:
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
/*!
  \class carico
  \brief This class is made to force an appliance.
  
  \author Davide
  \date lug 2005
*/  
class carico : public bannOnSx 
{
    Q_OBJECT
public:
     carico(sottoMenu  *, const char * ,char*,char*);     
     void inizializza();
public slots:
     void gestFrame(char*);
private slots:
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
/*!
  \class attuatAutomInt
  \brief This class is made to manage an interblocked actuator.
  
  This kind of actuators controls gates, stretches and so on. Clicking the right button the object closes, and on the button appears a stop image. A new pressure cause the stop af the automation. On the left-button there's the same behavior with the difference that the object is opened.
  \author Davide
  \date lug 2005
*/  
class attuatAutomInt : public bannOnOff
{
    Q_OBJECT
public:
     attuatAutomInt ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0);     
       void inizializza();      
public slots:
      //void gestFrame(char*);
      virtual void status_changed(QPtrList<device_status>);
private slots:
     void analizzaUp();
     void analizzaDown();
signals:
 //   void sendFrame(char *);           
private:
     char uprunning,dorunning;  
     char nomeFile1[MAX_PATH],nomeFile2[MAX_PATH],nomeFile3[MAX_PATH];
     device *dev;
};


#endif //ATTUAT_AUTOM_INT_H


/*****************************************************************
**attuatore automazione interbloccato sicuro
****************************************************************/

#ifndef ATTUAT_AUTOM_INT_SIC_H
#define ATTUAT_AUTOM_INT_SIC_H

#include "bannonoff.h"
/*!
  \class attuatAutomIntSic
  \brief This class is made to manage a secure interblocked actuator.
  
  This kind of actuators controls gates, stretches and so on. Pushing on the right button the object closes while releasing the same button the automation stops. On the left-button there's the same behavior with the difference that the object is opened.
  \author Davide
  \date lug 2005
*/  
class attuatAutomIntSic : public bannOnOff
{
    Q_OBJECT
public:
     attuatAutomIntSic ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0); 
     void inizializza();
public slots:
	//void gestFrame(char*);
	virtual void status_changed(QPtrList<device_status>);
private slots:
     void doPres();
     void upPres();
     void doRil();
     void upRil();
signals:
 //   void sendFrame(char *);           
private:
     char uprunning,dorunning;
     char nomeFile1[MAX_PATH],nomeFile2[MAX_PATH],nomeFile3[MAX_PATH];
     device *dev;
};


#endif //ATTUAT_AUTOM_INT_SIC_H


/*****************************************************************
**attuatore automazione temporizzato
****************************************************************/

#ifndef ATTUAT_AUTOM_TEMP_H
#define ATTUAT_AUTOM_TEMP_H

//static const char* tempi[]={"1'","2'","3'","4'","5'","15'"};//,"30''","0,5''"} ;
     
#include "bannonoff2scr.h"
/*!
  \class attuatAutomTemp
  \brief This class is made to manage a timed control.
  
  With this object is possible to have the actuator active for a certain time. After that time the actuator releases by itself. Clicking on the left button it is possible to select the time the actuator has to remain active; clicking on the right button the actuation effectively starts.
  \author Davide
  \date lug 2005
*/  
class attuatAutomTemp : public bannOnOff2scr
{
    Q_OBJECT
public:
     attuatAutomTemp ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0, QPtrList<QString> *lt = NULL);
    virtual void inizializza();
    ~attuatAutomTemp();
public slots:
      //virtual void gestFrame(char*);
     virtual void status_changed(QPtrList<device_status>);
protected slots:
     virtual void Attiva();
private slots:
     void CiclaTempo();
signals:
  //  void sendFrame(char *);           
protected:
     uchar cntTempi;   
     char tempo_display[100];
     uchar ntempi() ;
     void leggi_tempo(char *&);
     virtual void assegna_tempo_display() ;
     /*
       \brief Returns true if the object is a target for message
       TODO: MAKE THIS A METHOD OF class banner ?
     */
     //bool isForMe(openwebnet& message);
     QPtrList<QString> *tempi;
     device *dev;
};


#endif //ATTUAT_AUTOM_TEMP_H

/*****************************************************************
**attuatore automazione temporizzato nuovo a N tempi
****************************************************************/

#ifndef ATTUAT_AUTOM_TEMP_NUOVO_N_H
#define ATTUAT_AUTOM_TEMP_NUOVO_N_H

#include "bannonoff2scr.h"
/*!
  \class attuatAutomTempN
  \brief This class is made to manage a timed control.
  
  This object implements the new timed actuator with N time settings
  With this object is possible to have the actuator active for a certain time. After that time the actuator releases by itself. Clicking on the left button it is possible to select the time the actuator has to remain active; clicking on the right button the actuation effectively starts.
  \author Ciminaghi
  \date Apr 2006
*/  
class attuatAutomTempNuovoN : public attuatAutomTemp
{
    Q_OBJECT ;
public:
    attuatAutomTempNuovoN ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0, QPtrList<QString> *lt = NULL);
    void inizializza();
public slots:
      //void gestFrame(char*);
    void status_changed(QPtrList<device_status>); 
protected slots:
    void Attiva();
 protected:
 void assegna_tempo_display() ; 
 bool stato_noto;
signals:
//  void sendFrame(char *);           
};


#endif //ATTUAT_AUTOM_TEMP_NUOVO_N_H

/*****************************************************************
**attuatore automazione temporizzato nuovo a tempo fisso
****************************************************************/

#ifndef ATTUAT_AUTOM_TEMP_NUOVO_F_H
#define ATTUAT_AUTOM_TEMP_NUOVO_F_H

#include "bannon2scr.h"
/*!
  \class attuatAutomTempN
  \brief This class is made to manage a timed control.
  
  This object implements the new timed actuator with fixed time setting
  \author Ciminaghi
  \date Apr 2006
*/  
class attuatAutomTempNuovoF : public bannOn2scr
{
    Q_OBJECT ;
public:
    attuatAutomTempNuovoF ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaCentroSx=NULL,char* IconaCentroDx=NULL,char*IconDx=NULL, const char *tempo=NULL);
    void inizializza();
    void SetIcons(char *, char *, char *);
    void Draw();
public slots:
      //void gestFrame(char*);
    void status_changed(QPtrList<device_status>) ;
protected slots:
    void Attiva();
 void update(); 
 protected:
 char tempo[20] ;
 void leggi_tempo(char *&out); 
 void chiedi_stato();
 int h, m, s, val;
 QTimer *myTimer;
 bool stato_noto;
 bool temp_nota;
signals:
//  void sendFrame(char *);           
};


#endif //ATTUAT_AUTOM_TEMP_NUOVO_F_H


/*****************************************************************
**gruppo automazione
****************************************************************/

#ifndef GR_ATTUAT_INT_H
#define GR_ATTUAT_INT_H

#include "bann3but.h"
/*!
  \class grAttuatInt
  \brief This class is made to manage a number of  interblocked actuators.
  
  Clicking the right button the objects close,  clicking the left-button the objects open while clicking the middle button the objects stop.
  \author Davide
  \date lug 2005
*/  
class grAttuatInt : public bann3But
{
    Q_OBJECT
public:
     grAttuatInt(QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Icon=NULL,int periodo=0,int numFrame=0); 
/*! \brief This method is used to add an address list of the objects contained int he group managed by this class*/
     void setAddress(void*);	    
     void inizializza();
public slots:
     void Alza();
     void Abbassa();
     void Ferma();
signals:
//    void sendFrame(char *);           
private:
    QPtrList<QString> elencoDisp;
 };


#endif //GR_ATTUAT_INT_H


/*****************************************************************
**attuatore a pulsante (automaz e vct)
****************************************************************/

#ifndef ATTUAT_PULS_H
#define ATTUAT_PULS_H

#include "bannpuls.h"
/*!
  \class attuatPuls
  \brief This class is made to manage a pulse automation.
  
  This object is useful when there's the need to activate something only during the pressure time (for instance when dealing with a lock).
  \author Davide
  \date lug 2005
*/  
class attuatPuls : public bannPuls
{
    Q_OBJECT
public:
     attuatPuls( QWidget *parent=0, const char *name=NULL ,char*indirizzi=NULL,char* IconaSx=NULL,/*char* IconaDx=NULL,*/char*IconActive=NULL,char tipo=0,int periodo=0,int numFrame=0); 
     void inizializza();
private slots:
     void Attiva();
     void Disattiva();
signals:
 //   void sendFrame(char *);           
private:
     char type;
};



#endif //ATTUAT_PULS_H

/*****************************************************************
** Automatismi-Cancello con attuatore videocitofonia 
****************************************************************/

#ifndef _AUTOM_CANC_ATTUAT_VC_H_
#define _AUTOM_CANC_ATTUAT_VC_H_

#include "bannbuticon.h"
/*!
  \class 
  \brief This class represents an automated video-doorphone actuator
  \author Ciminaghi
  \date June 2006
*/  
class automCancAttuatVC : public bannButIcon
{
    Q_OBJECT
public:
     automCancAttuatVC ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL);     
    //void inizializza();      
public slots:
      //void gestFrame(char*);
      //virtual void status_changed(QPtrList<device_status>);
private slots:
      void Attiva(void);
signals:
 //   void sendFrame(char *);           
private:
     device *dev;
};


#endif // _AUTOM_CANC_ATTUAT_VC_H_


/*****************************************************************
** Automatismi-Cancello con attuatore ILLUMINAZIONE
****************************************************************/

#ifndef _AUTOM_CANC_ATTUAT_ILL_H_
#define _AUTOM_CANC_ATTUAT_ILL_H_

#include "bannbuticon.h"
/*!
  \class 
  \brief This class represents an automated light actuator
  \author Ciminaghi
  \date June 2006
*/  
class automCancAttuatIll : public bannButIcon
{
    Q_OBJECT
public:
     automCancAttuatIll ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,int t=18);     
    //void inizializza();      
public slots:
      //void gestFrame(char*);
      //virtual void status_changed(QPtrList<device_status>);
private slots:
      void Attiva(void);
signals:
 //   void sendFrame(char *);           
private:
     device *dev;
     int time;
};


#endif // _AUTOM_CANC_ATTUAT_VC_H_


/*****************************************************************
**Amplificatore
****************************************************************/

#ifndef  AMPLIFICATORE_H
#define AMPLIFICATORE_H

#include "bannregolaz.h"

/*!
  \class amplificatore
  \brief This class is made to manage an audio amplifier.
  
  This class is quite similar to \a dimmer's one; it only has to deal with different \a Open \a messages.
  \author Davide
  \date lug 2005
*/  
class amplificatore : public bannRegolaz
{
    Q_OBJECT
public:
     amplificatore( QWidget *, const char *,char*,char*,char*,char*,char*);          
     void inizializza();
public slots:
#if 0
    void gestFrame(char*);
#else
    void status_changed(QPtrList<device_status>);
#endif
private slots:
    void Accendi();
    void Spegni();
    void Aumenta();
    void Diminuisci();
signals:
//    void sendFrame(char *);          
private:
     char value;
     device *dev;
};

#endif //AMPLIFICATORE_H

/*****************************************************************
**gruppo amplificatori
****************************************************************/

#ifndef GR_AMPLI_H
#define GR_AMPLI_H

#include "bannregolaz.h"
/*!
  \class grAmplificatori
  \brief This class is made to manage a number of audio amplifier.
  
  It behaves essentially like \a amplificatore even if it doesn't represent the amplifiter's state since different amplifiers can have different states.
  \author Davide
  \date lug 2005
*/  
class grAmplificatori : public bannRegolaz
{
    Q_OBJECT
public:
     grAmplificatori  ( QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Iconsx=NULL,char*Icondx=NULL,int periodo=0,int numFrame=0);       
     void inizializza();
/*! \brief This method is used to add an address list of the objects contained int he group managed by this class*/     
     void setAddress(void*);	    
private slots:
     void Attiva();
     void Disattiva();
     void Aumenta();
     void Diminuisci();
signals:
 //   void sendFrame(char *);           
private:
    QPtrList<QString> elencoDisp;
    
};

#endif //GR_AMPLI_H

/*****************************************************************
**Sorgente_Radio
****************************************************************/

#ifndef  SORG_RADIO_H
#define SORG_RADIO_H

#include "bannciclaz.h"
#include "radio.h"
/*!
  \class banradio
  \brief This class is made to manage the FM tuner.
  
  This particoular banner is composed by 4 buttons from which is possible: change the sound source, open an toher page where to watch the detailed information about the tuner (frequency, RDS message, ...), go to the next/previous memorized frequency.
  \author Davide
  \date lug 2005
*/  
class banradio : public bannCiclaz
{
        Q_OBJECT
public:
    banradio( QWidget *parent,const char *name,char* indirizzo, int nbut=4, char *ambdescr="" );
   void 	inizializza();
 /*!
  \brief Sets the background color for the banner.
  
  The arguments are RGB components for the color.
*/           
   void 	setBGColor(int, int , int );
 /*!
  \brief Sets the foreground color for the banner.
  
  The arguments are RGB components for the color.
*/           
   void 	setFGColor(int , int , int );		
 /*!
  \brief Sets the background color for the banner.
  
  The argument is the QColor description of the color.
*/             
   void 	setBGColor(QColor );
 /*!
  \brief Sets the foreground color for the banner.
  
  The argument is the QColor description of the color.
*/             
   void 	setFGColor(QColor );	
public slots:
#if 0
     void 	gestFrame(char*);
#else
    void        status_changed(QPtrList<device_status>);
#endif
    void	show();
    void	hide();
    void 	SetText(const char *);
protected slots: 
    void 	ciclaSorg();
    void 	decBrano();
    void 	aumBrano();
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
protected:    
    radio*	 myRadio;    
    device *dev;
};

#endif //SORGENTE_RADIO_H

/*****************************************************************
**Sorgente
****************************************************************/

#ifndef  SORGENTE_H
#define SORGENTE_H

#include "bannciclaz.h"
#include "aux.h"
/*!
  \class sorgente
  \brief This class is made to manage an auxiliary sound source.
  
  This particoular banner is composed by 2 buttons from which is possible: change the sound source,  go to the next track.
  \author Davide
  \date lug 2005
*/  
class sorgente : public bannCiclaz
{
        Q_OBJECT
public:
    sorgente( QWidget *parent,const char *name,char* indirizzo, bool vecchio=true, char *ambdescr="");
   void inizializza();
public slots:
     void gestFrame(char*);
private slots:
    void ciclaSorg();
    void decBrano();
    void aumBrano();
signals:
  //  void sendFrame(char *);          
 protected:
    aux *myAux;
};

#endif //SORGENTE_H


/*****************************************************************
**Zona Termoregolazione
****************************************************************/

#ifndef  THERMO_H
#define THERMO_H

#include "banntermo.h"
/*!
  \class termoPage
  \brief This class is made to manage a thermoregulation zone.
  
  By this banner is possible to see the measured temperature, the set point, the offset locally setted and, if permitted by the central, to switch from automatic and manual control. When manual controlling the zone it is possible to change the setpoint.
  \author Davide
  \date lug 2005
*/  
class termoPage : public bannTermo
{
        Q_OBJECT
public:
    termoPage ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaMeno=NULL,char* IconaPiu=NULL,char*IconaMansx=NULL,char*IconaAuto=NULL,char*IconaAntigelo=NULL,char*IconaOff=NULL, QColor 	SecondForeground=QColor(0,0,0));      
   void 	inizializza();
   char* 	getChi();
   char*	getAutoIcon();
   char* 	getManIcon();
public slots:
#if 0
     void 	gestFrame(char*);
#else
     void       status_changed(QPtrList<device_status>);
#endif
     void	show();
     void	hide();
private slots:     
     void	aumSetpoint();
     void	decSetpoint();
     void	autoMan();
signals:
  // void 	sendFrame(char *);          
private:     
     char 	manIco[50];
     char 	autoIco[50];
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

#include "bannonicons.h"
/*!
  \class zonaAnti
  \brief This class is made to manage an anti-intrusion zone.
  
  By this banner is possible to see if the zone is active or not in a certain moment.
  \author Davide
  \date lug 2005
*/  
class zonaAnti : public bannOnIcons
{
    Q_OBJECT
public:
     zonaAnti( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL/*,char* IconaSx=NULL,char* IconaDx=NULL*/,char*IconActive=NULL,char*IconDisactive=NULL,char* iconParz=NULL, char *iconSparz=NULL, int periodo=0,int numFrame=0);   
    void inizializza();
    void SetIcons(char *, char *, char *);
    void Draw();
public slots:
#if 0
     void gestFrame(char*);
#else
     void status_changed(QPtrList<device_status>);
#endif
     char* getChi();
     void ToggleParzializza();
     void abilitaParz(bool);
     void clearChanged(void);
     int getIndex();
signals:
     void partChanged(zonaAnti *);
private:    
     void setIcons();
     char *parzIName;
     char *sparzIName;
     char *zonaAttiva;
     char *zonaNonAttiva;
     bool already_changed;
     device *dev;
};


#endif //ZONA_ANTI

/*****************************************************************
**Impianto Antiintrusione
****************************************************************/	
#ifndef IMP_ANTI_H
#define IMP_ANTI_H

#include "bann2butlab.h"
#include "tastiera.h"
/*!
  \class impAnti
  \brief This class is made to manage the anti-intrusion system.
  
  By this banner is possible to change the (dis)activation state of the system from the visualized one. If there is an alarm queue it also possible to acces a page describing the it.
  \author Davide
  \date lug 2005
*/  
class impAnti : public  bann2butLab
{
    Q_OBJECT
public:
     impAnti( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0);   
          void inizializza();
public slots:
#if 0
     void gestFrame(char*);
#else
     void status_changed(QPtrList<device_status>);
#endif
     char* getChi();
     void partChanged(zonaAnti*);
     void setZona(zonaAnti*);
     void openAckRx();
     void openNakRx();
private slots:     
     void Inserisci();
     void Disinserisci();
     void Insert1(char*);
     void Insert2();
     void Insert3();
     void DeInsert(char*);
signals:
    // void CodaAllarmi();
     void impiantoInserito();
     void abilitaParz(bool);
     void clearChanged(void);
private:    
     static const int MAX_ZONE = 8;
     tastiera* tasti;
     zonaAnti *le_zone[MAX_ZONE];
     bool send_part_msg;
     bool part_msg_sent;
     QTimer insert_timer;
     char *passwd;
     device *dev;
};


#endif //IMP_ANTI_H

/*****************************************************************
**Gestione Modulo scenari
****************************************************************/	
#ifndef MOD_SCEN_H
#define MOD_SCEN_H

#include "bann4taslab.h"
/*!
  \class gesModScen
  \brief This class is made to control a scenario of a \a din \a scenario \a module.
  
  From this banner is possible to actuate, clean and program the scenario controlled.
  \author Davide
  \date lug 2005
*/  
class gesModScen : public  bann4tasLab
{
    Q_OBJECT
public:
     gesModScen( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* Ico1=NULL,char* Ico2=NULL,char* Ico3=NULL,char* Ico4=NULL,\
		 char* Ico5=NULL, char* Ico6=NULL, char* Ico7=NULL);   
public slots:
#if 0
    void gestFrame(char*);
#else
    void status_changed(QPtrList<device_status>); 
#endif
    void inizializza();
private slots:    
    void attivaScenario();	
    void enterInfo();
    void exitInfo();
    void startProgScen();
    void stopProgScen();
    void cancScen();
signals:
private:    
    char iconOn[50];
    char iconStop[50];       
    char iconInfo[50];
    char iconNoInfo[50];       
    char cosa[10];
    char dove[10];
    unsigned char sendInProgr, bloccato, in_progr;
    device *dev;
};

#endif //MOD_SCEN_H

/*****************************************************************
** Scenario evoluto
****************************************************************/	
#ifndef SCEN_EVO_H
#define SCEN_EVO_H

#include "scenevocond.h"


/*!
  \class scenEvo
  \brief This class represents an advanced scenario management object
  \author Ciminaghi
  \date apr 2006
*/  
class scenEvo : public  bann3But
{
    Q_OBJECT
 private:
    QPtrList<scenEvo_cond> *condList;
    QPtrListIterator<scenEvo_cond> *cond_iterator;
    QString action;
    int serial_number;
    static int next_serial_number;
 public:
    scenEvo( QWidget *parent=0, const char *name=NULL, QPtrList<scenEvo_cond> *c=NULL, char* Ico1=NULL,char* Ico2=NULL,char* Ico3=NULL,char* Ico4=NULL,\
	     char* Ico5=NULL, char* Ico6=NULL, char* Ico7=NULL, 
	     QString act="", int enabled = 0);   
    //void SetIcons(char *, char *, char *, char *);
    void Draw();
    /*!
      \brief Returns action as an open message
    */
    const char *getAction() ;
    /*!
      \brief Sets action as an open message
      \param a pointer to open frame
    */
    void setAction(const char *a) ;
public slots:
 void gestFrame(char*);
 void inizializza();
 private slots:
	void toggleAttivaScev();
 void configScev();
 void forzaScev();
 void nextCond();
 void prevCond();
 void firstCond();
 void trig(bool forced = false);
 void freezed(bool);
private slots:    
signals:
 void frame_available(char *); 
private:    
};

#endif // SCEN_EVO_H

/*****************************************************************
** Scenario schedulato
****************************************************************/	
#ifndef SCEN_SCHED_H
#define SCEN_SCHED_H

#include "bann4but.h"

/*!
  \class scenEvo
  \brief This class represents a scheduled scenario management object
  \author Ciminaghi
  \date apr 2006
*/  
class scenSched : public  bann4But
{
    Q_OBJECT
 private:
    QString action_enable;
    QString action_disable;
    QString action_start;
    QString action_stop;
 public:
    scenSched( QWidget *parent=0, const char *name=NULL, char* IconaSx="",char *IconaCSx="", char *IconaCDx="", char* IconaDx="", char *action_enable="", char *action_disable="", char *action_start="", char *action_stop="");   
    /*!
      \brief Returns action as an open message
    */
    const char *getAction() ;
    /*!
      \brief Sets action as an open message
      \param a pointer to open frame
    */
    void setAction(const char *a) ;
    /*!
      Reimplemented draw
    */
    void Draw();
public slots:
    /*!
     \brief Enable scheduled scenario
    */
    void enable(void);
    /*! Disable scheduled scenario */
    void disable(void); 
    /*! Start scheduled scenario */
    void start(void);
    /*! Stop scheduled scenario */
    void stop(void);
signals:
};

#endif // SCEN_SCHED_H

/*****************************************************************
** Posto esterno
****************************************************************/	
#ifndef POSTO_EXT_H
#define POSTO_EXT_H

#include "bann4taslab.h"
#include "videocitof.h"

/*!
  \class postoExt
  \brief This class represents an outdoor station
  \author Ciminaghi
  \date apr 2006
*/  
class postoExt : public bann4tasLab
{
    Q_OBJECT
 private:
    QString where;
    QString descr;
    bool light;
    bool key;
    bool unknown;
    QString light_icon;
    QString key_icon;
    QString close_icon;
    static call_notifier_manager *cnm;
    static call_notifier *unknown_notifier;
 public:
    postoExt(QWidget *parent=0, const char *name=NULL, char* Icona1="",char *Icona2="", char *Icona3="", char* Icona4="", char *where="", char *light="", char *key="", char *unknown = "0");   
    /*!
      Reimplemented draw
    */
    //void Draw();
    //! Read where
    void get_where(QString&);
    //! Read description
    void get_descr(QString&);
    //! Get key mode
    bool get_key(void);
    //! Get light mode
    bool get_light(void);
    //! Get light icon name
    void get_light_icon(QString&);
    //! Get key icon name
    void get_key_icon(QString&);
    //! Cancel icon name
    void get_close_icon(QString&);
public  slots:
    //! Invoked when right button is pressed
    void stairlight_pressed(void);
    //! Invoked when right button is released 
    void stairlight_released(void); 
    //! Invoked when left button is clicked
    void open_door_clicked(void);
    //! Usual gestFrame
    void gestFrame(char *);
    //! Invoked when a frame has been captured by a call_notifier
    void frame_captured_handler(call_notifier *);
    //! Invoked when a call_notifier window is closed
    void call_notifier_closed(call_notifier *);
signals:
    //! Emitted when a frame is available
    void frame_available(char *);
};

#endif // POSTO_EXT_H

/*****************************************************************
** Ambiente diffusione sonora multicanale
****************************************************************/	
#ifndef AMB_DIFF_SON_H
#define AMB_DIFF_SON_H

#include "bannbut2icon.h"
#include "diffsonora.h"
#include "diffmulti.h"

//class sorgenteMulti;
class diffSonora;
class dati_sorgente_multi;
class dati_ampli_multi;

/*!
  \class postoExt
  \brief This class represents an env. in the multi-channel audio diff. sys.
  \author Ciminaghi
  \date jul 2006
*/  
class ambDiffSon : public bannBut2Icon
{
    Q_OBJECT
 public:
    ambDiffSon(QWidget *parent=0, const char *name=NULL, void *indirizzo=NULL, char* Icona1="",char *Icona2="", char *Icona3="", QPtrList<dati_sorgente_multi> *ls = NULL,
	       QPtrList<dati_ampli_multi> *la = NULL, diffSonora *ds=NULL);
    void Draw();
#if 0
    void addAmpli(char, char *, void *, char *, char *, char *, char *, int, 
		  int);
#endif
public slots:
    void configura(); 
 private: 
// QPtrList<sorgenteMulti> *lista_sorg;
 diffSonora *diffson;
};

#endif // AMB_DIFF_SON_H

/*****************************************************************
** Insieme Ambienti diffusione sonora multicanale
****************************************************************/	
#ifndef INS_AMB_DIFF_SON_H
#define INS_AMB_DIFF_SON_H

#include "bannbuticon.h"
#include "diffsonora.h"
#include "diffmulti.h"

//class sorgenteMulti;
class diffSonora;
class dati_sorgente_multi;
class dati_ampli_multi;

/*!
  \class insAmbDiffSon
  \brief This class represents an env. in the multi-channel audio diff. sys.
  \author Ciminaghi
  \date jul 2006
*/  
class insAmbDiffSon : public bannButIcon
{
    Q_OBJECT
 public:
    insAmbDiffSon(QWidget *parent=0, QPtrList<QString> *descrizioni=NULL, void *indirizzo=NULL, char* Icona1="",char *Icona2="", QPtrList<dati_sorgente_multi> *ls = NULL, QPtrList<dati_ampli_multi> *la = NULL, diffSonora *ds=NULL);
    void Draw();
public slots:
    void configura(); 
 private:
 diffSonora *diffson;
};

#endif // INS_AMB_DIFF_SON_H

#if 0
#endif // 0

/*****************************************************************
** Sorgente radio diffusione sonora multicanale
****************************************************************/	
#ifndef SORG_RADIO_DIFF_SON_H
#define SORG_RADIO_DIFF_SON_H

#include "bannciclaz.h"

/*!
  \class sorgenteMultiRadio
  \brief This class represents a radio source in the multi-channel audio diff. sys.
  \author Ciminaghi
  \date jul 2006
*/  
class sorgenteMultiRadio : public banradio
{
    Q_OBJECT
 public:
    sorgenteMultiRadio(QWidget *parent=0, const char *name=NULL, char *indirizzo="", char* Icona1="",char *Icona2="", char *Icona3="", char *ambDescr=""); 
public slots:
	void attiva();
 private:
};

#endif // SORG_RADIO_DIFF_SON_H

/*****************************************************************
** Sorgente aux diffusione sonora multicanale
****************************************************************/	
#ifndef SORG_AUX_DIFF_SON_H
#define SORG_AUX_DIFF_SON_H

#include "bannciclaz.h"

/*!
  \class sorgenteMultiAux
  \brief This class represents an aux source in the multi-channel audio diff. sys.
  \author Ciminaghi
  \date jul 2006
*/  
class sorgenteMultiAux : public sorgente
{
    Q_OBJECT
 public:
    sorgenteMultiAux(QWidget *parent=0, const char *name=NULL, char *indirizzo="", char* Icona1="",char *Icona2="", char *Icona3="", char *ambdescr=""); 
public slots:
	void attiva();
 private:
};

#endif // SORG_AUX_DIFF_SON_H


