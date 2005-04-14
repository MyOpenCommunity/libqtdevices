/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann2butlab.h
**
**Riga con tasto ON OFF, icona centrale e scritta sotto
**
****************************************************************/

#ifndef BANN2BUT_LAB_H
#define BANN2BUT_LAB_H

#include "banner.h"
#include <qwidget.h>
#include <qlabel.h>
#include <qpixmap.h>

class bann2butLab : public banner
{
    Q_OBJECT
public:
     bann2butLab( QWidget *, const char * );

  //   void setIcons(const char *onIcon=0, const char *offIcon=0);
public slots:
private:
  signals:
};


#define BUT2BL_ICON_DIM_X	 	120
#define BUT2BL_ICON_DIM_Y		60	
#define BAN2BL_BUT_DIM		60

#endif //BANN2BUT_LAB_H
