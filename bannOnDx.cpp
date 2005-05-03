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
#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>


bannOnDx::bannOnDx( QWidget *parent,const char *name )
        : banner( parent, name )
{
/*    addItem( BUT1 , 0, ( (MAX_HEIGHT/NUM_RIGHE)-BUTONDX_BUT_DIM_Y)/2 ,  BUTONDX_BUT_DIM_X ,BUTONDX_BUT_DIM_Y );
     addItem( TEXT , BUTONDX_BUT_DIM_X, 0 , MAX_WIDTH-BUTONDX_BUT_DIM_X,BUTONDX_BUT_DIM_Y);
  */     
     addItem( BUT1 , MAX_WIDTH-BUTONDX_BUT_DIM_X, ( (MAX_HEIGHT/NUM_RIGHE)-BUTONDX_BUT_DIM_Y)/2 ,  BUTONDX_BUT_DIM_X ,BUTONDX_BUT_DIM_Y );
     addItem( TEXT , 0, 0 , MAX_WIDTH-BUTONDX_BUT_DIM_X,MAX_HEIGHT/NUM_RIGHE);
    connect(this,SIGNAL(sxClick()),this,SIGNAL(click()));
}

bannOnSx::bannOnSx( QWidget *parent,const char *name )
        : banner( parent, name )
{
     addItem( BUT1 ,0/* MAX_WIDTH-BUTONDX_BUT_DIM_X*/, ( (MAX_HEIGHT/NUM_RIGHE)-BUTONSX_BUT_DIM_Y)/2 ,  BUTONSX_BUT_DIM_X ,BUTONSX_BUT_DIM_Y );
     addItem( TEXT ,BUTONSX_BUT_DIM_X, 0 , MAX_WIDTH-BUTONSX_BUT_DIM_X,MAX_HEIGHT/NUM_RIGHE);
     connect(this,SIGNAL(sxClick()),this,SIGNAL(click()));
}
