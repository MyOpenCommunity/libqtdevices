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

	BannToggle *off = getBanner(tr("Off"));
	const int off_id = 0;
	buttons.addButton(off->getButton(), off_id);

	BannToggle *low = getBanner(tr("Low brightness"));
	const int low_id = 1;
	buttons.addButton(low->getButton(), low_id);

	BannToggle *high = getBanner(tr("High brightness"));
	const int high_id = 2;
	buttons.addButton(high->getButton(), high_id);

	buttons.setExclusive(true);
	connect (this, SIGNAL(goDx()), SLOT(brightnessSelected()));
}

BannToggle *BrightnessPage::getBanner(const QString &banner_text)
{
	BannToggle *bann = new BannToggle(this);
	bann->SetTextU(banner_text);
	elencoBanner.append(bann);
	return bann;
}

void BrightnessPage::brightnessSelected()
{
	BrightnessControl::instance()->setBrightnessPolicy(
			static_cast<BrightnessControl::DefautPolicy>(buttons.checkedId()));
	// this will close the window
	emit Closed();
}

BannToggle::BannToggle(sottoMenu *parent, const char *name) : bannOnSx(parent, name)
{
	SetIcons(ICON_VUOTO, 1);
	sxButton->setCheckable(true);
}

BtButton *BannToggle::getButton()
{
	return sxButton;
}
