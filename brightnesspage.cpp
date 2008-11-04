/*!
 * \brightnesspage.cpp
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "brightnesspage.h"
#include "main.h" // for ICON_OK

BrightnessPage::BrightnessPage(QWidget *parent) : sottoMenu(parent)
{
	setNavBarMode(10, ICON_OK);
	QColor *bg, *fg, *fg2;
	readExtraConf(&bg, &fg, &fg2);
	setBGColor(*bg);
	setFGColor(*fg);
}

