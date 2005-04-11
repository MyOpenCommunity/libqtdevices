/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannondx.h
**
**Riga con tasto ON a dx e scritta da parte
**
****************************************************************/

#ifndef BANNONDX_H
#define BANNONDX_H

#include "banner.h"
#include <qwidget.h>
#include <qlabel.h>
#include <qpixmap.h>

class bannOnDx : public banner
{
    Q_OBJECT
public:
     bannOnDx( QWidget *, const char * );

public slots:
signals:
  void click();

private:

};


#define BUTONDX_BUT_DIM_X 	80
#define BUTONDX_BUT_DIM_Y	80
#define BUTONDX_H_SCRITTA	20

#endif //BANNONDX
