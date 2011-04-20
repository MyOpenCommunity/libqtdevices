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


#include "displaypage.h"
#include "bann1_button.h" // BannSimple
#include "bann2_buttons.h" // Bann2Buttons
#include "bann_settings.h"
#include "cleanscreen.h"
#include "brightnesspage.h"
#include "xml_functions.h" // getElement
#include "screensaverpage.h"
#include "skinmanager.h" // SkinContext, bt_global::skin
#include "calibration.h"

DisplayPage::DisplayPage(const QDomNode &config_node)
{
	buildPage();
	loadItems(config_node);
}

void DisplayPage::loadItems(const QDomNode &config_node)
{
	Bann2Buttons *b;

	SkinContext context(getTextChild(config_node, "cid").toInt());
	QString img_items = bt_global::skin->getImage("display_items");
	QString img_clean = bt_global::skin->getImage("cleanscreen");

	int wait_time = 45; // Default waiting time in seconds.
	QDomElement n = getElement(config_node, "cleaning/time");
	if (!n.isNull())
		wait_time = n.text().toInt();

	Window *w = new CleanScreen(img_clean, wait_time);

	BannSimple *simple = new BannSimple(img_clean);
	connect(simple, SIGNAL(clicked()), w, SLOT(showWindow()));
	page_content->appendBanner(simple);

#ifndef BT_HARDWARE_X11
	Calibration *calibration_window = new Calibration;
	b = new Bann2Buttons;
	b->initBanner(QString(), img_items, tr("Calibration"));
	connect(b, SIGNAL(rightClicked()), calibration_window, SLOT(showWindow()));
	connect(calibration_window, SIGNAL(Closed()), this, SLOT(showPage()));
	page_content->appendBanner(b);

	b = new Bann2Buttons;
	b->initBanner(QString(), img_items, tr("Brightness"));
	b->connectRightButton(new BrightnessPage);
	page_content->appendBanner(b);
#endif

	b = new Bann2Buttons;
	b->initBanner(QString(), img_items, tr("Screen Saver"));
	b->connectRightButton(new ScreenSaverPage(getChildWithName(config_node, "screensaver")));

	page_content->appendBanner(b);
}
