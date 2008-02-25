/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannOnDx.cpp
**
**Riga con tasto ON a Dx e scritta descrittiva da parte
**
****************************************************************/

#include "bannondx.h"
#include "main.h"
#include "sottomenu.h"
#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>


bannOnDx::bannOnDx(sottoMenu *parent,const char *name )
        : banner( parent, name )
{
    unsigned char nr=parent->getNumRig();

     addItem( BUT1 , MAX_WIDTH-BUTONDX_BUT_DIM_X, ( ( (MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE)/nr)-BUTONDX_BUT_DIM_Y)/2 ,  BUTONDX_BUT_DIM_X ,BUTONDX_BUT_DIM_Y );
     addItem( TEXT , 0, 0 , MAX_WIDTH-BUTONDX_BUT_DIM_X, ( (MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE)/nr));
    connect(this,SIGNAL(sxClick()),this,SIGNAL(click()));
}

bannOnSx::bannOnSx( sottoMenu *parent,const char *name )
        : banner( parent, name )
{
    unsigned char nr=parent->getNumRig();
     addItem( BUT1 ,0/* MAX_WIDTH-BUTONDX_BUT_DIM_X*/,  ( ( (MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE)/nr)-BUTONSX_BUT_DIM_Y)/2 ,  BUTONSX_BUT_DIM_X ,BUTONSX_BUT_DIM_Y );
     addItem( TEXT ,BUTONSX_BUT_DIM_X, 0 , MAX_WIDTH-BUTONSX_BUT_DIM_X, ( (MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE)/nr));
     connect(this,SIGNAL(sxClick()),this,SIGNAL(click()));
}
