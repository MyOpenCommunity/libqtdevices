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
#include "bann_settings.h"
#include "cleanscreen.h"
#include "brightnesspage.h"
#include "xml_functions.h" // getElement
#include "screensaverpage.h"
#include "skinmanager.h" // SkinContext, bt_global::skin
#include "calibration.h"
#include "navigation_bar.h"
#include "screensaver.h" // ScreenSaver::initData
#include "transitioneffects.h"

#include <QVBoxLayout>
#include <QWidget>

enum
{
	INACTIVE_BRIGHTNESS = 14155,
	CLEANSCREEN = 14152,
	CALIBRATION = 14153,
	SCREENSAVER = 14154,
	TRANSITIONEFFECTS = 14156
};

BannDisplay::BannDisplay(QString icon, QString descr)
{
	maximize_text = true;
	initBanner(QString(), icon, descr);
}


DisplayPage::DisplayPage(const QDomNode &config_node)
{
	QWidget *main_widget = new QWidget;
	main_layout = new QVBoxLayout(main_widget);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(10);

	BannerContent *content = new BannerContent;
	main_layout->addWidget(content, 1);
	buildPage(main_widget, content, new NavigationBar);
	setSpacing(10);
	loadItems(config_node);
}

void DisplayPage::loadItems(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	QString img_items = bt_global::skin->getImage("display_items");

	QDomNode page_node = getPageNodeFromChildNode(config_node, "lnk_pageID");
	foreach (const QDomNode &item, getChildren(page_node, "item"))
	{
		int link_id = getTextChild(item, "id").toInt();
		if (link_id == CLEANSCREEN)
		{
			int wait_time = getTextChild(item, "countdown").toInt() / 1000;
			loadCleanScreen(wait_time);
		}
#ifndef BT_HARDWARE_X11
		else if (link_id == CALIBRATION)
			loadCalibration(img_items);
		else if (link_id == INACTIVE_BRIGHTNESS)
			loadInactiveBrightness(img_items, item);
#endif
		else if (link_id == SCREENSAVER)
			loadScreenSaver(img_items, item);
		else if (link_id == TRANSITIONEFFECTS)
			loadTransitionEffects(img_items, item);
	}
}

void DisplayPage::loadCleanScreen(int wait_time)
{
	QString img_clean = bt_global::skin->getImage("cleanscreen");
	Window *w = new CleanScreen(img_clean, wait_time);

	BannSimple *simple = new BannSimple(img_clean);
	connect(simple, SIGNAL(clicked()), w, SLOT(showWindow()));
	page_content->appendBanner(simple);
}

#ifndef BT_HARDWARE_X11
void DisplayPage::loadCalibration(QString icon)
{
	Calibration *calibration_window = new Calibration;
	Bann2Buttons *b = new BannDisplay(icon, tr("Calibration"));
	connect(b, SIGNAL(rightClicked()), calibration_window, SLOT(showWindow()));
	connect(calibration_window, SIGNAL(Closed()), this, SLOT(showPage()));
	page_content->appendBanner(b);
}

void DisplayPage::loadInactiveBrightness(QString icon, const QDomNode &config_node)
{
	Bann2Buttons *b = new BannDisplay(icon, tr("Brightness"));
	b->connectRightButton(new InactiveBrightnessPage(config_node));
	connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	page_content->appendBanner(b);
}
#endif

void DisplayPage::loadScreenSaver(QString icon, const QDomNode &config_node)
{
	int timeout = getTextChild(config_node, "timeout").toInt() / 1000;
	int blank = getTextChild(config_node, "blankscreen").toInt() / 1000;
	bt_global::display->setScreenSaverTimeouts(timeout, blank == 0 ? 0 : timeout + blank);

	ScreenSaver::initData(config_node);

	Bann2Buttons *b = new BannDisplay(icon, tr("Screen Saver"));
	b->connectRightButton(new ScreenSaverPage(config_node));
	connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	page_content->appendBanner(b);
}

void DisplayPage::loadTransitionEffects(QString icon, const QDomNode &config_node)
{
	Bann2Buttons *b = new BannDisplay(icon, tr("Transition effects"));
	b->connectRightButton(new TransitionEffects(config_node));
	connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	page_content->appendBanner(b);
}
