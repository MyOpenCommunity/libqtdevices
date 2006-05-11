/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannon2scr.h
**
**Riga con testo a sx, 2 icone e tasto ON sulla destra
**
****************************************************************/

#ifndef BANNON2SCR_H
#define BANNON2SCR_H

#include "banner.h"
#include <qwidget.h>
#include <qlabel.h>
#include <qpixmap.h>


/*!
  \class bannOn2scr
  \brief This is a class that describes a banner with a button on the right, text on the bottom area and on the left, plus a couple of icons in the middle
  \author Ciminaghi
  \date apr 2006
*/   

class bannOn2scr : public banner
{
    Q_OBJECT
public:
     bannOn2scr( QWidget *, const char * );

     void setIcons(const char *sxIcon=0, const char *dxIcon=0, 
		   const char *onIcon=0);
public slots:	
private:
  signals:

};


#define BANON2SCR_TEXT1_DIM_X          100
#define BANON2SCR_TEXT1_DIM_Y           60
#define BUTON2SCR_ICON_DIM_X 		40
#define BUTON2SCR_ICON_DIM_Y            60
#define BANON2SCR_BUT_DIM		60

#endif //BANNON2SCR_H
