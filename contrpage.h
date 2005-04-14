/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** contrPage.h
**
**definizioni della pagina di definizione contrasto
**
****************************************************************/

#ifndef CONTRPAGE_H
#define CONTRPAGE_H

#include <qframe.h>
#include "btlabel.h"
#include <qprocess.h>
#include "btbutton.h"
#include "main.h"
#include <qcursor.h>
    
class  contrPage : public QWidget
{
   Q_OBJECT
public:
    contrPage( QWidget *parent=0, const char *name=0 );
   void 	setBGColor(int, int , int );
   void 	setFGColor(int , int , int );
   void 	setBGColor(QColor);
   void 	setFGColor(QColor);
   int 	setBGPixmap(char* );
   BtButton *aumBut, *decBut,*okBut;
   BtLabel* paintLabel,*colorBar;
   void 	draw(); 
    
signals:
    void 	Close();
    
public slots:
    void aumContr();    
    void decContr();    
private:
};

#define BUT_DIM	60
#define IMG_X		181
#define IMG_Y		128

#endif // CONTRPAGE_H
