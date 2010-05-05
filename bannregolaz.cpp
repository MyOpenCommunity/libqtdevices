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


#include "bannregolaz.h"
#include "btbutton.h"
#include "fontmanager.h" // fontmanager

#include <QLabel>
#include <QHBoxLayout>

#define TIME_RIP_REGOLAZ 500

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
	vbox->setSpacing(0);
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
	text->setText(banner_text);
}

void BannLevel::initBanner(const QString &left, const QString &center_left, const QString &center_right,
		const QString &right, const QString &banner_text)
{
	left_button->setImage(left);
	center_left_button->setImage(center_left);
	center_right_button->setImage(center_right);
	right_button->setImage(right);
	text->setText(banner_text);
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
