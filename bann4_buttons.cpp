#include "bann4_buttons.h"
#include "main.h"


#define BAN4BUT_DIM 60
#define BUT4TL_DIM 60
#define ICO4TL_DIM 120


bann4But::bann4But(QWidget *parent) : banner(parent)
{
	// sx
	addItem(BUT1,(MAX_WIDTH/4-BAN4BUT_DIM)/2,0,BAN4BUT_DIM , BAN4BUT_DIM);
	// csx
	addItem(BUT3, MAX_WIDTH/4+(MAX_WIDTH/4-BAN4BUT_DIM)/2 , 0 , BAN4BUT_DIM ,BAN4BUT_DIM);
	// cdx
	addItem(BUT4, MAX_WIDTH/2+(MAX_WIDTH/4-BAN4BUT_DIM)/2 , 0 ,BAN4BUT_DIM, BAN4BUT_DIM);
	// dx
	addItem(BUT2, MAX_WIDTH*3/4+(MAX_WIDTH/4-BAN4BUT_DIM)/2 , 0 , BAN4BUT_DIM , BAN4BUT_DIM );
	addItem(TEXT,0, BAN4BUT_DIM , MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE-BAN4BUT_DIM);
	Draw();
}


bann4tasLab::bann4tasLab(QWidget *parent) : banner(parent)
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

