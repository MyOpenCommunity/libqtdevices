#include "bann2_buttons.h"
#include "btbutton.h"

#include <QWidget>
#include <QLabel>


#define BAN2BUT_BUT_DIM 60
#define BUTONOFF_ICON_DIM_X 120
#define BUTONOFF_ICON_DIM_Y 60
#define BANONOFF_BUT_DIM 60
#define BUTONOFF2SCR_ICON_DIM_X 80
#define BUTONOFF2SCR_ICON_DIM_Y 60
#define BANONOFF2SCR_BUT_DIM 60


bann2But::bann2But(QWidget *parent) : banner(parent)
{
	addItem(BUT1, 0,(banner_height - BAN2BUT_BUT_DIM)/2 , BAN2BUT_BUT_DIM , BAN2BUT_BUT_DIM);
	addItem(BUT2, banner_width - BAN2BUT_BUT_DIM ,(banner_height - BAN2BUT_BUT_DIM)/2 , BAN2BUT_BUT_DIM , BAN2BUT_BUT_DIM);
	addItem(TEXT, BAN2BUT_BUT_DIM, 0, banner_width - 2 * BAN2BUT_BUT_DIM, banner_height);
}


bann2ButLab::bann2ButLab(QWidget *parent) : banner(parent)
{
	addItem(BUT1, 0, 0, BAN2BUT_BUT_DIM , BAN2BUT_BUT_DIM);
	addItem(BUT2, banner_width - BAN2BUT_BUT_DIM, 0, BAN2BUT_BUT_DIM, BAN2BUT_BUT_DIM);
	addItem(TEXT, BAN2BUT_BUT_DIM , 0,banner_width - 2 * BAN2BUT_BUT_DIM , BAN2BUT_BUT_DIM);
	addItem(TEXT2, 0, BAN2BUT_BUT_DIM-5, banner_width , 25);
}

void bann2ButLab::setAutoRepeat()
{
	sxButton->setAutoRepeat(true);
	dxButton->setAutoRepeat(true);
}


QSize bann2ButLab::sizeHint() const
{
	return banner::sizeHint() + QSize(0, 20);
}



bannOnOff::bannOnOff(QWidget *parent) : banner(parent)
{
	addItem(BUT1, banner_width-BANONOFF_BUT_DIM , 0 , BANONOFF_BUT_DIM , BANONOFF_BUT_DIM);
	addItem(BUT2, 0, 0, BANONOFF_BUT_DIM , BANONOFF_BUT_DIM);
	addItem(TEXT, 0, BANONOFF_BUT_DIM, banner_width , banner_height-BANONOFF_BUT_DIM);
	addItem(ICON, BANONOFF_BUT_DIM, 0, BUTONOFF_ICON_DIM_X , BUTONOFF_ICON_DIM_Y);
}


bannOnOff2scr::bannOnOff2scr(QWidget *parent) : banner(parent)
{
	addItem(BUT1, 0, 0, BANONOFF2SCR_BUT_DIM , BANONOFF2SCR_BUT_DIM);
	addItem(BUT2, banner_width-BANONOFF2SCR_BUT_DIM, 0, BANONOFF2SCR_BUT_DIM, BANONOFF2SCR_BUT_DIM);
	addItem(TEXT, 0, BANONOFF2SCR_BUT_DIM, banner_width, banner_height-BANONOFF2SCR_BUT_DIM);
	addItem(ICON, banner_width-BANONOFF2SCR_BUT_DIM-BUTONOFF2SCR_ICON_DIM_X, 0, BUTONOFF2SCR_ICON_DIM_X, BUTONOFF2SCR_ICON_DIM_Y);
	addItem(TEXT2, BANONOFF2SCR_BUT_DIM, 0, banner_width-2*BANONOFF2SCR_BUT_DIM-BUTONOFF2SCR_ICON_DIM_X, BUTONOFF2SCR_ICON_DIM_Y);
}

