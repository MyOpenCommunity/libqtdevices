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
#include <qwidget.h>
#include <qlabel.h>
#include <qpixmap.h>

class bann3But : public banner
{
    Q_OBJECT
public:
     bann3But( QWidget *, const char * );

//     void setIcons(const char *onIcon=0);//, const char *offIcon=0);
public slots:

private:
  signals:
  void centerClick();
};



#define BAN3BUT_BUT_DIM		60
#define BAN3BUT_BUTCEN_DIM		120

#endif //BANN3BUT_H
