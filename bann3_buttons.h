/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann3_buttons.h
**
** In this file there are contained all generic banners with 3 buttons.
**
****************************************************************/

#ifndef BANN3_BUTTONS_H
#define BANN3_BUTTONS_H

#include "banner.h"

class QWidget;


/*!
  \class bann3But
  \brief This is a class that describes a banner with a text between 2 buttons 
  \author Davide
  \date lug 2005
*/
class bann3But : public banner
{
Q_OBJECT
public:
	bann3But(QWidget *parent);

signals:
	void centerClick();
};

#endif // BANN3_BUTTONS_H
