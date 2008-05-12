/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannonoff2scr.h
**
**Riga con tasto ON OFF, icona centrale e scritta sotto e scritta aggiuntiva
**
****************************************************************/

#ifndef BANNONOFF2SCR_H
#define BANNONOFF2SCR_H

#include "banner.h"
#include <qwidget.h>

/*!
  \class bannOnOff2scr
  \brief This is a class that describes a banner with a button on the right and on the left and a text on the bottom area and an other the center-left
  \author Davide
  \date lug 2005
*/   

class bannOnOff2scr : public banner
{
    Q_OBJECT
public:
     bannOnOff2scr( QWidget *, const char * );

     void setIcons(const char *onIcon=0, const char *offIcon=0);
public slots:	
private:
  signals:

};


#define BUTONOFF2SCR_ICON_DIM_X 		80
#define BUTONOFF2SCR_ICON_DIM_Y		60	
#define BANONOFF2SCR_BUT_DIM		60

#endif //BANNONOFF2SCR_H
