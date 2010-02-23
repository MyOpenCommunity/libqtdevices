/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "brightnesspage.h"
#include "displaycontrol.h" // bt_global::display
#include "bann1_button.h" // bannOnSx
#include "banner.h"
#include "btbutton.h"

#include <QDebug>


BrightnessPage::BrightnessPage() : SingleChoicePage(true)
{
	setNumRighe(4);
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

	for (int i = 0; i < elencoBanner.size(); ++i)
	{
		BtButton *b = static_cast<bannOnSx*>(elencoBanner.at(i))->getButton();
		if (buttons.id(b) == getCurrentId())
			b->setChecked(true);

		if (banners_active)
			b->enable();
		else
			b->disable();
	}
	SingleChoicePage::showEvent(e);
}
