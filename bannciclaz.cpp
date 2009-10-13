/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** bannCiclaz.cpp
 **
 **Riga con tasto ON OFF, icona centrale divisa in due per regolazione
 **
 ****************************************************************/

#include "bannciclaz.h"

#define BANCICL_BUT_DIM_X 60
#define BANCICL_BUT_DIM_Y 60
#define BANCICL_H_SCRITTA 20


bannCiclaz::bannCiclaz(QWidget *parent,int nbut) : banner(parent)
{
	addItem(BUT1, 0, (banner_height - BANCICL_BUT_DIM_Y)/2, BANCICL_BUT_DIM_X, BANCICL_BUT_DIM_Y);
	addItem(BUT2, banner_width-BANCICL_BUT_DIM_X, (banner_height-BANCICL_BUT_DIM_Y)/2, BANCICL_BUT_DIM_X, BANCICL_BUT_DIM_Y);

	if (nbut == 4) 
	{
		addItem(BUT3, BANCICL_BUT_DIM_X, BANCICL_H_SCRITTA, (banner_width-2*BANCICL_BUT_DIM_X)/2,
			banner_height- BANCICL_H_SCRITTA);
		addItem(BUT4, banner_width / 2, BANCICL_H_SCRITTA, (banner_width-  2*BANCICL_BUT_DIM_X)/2,
			 banner_height - BANCICL_H_SCRITTA);
	}
	else if (nbut == 3)
	{
		addItem(BUT3, banner_width/2 - BANCICL_BUT_DIM_X/2, BANCICL_H_SCRITTA, BANCICL_BUT_DIM_X,
			banner_height - BANCICL_H_SCRITTA);
	}

	addItem(TEXT, BANCICL_BUT_DIM_X, 0, banner_width - 2*BANCICL_BUT_DIM_X, BANCICL_H_SCRITTA);
}

