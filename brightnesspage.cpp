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
#include "brightnesscontrol.h"
#include "main.h" // for ICON_{OK,VUOTO}
#include "bannondx.h"
#include "btbutton.h"

#include <QDebug>


BrightnessPage::BrightnessPage(QWidget *parent) : sottoMenu(parent)
{
	setNavBarMode(10, ICON_OK);
	buttons.setExclusive(true);

	addBanner(tr("Off"), BRIGHTNESS_OFF);
	addBanner(tr("Low brightness"), BRIGHTNESS_LOW);
	addBanner(tr("Normal brightness"), BRIGHTNESS_NORMAL);

	connect(this, SIGNAL(goDx()), SLOT(brightnessSelected()));
}

void BrightnessPage::addBanner(const QString &text, int id)
{
	bannOnSx *bann = new bannOnSx(this, ICON_VUOTO);
	BtButton *btn = bann->getButton();
	btn->setCheckable(true);
	bann->setText(text);
	buttons.addButton(btn, id);
	elencoBanner.append(bann);

	if (id == BrightnessControl::instance()->currentLevel())
		btn->setChecked(true);
}

void BrightnessPage::brightnessSelected()
{
	BrightnessControl::instance()->setLevel(static_cast<BrightnessLevel>(buttons.checkedId()));
	emit Closed();
}

void BrightnessPage::showEvent(QShowEvent *event)
{
	sottoMenu::showEvent(event);
	int level = BrightnessControl::instance()->currentLevel();
	QAbstractButton* btn;
	foreach (btn, buttons.buttons())
		if (buttons.id(btn) == level)
			btn->setChecked(true);
}
