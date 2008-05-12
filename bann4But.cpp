/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann4But.cpp
**
**Riga con 4 tasti e riga sotto
**
****************************************************************/

#include "bannfrecce.h"
#include "main.h"
#include "bann4but.h"

bann4But::bann4But( QWidget *parent,const char *name )
	: banner( parent, name )
{
    // sx
    addItem(BUT1,(MAX_WIDTH/4-BAN4BUT_DIM)/2,0,BAN4BUT_DIM , BAN4BUT_DIM);    
    // csx
    addItem(BUT3 , MAX_WIDTH/4+(MAX_WIDTH/4-BAN4BUT_DIM)/2 , 0 , BAN4BUT_DIM ,BAN4BUT_DIM);
    // cdx
    addItem(BUT4 , MAX_WIDTH/2+(MAX_WIDTH/4-BAN4BUT_DIM)/2 , 0 ,BAN4BUT_DIM, BAN4BUT_DIM);		    
    // dx
    addItem(BUT2, MAX_WIDTH*3/4+(MAX_WIDTH/4-BAN4BUT_DIM)/2 , 0 , BAN4BUT_DIM , BAN4BUT_DIM );
    addItem(TEXT,0, BAN4BUT_DIM , MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE-BAN4BUT_DIM);
    Draw();
}
