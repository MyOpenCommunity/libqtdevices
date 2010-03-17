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
#include "bann1_button.h" // bannOnDx
#include "bann_settings.h"
#include "cleanscreen.h"
#include "brightnesspage.h"
#include "xml_functions.h" // getElement
#include "screensaverpage.h"
#include "skinmanager.h" // SkinContext, bt_global::skin
#include "bannercontent.h"


DisplayPage::DisplayPage(const QDomNode &config_node)
{
	buildPage();
	loadItems(config_node);
}

void DisplayPage::loadItems(const QDomNode &config_node)
{
	banner *b;

	SkinContext context(getTextChild(config_node, "cid").toInt());
	QString img_items = bt_global::skin->getImage("display_items");
	QString img_clean = bt_global::skin->getImage("cleanscreen");

	int wait_time = 45; // Default waiting time in seconds.
	QDomElement n = getElement(config_node, "cleaning/time");
	if (!n.isNull())
		wait_time = n.text().toInt();

	Window *w = new CleanScreen(img_clean, wait_time);

	b = new BannSimple(img_clean);
	connect(b, SIGNAL(clicked()), w, SLOT(showWindow()));
	page_content->appendBanner(b);
#ifndef BT_HARDWARE_X11
	b = new calibration(this, img_items);
	b->setText(tr("Calibration"));
	b->Draw();
	page_content->appendBanner(b);

	b = new bannOnDx(this,img_items, new BrightnessPage());
	connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	b->setText(tr("Brightness"));
	b->Draw();
	page_content->appendBanner(b);
#endif
	bannOnDx *bann = new bannOnDx(this, img_items, new ScreenSaverPage());
	connect(bann, SIGNAL(pageClosed()), SLOT(showPage()));
	bann->setText(tr("Screen Saver"));
	bann->Draw();
	page_content->appendBanner(bann);
}
