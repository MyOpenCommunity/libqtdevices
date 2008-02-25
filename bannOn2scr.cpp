/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannOnOff2scr.cpp
**
**Riga con tasto ON OFF, icona centrale e scritta sotto e scritta aggiuntiva
**
****************************************************************/

#include "bannon2scr.h"
#include "main.h"
//#include "btbutton.h"
#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>


bannOn2scr::bannOn2scr( QWidget *parent,const char *name )
        : banner( parent, name )
{
   int x = 0, y = 0;
   addItem(TEXT2, x, y, BANON2SCR_TEXT1_DIM_X, BANON2SCR_TEXT1_DIM_Y);
   x = BANON2SCR_TEXT1_DIM_X ;
   addItem(ICON, x, y, BUTON2SCR_ICON_DIM_X, BUTON2SCR_ICON_DIM_Y);
   x += BUTON2SCR_ICON_DIM_X ;
   addItem(ICON2, x, y, BUTON2SCR_ICON_DIM_X, BUTON2SCR_ICON_DIM_Y);
   x += BUTON2SCR_ICON_DIM_X ;
   addItem(BUT2, x, y, BANON2SCR_BUT_DIM, BANON2SCR_BUT_DIM);
   y = BANON2SCR_BUT_DIM ;
   x = 0;
   addItem(TEXT, x, y, MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE - BANON2SCR_BUT_DIM);
}

void bannOn2scr::setIcons(const char *sxIcon, const char *dxIcon, 
			     const char *onIcon)
{
   banner::SetIcons(sxIcon, dxIcon, onIcon);
}
