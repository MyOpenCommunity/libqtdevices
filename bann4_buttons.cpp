#include "bann4_buttons.h"

#define BAN4BUT_DIM 60
#define BUT4TL_DIM 60
#define ICO4TL_DIM 120


bann4But::bann4But(QWidget *parent) : banner(parent)
{
	// sx
	addItem(BUT1,(banner_width/4-BAN4BUT_DIM)/2, 0, BAN4BUT_DIM , BAN4BUT_DIM);
	// csx
	addItem(BUT3, banner_width/4+(banner_width/4-BAN4BUT_DIM)/2, 0, BAN4BUT_DIM ,BAN4BUT_DIM);
	// cdx
	addItem(BUT4, banner_width/2+(banner_width/4-BAN4BUT_DIM)/2, 0, BAN4BUT_DIM, BAN4BUT_DIM);
	// dx
	addItem(BUT2, banner_width*3/4+(banner_width/4-BAN4BUT_DIM)/2, 0, BAN4BUT_DIM, BAN4BUT_DIM);
	addItem(TEXT, 0, BAN4BUT_DIM, banner_width, banner_height-BAN4BUT_DIM);
	Draw();
}


bann4tasLab::bann4tasLab(QWidget *parent) : banner(parent)
{
	// sx
	addItem(BUT1, (banner_width/4-BUT4TL_DIM)/2, 0, BUT4TL_DIM, BUT4TL_DIM);
	// csx
	addItem(BUT3, banner_width/4+(banner_width/4-BUT4TL_DIM)/2, 0, BUT4TL_DIM, BUT4TL_DIM);
	// cdx
	addItem(BUT4, banner_width/2+(banner_width/4-BUT4TL_DIM)/2, 0, BUT4TL_DIM, BUT4TL_DIM);
	// dx
	addItem(BUT2, banner_width*3/4+(banner_width/4-BUT4TL_DIM)/2, 0, BUT4TL_DIM, BUT4TL_DIM);
	addItem(ICON, (banner_width-ICO4TL_DIM)/2, 0, ICO4TL_DIM, BUT4TL_DIM);
	addItem(TEXT, 0, BUT4TL_DIM , banner_width, banner_height-BUT4TL_DIM);
	impostaAttivo(2);
	Draw();
}

