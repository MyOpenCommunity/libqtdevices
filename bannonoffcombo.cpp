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


#include "bannonoffcombo.h"
#include "btbutton.h"
#include "skinmanager.h" // bt_global::skin
#include "icondispatcher.h" // bt_global::icons_cache
#include "fontmanager.h" // bt_global::font

#include <QLabel>
#include <QHBoxLayout>

BannOnOffCombo::BannOnOffCombo(QWidget *parent) : BannerNew(parent)
{
	left_button = new BtButton;
	right_button = new BtButton;
	internal_label = createTextLabel(Qt::AlignCenter, bt_global::font->get(FontManager::BANNERTEXT));
	internal_label->setProperty("SecondFgColor", true);
	center_icon = new QLabel;
	right_icon = new QLabel;
	left_icon = new QLabel;
	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));

	// central box with balance icons and balance text
	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->addWidget(left_icon);
	hbox->addWidget(center_icon);
	hbox->addWidget(internal_label);
	hbox->addWidget(right_icon);

	QGridLayout *l = new QGridLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addWidget(left_button, 0, 0, Qt::AlignLeft);
	l->addLayout(hbox, 0, 1);
	l->setColumnStretch(1, 2);
	l->addWidget(right_button, 0, 2, Qt::AlignRight);
	l->addWidget(text, 1, 0, 1, 3);
}

void BannOnOffCombo::initBanner(const QString &left, const QString center_left, const QString &center,
	const QString &center_right, const QString &right, ComboStatus init_status, const QString &banner_text)
{
	left_button->setImage(left);
	right_button->setImage(right);

	left_icon->setPixmap(*bt_global::icons_cache.getIcon(center_left));
	center_icon->setPixmap(*bt_global::icons_cache.getIcon(center));
	right_icon->setPixmap(*bt_global::icons_cache.getIcon(center_right));

	text->setText(banner_text);
	changeStatus(init_status);
}

void BannOnOffCombo::setInternalText(const QString &t)
{
	internal_label->setText(t);
}

void BannOnOffCombo::changeStatus(ComboStatus st)
{
	switch (st)
	{
	case CENTER:
		center_icon->setVisible(true);
		left_icon->setVisible(false);
		right_icon->setVisible(false);
		internal_label->setVisible(false);
		break;
	case SX:
		center_icon->setVisible(false);
		left_icon->setVisible(true);
		right_icon->setVisible(false);
		internal_label->setVisible(true);
		break;
	case DX:
		center_icon->setVisible(false);
		left_icon->setVisible(false);
		right_icon->setVisible(true);
		internal_label->setVisible(true);
		break;
	default:
		qWarning("STATUS UNKNOWN on BannOnOffCombo::changeStatus");
	}
}
