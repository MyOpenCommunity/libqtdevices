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
#include "bannercontent.h"
#include "banner.h"
#include "btbutton.h"
#include "singlechoicecontent.h"
#include <QDebug>


BrightnessPage::BrightnessPage()
{
	addBanner(SingleChoice::createBanner(tr("Off")), BRIGHTNESS_OFF);
	addBanner(SingleChoice::createBanner(tr("Low brightness")), BRIGHTNESS_LOW);
	addBanner(SingleChoice::createBanner(tr("Normal brightness")), BRIGHTNESS_NORMAL);
	addBanner(SingleChoice::createBanner(tr("High brightness")), BRIGHTNESS_HIGH);
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

	setCheckedId(getCurrentId());
	foreach (QAbstractButton *bt, page_content->getButtons())
	{
		BtButton *b = static_cast<BtButton*>(bt);

		if (banners_active)
			b->enable();
		else
			b->disable();
	}
	SingleChoicePage::showEvent(e);
}
