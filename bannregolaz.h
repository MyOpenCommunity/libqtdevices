/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannregolaz.h
**
**Riga con tasto ON OFF, icona centrale divisa in 2 per regolare
**
****************************************************************/

#ifndef BANNREGOLAZ_H
#define BANNREGOLAZ_H

#include "banner.h"
#include <qwidget.h>
#include <qlabel.h>
#include <qpixmap.h>


/*!
  \class bannRegolaz
  \brief This is a class that describes a banner with a button on the right and on the left and two adjacent buttons on the center (thought to represent each one an half of the same image) and a text in the bottom.
  \author Davide
  \date lug 2005
*/   

class bannRegolaz : public banner
{
    Q_OBJECT
public:
     bannRegolaz( QWidget *, const char * );

private slots:
  void armTimRipdx();
  void armTimRipsx();
  void killTimRipdx();
  void killTimRipsx();
  signals:
private:
  QTimer *timRip;

};


#define BUTREGOL_ICON_DIM_X 	120
#define BUTREGOL_ICON_DIM_Y		60	
#define BANREGOL_BUT_DIM		60



#endif //BANNREGOLAZ_H
