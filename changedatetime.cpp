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


#include "changedatetime.h"
#include "datetime.h"
#include "btbutton.h"
#include "skinmanager.h"
#include "main.h" // getConfElement
#include "platform_device.h"
#include "devices_cache.h"

#include <QLabel>
#include <QVBoxLayout>


// ChangeTime implementation

ChangeTime::ChangeTime()
	: timer_id(0)
{
	dev = bt_global::add_device_to_cache(new PlatformDevice);

	QLabel *img = new QLabel;
	img->setPixmap(bt_global::skin->getImage("time_icon"));

	edit = new BtTimeEdit(this, BtTimeEdit::DISPLAY_SECONDS);

	BtButton *ok = new BtButton;
	ok->setImage(bt_global::skin->getImage("ok"));

	date = new ChangeDate();

	connect(ok, SIGNAL(clicked()), SLOT(acceptTime()));
	connect(date, SIGNAL(Closed()), SIGNAL(Closed()));

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(10, 0, 10, 0);
	l->setSpacing(0);

	l->addWidget(img, 0, Qt::AlignCenter);
	l->addWidget(edit, 1);
	l->addWidget(ok);
}

void ChangeTime::acceptTime()
{
	dev->setTime(edit->time());

	// go to page date and stop the timer to update seconds
	date->showPage();
}

void ChangeTime::startTimeUpdate()
{
	// display current time and start the update timer
	edit->setTime(QTime::currentTime());
	if (timer_id == 0)
		timer_id = startTimer(1000);
}

void ChangeTime::showEvent(QShowEvent *event)
{
	startTimeUpdate();
}

void ChangeTime::hideEvent(QHideEvent *event)
{
	if (timer_id != 0)
		killTimer(timer_id);
	timer_id = 0;
}

void ChangeTime::timerEvent(QTimerEvent *event)
{
	// update displayed time
	BtTime t = edit->time().addSecond(1);
	edit->setTime(t);
}

void ChangeTime::showPage()
{
	startTimeUpdate();

	Page::showPage();
}


// ChangeDate implementation

ChangeDate::ChangeDate()
{
	dev = bt_global::add_device_to_cache(new PlatformDevice);

	QLabel *img = new QLabel;
	img->setPixmap(bt_global::skin->getImage("date_icon"));

	edit = new BtDateEdit(this);
	edit->setAllowPastDates(true);

	BtButton *ok = new BtButton;
	ok->setImage(bt_global::skin->getImage("ok"));

	connect(ok, SIGNAL(clicked()), SLOT(acceptDate()));

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(10, 0, 10, 0);
	l->setSpacing(0);

	l->addWidget(img, 0, Qt::AlignCenter);
	l->addWidget(edit, 1);
	l->addWidget(ok);
}

void ChangeDate::showPage()
{
	edit->setDate(QDate::currentDate());

	Page::showPage();
}

void ChangeDate::acceptDate()
{
	dev->setDate(edit->date());
	emit Closed();
}
