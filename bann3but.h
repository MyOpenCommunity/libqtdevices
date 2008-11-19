/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann3but.h
**
**Riga con tasto ON OFF, icona centrale e scritta sotto
**
****************************************************************/

#ifndef BANN3BUT_H
#define BANN3BUT_H

#include "banner.h"

#include <QWidget>

#define BAN3BUT_BUT_DIM 60
#define BAN3BUT_BUTCEN_DIM 120


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

#endif //BANN3BUT_H
