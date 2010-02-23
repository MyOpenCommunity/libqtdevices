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


#include "cleanscreen.h"
#include "xml_functions.h" // getChildWithId
#include "fontmanager.h" // bt_global::font
#include "icondispatcher.h" // bt_global::icons_cache
#include "displaycontrol.h" // bt_global::display
#include "main.h" // IMG_PATH
#include "btmain.h" //bt_global::btmain

#include <QLabel>

#include <assert.h>


CleanScreen::CleanScreen(QString img_clean, int clean_time)
{
	connect(&secs_timer, SIGNAL(timeout()), SLOT(updateRemainingTime()));

	connect(&timer, SIGNAL(timeout()), SLOT(handleClose()));
	timer.setSingleShot(true);

	time_label = new QLabel(this);
	time_label->setFont(bt_global::font->get(FontManager::TEXT));
	time_label->setAlignment(Qt::AlignHCenter);
	time_label->setGeometry(TIME_LABEL_X, TIME_LABEL_Y, TIME_LABEL_WIDTH, TIME_LABEL_HEIGHT);

	icon_label = new QLabel(this);
	icon_label->setPixmap(*bt_global::icons_cache.getIcon(img_clean));
	icon_label->setAlignment(Qt::AlignHCenter);
	icon_label->setGeometry(ICON_LABEL_X, ICON_LABEL_Y, ICON_LABEL_WIDTH, ICON_LABEL_HEIGHT);

	wait_time_sec = clean_time;
}

void CleanScreen::updateRemainingTime()
{
	++secs_counter;
	update();
}

void CleanScreen::showPage()
{
	if (!secs_timer.isActive())
	{
		qDebug("reset timer cleanscreen");
		secs_timer.start(1 * 1000);
		timer.start(wait_time_sec * 1000);
		secs_counter = 0;
		bt_global::display.forceOperativeMode(true);
	}
	Page::showPage();
}

void CleanScreen::handleClose()
{
	bt_global::display.forceOperativeMode(false);
	secs_timer.stop();
	emit Closed();
}

void CleanScreen::paintEvent(QPaintEvent *e)
{
	// we use QTime only to format the output
	QTime remaining_time;
	remaining_time = remaining_time.addMSecs((wait_time_sec - secs_counter) * 1000);
	if (remaining_time.minute())
		time_label->setText(remaining_time.toString("mm:ss"));
	else
		time_label->setText(remaining_time.toString("ss"));
}
