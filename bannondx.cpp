/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannOnDx.cpp
**
**Riga con tasto ON a Dx e scritta descrittiva da parte
**
****************************************************************/

#include "bannondx.h"
#include "sottomenu.h"
#include "page.h"
#include "main.h"


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

	linked_page = page;
	if (linked_page)
	{
		linked_page->hide();
		connect(this, SIGNAL(sxClick()), linked_page, SLOT(showFullScreen()));
		connect(linked_page, SIGNAL(Closed()), linked_page, SLOT(hide()));
	}
}

void bannOnDx::hideEvent(QHideEvent *event)
{
	if (linked_page)
		linked_page->hide();
}

bannOnDx::~bannOnDx()
{
	delete linked_page;
}


bannOnSx::bannOnSx(sottoMenu *parent) : banner(parent)
{
	unsigned char nr=parent->getNumRig();
	addItem(BUT1, 0,  (((MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE)/nr)-BUTONSX_BUT_DIM_Y)/2 ,  BUTONSX_BUT_DIM_X ,
		BUTONSX_BUT_DIM_Y);
	addItem(TEXT ,BUTONSX_BUT_DIM_X, 0 , MAX_WIDTH-BUTONSX_BUT_DIM_X, ((MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE)/nr));
	connect(this,SIGNAL(sxClick()),this,SIGNAL(click()));
}
