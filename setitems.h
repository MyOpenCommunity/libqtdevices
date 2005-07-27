/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** setitems.h
**
**definizioni dei vari items implementati per impostazioni
**
****************************************************************/
#include <qwidget.h>

#include "banner.h"
#include "bannondx.h"
#include "genericfunz.h"


/*****************************************************************
**setDataOra
****************************************************************/

#ifndef SETDATAORA
#define SETDATAORA


#include "impostatime.h"
/*!
  \class setDataOra
  \brief This class is made to set time and date.

  \author Davide
  \date lug 2005
*/  
class setDataOra : public bannOnDx 
{
    Q_OBJECT
public:
     setDataOra( sottoMenu  *, const char * );     
signals:
  //    void sendFrame(char *); 
public slots:
//     void Attiva();
private:
       impostaTime settalora;

};
#endif //SETDATAORA

/*****************************************************************
**impostaSveglia
****************************************************************/

#ifndef IMPSVEGLIA
#define IMPSVEGLIA

#include "sveglia.h"
#include	"diffsonora.h"
#include "bann2but.h"
/*!
  \class impostaSveglia
  \brief This class is made to make alarm clock settings.

  \author Davide
  \date lug 2005
*/              
class impostaSveglia : public bann2But 
{
    Q_OBJECT
public:
     impostaSveglia( QWidget *, const char * ,diffSonora*, char* , char* , char*, char*, int, int, char* , char*, char* , char*, char*,int);     
/*!
  \brief changes the abilitation af the alarm set
*/      
     void 	setAbil(bool);
/*!
  \brief forces a eeprom read to initialyze alarm set settings
*/       
     void inizializza();
signals:
/*!
  \brief Emitted to turn alarm clock off
*/        
     void spegniSveglia();
public slots:	
/*!
  \brief Analizes \a Open \a frames coming form the system 
*/   
      void gestFrame(char*);
/*!
  \brief changes the alarm set abilitation
*/   
      void 	toggleAbil();
/*!
  \brief Shows the object
*/       
      void	show();
private:
      sveglia* svegliolina;
      char iconOn[50];
      char iconOff[50]; 
};
#endif //IMPSVEGLIA


/*****************************************************************
**calibration
****************************************************************/

#ifndef CALIBRATION
#define CALIBRATION

#include "calibrate.h"
/*!
  \class calibration
  \brief Calibrate the device

  \author Davide
  \date lug 2005
*/  
class calibration : public bannOnDx
{
    Q_OBJECT
public:
     calibration( sottoMenu  *, const char *,const char *);     
signals:

private slots:	
     void 	doCalib();
     void	fineCalib();
private:
      Calibrate* calib;
};
#endif //CALIBRATION


/*****************************************************************
**beep
****************************************************************/

#ifndef BEEP_H
#define BEEP_H
/*!
  \class setDataOra
  \brief Beep (dis)abilitation 

  \author Davide
  \date lug 2005
*/  
class impBeep : public bannOnSx
{
    Q_OBJECT
public:
     impBeep( sottoMenu  *, const char *,char *,const char *,const char *);     
signals:

public slots:	
     void 	toggleBeep();
private:
      char iconOn[50];
      char iconOff[50]; 
};
#endif //BEEP_H

/*****************************************************************
**contrasto
****************************************************************/

#ifndef IMP_CONTR_H
#define IMP_CONTR_H

#include "contrpage.h"
/*!
  \class setDataOra
  \brief Contast regulation 
  
  \author Davide
  \date lug 2005
*/  
class impContr : public bannOnDx
{
    Q_OBJECT
public:
     impContr( sottoMenu  *, const char *,char *,const char *);     
signals:

private slots:	
     void 	showContr();
     void 	contrMade();
private:
     contrPage* contrasto; 
};
#endif //IMP_CONTR_H

/*****************************************************************
**versione 
****************************************************************/

#ifndef VERSIONE_H
#define VERSIONE_H

#include "versio.h"
/*!
  \class machVers
  \brief Displaies the machine versions (Fw / pic / Hw)

  \author Davide
  \date lug 2005
*/  
class machVers : public bannOnDx
{
    Q_OBJECT
public:
     machVers( sottoMenu  *, const char *,  versio*, const char*);     
signals:
private slots:
     void tiempout();
     void showVers();
private:
  versio * v;
  QTimer *tiempo;
};
#endif //VERSIONE_H


/*****************************************************************
**passWord
****************************************************************/

#ifndef PASSWORD_H
#define PASSWORD_H


#include "bann2but.h"
#include "tastiera.h"
/*!
  \class impPassword
  \brief Manages the password.

  It's possible (dis)abilitate the password and to change the actual password.
  \author Davide
  \date lug 2005
*/  
class impPassword : public bann2But 
{
    Q_OBJECT
public:
     impPassword (QWidget *,const char *, char* , char*, char*,char*,int );     
//     void 	setActivation(bool);
signals:
/*!
  \brief  Emitted when the password is (dis)abilitated so BtMain knows if has to ask password or not
*/           
     void activatePaswd(bool);
/*!
  \brief  Emitted when the password is changed so BtMain knows which password has to wait
*/           
     void 	setPwd(bool,char*);
public slots:	
/*!
  \brief  Changes the activation state
*/ 
      void 	toggleActivation();
/*!
  \brief  Shows the banner
*/ 
      void	show();
/*!
  \brief  Shows the keypad to compose the password key
*/      
      void  reShow1(char*);
/*!
  \brief  Shows the keypad to compose the confirmation of the password key
*/      
      void  reShow2(char*);      
/*!
  \brief  Stops the error beep made when the password insertion is wrong
*/          
      void  tiempout();
/*!
  \brief  Reimplements QWidget::setEnabled(bool)
*/           
      void setEnabled ( bool );
private:
      char paswd[10];
      bool active;
      char iconOn[50];
      char iconOff[50];       
      tastiera *tasti;
      bool sb;
      QTimer * tiempo;
      unsigned char starting;
};
#endif //PASSWORD_H



