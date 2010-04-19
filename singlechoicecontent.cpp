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


#include "singlechoicecontent.h"
#include "state_button.h"
#include "skinmanager.h"

#include <QLayout>


CheckableBanner *SingleChoice::createBanner(const QString &text, const QString &right_icon)
{
	return new CheckableBanner(text, right_icon);
}

CheckableBanner::CheckableBanner(const QString &text, const QString &right_icon)
	: Bann2StateButtons(0)
{
	initBanner(bt_global::skin->getImage("unchecked"), right_icon, text);
#ifdef LAYOUT_TOUCHX
	layout()->setContentsMargins(0, 0, 0, 10);
#endif
	left_button->setOnOff();
	left_button->setCheckable(true);
	left_button->setOffImage(bt_global::skin->getImage("unchecked"));
	left_button->setOnImage(bt_global::skin->getImage("checked"));
}

BtButton *CheckableBanner::getButton()
{
	return left_button;
}


SingleChoiceContent::SingleChoiceContent()
{
	buttons.setExclusive(true);

	connect(&buttons, SIGNAL(buttonClicked(int)),
		this, SIGNAL(bannerSelected(int)));
}

void SingleChoiceContent::addBanner(CheckableBanner *bann, int id)
{
	buttons.addButton(bann->getButton(), id);
	appendBanner(bann);
}

int SingleChoiceContent::checkedId() const
{
	return buttons.checkedId();
}

void SingleChoiceContent::setCheckedId(int id)
{
	buttons.button(id)->setChecked(true);
}

QList<QAbstractButton*> SingleChoiceContent::getButtons()
{
	return buttons.buttons();
}
