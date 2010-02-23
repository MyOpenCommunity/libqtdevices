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


#include "bann3_buttons.h"
#include "fontmanager.h" // FontManager
#include "btbutton.h"

#include <QLabel>
#include <QHBoxLayout>

#define BAN3BUT_BUT_DIM 60
#define BAN3BUT_BUTCEN_DIM 120
#define BUT3BL_ICON_DIM_X 120
#define BUT3BL_ICON_DIM_Y 60
#define BAN3BL_BUT_DIM 60

Bann3Buttons::Bann3Buttons(QWidget *parent) :
	BannerNew(parent)
{
	right_button = new BtButton;
	center_button = new BtButton;
	left_button = new BtButton;
	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::TEXT));

	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(0);
	hbox->addWidget(left_button, 0, Qt::AlignLeft);
	hbox->addWidget(center_button, 1, Qt::AlignHCenter);
	hbox->addWidget(right_button, 0, Qt::AlignRight);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addLayout(hbox);
	l->addWidget(text);
}

void Bann3Buttons::initBanner(const QString &left, const QString &center, const QString &right,
	const QString &banner_text)
{
	right_button->setImage(right);
	center_button->setImage(center);
	left_button->setImage(left);
	text->setText(banner_text);
}

bann3But::bann3But(QWidget *parent) : banner(parent)
{
	addItem(BUT1, 0, 0, BAN3BUT_BUT_DIM, BAN3BUT_BUT_DIM);
	addItem(BUT2, banner_width - BAN3BUT_BUT_DIM, 0 , BAN3BUT_BUT_DIM , BAN3BUT_BUT_DIM);
	addItem(TEXT, 0, BAN3BUT_BUT_DIM, banner_width, banner_height - BAN3BUT_BUT_DIM);
	addItem(BUT3, banner_width/2 - BAN3BUT_BUTCEN_DIM/2, 0, BAN3BUT_BUTCEN_DIM, BAN3BUT_BUT_DIM);
	connect(this,SIGNAL(csxClick()),this,SIGNAL(centerClick()));
}


bann3ButLab::bann3ButLab(QWidget *parent) : banner(parent)
{
	addItem(BUT1, banner_width - BAN3BL_BUT_DIM, 0, BAN3BL_BUT_DIM, BAN3BL_BUT_DIM);
	addItem(BUT2, 0, 0, BAN3BL_BUT_DIM, BAN3BL_BUT_DIM);
	addItem(BUT4, 0, 0, BAN3BL_BUT_DIM, BAN3BL_BUT_DIM);
	addItem(TEXT, 0, BAN3BL_BUT_DIM, banner_width, banner_height - BAN3BL_BUT_DIM);
	addItem(ICON, BAN3BL_BUT_DIM, 0, BUT3BL_ICON_DIM_X, BUT3BL_ICON_DIM_Y);
}

