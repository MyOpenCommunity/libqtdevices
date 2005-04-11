/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann2but.h
**
**Riga con tasto ON OFF, icona centrale e scritta sotto
**
****************************************************************/

#ifndef BANN2BUT_H
#define BANN2BUT_H

#include "banner.h"
#include <qwidget.h>
#include <qlabel.h>
#include <qpixmap.h>

class bann2But : public banner
{
    Q_OBJECT
public:
     bann2But( QWidget *, const char * );

//     void setIcons(const char *onIcon=0);//, const char *offIcon=0);
public slots:

private:
  signals:
  void centerClick();
};



#define  BAN2BUT_BUT_DIM		80
//#define BAN2BUT_SCRITTA_DIM		120

#endif //BANN2BUT_H
