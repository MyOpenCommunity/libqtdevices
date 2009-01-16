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
#include "brightnesscontrol.h" // bt_global::brightness


BrightnessPage::BrightnessPage()
{
	addBanner(tr("Off"), BRIGHTNESS_OFF);
	addBanner(tr("Low brightness"), BRIGHTNESS_LOW);
	addBanner(tr("Normal brightness"), BRIGHTNESS_NORMAL);
}

int BrightnessPage::getCurrentId()
{
	return bt_global::brightness.currentLevel();
}

void BrightnessPage::bannerSelected(int id)
{
	bt_global::brightness.setLevel(static_cast<BrightnessLevel>(id));
}

