/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannondx.h
**
**Riga con tasto ON a dx e scritta da parte
**
****************************************************************/

#ifndef BANNONDX_H
#define BANNONDX_H

#include "banner.h"

class sottoMenu;


/*!
  \class bannOnDx
  \brief This is a class that describes a banner with a button on the right and a text on the remaining area
  \author Davide
  \date lug 2005
*/


class bannOnDx : public banner
{
Q_OBJECT
public:
	bannOnDx(sottoMenu *parent);
signals:
	void click();
};


#define BUTONDX_BUT_DIM_X 60
#define BUTONDX_BUT_DIM_Y 60
#define BUTONSX_BUT_DIM_X 60
#define BUTONSX_BUT_DIM_Y 60
#define BUTONDX_H_SCRITTA 20

#endif //BANNONDX

#ifndef BANNONSX_H
#define BANNONSX_H

/*!
  \class bannOnSx
  \brief This is a class that describes a banner with a button on the left and a text on the remaining area
  \author Davide
  \date lug 2005
*/
class bannOnSx : public banner
{
Q_OBJECT
public:
	bannOnSx(sottoMenu *parent);
signals:
	void click();
};

#endif //BANNONSX
