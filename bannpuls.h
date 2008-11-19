/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannpuls.h
**
**Riga con tasto ON a dx, scritta sotto e immagine da parte
**
****************************************************************/

#ifndef BANNPULS_H
#define BANNPULS_H

#include "banner.h"

#include <QWidget>

#define BANPULS_BUT_DIM  60
#define BANPULS_ICON_DIM_X 120
#define BANPULS_ICON_DIM_Y 60

/*!
  \class bannOnDx
  \brief This is a class that describes a banner with a button on the right, an icon in the center and a text on the bottom
  \author Davide
  \date lug 2005
*/

class bannPuls : public banner
{
Q_OBJECT
public:
	bannPuls(QWidget *parent);

signals:
	void click();
	void pressed();
	void released();
};


#endif //BANNPULS_H
