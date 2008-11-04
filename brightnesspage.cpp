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
#include "main.h" // for ICON_{OK,VUOTO}
#include "btbutton.h"

BrightnessPage::BrightnessPage(QWidget *parent) : sottoMenu(parent)
{
	setNavBarMode(10, ICON_OK);
	QColor *bg, *fg, *fg2;
	readExtraConf(&bg, &fg, &fg2);
	setBGColor(*bg);
	setFGColor(*fg);

	BannToggle *off = getBanner(tr("Off"));
	buttons.insert(off->getButton(), 0);

	BannToggle *low = getBanner(tr("Low brightness"));
	buttons.insert(low->getButton(), 1);

	BannToggle *high = getBanner(tr("High brightness"));
	buttons.insert(high->getButton(), 2);

	buttons.setExclusive(true);
	connect (this, SIGNAL(goDx()), SLOT(brightnessSelected()));
}

BannToggle *BrightnessPage::getBanner(const QString &banner_text)
{
	BannToggle *bann = new BannToggle(this);
	bann->SetTextU(banner_text);
	elencoBanner.append(bann);
	bann->setFGColor(paletteForegroundColor());
	bann->setBGColor(paletteBackgroundColor());
	return bann;
}

void BrightnessPage::brightnessSelected()
{
	// TODO: set the new brightness policy
	// this will close the window
	emit Closed();
}

BannToggle::BannToggle(sottoMenu *parent, const char *name) : bannOnSx(parent, name)
{
	SetIcons(ICON_VUOTO, 1);
	sxButton->setToggleButton(true);
}

BtButton *BannToggle::getButton()
{
	return sxButton;
}
