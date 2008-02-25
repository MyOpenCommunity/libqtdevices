/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannOnOff2scr.cpp
**
**Riga con tasto ON OFF, icona centrale e scritta sotto e scritta aggiuntiva
**
****************************************************************/

#include "bannonoff2scr.h"
#include "main.h"
//#include "btbutton.h"
#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>


bannOnOff2scr::bannOnOff2scr( QWidget *parent,const char *name )
        : banner( parent, name )
{
    addItem( BUT1 , 0 , 0 , BANONOFF2SCR_BUT_DIM , BANONOFF2SCR_BUT_DIM );
    addItem( BUT2 , MAX_WIDTH-BANONOFF2SCR_BUT_DIM , 0 , BANONOFF2SCR_BUT_DIM , BANONOFF2SCR_BUT_DIM );
    addItem( TEXT , 0 , BANONOFF2SCR_BUT_DIM , MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE-BANONOFF2SCR_BUT_DIM );
    addItem( ICON , MAX_WIDTH-BANONOFF2SCR_BUT_DIM-BUTONOFF2SCR_ICON_DIM_X , 0 , BUTONOFF2SCR_ICON_DIM_X , BUTONOFF2SCR_ICON_DIM_Y );
//      addItem( ICON , 60 , 0 , 120 , BUTONOFF2SCR_ICON_DIM_Y );
    addItem( TEXT2 , BANONOFF2SCR_BUT_DIM ,0/*(BUTONOFF2SCR_ICON_DIM_Y -(MAX_HEIGHT/NUM_RIGHE-BUTONOFF2SCR_ICON_DIM_Y))/2*/ ,MAX_WIDTH-2*BANONOFF2SCR_BUT_DIM-BUTONOFF2SCR_ICON_DIM_X, /*MAX_HEIGHT/NUM_RIGHE-BANONOFF2SCR_BUT_DIM */BUTONOFF2SCR_ICON_DIM_Y);
    
}


void bannOnOff2scr::setIcons( const char *onIcon , const char *offIcon)
{
    SetIcons(ICON_ON,ICON_OFF,onIcon,offIcon);
}
