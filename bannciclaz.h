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
#include <qlabel.h>
#include <qpixmap.h>

class bannCiclaz : public banner
{
    Q_OBJECT
public:
     bannCiclaz( QWidget *, const char * );

public slots:

signals:

private:
};


#define BANCICL_BUT_DIM_X 		60
#define BANCICL_BUT_DIM_Y		60	
#define BANCICL_H_SCRITTA		20



#endif //BANNCICLAZ_H
