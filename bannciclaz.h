/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannciclaz.h
**
**Riga con tasto ON OFF, icona centrale divisa in 2 per regolare
**
****************************************************************/

#ifndef BANNCICLAZ_H
#define BANNCICLAZ_H

#include "banner.h"

#include <qwidget.h>

#define BANCICL_BUT_DIM_X 60
#define BANCICL_BUT_DIM_Y 60
#define BANCICL_H_SCRITTA 20

/*!
 * \class bann4tasLab
 * \brief This is a class that describes a banner with 4 buttons and a text over the two internal buttons.
 *
 * The two external buttons are big while the internal ones leave the space to host the descripting text over them.
 * \author Davide
 * \date lug 2005
 */
class bannCiclaz : public banner
{
Q_OBJECT
public:
     bannCiclaz(QWidget *, const char *, int nbuts=4);
};

#endif //BANNCICLAZ_H
