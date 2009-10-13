/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannRegolaz.cpp
**
**Riga con tasto ON OFF, icona centrale divisa in due per regolazione
**
****************************************************************/

#include "bannregolaz.h"
#include "main.h" // TIME_RIP_REGOLAZ

#include <QTimer>


bannRegolaz::bannRegolaz(QWidget *parent) : banner(parent)
{
	addItem(BUT2, 0, 0, BANREGOL_BUT_DIM , BANREGOL_BUT_DIM);
	addItem(BUT1, banner_width - BANREGOL_BUT_DIM, 0, BANREGOL_BUT_DIM , BANREGOL_BUT_DIM);
	addItem(BUT3, (banner_width - BUTREGOL_ICON_DIM_X)/2, 0, BUTREGOL_ICON_DIM_X/2, BUTREGOL_ICON_DIM_Y);
	addItem(BUT4, banner_width/2, 0, BUTREGOL_ICON_DIM_X/2, BUTREGOL_ICON_DIM_Y);
	addItem(TEXT, 0, BANREGOL_BUT_DIM, banner_width, banner_height - BANREGOL_BUT_DIM);

	timRip = NULL;

	connect(this,SIGNAL(cdxPressed()),this,SLOT(armTimRipdx()));
	connect(this,SIGNAL(cdxReleased()),this,SLOT(killTimRipdx()));
	connect(this,SIGNAL(csxPressed()),this,SLOT(armTimRipsx()));
	connect(this,SIGNAL(csxReleased()),this,SLOT(killTimRipsx()));
}

void bannRegolaz::armTimRipdx()
{
	if  (!timRip)
		timRip = new QTimer(this);
	if (!(timRip->isActive()))
	{
		timRip->start(TIME_RIP_REGOLAZ);
		disconnect(timRip,SIGNAL(timeout()),this,SIGNAL(csxClick()));
		connect(timRip,SIGNAL(timeout()),this,SIGNAL(cdxClick()));
	}
}

void bannRegolaz::armTimRipsx()
{
	if (!timRip)
		timRip = new QTimer(this);

	if (!(timRip->isActive()))
	{
		timRip->start(TIME_RIP_REGOLAZ);
		disconnect(timRip,SIGNAL(timeout()),this,SIGNAL(cdxClick()));
		connect(timRip,SIGNAL(timeout()),this,SIGNAL(csxClick()));
	}
}

void bannRegolaz::killTimRipdx()
{
	if (timRip)
		timRip->stop();
}

void bannRegolaz::killTimRipsx()
{
	if (timRip)
		timRip->stop();
}
