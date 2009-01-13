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
#include "bannondx.h"
#include "btbutton.h"


BrightnessPage::BrightnessPage(QWidget *parent) : sottoMenu(parent)
{
	setNavBarMode(10, ICON_OK);

	const int off_id = 0;
	const int low_id = 1;
	const int high_id = 2;

	addBanner(tr("Off"), off_id);
	addBanner(tr("Low brightness"), low_id);
	addBanner(tr("High brightness"), high_id);

	buttons.setExclusive(true);
	connect(this, SIGNAL(goDx()), SLOT(brightnessSelected()));
}

void BrightnessPage::addBanner(const QString &text, int id)
{
	bannOnSx *bann = new bannOnSx(this, ICON_VUOTO);
	bann->getButton()->setCheckable(true);
	bann->setText(text);
	buttons.addButton(bann->getButton(), id);
	elencoBanner.append(bann);
}

void BrightnessPage::brightnessSelected()
{
	BrightnessControl::instance()->setBrightnessPolicy(
			static_cast<BrightnessControl::DefautPolicy>(buttons.checkedId()));
	// this will close the window
	emit Closed();
}

