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


#include "singlechoicepage.h"
#include "main.h" // for ICON_{OK,VUOTO}
#include "bann1_button.h" // bannOnSx
#include "btbutton.h"

#include <QDebug>


SingleChoicePage::SingleChoicePage(bool paging)
{
	if (paging)
		setNavBarMode(4, ICON_OK);
	else
		setNavBarMode(10, ICON_OK);

	buttons.setExclusive(true);

	connect(this, SIGNAL(goDx()), SLOT(okPressed()));
}

void SingleChoicePage::addBanner(const QString &text, int id)
{
	bannOnSx *bann = new bannOnSx(this, ICON_VUOTO);
	BtButton *btn = bann->getButton();
	btn->setCheckable(true);
	bann->setText(text);
	buttons.addButton(btn, id);
	elencoBanner.append(bann);

	if (id == getCurrentId())
		btn->setChecked(true);
}

void SingleChoicePage::okPressed()
{
	bannerSelected(buttons.checkedId()); // update the current id and do custom actions
	foreach (QAbstractButton *btn, buttons.buttons())
		if (buttons.id(btn) == getCurrentId())
			btn->setChecked(true);

	emit Closed();
}

