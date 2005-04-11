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
#include <qlabel.h>
#include <qpixmap.h>

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
