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

#include <QDebug>


DisplayPage::DisplayPage(const QDomNode &config_node)
{
	setNumRighe(4);
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

	b = new bannSimple(this, img_clean, new CleanScreen(img_clean, wait_time));
	connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	b->setText(tr("Clean Screen"));
	appendBanner(b);

	b = new calibration(this, img_items);
	connect(b, SIGNAL(startCalib()), this, SIGNAL(startCalib()));
	connect(b, SIGNAL(endCalib()), this, SIGNAL(endCalib()));
	b->setText(tr("Calibration"));
	appendBanner(b);

	b = new bannOnDx(this,img_items, new BrightnessPage());
	connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	b->setText(tr("Brightness"));
	appendBanner(b);

	b = new bannOnDx(this,img_items, new ScreenSaverPage());
	connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	b->setText(tr("Screen Saver"));
	appendBanner(b);
}

