#include "bann1_button.h"
#include "sottomenu.h"
#include "main.h" // MAX_WIDTH, MAX_HEIGHT, ..


#define BUTONDX_BUT_DIM_X 60
#define BUTONDX_BUT_DIM_Y 60
#define BUTONSX_BUT_DIM_X 60
#define BUTONSX_BUT_DIM_Y 60
#define BUTONDX_H_SCRITTA 20
#define BANNONICONS_ICON_DIM 60
#define BANNONICONS_BUT_DIM 60
#define BANON2SCR_TEXT1_DIM_X 100
#define BANON2SCR_TEXT1_DIM_Y 60
#define BUTON2SCR_ICON_DIM_X 40
#define BUTON2SCR_ICON_DIM_Y 60
#define BANON2SCR_BUT_DIM 60
#define BANNBUT2ICON_TEXT_DIM_X 240
#define BANNBUT2ICON_TEXT_DIM_Y 20
#define BANNBUT2ICON_ICON_DIM_X 60
#define BANNBUT2ICON_ICON_DIM_Y 60
#define BANNBUT2ICON_BUT_DIM 60


bannPuls::bannPuls(QWidget *parent) : banner(parent)
{
	addItem(BUT1, MAX_WIDTH-BANPULS_BUT_DIM, 0,  BANPULS_BUT_DIM ,BANPULS_BUT_DIM);
	addItem(ICON,BANPULS_BUT_DIM, 0,  BANPULS_ICON_DIM_X ,BANPULS_ICON_DIM_Y);
	addItem(TEXT, 0 , BANPULS_BUT_DIM , MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE-BANPULS_BUT_DIM);

	connect(this,SIGNAL(sxClick()),this,SIGNAL(click()));
	connect(this,SIGNAL(sxPressed()),this,SIGNAL(pressed()));
	connect(this,SIGNAL(sxReleased()),this,SIGNAL(released()));
}


bannOnDx::bannOnDx(sottoMenu *parent, QString icon, Page *page) : banner(parent)
{
	unsigned char nr = parent->getNumRig();

	addItem(BUT1, MAX_WIDTH-BUTONDX_BUT_DIM_X, (((MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE)/nr)-BUTONDX_BUT_DIM_Y)/2 ,
		BUTONDX_BUT_DIM_X ,BUTONDX_BUT_DIM_Y);
	addItem(TEXT , 0, 0 , MAX_WIDTH-BUTONDX_BUT_DIM_X,
		((MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE)/nr));
	connect(this, SIGNAL(sxClick()), this, SIGNAL(click()));

	if (!icon.isEmpty())
		SetIcons(icon, 1);

	connectDxButton(page);
	Draw();
}


bannOnSx::bannOnSx(sottoMenu *parent, QString icon) : banner(parent)
{
	unsigned char nr = parent->getNumRig();
	addItem(BUT1, 0, (((MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE)/nr) - BUTONSX_BUT_DIM_Y)/2, BUTONSX_BUT_DIM_X,
		BUTONSX_BUT_DIM_Y);
	addItem(TEXT ,BUTONSX_BUT_DIM_X, 0, MAX_WIDTH - BUTONSX_BUT_DIM_X, ((MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE)/nr));
	connect(this, SIGNAL(sxClick()), this, SIGNAL(click()));

	if (!icon.isEmpty())
		SetIcons(icon, 1);
	Draw();
}

BtButton *bannOnSx::getButton()
{
	return sxButton;
}


bannOnIcons::bannOnIcons(QWidget *parent) : banner(parent)
{
	addItem(BUT1 , 0 , 0 , BANNONICONS_BUT_DIM, BANNONICONS_BUT_DIM);
	addItem(TEXT , 0 , BANNONICONS_BUT_DIM , MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE-BANNONICONS_BUT_DIM);
	addItem(ICON , BANNONICONS_BUT_DIM , 0 , BANNONICONS_BUT_DIM , BANNONICONS_BUT_DIM);
	addItem(ICON2 , 2*BANNONICONS_BUT_DIM , 0 , BANNONICONS_BUT_DIM , BANNONICONS_BUT_DIM);
}


bannOn2scr::bannOn2scr(QWidget *parent) : banner(parent)
{
	int x = 0, y = 0;
	addItem(TEXT2, x, y, BANON2SCR_TEXT1_DIM_X, BANON2SCR_TEXT1_DIM_Y);
	x = BANON2SCR_TEXT1_DIM_X;
	addItem(ICON, x, y, BUTON2SCR_ICON_DIM_X, BUTON2SCR_ICON_DIM_Y);
	x += BUTON2SCR_ICON_DIM_X;
	addItem(ICON2, x, y, BUTON2SCR_ICON_DIM_X, BUTON2SCR_ICON_DIM_Y);
	x += BUTON2SCR_ICON_DIM_X;
	addItem(BUT2, x, y, BANON2SCR_BUT_DIM, BANON2SCR_BUT_DIM);
	y = BANON2SCR_BUT_DIM;
	x = 0;
	addItem(TEXT, x, y, MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE - BANON2SCR_BUT_DIM);
}


bannBut2Icon::bannBut2Icon(QWidget *parent) : banner(parent)
{
	addItem(BUT1 , MAX_WIDTH - BANNBUT2ICON_BUT_DIM, 0, BANNBUT2ICON_BUT_DIM, BANNBUT2ICON_BUT_DIM);
	addItem(ICON, BANNBUT2ICON_BUT_DIM, 0, BANNBUT2ICON_ICON_DIM_X, BANNBUT2ICON_ICON_DIM_Y);
	addItem(ICON2, BANNBUT2ICON_BUT_DIM + BANNBUT2ICON_ICON_DIM_X , 0, BANNBUT2ICON_ICON_DIM_X, BANNBUT2ICON_ICON_DIM_Y);
	addItem(TEXT, 0, BANNBUT2ICON_BUT_DIM, MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE - BANNBUT2ICON_BUT_DIM);
}

