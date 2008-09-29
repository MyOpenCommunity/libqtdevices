/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannButIcon.cpp
**
** Riga con icona e tasto a dx
**
****************************************************************/

#include "bannbuticon.h"
#include "main.h"


bannButIcon::bannButIcon(QWidget *parent,const char *name) : banner(parent, name)
{
	addItem(BUT1 , MAX_WIDTH - BANNBUTICON_BUT_DIM, 0, BANNBUTICON_BUT_DIM, BANNBUTICON_BUT_DIM);
	addItem(ICON, BANNBUTICON_BUT_DIM, 0, BANNBUTICON_ICON_DIM_X, BANNBUTICON_ICON_DIM_Y);
	addItem(TEXT, 0, BANNBUTICON_BUT_DIM, MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE - BANNBUTICON_BUT_DIM);
}


