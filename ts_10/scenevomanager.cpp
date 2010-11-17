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
#include "fontmanager.h"

#include <QGridLayout>
#include <QDebug>
#include <QLabel>


ScenEvoManager::ScenEvoManager(QString title, ScenEvoTimeCondition *time_cond, ScenEvoDeviceCondition *device_cond)
{
	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	buildPage(new QWidget, nav_bar, title, TINY_TITLE_HEIGHT);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(reset()));


	QGridLayout *main_layout = new QGridLayout(page_content);
	main_layout->setContentsMargins(30, 30, 30, 30);
	main_layout->setSpacing(0);

	QVBoxLayout *left_box = new QVBoxLayout;
	left_box->setContentsMargins(0, 0, 0, 0);
	left_box->setSpacing(5);

	QLabel *left_image = new QLabel;
	left_image->setPixmap(*bt_global::icons_cache.getIcon(bt_global::skin->getImage("watch")));
	left_box->addWidget(left_image, 0, Qt::AlignHCenter);

	QLabel *left_descr = new QLabel;
	left_descr->setFont(bt_global::font->get(FontManager::TEXT));
	left_descr->setText(tr("Time Conditions-Hour & Minute"));
	left_box->addWidget(left_descr, 0, Qt::AlignHCenter);

	main_layout->addLayout(left_box, 0, 0, 1, 1, Qt::AlignLeft);

	if (time_cond)
		main_layout->addWidget(time_cond, 0, 1, 2, 1, Qt::AlignRight | Qt::AlignTop);

	if (device_cond)
	{
		QGridLayout *device_layout = new QGridLayout;
		device_layout->setContentsMargins(0, 0, 0, 0);
		device_layout->setSpacing(20);
		QLabel *label_icon = new QLabel;
		label_icon->setPixmap(*bt_global::icons_cache.getIcon(device_cond->getIcon()));
		label_icon->setFixedSize(label_icon->pixmap()->size());

		QLabel *label_text = new QLabel;
		label_text->setFont(bt_global::font->get(FontManager::TEXT));
		label_text->setText(device_cond->getDescription());
		device_layout->addWidget(label_icon, 0, 0);
		device_layout->addWidget(label_text, 0, 1);
		device_layout->addWidget(device_cond, 1, 0, 1, 2);

		main_layout->addLayout(device_layout, 1, 0, 2, 1, Qt::AlignBottom | Qt::AlignLeft);
	}

	BtButton *ok = new BtButton;
	ok->setImage(bt_global::skin->getImage("ok"));
	connect(ok, SIGNAL(clicked()), SIGNAL(Closed()));
	connect(ok, SIGNAL(clicked()), SIGNAL(save()));
	main_layout->addWidget(ok, 2, 1, 1, 1, Qt::AlignRight | Qt::AlignBottom);
}

