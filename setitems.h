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
private:
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
     impostaSveglia( QWidget *, const char * ,diffSonora* , char* , char*, char*, int, int);     
     void 	setAbil(bool);
signals:
//     void sendFrame(char *); 
     void spegniSveglia();
//     void freeze(bool);
public slots:	
      void deFreez();
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
     calibration( QWidget *, const char *);     
signals:

public slots:	
     void 	doCalib();
private:
      Calibrate* calib;
};
#endif //CALIBRATION
