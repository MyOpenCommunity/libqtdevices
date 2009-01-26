/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann4_buttons.h
**
** In this file there are contained all generic banners with 4 buttons.
**
****************************************************************/

#ifndef BANN4_BUTTONS_H
#define BANN4_BUTTONS_H

#include "banner.h"

class QWidget;


#define BAN4BUT_DIM 60

/*!
  \class bann4But
  \brief This is a class that describes a banner with 4 buttons
  \author Ciminaghi
  \date June 2006
*/
class bann4But : public banner
{
Q_OBJECT
public:
	bann4But(QWidget *);
};

#endif // BANN4_BUTTONS_H
