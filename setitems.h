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

class setDataOra : public bannOnDx 
{
    Q_OBJECT
public:
     setDataOra( QWidget *, const char * );     
signals:
  //    void sendFrame(char *); 
public slots:
//     void Attiva();
      impostaTime settalora;
//private:
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

class impostaSveglia : public bann2But 
{
    Q_OBJECT
public:
     impostaSveglia( QWidget *, const char * ,diffSonora* , char* , char*, char*, int, int, char* , char*, char* , char*);     
     void 	setAbil(bool);
signals:
//     void sendFrame(char *); 
     void spegniSveglia();
//     void freeze(bool);
public slots:	
      void gestFrame(char*);
      void 	toggleAbil();
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

class calibration : public bannOnDx
{
    Q_OBJECT
public:
     calibration( QWidget *, const char *,const char *);     
signals:

public slots:	
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

class impBeep : public bannOnDx
{
    Q_OBJECT
public:
     impBeep( QWidget *, const char *,const char *,const char *);     
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

class impContr : public bannOnDx
{
    Q_OBJECT
public:
     impContr( QWidget *, const char *,const char *);     
signals:

public slots:	
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
class machVers : public bannOnDx
{
    Q_OBJECT
public:
     machVers( QWidget *, const char *,  versio*, const char*);     
signals:
public slots:
     void tiempout();
     void showVers();
private:
  versio * v;
  QTimer *tiempo;
};
#endif //VERSIONE_H



