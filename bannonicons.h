/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannonicons.h
**
**Riga con tasto ON/OFF, 2 icone centrali e scritta sotto
**
****************************************************************/

#ifndef BANNONICONS
#define BANNONICONS

#include "banner.h"

#include <QWidget>

/*!
  \class bannOnIcons
  \brief vattelapesca
  \author Davide
  \date lug 2005
*/

class bannOnIcons : public banner
{
Q_OBJECT
public:
	bannOnIcons(QWidget *parent);
};


#define BANNONICONS_ICON_DIM 60
#define BANNONICONS_BUT_DIM 60

#endif //BANNONICONS
