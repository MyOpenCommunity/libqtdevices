/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannbuticon.h
**
** Icona nel centro e pulsante sulla destra
**
****************************************************************/

#ifndef BANNBUTICON_H
#define BANNBUTICON_H

#include "banner.h"

#include <QWidget>


#define BANNBUTICON_TEXT_DIM_X          240
#define BANNBUTICON_TEXT_DIM_Y           20
#define BANNBUTICON_ICON_DIM_X          120
#define BANNBUTICON_ICON_DIM_Y           60
#define BANNBUTICON_BUT_DIM              60


/*!
  \class bannButIcon
  \brief This is a class that describes a banner with a button on the right, 
  an icon in the middle and some text below
  \author Ciminaghi
  \date jun 2006
*/

class bannButIcon : public banner
{
Q_OBJECT
public:
	bannButIcon(QWidget *parent);
};


#endif //BANNBUTICON_H
