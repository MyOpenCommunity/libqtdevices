/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann2_buttons.h
**
** In this file there are contained all generic banners with 2 buttons.
**
****************************************************************/

#ifndef BANN2_BUTTONS_H
#define BANN2_BUTTONS_H

#include "banner.h"

class QWidget;


/*!
  \class bann2But
  \brief This is a class that describes a banner with a text between 2 buttons 
  \author Davide
  \date lug 2005
*/
class bann2But : public banner
{
Q_OBJECT
public:
	bann2But(QWidget *parent);
};


/*!
  \class bannOnOff
  \brief This is a class that describes a banner with a button on the right and on the left, an icon on the center and a text on the bottom
  \author Davide
  \date lug 2005
*/
class bannOnOff : public banner
{
Q_OBJECT
public:
	bannOnOff(QWidget *w=0);
};


/*!
  \class bannOnOff2scr
  \brief This is a class that describes a banner with a button on the right and on the left and a text on the bottom area and an other the center-left
  \author Davide
  \date lug 2005
*/
class bannOnOff2scr : public banner
{
Q_OBJECT
public:
	bannOnOff2scr(QWidget *w=0);
};

#endif // BANN2_BUTTONS_H
