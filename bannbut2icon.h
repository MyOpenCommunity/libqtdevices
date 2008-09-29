/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannbut2icon.h
**
** 2 icone nel centro e pulsante sulla destra
**
****************************************************************/

#ifndef BANNBUT2ICON_H
#define BANNBUT2ICON_H

#include "banner.h"
#include <qwidget.h>



#define BANNBUT2ICON_TEXT_DIM_X          240
#define BANNBUT2ICON_TEXT_DIM_Y           20
#define BANNBUT2ICON_ICON_DIM_X           60
#define BANNBUT2ICON_ICON_DIM_Y           60
#define BANNBUT2ICON_BUT_DIM              60


/*!
  \class bannBut2Icon
  \brief This is a class that describes a banner with a button on the right, 
  two icons in the middle and some text below
  \author Ciminaghi
  \date jun 2006
*/

class bannBut2Icon : public banner
{
Q_OBJECT
public:
	bannBut2Icon(QWidget *, const char * = 0);
};

#endif //BANNBUT2ICON_H
