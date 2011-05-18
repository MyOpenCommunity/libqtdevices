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


#include "bann4_buttons.h"
#include "btbutton.h"
#include "fontmanager.h" // FontManager
#include "icondispatcher.h" // icons_cache
#include "labels.h" // ScrollingLabel

#include <QGridLayout>



#define TIME_RIP_REGOLAZ 500


Bann4ButtonsIcon::Bann4ButtonsIcon(QWidget *parent) :
	BannerNew(parent)
{
	right_button = new BtButton;
	center_right_button = new BtButton;
	left_button = new BtButton;
	center_left_button = new BtButton;

	center_icon = new QLabel;
	text = createTextLabel(Qt::AlignCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));

	QGridLayout *l = new QGridLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);

	l->addWidget(left_button, 0, 0, Qt::AlignLeft);
	l->setColumnStretch(0, 1);
	l->addWidget(center_left_button, 0, 1, Qt::AlignHCenter);
	l->addWidget(center_icon, 0, 1, 1, 2, Qt::AlignHCenter);
	l->addWidget(center_right_button, 0, 2, Qt::AlignHCenter);
	l->addWidget(right_button, 0, 3, Qt::AlignRight);
	l->addWidget(text, 1, 0, 1, 4);

	for (int i = 0; i < 4; ++i)
		l->setColumnStretch(i, 1);
}

void Bann4ButtonsIcon::initBanner(const QString &_right, const QString &_right_alt, const QString &_center,
	const QString &_c_left, const QString &_c_right, const QString &_left,
	const QString &_left_alt, States init_state, const QString &banner_text)
{
	center_left_button->setImage(_c_left);
	center_right_button->setImage(_c_right);
	center_icon->setPixmap(*bt_global::icons_cache.getIcon(_center));
	center_icon->setMinimumSize(center_icon->pixmap()->size());
	text->setText(banner_text);

	left_icon = _left;
	left_alt = _left_alt;
	right_icon = _right;
	right_alt = _right_alt;

	setState(init_state);
	setEditingState(EDIT_INACTIVE);
}

void Bann4ButtonsIcon::setState(States new_state)
{
	switch (new_state)
	{
	case LOCKED:
		center_icon->show();
		center_left_button->hide();
		center_right_button->hide();
		right_button->hide();
		break;
	case UNLOCKED:
		center_icon->show();
		center_left_button->hide();
		center_right_button->hide();
		right_button->show();
		right_button->setImage(right_icon);
		break;
	case EDIT_VIEW:
		center_icon->hide();
		center_left_button->show();
		center_right_button->show();
		right_button->show();
		right_button->setImage(right_alt);
		break;
	}
}

void Bann4ButtonsIcon::setEditingState(EditingStates edit_state)
{
	switch (edit_state)
	{
	case EDIT_ACTIVE:
		left_button->setImage(left_alt);
		break;
	case EDIT_INACTIVE:
		left_button->setImage(left_icon);
		break;
	}
}


Bann4Buttons::Bann4Buttons(QWidget *parent) :
	BannerNew(parent)
{
	right_button = new BtButton;
	center_right_button = new BtButton;
	center_left_button = new BtButton;
	left_button = new BtButton;
	text = new ScrollingLabel;
	text->setAlignment(Qt::AlignHCenter);

	QGridLayout *grid = new QGridLayout(this);
	grid->setContentsMargins(0, 0, 0, 0);
#ifdef LAYOUT_TS_3_5
	grid->setHorizontalSpacing(10);
	grid->setVerticalSpacing(5);
#else
	grid->setSpacing(0);
#endif
	grid->addWidget(left_button, 0, 0);

	// Sometimes we have two images that compose an unique image for the user:
	// to handle this case (setCentralSpacing(false)) we use a horizontal layout
	// and remove the spacing from the two images.
	center_layout = new QHBoxLayout;
	center_layout->setContentsMargins(0, 0, 0, 0);
	center_layout->setSpacing(10);

	center_layout->addWidget(center_left_button);
	center_layout->addWidget(center_right_button);

	grid->addLayout(center_layout, 0, 1, 1, 2);
	grid->addWidget(right_button, 0, 3);
	for (int i = 0; i < grid->columnCount(); ++i)
		grid->setColumnStretch(i, 1);

	grid->addWidget(text, 1, 0, 1, 4);
}

void Bann4Buttons::initBanner(const QString &right, const QString &center_right,
	const QString &center_left, const QString &left, const QString &banner_text)
{
	initButton(right_button, right);
	initButton(center_right_button, center_right);
	initButton(center_left_button, center_left);
	initButton(left_button, left);
	initLabel(text, banner_text, bt_global::font->get(FontManager::BANNERDESCRIPTION));
}

void Bann4Buttons::setCentralSpacing(bool spaced)
{
	center_layout->setSpacing(spaced ? 10 : 0);
}


BannLevel::BannLevel(QWidget *parent) :
	BannerNew(parent)
{
	left_button = new BtButton;
	center_left_button = new BtButton;
	center_right_button = new BtButton;
	right_button = new BtButton;

	QGridLayout *grid = new QGridLayout;
	grid->setContentsMargins(0, 0, 0, 0);
	grid->setSpacing(0);
	grid->addWidget(left_button, 0, 0);
	grid->addWidget(center_left_button, 0, 1, Qt::AlignRight);
	grid->addWidget(center_right_button, 0, 2, Qt::AlignLeft);
	grid->addWidget(right_button, 0, 3);

	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));

	QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->setContentsMargins(0, 0, 0, 0);

#ifdef LAYOUT_TS_3_5
	vbox->setSpacing(5);
#else
	vbox->setSpacing(0);
#endif

	vbox->addLayout(grid);
	vbox->addWidget(text);

	timer.setInterval(TIME_RIP_REGOLAZ);
	// start rate controlling timer
	connect(center_left_button, SIGNAL(pressed()), SLOT(startLeftTimer()));
	connect(center_right_button, SIGNAL(pressed()), SLOT(startRightTimer()));

	connect(center_left_button, SIGNAL(released()), &timer, SLOT(stop()));
	connect(center_right_button, SIGNAL(released()), &timer, SLOT(stop()));

	// we also want the user to be able to bash the button until the screen is broken
	connect(center_left_button, SIGNAL(clicked()), SIGNAL(center_left_clicked()));
	connect(center_right_button, SIGNAL(clicked()), SIGNAL(center_right_clicked()));
}

void BannLevel::initBanner(const QString &banner_text)
{
	text->setScrollingText(banner_text);
}

void BannLevel::initBanner(const QString &left, const QString &center_left, const QString &center_right,
		const QString &right, const QString &banner_text)
{
	left_button->setImage(left);
	center_left_button->setImage(center_left);
	center_right_button->setImage(center_right);
	right_button->setImage(right);
	text->setScrollingText(banner_text);
}

void BannLevel::setCenterLeftIcon(const QString &image)
{
	center_left_button->setImage(image);
}

void BannLevel::setCenterRightIcon(const QString &image)
{
	center_right_button->setImage(image);
}

void BannLevel::startLeftTimer()
{
	if (!timer.isActive())
	{
		timer.disconnect(SIGNAL(timeout()));
		connect(&timer, SIGNAL(timeout()), SIGNAL(center_left_clicked()));
		timer.start();
	}
}

void BannLevel::startRightTimer()
{
	if (!timer.isActive())
	{
		timer.disconnect(SIGNAL(timeout()));
		connect(&timer, SIGNAL(timeout()), SIGNAL(center_right_clicked()));
		timer.start();
	}
}
