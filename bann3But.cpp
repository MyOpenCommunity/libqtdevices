/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann3But.cpp
**
**Riga con tasto ON OFF, icona centrale e scritta sotto
**
****************************************************************/

#include "bann3but.h"
#include "main.h"
//#include "btbutton.h"
#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>


bann3But::bann3But( QWidget *parent,const char *name )
        : banner( parent, name )
{
    addItem( BUT1 , 0 , 0 , BAN3BUT_BUT_DIM , BAN3BUT_BUT_DIM );
    addItem( BUT2 , MAX_WIDTH-BAN3BUT_BUT_DIM , 0 , BAN3BUT_BUT_DIM , BAN3BUT_BUT_DIM );
    addItem( TEXT , 0 , BAN3BUT_BUT_DIM , MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE-BAN3BUT_BUT_DIM );
    addItem( BUT3 , MAX_WIDTH/2-BAN3BUT_BUTCEN_DIM/2, 0 , BAN3BUT_BUTCEN_DIM, BAN3BUT_BUT_DIM);
    connect(this,SIGNAL(csxClick()),this,SIGNAL(centerClick()));
}


/*void bann3But::setIcons( const char *onIcon)//, const char *offIcon)
{
    SetIcons(ICON_ON_60,ICON_OFF_60,onIcon);
}*/
