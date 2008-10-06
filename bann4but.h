/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann4but.h
**
**Riga con 4 tasti e scritta sotto
**
****************************************************************/

#ifndef BANN4BUT_H
#define BANN4BUT_H

#include "banner.h"
#include <qwidget.h>


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
	bann4But(QWidget *, const char *);
};

#endif //BANN3BUT_H
