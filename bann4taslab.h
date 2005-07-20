/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann4taslab.h
**
**Riga con tasto ON OFF, icona centrale e scritta sotto
**
****************************************************************/

#ifndef BANN4TASLAB_H
#define BANN4TASLAB_H

#include "banner.h"
#include "main.h"

/*!
  \class bann4tasLab
  \brief This is a class that describes a banner with 4 buttons a label and a text under them.
  In this banner it is possible to have 4 buttons visible and the label hidden or two external buttons visible with the label and the central two buttons hidden
  \author Davide
  \date lug 2005
*/    

class bann4tasLab : public banner
{
    Q_OBJECT
public:
     bann4tasLab( QWidget * parent=NULL, const char *name=NULL);     
signals:
private:
};

#define BUT4TL_DIM 		60
#define ICO4TL_DIM		120	


#endif //BANN4TASLAB_H
