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

bann2butLab::bann2butLab(QWidget *parent,const char *name) : banner(parent, name)
{
	addItem(BUT1, MAX_WIDTH - BAN2BL_BUT_DIM , 0 , BAN2BL_BUT_DIM , BAN2BL_BUT_DIM);
	addItem(BUT2, 0 , 0 , BAN2BL_BUT_DIM , BAN2BL_BUT_DIM);
	addItem(BUT4, 0 , 0 , BAN2BL_BUT_DIM , BAN2BL_BUT_DIM);
	addItem(TEXT, 0 , BAN2BL_BUT_DIM , MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE - BAN2BL_BUT_DIM);
	addItem(ICON, BAN2BL_BUT_DIM , 0 , BUT2BL_ICON_DIM_X , BUT2BL_ICON_DIM_Y);
}
