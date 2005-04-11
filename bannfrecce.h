/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannonoff.h
**
**Riga con tasto ON OFF, icona centrale e scritta sotto
**
****************************************************************/

#ifndef BANNFRECCE
#define BANNFRECCE

#include "banner.h"
#include "main.h"

#include <qwidget.h>
#include <qlabel.h>
#include <qpixmap.h>

class bannFrecce : public banner
{
    Q_OBJECT
public:
     bannFrecce( QWidget * parent=NULL, const char *name=NULL ,uchar num=3,char* IconBut4=ICON_FRECCIA_DX);
     
signals:
  void  backClick(); 
  void  upClick(); 
  void  downClick(); 
  void  forwardClick(); 

private:

};


#define BUTFRECCE_DIM_X 		60
#define BUTFRECCE_DIM_Y		60	


#endif //BANNFRECCE
