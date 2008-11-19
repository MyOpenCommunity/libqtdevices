/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannOnIcons.cpp
**
**Riga con tasto ON/OFF e due icone centrali
**
****************************************************************/

#include "bannonicons.h"
#include "main.h"


bannOnIcons::bannOnIcons(QWidget *parent) : banner(parent)
{
	addItem(BUT1 , 0 , 0 , BANNONICONS_BUT_DIM, BANNONICONS_BUT_DIM);
	addItem(TEXT , 0 , BANNONICONS_BUT_DIM , MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE-BANNONICONS_BUT_DIM);
	addItem(ICON , BANNONICONS_BUT_DIM , 0 , BANNONICONS_BUT_DIM , BANNONICONS_BUT_DIM);
	addItem(ICON2 , 2*BANNONICONS_BUT_DIM , 0 , BANNONICONS_BUT_DIM , BANNONICONS_BUT_DIM);
}




