/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannOnOff.cpp
**
**Riga con tasto ON OFF, icona centrale e scritta sotto
**
****************************************************************/

#include "bannonoff.h"
#include "main.h"
//#include "btbutton.h"
#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>


bannOnOff::bannOnOff( QWidget *parent,const char *name )
        : banner( parent, name )
{
    addItem( BUT1 , MAX_WIDTH-BANONOFF_BUT_DIM , 0 , BANONOFF_BUT_DIM , BANONOFF_BUT_DIM );
    addItem( BUT2 , 0 , 0 , BANONOFF_BUT_DIM , BANONOFF_BUT_DIM );
    addItem( TEXT , 0 , BANONOFF_BUT_DIM , MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE-BANONOFF_BUT_DIM );
    addItem( ICON , BANONOFF_BUT_DIM , 0 , BUTONOFF_ICON_DIM_X , BUTONOFF_ICON_DIM_Y );
    
   /*connect(this,SIGNAL(sxClick()),this,SIGNAL(sxClick()));
    connect(this,SIGNAL(dxClick()),this,SIGNAL(dxClick()));*/
}


void bannOnOff::setIcons( const char *onIcon , const char *offIcon)
{
    SetIcons(ICON_ON,ICON_OFF,onIcon,offIcon);
}

