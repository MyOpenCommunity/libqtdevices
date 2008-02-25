/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannCiclaz3but.cpp
**
** 
**
****************************************************************/

#include "bannciclaz3but.h"
#include "main.h"

#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>


 bannCiclaz3but:: bannCiclaz3but( QWidget *parent,const char *name )
        : banner( parent, name )
{
    addItem( BUT1 , 0 ,(MAX_HEIGHT/NUM_RIGHE-BANCICL3BUT_BUT_DIM_Y)/2 , BANCICL3BUT_SMALL_BUT_DIM_X , BANCICL3BUT_BUT_DIM_Y );
    addItem( BUT2 , MAX_WIDTH-BANCICL3BUT_SMALL_BUT_DIM_X , (MAX_HEIGHT/NUM_RIGHE-BANCICL3BUT_BUT_DIM_Y)/2 , BANCICL3BUT_SMALL_BUT_DIM_X , BANCICL3BUT_BUT_DIM_Y );
    
    addItem( BUT3 , 
	     MAX_WIDTH/2  - BANCICL3BUT_SMALL_BUT_DIM_X/2, 
	     BANCICL3BUT_H_SCRITTA, 
	     BANCICL3BUT_SMALL_BUT_DIM_X, 
	     MAX_HEIGHT/NUM_RIGHE - BANCICL3BUT_H_SCRITTA);
    
    addItem( TEXT , BANCICL3BUT_SMALL_BUT_DIM_X , 0 , MAX_WIDTH-2*BANCICL3BUT_SMALL_BUT_DIM_X, BANCICL3BUT_H_SCRITTA );    
}





