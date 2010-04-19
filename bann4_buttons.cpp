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

#include <QHBoxLayout>
#include <QLabel>

#define BAN4BUT_DIM 60
#define BUT4TL_DIM 60
#define ICO4TL_DIM 120


Bann4ButtonsIcon::Bann4ButtonsIcon(QWidget *parent) :
	BannerNew(parent)
{
	right_button = new BtButton;
	center_right_button = new BtButton;
	left_button = new BtButton;
	center_left_button = new BtButton;

	center_icon = new QLabel;
	text = createTextLabel(Qt::AlignCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));

	QHBoxLayout *l = new QHBoxLayout;
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addWidget(left_button, 0, Qt::AlignLeft);
	l->addWidget(center_left_button);
	l->addWidget(center_icon, 1, Qt::AlignHCenter);
	l->addWidget(center_right_button);
	l->addWidget(right_button, 0, Qt::AlignRight);

	QVBoxLayout *v = new QVBoxLayout(this);
	v->setContentsMargins(0, 0, 0, 0);
	v->setSpacing(0);
	v->addLayout(l);
	v->addWidget(text);
}

void Bann4ButtonsIcon::initBanner(const QString &_right, const QString &_right_alt, const QString &_center,
	const QString &_c_left, const QString &_c_right, const QString &_left,
	const QString &_left_alt, States init_state, const QString &banner_text)
{
	center_left_button->setImage(_c_left);
	center_right_button->setImage(_c_right);
	center_icon->setPixmap(*bt_global::icons_cache.getIcon(_center));
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
	text = new QLabel;
	text->setAlignment(Qt::AlignHCenter);

	QGridLayout *grid = new QGridLayout;
	grid->setContentsMargins(0, 0, 0, 0);
	grid->setSpacing(0);
	grid->addWidget(left_button, 0, 0);
	grid->addWidget(center_left_button, 0, 1);
	grid->addWidget(center_right_button, 0, 2);
	grid->addWidget(right_button, 0, 3);
	for (int i = 0; i < grid->columnCount(); ++i)
		grid->setColumnStretch(i, 1);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addLayout(grid);
	l->addWidget(text);
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



bann4But::bann4But(QWidget *parent) : BannerOld(parent)
{
	// sx
	addItem(BUT1,(banner_width/4-BAN4BUT_DIM)/2, 0, BAN4BUT_DIM , BAN4BUT_DIM);
	// csx
	addItem(BUT3, banner_width/4+(banner_width/4-BAN4BUT_DIM)/2, 0, BAN4BUT_DIM ,BAN4BUT_DIM);
	// cdx
	addItem(BUT4, banner_width/2+(banner_width/4-BAN4BUT_DIM)/2, 0, BAN4BUT_DIM, BAN4BUT_DIM);
	// dx
	addItem(BUT2, banner_width*3/4+(banner_width/4-BAN4BUT_DIM)/2, 0, BAN4BUT_DIM, BAN4BUT_DIM);
	addItem(TEXT, 0, BAN4BUT_DIM, banner_width, banner_height-BAN4BUT_DIM);
	Draw();
}


bann4tasLab::bann4tasLab(QWidget *parent) : BannerOld(parent)
{
	// sx
	addItem(BUT1, (banner_width/4-BUT4TL_DIM)/2, 0, BUT4TL_DIM, BUT4TL_DIM);
	// csx
	addItem(BUT3, banner_width/4+(banner_width/4-BUT4TL_DIM)/2, 0, BUT4TL_DIM, BUT4TL_DIM);
	// cdx
	addItem(BUT4, banner_width/2+(banner_width/4-BUT4TL_DIM)/2, 0, BUT4TL_DIM, BUT4TL_DIM);
	// dx
	addItem(BUT2, banner_width*3/4+(banner_width/4-BUT4TL_DIM)/2, 0, BUT4TL_DIM, BUT4TL_DIM);
	addItem(ICON, (banner_width-ICO4TL_DIM)/2, 0, ICO4TL_DIM, BUT4TL_DIM);
	addItem(TEXT, 0, BUT4TL_DIM , banner_width, banner_height-BUT4TL_DIM);
	impostaAttivo(2);
	Draw();
}

