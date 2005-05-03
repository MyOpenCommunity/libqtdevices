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
#include "btlabel.h"
#include <qcheckbox.h>


#define 	AMPLI_NUM 	100


class  sveglia : public QFrame
{
   Q_OBJECT
public:
    sveglia( QWidget *parent=0, const char *name=0, uchar freq=1, uchar t=0, diffSonora* diso=NULL, char* frame=NULL, char* descr1=NULL,char* descr2=NULL,char* descr3=NULL,char* descr4=NULL, char*h="12",char* m="0");
   void 	setBGColor(QColor);
   void 	setFGColor(QColor);
   void 	setBGColor(int, int , int );
   void 	setFGColor(int , int , int );
   int 	setBGPixmap(char* );
   void 	setSerNum(int);
   BtButton*  but[4];
   BtLabel* Immagine;
   BtButton* choice[4];
   BtLabel* testiChoice[4];

   void	activateSveglia(bool);
   bool	getActivation();
   void 	inizializza();
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
    void 	spegniSveglia(bool);
private:
    void 	drawSelectPage();
   timeScript* dataOra;
   bannFrecce * bannNavigazione;
   uchar 	tipoSveglia,conta2min,sorgente,stazione,tipo,aggiornaDatiEEprom;
   int 	serNum;
   bool 	buzAbilOld;
   unsigned int contaBuzzer;
   QDateTime* oraSveglia;
   diffSonora* difson;
   uchar	volSveglia[AMPLI_NUM];
   bool	gesFrameAbil,svegliaAbil,onceToGest;
   QTimer   *minuTimer,*aumVolTimer;
   char*	frame;
   char text1[50]; 
   char text2[50]; 
   char text3[50]; 
   char text4[50];       
};

enum sveFreq{
                SEMPRE = 1,
	ONCE = 0,
	FERIALI = 2,
	FESTIVI= 3
   };

enum sveType{
                BUZZER = 0,
	DI_SON = 1,
	FRAME = 2
   };



#define BASE_EEPROM	11360
#define KEY_LENGTH	5
#define AL_KEY		"\125\252\125\252\125"
#define SORG_PAR	2
#endif // SVEGLIA_H
