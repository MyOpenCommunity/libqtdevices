/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** sveglia.h
**
**definizioni della pagine di impostazione sveglia
**
****************************************************************/

#ifndef SVEGLIA_H
#define SVEGLIA_H

#include <qframe.h>
#include <qlabel.h>
#include <qprocess.h>
#include <qpushbutton.h>
#include "btbutton.h"
#include "main.h"
#include "timescript.h"
#include "bannfrecce.h"
#include "diffsonora.h"
#include <qcheckbox.h>


#define 	AMPLI_NUM 	100


class  sveglia : public QFrame
{
   Q_OBJECT
public:
    sveglia( QWidget *parent=0, const char *name=0, uchar freq=1, uchar t=0, diffSonora* diso=NULL, char* frame=NULL);
   void 	setBGColor(QColor);
   void 	setFGColor(QColor);
   void 	setBGColor(int, int , int );
   void 	setFGColor(int , int , int );
   int 	setBGPixmap(char* );
   BtButton*  but[4];
   QLabel* Immagine;
   BtButton* choice[4];
   QLabel* testiChoice[4];

   void	activateSveglia(bool);
   bool	getActivation();
signals:
   void 	sendFrame(char*);
   void	ImClosed();
//    void 	gestFrame(char*);
   void	freeze(bool);
   
public slots:
    void 	okTime();	
    void 	mostra();
    void 	sel1(bool);
    void 	sel2(bool);
    void 	sel3(bool);
    void 	sel4(bool); 
    void 	Closed();
    void 	okTipo();
    void 	verificaSveglia();
    void	aumVol();
    void 	buzzerAlarm();
    void 	gestFrame(char*);
    void 	spegniSveglia();
private:
    void 	drawSelectPage();
   timeScript* dataOra;
   bannFrecce * bannNavigazione;
   uchar 	tipoSveglia,conta2min,sorgente,stazione,tipo;
   int 	contaBuzzer;
   QDateTime* oraSveglia;
   diffSonora* difson;
   uchar	volSveglia[AMPLI_NUM];
   bool	gesFrameAbil,svegliaAbil,onceToGest;
   QTimer   *minuTimer,*aumVolTimer;
   char*	frame;
};

enum sveFreq{
                SEMPRE = 1,
	ONCE = 0,
	FERIALI = 2,
	FESTIVI= 3
   };

enum sveType{
                BUZZER,
	DI_SON,
	FRAME
   };

#endif // SVEGLIA_H
