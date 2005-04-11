/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** radio.h
**
**definizioni della pagina di visualizzazione radio
**
****************************************************************/

#ifndef RADIO_H
#define RADIO_H

#include <qframe.h>
#include <qlabel.h>
#include <qprocess.h>
#include "btbutton.h"
#include "main.h"
#include "bannfrecce.h"
#include <qlcdnumber.h>


    
class  radio : public QWidget
{
   Q_OBJECT
public:
    radio( QWidget *parent=0, const char *name=0 );
   void 	setBGColor(int, int , int );
   void 	setFGColor(int , int , int );
   void 	setBGColor(QColor);
   void 	setFGColor(QColor);
   int 	setBGPixmap(char* );
   BtButton *memoBut,*decBut,*aumBut,*autoBut,*manBut,*cicBut;
   BtButton *unoBut,*dueBut,*treBut,*quatBut,*cinBut,*cancBut;
   QLabel* rdsLabel,* radioName,*progrText;
   QLCDNumber *freq;
   void 	setName(char*);
   char* 	getName();
   void 	setFreq(float);
   float 	getFreq();
   void 	setRDS(char*);
   char* 	getRDS();    
   void 	setStaz(uchar);
   uchar 	getStaz();
   bool 	isManual();
   void 	draw(); 
    
signals:
    void 	Closed();
//    void 	gestFrame(char*);
    void 	aumFreqAuto();
    void	decFreqAuto();
    void	aumFreqMan();
    void	decFreqMan();
    void	memoFreq(uchar);
    void 	changeStaz();
    void	richFreq();
    void 	entroParam();
    void	escoParam();
    
public slots:
    void showRadio();
    void setAuto();
    void setMan();
    void memo1();
    void memo2();
    void memo3();
    void memo4();
    void memo5();
    void cambiaContesto();
    void ripristinaContesto();
    void verTas();
/*    void	changeStaz();
    void	aumFreq();
    void	decFreq();
    void	autoMan();
    void	manAuto();
    void	memo();
    void	changeRDS();*/
private:
    float	frequenza;
    uchar	stazione;
    char 	rds[9];
    char 	nome[15];
    bool 	manual;
    bannFrecce * bannNavigazione;
};


#endif // RADIO_H
