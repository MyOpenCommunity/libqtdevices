/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannBut2Icon.cpp
**
** Riga con due icona e tasto a dx
**
****************************************************************/

#include "bannbut2icon.h"
#include "main.h"
//#include "btbutton.h"
#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>


bannBut2Icon::bannBut2Icon( QWidget *parent,const char *name )
  : banner( parent, name )
{
    addItem( BUT1 , MAX_WIDTH - BANNBUT2ICON_BUT_DIM, 0, 
	     BANNBUT2ICON_BUT_DIM, BANNBUT2ICON_BUT_DIM);
    addItem( ICON, BANNBUT2ICON_BUT_DIM, 0,
	     BANNBUT2ICON_ICON_DIM_X, BANNBUT2ICON_ICON_DIM_Y);
    addItem( ICON2, BANNBUT2ICON_BUT_DIM + BANNBUT2ICON_ICON_DIM_X , 0,
	     BANNBUT2ICON_ICON_DIM_X, BANNBUT2ICON_ICON_DIM_Y);
    addItem( TEXT, 0, BANNBUT2ICON_BUT_DIM, MAX_WIDTH , 
	     MAX_HEIGHT/NUM_RIGHE - BANNBUT2ICON_BUT_DIM);
}


