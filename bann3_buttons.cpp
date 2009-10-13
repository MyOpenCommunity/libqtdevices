#include "bann3_buttons.h"

#define BAN3BUT_BUT_DIM 60
#define BAN3BUT_BUTCEN_DIM 120
#define BUT3BL_ICON_DIM_X 120
#define BUT3BL_ICON_DIM_Y 60
#define BAN3BL_BUT_DIM 60


bann3But::bann3But(QWidget *parent) : banner(parent)
{
	addItem(BUT1, 0, 0, BAN3BUT_BUT_DIM, BAN3BUT_BUT_DIM);
	addItem(BUT2, banner_width - BAN3BUT_BUT_DIM, 0 , BAN3BUT_BUT_DIM , BAN3BUT_BUT_DIM);
	addItem(TEXT, 0, BAN3BUT_BUT_DIM, banner_width, banner_height - BAN3BUT_BUT_DIM);
	addItem(BUT3, banner_width/2 - BAN3BUT_BUTCEN_DIM/2, 0, BAN3BUT_BUTCEN_DIM, BAN3BUT_BUT_DIM);
	connect(this,SIGNAL(csxClick()),this,SIGNAL(centerClick()));
}


bann3ButLab::bann3ButLab(QWidget *parent) : banner(parent)
{
	addItem(BUT1, banner_width - BAN3BL_BUT_DIM, 0, BAN3BL_BUT_DIM, BAN3BL_BUT_DIM);
	addItem(BUT2, 0, 0, BAN3BL_BUT_DIM, BAN3BL_BUT_DIM);
	addItem(BUT4, 0, 0, BAN3BL_BUT_DIM, BAN3BL_BUT_DIM);
	addItem(TEXT, 0, BAN3BL_BUT_DIM, banner_width, banner_height - BAN3BL_BUT_DIM);
	addItem(ICON, BAN3BL_BUT_DIM, 0, BUT3BL_ICON_DIM_X, BUT3BL_ICON_DIM_Y);
}

