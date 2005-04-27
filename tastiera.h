/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** tastiera.h
**
**definizioni della pagina di tastiera numerica
**
****************************************************************/

#ifndef TASTIERA_H
#define TASTIERA_H

#include <qframe.h>
#include "btlabel.h"
#include <qprocess.h>
#include "btbutton.h"
#include "main.h"
#include <qcursor.h>
    
class  tastiera : public QWidget
{
   Q_OBJECT
public:
    tastiera( QWidget *parent=0, const char *name=0 );
   void 	setBGColor(int, int , int );
   void 	setFGColor(int , int , int );
   void 	setBGColor(QColor);
   void 	setFGColor(QColor);
   int 	setBGPixmap(char* );
   BtButton *zeroBut, *unoBut,*dueBut,*treBut,*quatBut,*cinBut,*seiBut, *setBut, *ottBut, *novBut, *cancBut, *okBut;
   BtLabel* digitLabel, *scrittaLabel;
   void 	draw(); 
   void	setMode(char);
    
signals:
    void 	Closed(char*);
    
public slots:
    void ok();
    void canc();
    void showTastiera();
    void press0();
    void press1();
    void press2();
    void press3();
    void press4();
    void press5();
    void press6();
    void press7();
    void press8();
    void press9();    
private:
    char 	pwd[6];
    char mode;
};

enum tastiType{
                HIDDEN,
	CLEAN
   };

#define BUT_DIM	60
#define LINE 		MAX_HEIGHT/5
#define POSX1		(MAX_WIDTH-BUT_DIM*3)/6
#define POSX2		POSX1*3+BUT_DIM
#define POSX3		POSX2+POSX1*2+BUT_DIM

#endif // TASTIERA_H
