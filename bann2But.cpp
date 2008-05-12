/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann2But.cpp
**
**Riga con tasto ON OFF, icona centrale e scritta sotto
**
****************************************************************/

#include "bann2but.h"
#include "main.h"

bann2But::bann2But( QWidget *parent,const char *name )
        : banner( parent, name )
{
    addItem( BUT1 , 0 ,(MAX_HEIGHT/NUM_RIGHE-BAN2BUT_BUT_DIM)/2 , BAN2BUT_BUT_DIM , BAN2BUT_BUT_DIM );
    addItem( BUT2 , MAX_WIDTH-BAN2BUT_BUT_DIM ,(MAX_HEIGHT/NUM_RIGHE-BAN2BUT_BUT_DIM)/2 , BAN2BUT_BUT_DIM , BAN2BUT_BUT_DIM );
    addItem( TEXT , BAN2BUT_BUT_DIM , 0, MAX_WIDTH-2* BAN2BUT_BUT_DIM , MAX_HEIGHT/NUM_RIGHE );
}


