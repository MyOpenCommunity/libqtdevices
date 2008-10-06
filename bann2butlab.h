/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann2butlab.h
**
**Riga con tasto ON OFF, icona centrale e scritta sotto
**
****************************************************************/

#ifndef BANN2BUT_LAB_H
#define BANN2BUT_LAB_H

#include "banner.h"

#include <qwidget.h>


#define BUT2BL_ICON_DIM_X 120
#define BUT2BL_ICON_DIM_Y 60
#define BAN2BL_BUT_DIM 60

/*!
  \class bann2butLab
  \brief This is a class that describes a banner with a label between 2 buttons and a text under them
  There are 2 alternative buttons on the left and only one is visible at a time.
  \author Davide
  \date lug 2005
*/

class bann2butLab : public banner
{
Q_OBJECT
public:
	bann2butLab(QWidget *, const char *);
};



#endif //BANN2BUT_LAB_H
