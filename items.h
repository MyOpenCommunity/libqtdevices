/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** items.h
 **
 **definizioni dei vari items implementati
 **
 ****************************************************************/

#ifndef ITEMS_H
#define ITEMS_H

#include <qwidget.h>
#include <qptrlist.h>
#include "openclient.h"
#include "banner.h"
#include "main.h"

// Includes for sigle Items
#include "bannregolaz.h"
#include "device.h"
#include "frame_interpreter.h"
#include "bannonoff.h"
#include "bannondx.h" // Scenario e Carico includevano questo ma ereditano da bannOnSx
#include "bannonoff2scr.h"
#include "bannon2scr.h"
#include "bann3but.h"
#include "bannpuls.h"
#include "bannbuticon.h"
#include "bannciclaz.h"
#include "radio.h"
#include "aux.h"
#include "multimedia_source.h"
#include "banntermo.h"
#include "bannonicons.h"
#include "bann2butlab.h"
#include "bann4taslab.h"
#include "scenevocond.h" // era incluso nell'item scenario evoluto, probabilmente può essere sostituito da forward declarations
#include "bann4but.h"
#include "videocitof.h"
#include "bannbut2icon.h"
#include "diffmulti.h"


#include "diffsonora.h"

/// Forward Declarations
class diffSonora;
class diffmulti; //aggiunta da me per far compilare
class dati_sorgente_multi;
class dati_ampli_multi;
class tastiera;

/*****************************************************************
 **Dimmer
 ****************************************************************/
/*!
 * \class dimmer
 * \brief This is the dimmer-banner class.
 * \author Davide
 * \date lug 2005
 */
class dimmer : public bannRegolaz
{
Q_OBJECT
protected:
	char pul;
	char gruppi[4];
	device *dev;
public:
	dimmer( QWidget *, const char *, char *, char *, char *, char *, char *, char *, bool to_be_connect = true );
	virtual void inizializza(bool forza = false);
	void Draw();
private slots:
	virtual void Accendi();
	virtual void Spegni();
	virtual void Aumenta();
	virtual void Diminuisci();
public slots:
	virtual void status_changed(QPtrList<device_status>);
signals:
	void frame_available(char *);

};


/*****************************************************************
 **Dimmer 100 Livelli
 ****************************************************************/
/*!
 * \class dimmer 100 livelli
 * \brief This is the 100 lev dimmer-banner class.
 * \author Ciminaghi
 * \date Mar 2006
 */
class dimmer100 : public dimmer
{
Q_OBJECT ;
private:
	/*!
	 * \brief decode msg code, lev and speed from open message
	 * returns true if msg_open is a message for a new dimmer, 
	 * false otherwise
	 */
	bool decCLV(openwebnet&, char& code, char& lev, char& speed,
	char& h, char& m, char& s); 
	int softstart, softstop;
	int last_on_lev;
	int speed;
public:
	dimmer100( QWidget *, const char *, char *, char *, char *, char *, char *, char *, int, int );
	void inizializza(bool forza=false);
	void status_changed(QPtrList<device_status>);
private slots:
	void Accendi();
	void Spegni();
	void Aumenta();
	void Diminuisci();
};


/*****************************************************************
 **Attuatore Automazione
 ****************************************************************/
/*!
 * \class attuatAutom
 * \brief This is the \a automation \a actuator-banner class.
 * This class is used to represent both lightings and various kind of automation such as fan and irrigation.
 * \author Davide
 * \date lug 2005
 */
class attuatAutom : public bannOnOff
{
Q_OBJECT
private:
	char gruppi[4];
	device *dev;
public:
	attuatAutom( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0);
	void inizializza(bool forza=false);
public slots:
	void status_changed(QPtrList<device_status>);
	private slots:
	void Attiva();
	void Disattiva();
	char* getChi();
};


/*****************************************************************
 **Gruppo Automazione
 ****************************************************************/
/*!
 * \class grAttuatAutom
 * \brief This class is made to control a number of automation actouators.
 *
 * It behaves essentially like attuatAutom but it doesn't represent the actuators 
 * state since different actuators can have different states.
 * \author Davide
 * \date lug 2005
 */
class grAttuatAutom : public bannOnOff
{
Q_OBJECT
private:
	QPtrList<QString> elencoDisp;
public:
	grAttuatAutom  ( QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Icon=NULL,int periodo=0,int numFrame=0);
	/*! \brief This method is used to add an address list of the objects contained int he group managed by this class*/
	void setAddress(void*);
private slots:
	void Attiva();
	void Disattiva();
};


/*****************************************************************
 **gruppo dimmer
 ****************************************************************/
/*!
 * \class grDimmer
 * \brief This class is made to control a number of dimmers.
 *
 * It behaves essentially like \a dimmer even if it doesn't represent the dimmer's state 
 * since different dimmers can have different states.
 * \author Davide
 * \date lug 2005
 */
class grDimmer : public bannRegolaz
{
Q_OBJECT
protected:
	QPtrList<QString> elencoDisp;
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
};


/*****************************************************************
 **gruppo dimmer 100 livelli
 ****************************************************************/
/*!
 * \class grDimmer100
 * \brief This class is made to control a number of 100 levels dimmers
 *
 * It behaves essentially like \a dimmer group
 * \author Davide
 * \date Jun 2006
 */
class grDimmer100 : public grDimmer
{
Q_OBJECT
private:
	QValueList<int> soft_start;
	QValueList<int> soft_stop;
public:
	grDimmer100  ( QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Iconsx=NULL,char*Icondx=NULL,int periodo=0,int numFrame=0, QValueList<int> sstart = QValueList<int>(), QValueList<int> sstop = QValueList<int>());
private slots:
	void Attiva();
	void Disattiva();
	void Aumenta();
	void Diminuisci();
};


/*****************************************************************
 **scenario
 ****************************************************************/
/*!
 * \class scenario
 * \brief This class is made to control a scenario of a \a scenario \a unit.
 *
 * \author Davide
 * \date lug 2005
 */
class scenario : public bannOnSx 
{
Q_OBJECT
public:
	scenario( sottoMenu  *, const char *, char *, char *);
	void inizializza();
private slots:
	void Attiva();
};


/*****************************************************************
 **carico
 ****************************************************************/
/*!
 * \class carico
 * \brief This class is made to force an appliance.
 *
 * \author Davide
 * \date lug 2005
 */
class carico : public bannOnSx 
{
Q_OBJECT
public:
	carico(sottoMenu  *, const char * ,char*,char*);
	void inizializza();
private slots:
	void Attiva();
public slots:
	void gestFrame(char*);
};


/*****************************************************************
 **attuatore automazione interbloccato
 ****************************************************************/
/*!
 * \class attuatAutomInt
 * \brief This class is made to manage an interblocked actuator.
 *
 * This kind of actuators controls gates, stretches and so on. Clicking the right button the object closes, and on the button
 * appears a stop image. A new pressure cause the stop af the automation. On the left-button there's the same behavior with
 * the difference that the object is opened.
 * \author Davide
 * \date lug 2005
 */
class attuatAutomInt : public bannOnOff
{
Q_OBJECT
private:
	char uprunning,dorunning;  
	char nomeFile1[MAX_PATH],nomeFile2[MAX_PATH],nomeFile3[MAX_PATH];
	device *dev;
public:
	attuatAutomInt ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0);
	void inizializza();
private slots:
	void analizzaUp();
	void analizzaDown();
public slots:
	virtual void status_changed(QPtrList<device_status>);
};


/*****************************************************************
 **attuatore automazione interbloccato sicuro
 ****************************************************************/
/*!
 * \class attuatAutomIntSic
 * \brief This class is made to manage a secure interblocked actuator.
 *
 * This kind of actuators controls gates, stretches and so on. Pushing on the right button the object closes while releasing
 * the same button the automation stops. On the left-button there's the same behavior with the difference that the object
 * is opened.
 * \author Davide
 * \date lug 2005
 */
class attuatAutomIntSic : public bannOnOff
{
Q_OBJECT
private:
	char uprunning,dorunning;
	char nomeFile1[MAX_PATH],nomeFile2[MAX_PATH],nomeFile3[MAX_PATH];
	device *dev;
public:
	attuatAutomIntSic ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0);
	void inizializza();
private slots:
	void doPres();
	void upPres();
	void doRil();
	void upRil();
	void sendStop();
public slots:
	virtual void status_changed(QPtrList<device_status>);
};


/*****************************************************************
 **attuatore automazione temporizzato
 ****************************************************************/
/*!
 * \class attuatAutomTemp
 * \brief This class is made to manage a timed control.
 *
 * With this object is possible to have the actuator active for a certain time. After that time the actuator releases by itself.
 * Clicking on the left button it is possible to select the time the actuator has to remain active; clicking on the right button
 * the actuation effectively starts.
 * \author Davide
 * \date lug 2005
 */
class attuatAutomTemp : public bannOnOff2scr
{
Q_OBJECT
protected:
	uchar cntTempi;   
	char tempo_display[100];
	uchar ntempi() ;
	void leggi_tempo(char *&);
	virtual void assegna_tempo_display();
	/*!
	 *  \brief Returns true if the object is a target for message
	 *	TODO: MAKE THIS A METHOD OF class banner ?
	 */
	//bool isForMe(openwebnet& message);
	QPtrList<QString> *tempi;
	device *dev;
public:
	attuatAutomTemp ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0, QPtrList<QString> *lt = NULL);
	virtual void inizializza(bool forza=false);
	~attuatAutomTemp();
private slots:
	void CiclaTempo();
protected slots:
	virtual void Attiva();
public slots:
	virtual void status_changed(QPtrList<device_status>);
};


/*****************************************************************
 **attuatore automazione temporizzato nuovo a N tempi
 ****************************************************************/
/*!
 * \class attuatAutomTempN
 * \brief This class is made to manage a timed control.
 *
 * This object implements the new timed actuator with N time settings
 * With this object is possible to have the actuator active for a certain time. After that time the actuator releases by itself.
 * Clicking on the left button it is possible to select the time the actuator has to remain active; clicking on the right
 * button the actuation effectively starts.
 * \author Ciminaghi
 * \date Apr 2006
 */
class attuatAutomTempNuovoN : public attuatAutomTemp
{
Q_OBJECT ;
protected:
	void assegna_tempo_display();
	bool stato_noto;
public:
	attuatAutomTempNuovoN ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0, QPtrList<QString> *lt = NULL);
	void inizializza(bool forza=false);
protected slots:
	void Attiva();
public slots:
	void status_changed(QPtrList<device_status>);
};


/*****************************************************************
 **attuatore automazione temporizzato nuovo a tempo fisso
 ****************************************************************/
/*!
 * \class attuatAutomTempN
 * \brief This class is made to manage a timed control.
 *
 * This object implements the new timed actuator with fixed time setting
 * \author Ciminaghi
 * \date Apr 2006
 */
class attuatAutomTempNuovoF : public bannOn2scr
{
Q_OBJECT ;
protected:
	char tempo[20];
	void leggi_tempo(char *&out); 
	void chiedi_stato();
	int h, m, s, val;
	QTimer *myTimer;
	bool stato_noto;
	bool temp_nota;
	bool update_ok;
	int  tentativi_update;
public:
	attuatAutomTempNuovoF ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaCentroSx=NULL,char* IconaCentroDx=NULL,char*IconDx=NULL, const char *tempo=NULL);
	void inizializza(bool forza=false);
	void SetIcons(char *, char *, char *);
	void Draw();
protected slots:
	void Attiva();
	void update();
public slots:
	void status_changed(QPtrList<device_status>);
};


/*****************************************************************
 **gruppo automazione
 ****************************************************************/
/*!
 * \class grAttuatInt
 * \brief This class is made to manage a number of  interblocked actuators.
 *
 * Clicking the right button the objects close,  clicking the left-button the objects open while clicking the middle 
 * button the objects stop.
 * \author Davide
 * \date lug 2005
 */
class grAttuatInt : public bann3But
{
Q_OBJECT
private:
	QPtrList<QString> elencoDisp;
public:
	grAttuatInt(QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Icon=NULL,int periodo=0,int numFrame=0);
	/*! \brief This method is used to add an address list of the objects contained int he group managed by this class */
	void setAddress(void*);
	void inizializza();
public slots:
	void Alza();
	void Abbassa();
	void Ferma();
};


/*****************************************************************
 **attuatore a pulsante (automaz e vct)
 ****************************************************************/
/*!
 * \class attuatPuls
 * \brief This class is made to manage a pulse automation.
 *
 * This object is useful when there's the need to activate something only during the pressure time 
 * (for instance when dealing with a lock).
 * \author Davide
 * \date lug 2005
 */
class attuatPuls : public bannPuls
{
Q_OBJECT
private:
	char type;
public:
	attuatPuls( QWidget *parent=0, const char *name=NULL ,char*indirizzi=NULL,char* IconaSx=NULL,/*char* IconaDx=NULL,*/char*IconActive=NULL,char tipo=0,int periodo=0,int numFrame=0); 
	void inizializza();
private slots:
	void Attiva();
	void Disattiva();
};


/*****************************************************************
 ** Automatismi-Cancello con attuatore videocitofonia 
 ****************************************************************/
/*!
 * \class
 * \brief This class represents an automated video-doorphone actuator
 * \author Ciminaghi
 * \date June 2006
 */
class automCancAttuatVC : public bannButIcon
{
Q_OBJECT
private:
	device *dev;
public:
	automCancAttuatVC ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL);
private slots:
	void Attiva(void);
};


/*****************************************************************
 ** Automatismi-Cancello con attuatore ILLUMINAZIONE
 ****************************************************************/
/*!
 * \class 
 * \brief This class represents an automated light actuator
 * \author Ciminaghi
 * \date June 2006
 */
class automCancAttuatIll : public bannButIcon
{
Q_OBJECT
private:
	device *dev;
	QString time;
public:
	automCancAttuatIll ( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL, QString *t = NULL);
private slots:
	void Attiva(void);
};


/*****************************************************************
 **Amplificatore
 ****************************************************************/
/*!
 * \class amplificatore
 * \brief This class is made to manage an audio amplifier.
 *
 * This class is quite similar to \a dimmer's one; it only has to deal with different \a Open \a messages.
 * \author Davide
 * \date lug 2005
 */
class amplificatore : public bannRegolaz
{
Q_OBJECT
private:
	char value;
	device *dev;
public:
	amplificatore( QWidget *, const char *,char*,char*,char*,char*,char*);
	void inizializza();
private slots:
	void Accendi();
	void Spegni();
	void Aumenta();
	void Diminuisci();
public slots:
	void status_changed(QPtrList<device_status>);
};


/*****************************************************************
 **gruppo amplificatori
 ****************************************************************/
/*!
 * \class grAmplificatori
 * \brief This class is made to manage a number of audio amplifier.
 *
 * It behaves essentially like \a amplificatore even if it doesn't represent the amplifiter's state since different
 * amplifiers can have different states.
 * \author Davide
 * \date lug 2005
 */
class grAmplificatori : public bannRegolaz
{
Q_OBJECT
private:
	QPtrList<QString> elencoDisp;
public:
	grAmplificatori  ( QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Iconsx=NULL,char*Icondx=NULL,int periodo=0,int numFrame=0);
	void inizializza();
	/*! \brief This method is used to add an address list of the objects contained int he group managed by this class */
	void setAddress(void*);
private slots:
	void Attiva();
	void Disattiva();
	void Aumenta();
	void Diminuisci();
};


/*****************************************************************
 **Sorgente_Radio
 ****************************************************************/
/*!
 * \class banradio
 * \brief This class is made to manage the FM tuner.
 *
 * This particoular banner is composed by 4 buttons from which is possible: change the sound source, open an toher page
 * where to watch the detailed information about the tuner (frequency, RDS message, ...), go to the next/previous
 * memorized frequency.
 * \author Davide
 * \date lug 2005
 */
class banradio : public bannCiclaz
{
Q_OBJECT
protected:
	void pre_show();
	radio* myRadio;
	bool old_diffson;
	device *dev;
public:
	banradio( QWidget *parent,const char *name,char* indirizzo, int nbut=4, const QString & ambdescr="");
	void inizializza();
	/*!
	  \brief Sets the background color for the banner.

	  The arguments are RGB components for the color.
	*/
	void setBGColor(int, int , int );
	/*!
	  \brief Sets the foreground color for the banner.

	  The arguments are RGB components for the color.
	*/
	void setFGColor(int , int , int );
	/*!
	  \brief Sets the background color for the banner.

	  The argument is the QColor description of the color.
	*/
	void setBGColor(QColor );
	/*!
	  \brief Sets the foreground color for the banner.

	  The argument is the QColor description of the color.
	*/
	void setFGColor(QColor );
protected slots:
	void ciclaSorg();
	void decBrano();
	void aumBrano();
	void aumFreqAuto();
	void decFreqAuto();
	void aumFreqMan();
	void decFreqMan();
	void changeStaz();
	void memoStaz(uchar);
	void richFreq();
	void stopRDS();
	void startRDS();
	void grandadChanged(QWidget *);
public slots:
	void status_changed(QPtrList<device_status>);
	virtual void	show();
	void hide();
	void SetTextU( const QString & );
};


/*****************************************************************
 **Sorgente_AUX
 ****************************************************************/
/*!
 * \class sorgente_aux
 * \brief This class is made to manage an auxiliary sound source.
 *
 * This particoular banner is composed by 2 buttons from which is possible: change the sound source,  go to the next track.
 * \author Davide
 * \date lug 2005
 */
class sorgente_aux : public bannCiclaz
{
Q_OBJECT
protected:
	aux *myAux;
	bool vecchia;
public:
	sorgente_aux( QWidget *parent,const char *name,char* indirizzo, bool vecchio=true, char *ambdescr="");
	void inizializza();
private slots:
	void ciclaSorg();
	void decBrano();
	void aumBrano();
public slots:
	void gestFrame(char*);
	void hide();
};


/**
 * \brief This class is made to manage a multimedia source.
 */
class BannerSorgenteMultimedia : public bannCiclaz
{
Q_OBJECT
public:
	BannerSorgenteMultimedia(QWidget *parent, const char *name, char *indirizzo, int where, int nbutt);
public slots:
	void gestFrame(char *);
	void hide();
private slots:
	void ciclaSorg();
	void decBrano();
	void aumBrano();
	void menu();
protected:
	MultimediaSource source_menu;
};

/**
 * \brief This class is made to manage a multichannel multimedia source.
 */
class BannerSorgenteMultimediaMC : public BannerSorgenteMultimedia
{
Q_OBJECT
public:
	BannerSorgenteMultimediaMC(QWidget *parent, const char *name, char *indirizzo, int where,
		const char *icon_onoff, const char *icon_cycle, const char *icon_settings);

public slots:
	void attiva();
	void addAmb(char *);
	void ambChanged(char *, bool, void *);
signals:
	void active(int, int);

private:
	QString indirizzo_semplice;
	QStringList indirizzi_ambienti;
	bool multiamb;
	int indirizzo_ambiente;
};

/*****************************************************************
 **Zona Termoregolazione
 ****************************************************************/
/*!
 * \class termoPage
 * \brief This class is made to manage a thermoregulation zone.
 *
 * By this banner is possible to see the measured temperature, the set point, the offset locally setted and, if permitted
 * by the central, to switch from automatic and manual control. When manual controlling the zone it is possible to change
 * the setpoint.
 * \author Davide
 * \date lug 2005
 */
class termoPage : public bannTermo
{
Q_OBJECT
private:
	/// Tipo Centrale: 0=3550 (99 zone), 1=4695 (4 zone)
	int type;
	/// Se type=1 allora indirizzo centrale BASIC (da 01 a 99)
	QString ind_centrale;
	device *dev;
	char manIco[50];
	char autoIco[50];
	QTimer setpoint_timer;
public:
	termoPage (QWidget *parent, devtype_t devtype, const char *name, char *indirizzo,
		QPtrList<QString> &icon_names,
		QColor SecondForeground = QColor(0,0,0), int type = 0, const QString &ind_centrale = 0);
	void inizializza();
	char* getChi();
	char* getAutoIcon();
	char* getManIcon();
	void SetSetpoint(float icx);
	int delta_setpoint;
private slots:
	void aumSetpoint();
	void decSetpoint();
	void autoMan();
	void sendSetpoint();
	void handleFancoilButtons(int button_number);
public slots:
	void status_changed(QPtrList<device_status>);
	void show();
	void hide();
};


/*****************************************************************
 **Zona Antiintrusione
 ****************************************************************/
/*!
 *  \class zonaAnti
 * \brief This class is made to manage an anti-intrusion zone.
 *
 * By this banner is possible to see if the zone is active or not in a certain moment.
 * \author Davide
 * \date lug 2005
 */
class zonaAnti : public bannOnIcons
{
Q_OBJECT
private:
	void setIcons();
	char *parzIName;
	char *sparzIName;
	char *zonaAttiva;
	char *zonaNonAttiva;
	bool already_changed;
	device *dev;
public:
	zonaAnti( QWidget *parent=0, const QString & name=NULL ,char*indirizzo=NULL/*,char* IconaSx=NULL,char* IconaDx=NULL*/,char*IconActive=NULL,char*IconDisactive=NULL,char* iconParz=NULL, char *iconSparz=NULL, int periodo=0,int numFrame=0);
	void inizializza();
	void SetIcons(char *, char *, char *);
	void Draw();
public slots:
	void status_changed(QPtrList<device_status>);
	char* getChi();
	void ToggleParzializza();
	void abilitaParz(bool);
	void clearChanged(void);
	int getIndex();
signals:
	void partChanged(zonaAnti *);
};


/*****************************************************************
 **Impianto Antiintrusione
 ****************************************************************/
/*!
 * \class impAnti
 * \brief This class is made to manage the anti-intrusion system.
 *
 * By this banner is possible to change the (dis)activation state of the system from the visualized one.
 * If there is an alarm queue it also possible to acces a page describing the it.
 * \author Davide
 * \date lug 2005
 */
class impAnti : public  bann2butLab
{
Q_OBJECT
private:
	static const int MAX_ZONE = 8;
	tastiera* tasti;
	zonaAnti *le_zone[MAX_ZONE];
	bool send_part_msg;
	bool part_msg_sent;
	bool inserting;
	QTimer insert_timer;
	char *passwd;
	device *dev;
public:
	impAnti( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*IconActive=NULL,char*IconDisactive=NULL,int periodo=0,int numFrame=0);
private slots:
	void inizializza();
	void Inserisci();
	void Disinserisci();
	void Insert1(char*);
	void Insert2();
	void Insert3();
	void DeInsert(char*);
public slots:
	void status_changed(QPtrList<device_status>);
	char* getChi();
	void partChanged(zonaAnti*);
	void setZona(zonaAnti*);
	int getIsActive(int zona);
	void ToSendParz(bool s);
	void openAckRx();
	void openNakRx();
	void hide();
signals:
	void impiantoInserito();
	void abilitaParz(bool);
	void clearChanged(void);
	void clearAlarms(void);
};


/*****************************************************************
 **Gestione Modulo scenari
 ****************************************************************/
/*!
 * \class gesModScen
 * \brief This class is made to control a scenario of a \a din \a scenario \a module.
 *
 * From this banner is possible to actuate, clean and program the scenario controlled.
 * \author Davide
 * \date lug 2005
 */
class gesModScen : public  bann4tasLab
{
Q_OBJECT
private:
	char iconOn[50];
	char iconStop[50];
	char iconInfo[50];
	char iconNoInfo[50];
	char cosa[10];
	char dove[10];
	unsigned char sendInProgr, bloccato, in_progr;
	device *dev;
public:
	gesModScen( QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* Ico1=NULL,char* Ico2=NULL,char* Ico3=NULL,char* Ico4=NULL, char* Ico5=NULL, char* Ico6=NULL, char* Ico7=NULL);
private slots:
	void attivaScenario();
	void enterInfo();
	void exitInfo();
	void startProgScen();
	void stopProgScen();
	void cancScen();
public slots:
	void status_changed(QPtrList<device_status>);
	void inizializza();
};


/*****************************************************************
 ** Scenario evoluto
 ****************************************************************/
/*!
 * \class scenEvo
 * \brief This class represents an advanced scenario management object
 * \author Ciminaghi
 * \date apr 2006
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
	scenEvo( QWidget *parent=0, const char *name=NULL, QPtrList<scenEvo_cond> *c=NULL, char* Ico1=NULL,char* Ico2=NULL,char* Ico3 = NULL,char* Ico4=NULL, char* Ico5=NULL, char* Ico6=NULL, char* Ico7=NULL, QString act="", int enabled = 0);
	void Draw();
	/*! \brief Returns action as an open message */
	const char *getAction();
	/*!
	 * \brief Sets action as an open message
	 * \param a pointer to open frame
	 */
	void setAction(const char *a);
private slots:
	void toggleAttivaScev();
	void configScev();
	void forzaScev();
	void nextCond();
	void prevCond();
	void firstCond();
	void trig(bool forced = false);
	void freezed(bool);
	void saveAndApplyAll();
	void resetAll();
	void hide();
public slots:
	void gestFrame(char*);
	void inizializza();
signals:
	void frame_available(char *);
};


/*****************************************************************
 ** Scenario schedulato
 ****************************************************************/
/*!
 * \class scenEvo
 * \brief This class represents a scheduled scenario management object
 * \author Ciminaghi
 * \date apr 2006
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
	 * \brief Returns action as an open message
	 */
	const char *getAction() ;
	/*!
	 * \brief Sets action as an open message
	 * \param a pointer to open frame
	 */
	void setAction(const char *a);
	/*!
	 * Reimplemented draw
	 */
	void Draw();
public slots:
	/*!
	 * \brief Enable scheduled scenario
	 */
	void enable(void);
	/*! Disable scheduled scenario */
	void disable(void); 
	/*! Start scheduled scenario */
	void start(void);
	/*! Stop scheduled scenario */
	void stop(void);
};


/*****************************************************************
 ** Posto esterno
 ****************************************************************/
/*!
 * \class postoExt
 * \brief This class represents an outdoor station
 * \author Ciminaghi
 * \date apr 2006
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
	postoExt(QWidget *parent=0, const char *name=NULL, char* Icona1="",char *Icona2="", char *Icona3="", char* Icona4="", char *where="", char *light="", char *key="", char *unknown = "0", char *txt1 = "", char *txt2 = "", char *txt3 = "");
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
	//! Emitted on freezed
	void freezed(bool);
};


/*****************************************************************
 ** Ambiente diffusione sonora multicanale
 ****************************************************************/
/*!
 * \class postoExt
 * \brief This class represents an env. in the multi-channel audio diff. sys.
 * \author Ciminaghi
 * \date jul 2006
 */
class ambDiffSon : public bannBut2Icon
{
Q_OBJECT
private:
	diffSonora *diffson;
	diffmulti *diffmul;
	sottoMenu *sorgenti;
	int actSrc;
	bool is_draw;
public:
	ambDiffSon(QWidget *parent=0, const char *name=NULL, void *indirizzo=NULL, char* Icona1="",char *Icona2="", char *Icona3="", QPtrList<dati_ampli_multi> *la = NULL, diffSonora *ds=NULL, sottoMenu *sorg=NULL, diffmulti *dm=NULL);
	void Draw();
	void hide();
	void setDraw(bool d);
	bool isDraw();
public slots:
	void configura(); 
	//! receives amb index and active source index
	void actSrcChanged(int, int);
signals:
	void ambChanged(char *, bool, void *);
};


/*****************************************************************
 ** Insieme Ambienti diffusione sonora multicanale
 ****************************************************************/
/*!
 * \class insAmbDiffSon
 * \brief This class represents an env. in the multi-channel audio diff. sys.
 * \author Ciminaghi
 * \date jul 2006
 */
class insAmbDiffSon : public bannButIcon
{
Q_OBJECT
private:
	diffSonora *diffson;
	diffmulti *diffmul;
	sottoMenu *sorgenti;
public:
	insAmbDiffSon(QWidget *parent=0, QPtrList<QString> *descrizioni=NULL, void *indirizzo=NULL, char* Icona1="",char *Icona2="", QPtrList<dati_ampli_multi> *la = NULL, diffSonora *ds=NULL, sottoMenu *sorg=NULL, diffmulti *dm=NULL);
	void Draw();
public slots:
	void configura();
	//! receives amb index and active source index
	void actSrcChanged(int, int);
signals:
	void ambChanged(char *, bool, void *);
};


/*****************************************************************
 ** Sorgente radio diffusione sonora multicanale
 ****************************************************************/
/*!
 * \class sorgenteMultiRadio
 * \brief This class represents a radio source in the multi-channel audio diff. sys.
 * \author Ciminaghi
 * \date jul 2006
 */
class sorgenteMultiRadio : public banradio
{
Q_OBJECT
private:
	QString indirizzo_semplice;
	QStringList indirizzi_ambienti;
	bool multiamb;
	int indirizzo_ambiente;
public:
	sorgenteMultiRadio(QWidget *parent=0, const char *name=NULL, char *indirizzo="", char* Icona1="",char *Icona2="", char *Icona3="", char *ambDescr="");
public slots:
	void attiva();
	void addAmb(char *);
	void ambChanged( const QString &, bool, void *);
	void show();
signals:
	void active(int, int);
};


/*****************************************************************
 ** Sorgente aux diffusione sonora multicanale
 ****************************************************************/
/*!
 * \class sorgenteMultiAux
 * \brief This class represents an aux source in the multi-channel audio diff. sys.
 * \author Ciminaghi
 * \date jul 2006
 */
class sorgenteMultiAux : public sorgente_aux
{
Q_OBJECT
private:
	QString indirizzo_semplice;
	QStringList indirizzi_ambienti;
	bool multiamb;
	int indirizzo_ambiente;
public:
	sorgenteMultiAux(QWidget *parent=0, const char *name=NULL, char *indirizzo="", char* Icona1="",char *Icona2="", char *Icona3="", char *ambdescr="");
	void addAmb(char *);
public slots:
	void attiva();
	void ambChanged(const QString &, bool, void *);
signals:
	void active(int, int);
};


#endif // ITEMS_H
