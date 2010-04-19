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


#include "scenevomanager.h"
#include "icondispatcher.h" // bt_global::icons_cache
#include "skinmanager.h" // bt_global::skin
#include "btbutton.h"
#include "scenevocond.h"
#include "navigation_bar.h"

#include <QGridLayout>
#include <QDebug>
#include <QLabel>


ScenEvoManager::ScenEvoManager(ScenEvoTimeCondition *time_cond, ScenEvoDeviceCondition *device_cond)
{
	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	buildPage(new QWidget, nav_bar);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(reset()));

	QVBoxLayout *main_layout = new QVBoxLayout(page_content);
	main_layout->setContentsMargins(30, 30, 30, 30);
	main_layout->setSpacing(0);

	QHBoxLayout *first_row = new QHBoxLayout;
	first_row->setContentsMargins(0, 0, 0, 0);
	first_row->setSpacing(5);

	QVBoxLayout *left_box = new QVBoxLayout;
	left_box->setContentsMargins(0, 0, 0, 0);
	left_box->setSpacing(5);

	QLabel *left_image = new QLabel;
	left_image->setPixmap(*bt_global::icons_cache.getIcon(bt_global::skin->getImage("watch")));
	left_box->addWidget(left_image, 0, Qt::AlignHCenter);

	QLabel *left_descr = new QLabel;
	left_descr->setText(tr("Time Conditions\nHour & Minute"));
	left_box->addWidget(left_descr, 0, Qt::AlignHCenter);
	left_box->addStretch(1);

	first_row->addLayout(left_box);
	first_row->addStretch(1);

	if (time_cond)
		first_row->addWidget(time_cond);

	main_layout->addLayout(first_row);
	main_layout->addStretch(1);

	QHBoxLayout *second_row = new QHBoxLayout;
	second_row->setContentsMargins(0, 0, 0, 0);
	second_row->setSpacing(5);

	if (device_cond)
		second_row->addWidget(device_cond);

	second_row->addStretch(1);
	BtButton *ok = new BtButton;
	ok->setImage(bt_global::skin->getImage("ok"));
	connect(ok, SIGNAL(clicked()), SIGNAL(Closed()));
	connect(ok, SIGNAL(clicked()), SIGNAL(save()));
	second_row->addWidget(ok);
	main_layout->addLayout(second_row);
}

