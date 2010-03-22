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


#include "screensaverpage.h"
#include "screensaver.h"
#include "displaycontrol.h" // (*bt_global::display)

#include <QAbstractButton>

ScreenSaverPage::ScreenSaverPage() : SingleChoicePage(true)
{
	setNumRighe(4);
	setScrollStep(4);
	addBanner(tr("No screensaver"), ScreenSaver::NONE);
	addBanner(tr("Line"), ScreenSaver::LINES);
	addBanner(tr("Balls"), ScreenSaver::BALLS);
	addBanner(tr("Time"), ScreenSaver::TIME);
	addBanner(tr("Text"), ScreenSaver::TEXT);
	//addBanner(tr("Deform"), ScreenSaver::DEFORM); // the deform is for now unavailable!
}

void ScreenSaverPage::showPage()
{
	buttons.button(getCurrentId())->setChecked(true);
	SingleChoicePage::showPage();
}

int ScreenSaverPage::getCurrentId()
{
	return (*bt_global::display).currentScreenSaver();
}

void ScreenSaverPage::bannerSelected(int id)
{
	(*bt_global::display).setScreenSaver(static_cast<ScreenSaver::Type>(id));
	if (id == ScreenSaver::NONE)
		(*bt_global::display).setBrightness(BRIGHTNESS_OFF);
}
