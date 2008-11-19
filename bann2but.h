/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann2but.h
**
**Riga con tasto ON OFF, icona centrale e scritta sotto
**
****************************************************************/

#ifndef BANN2BUT_H
#define BANN2BUT_H

#include "banner.h"

#include <QWidget>

#define  BAN2BUT_BUT_DIM 60

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




#endif //BANN2BUT_H
