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
#include "displaycontrol.h" // bt_global::display
#include "bann1_button.h" // bannOnSx
#include "content_widget.h"
#include "banner.h"
#include "btbutton.h"

#include <QDebug>


BrightnessPage::BrightnessPage()
{
	addBanner(tr("Off"), BRIGHTNESS_OFF);
	addBanner(tr("Low brightness"), BRIGHTNESS_LOW);
	addBanner(tr("Normal brightness"), BRIGHTNESS_NORMAL);
	addBanner(tr("High brightness"), BRIGHTNESS_HIGH);
}

int BrightnessPage::getCurrentId()
{
	return bt_global::display.currentBrightness();
}

void BrightnessPage::bannerSelected(int id)
{
	bt_global::display.setBrightness(static_cast<BrightnessLevel>(id));
}

void BrightnessPage::showEvent(QShowEvent *e)
{
	bool banners_active = (bt_global::display.currentBrightness() != BRIGHTNESS_OFF ||
		bt_global::display.currentScreenSaver() != ScreenSaver::NONE);

	for (int i = 0; i < content_widget->bannerCount(); ++i)
	{
		BtButton *b = static_cast<bannOnSx*>(content_widget->getBanner(i))->getButton();
		if (buttons.id(b) == getCurrentId())
			b->setChecked(true);

		if (banners_active)
			b->enable();
		else
			b->disable();
	}
	SingleChoicePage::showEvent(e);
}
