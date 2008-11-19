/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannPuls.cpp
**
**Riga con tasto ON a dx, scritta sotto e immagine da parte
**
****************************************************************/

#include "bannpuls.h"
#include "main.h"

bannPuls::bannPuls(QWidget *parent) : banner(parent)
{
	addItem(BUT1, MAX_WIDTH-BANPULS_BUT_DIM, 0,  BANPULS_BUT_DIM ,BANPULS_BUT_DIM);
	addItem(ICON,BANPULS_BUT_DIM, 0,  BANPULS_ICON_DIM_X ,BANPULS_ICON_DIM_Y);
	addItem(TEXT, 0 , BANPULS_BUT_DIM , MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE-BANPULS_BUT_DIM);
	
	connect(this,SIGNAL(sxClick()),this,SIGNAL(click()));
	connect(this,SIGNAL(sxPressed()),this,SIGNAL(pressed()));
	connect(this,SIGNAL(sxReleased()),this,SIGNAL(released()));
}

