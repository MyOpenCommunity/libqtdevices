/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann4tasLab.cpp
**
**Riga con tasto ON OFF, icona centrale e scritta sotto
**
****************************************************************/


#include "bann4taslab.h"
#include "main.h"

bann4tasLab::bann4tasLab(QWidget *parent,const char *name) : banner(parent, name)
{
	// sx
	addItem(BUT1,(MAX_WIDTH/4-BUT4TL_DIM)/2,0,BUT4TL_DIM , BUT4TL_DIM );
	// csx
	addItem(BUT3, MAX_WIDTH/4+(MAX_WIDTH/4-BUT4TL_DIM)/2 , 0 , BUT4TL_DIM , BUT4TL_DIM);
	// cdx
	addItem(BUT4, MAX_WIDTH/2+(MAX_WIDTH/4-BUT4TL_DIM)/2 , 0 ,BUT4TL_DIM, BUT4TL_DIM);
	// dx
	addItem(BUT2, MAX_WIDTH*3/4+(MAX_WIDTH/4-BUT4TL_DIM)/2 , 0 , BUT4TL_DIM , BUT4TL_DIM );
	addItem(ICON, (MAX_WIDTH-ICO4TL_DIM)/2,0,ICO4TL_DIM, BUT4TL_DIM );
	addItem(TEXT, 0, BUT4TL_DIM , MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE-BUT4TL_DIM);
	impostaAttivo(2);
	Draw();
}
