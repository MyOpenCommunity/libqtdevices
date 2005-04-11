/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannCiclaz.cpp
**
**Riga con tasto ON OFF, icona centrale divisa in due per regolazione
**
****************************************************************/

#include "bannciclaz.h"
#include "main.h"

#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>


 bannCiclaz:: bannCiclaz( QWidget *parent,const char *name )
        : banner( parent, name )
{
    addItem( BUT1 , 0 ,(MAX_HEIGHT/NUM_RIGHE-BANCICL_BUT_DIM_Y)/2 , BANCICL_BUT_DIM_X , BANCICL_BUT_DIM_Y );
    addItem( BUT2 , MAX_WIDTH-BANCICL_BUT_DIM_X , (MAX_HEIGHT/NUM_RIGHE-BANCICL_BUT_DIM_Y)/2 , BANCICL_BUT_DIM_X , BANCICL_BUT_DIM_Y );
    
    addItem( BUT3 ,BANCICL_BUT_DIM_X ,BANCICL_H_SCRITTA, (MAX_WIDTH-2*BANCICL_BUT_DIM_X)/2, MAX_HEIGHT/NUM_RIGHE- BANCICL_H_SCRITTA);
    addItem( BUT4 , MAX_WIDTH/2 , BANCICL_H_SCRITTA, (MAX_WIDTH-2*BANCICL_BUT_DIM_X)/2 , MAX_HEIGHT/NUM_RIGHE- BANCICL_H_SCRITTA );
    
    addItem( TEXT , BANCICL_BUT_DIM_X , 0 , MAX_WIDTH-2*BANCICL_BUT_DIM_X, BANCICL_H_SCRITTA );    
}



#define BANCICL_BUT_DIM_X 		60
#define BANCICL_BUT_DIM_Y		60	
#define BANCICL_H_SCRITTA		20
