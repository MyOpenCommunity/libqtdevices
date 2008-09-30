/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannciclaz.h
**
**Riga per sorgente diff. son. multicanale
**
****************************************************************/

#ifndef BANNCICLAZ3BUT_H
#define BANNCICLAZ3BUT_H

#include "banner.h"

#include <qwidget.h>


#define BANCICL3BUT_SMALL_BUT_DIM_X 60
#define BANCICL3BUT_BIG_BUT_DIM_X 120
#define BANCICL3BUT_BUT_DIM_Y 60
#define BANCICL3BUT_H_SCRITTA 20

/*!
  \class bannCiclaz3but
  \brief This is a class that describes a banner with 3 buttons and a text along the center button
  \author Ciminaghi
  \date lug 2006
*/

class bannCiclaz3but : public banner
{
Q_OBJECT
public:
	bannCiclaz3but(QWidget *, const char *);
};




#endif //BANNCICLAZ3BUT_H
