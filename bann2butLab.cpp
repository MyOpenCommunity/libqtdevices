/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann2butLab.cpp
**
**Riga con 3 tasti (2 sovrapposti) e label in mezzo
**
****************************************************************/

#include "bann2butlab.h"
#include "main.h"
//#include "btbutton.h"
#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>

bann2butLab::bann2butLab( QWidget *parent,const char *name )
        : banner( parent, name )
{
//    addItem( BUT1 , 160 , 0 , 60 , 60 );
    addItem( BUT1 , MAX_WIDTH-BAN2BL_BUT_DIM , 0 , BAN2BL_BUT_DIM , BAN2BL_BUT_DIM );    
    addItem( BUT2 , 0 , 0 , BAN2BL_BUT_DIM , BAN2BL_BUT_DIM );
    addItem( BUT4 , 0 , 0 , BAN2BL_BUT_DIM , BAN2BL_BUT_DIM );    
    addItem( TEXT , 0 , BAN2BL_BUT_DIM , MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE-BAN2BL_BUT_DIM );
    addItem( ICON , BAN2BL_BUT_DIM , 0 , BUT2BL_ICON_DIM_X , BUT2BL_ICON_DIM_Y );
    
   /*connect(this,SIGNAL(sxClick()),this,SIGNAL(sxClick()));
    connect(this,SIGNAL(dxClick()),this,SIGNAL(dxClick()));*/
}


/*void bann2butLab::setIcons( const char *onIcon , const char *offIcon)
{
    SetIcons(ICON_ON_60,ICON_OFF_60,onIcon,offIcon);
}*/

