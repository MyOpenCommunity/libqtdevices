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
#include "banner.h"
#include "btbutton.h"
#include "xml_functions.h" // getTextChild
#include "generic_functions.h" // setCfgValue

#include <QDebug>


InactiveBrightnessPage::InactiveBrightnessPage(const QDomNode &config_node)
{
	addBanner(SingleChoice::createBanner(tr("Off")), BRIGHTNESS_OFF);
	addBanner(SingleChoice::createBanner(tr("Low brightness")), BRIGHTNESS_LOW);
	addBanner(SingleChoice::createBanner(tr("Normal brightness")), BRIGHTNESS_NORMAL);
	addBanner(SingleChoice::createBanner(tr("High brightness")), BRIGHTNESS_HIGH);

	// to save parameters
	item_id = getTextChild(config_node, "itemID").toInt();

	// this load the current inactive brightness into the global display object
	bannerSelected(getTextChild(config_node, "level").toInt());

	// Because the transition effects take a 'photo' of the page before showing it
	// we have to select the banner that represents the current brightness _before_
	// the showEvent/showPage. So, we do that after the Closed signal (and we use
	// a queued connection because the transition effects take a photo also when
	// we close the page).
	// But because the current inactive brightness can be changed also by the
	// screensaver page we have to check the status of the page even in the showEvent
	// method.
	connect(this, SIGNAL(Closed()), SLOT(updateStatus()), Qt::QueuedConnection);
	updateStatus(true);
}

int InactiveBrightnessPage::getCurrentId()
{
	return bt_global::display->inactiveBrightness();
}

void InactiveBrightnessPage::bannerSelected(int id)
{
	bt_global::display->setInactiveBrightness(static_cast<BrightnessLevel>(id));
	QMap<QString, QString> data;
	data["level"] = QString::number(id);
	setCfgValue(data, item_id);
}

void InactiveBrightnessPage::updateStatus(bool force)
{
	if (force  == false && last_brightness == bt_global::display->inactiveBrightness())
		return;

	bool banners_active = (bt_global::display->inactiveBrightness() != BRIGHTNESS_OFF ||
		bt_global::display->currentScreenSaver() != ScreenSaver::NONE);

	setCheckedId(getCurrentId());
	foreach (QAbstractButton *bt, page_content->getButtons())
	{
		BtButton *b = static_cast<BtButton*>(bt);

		if (banners_active)
			b->enable();
		else
			b->disable();
	}
}

void InactiveBrightnessPage::showEvent(QShowEvent *e)
{
	updateStatus();
	SingleChoicePage::showEvent(e);
}
