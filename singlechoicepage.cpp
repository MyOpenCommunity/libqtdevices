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
#include "navigation_bar.h"
#include "singlechoicecontent.h"
#include "btbutton.h"

#include <QDebug>


SingleChoicePage::SingleChoicePage(const QString &title)
{
#ifdef LAYOUT_BTOUCH
	buildPage(new SingleChoiceContent, new NavigationBar("ok"), title);
#else
	buildPage(new SingleChoiceContent, new NavigationBar, title);
#endif

	connect(this, SIGNAL(forwardClick()), SLOT(okPressed()));
}

void SingleChoicePage::addBanner(CheckableBanner *bann, int id)
{
	page_content->addBanner(bann, id);

	if (id == getCurrentId())
		setCheckedId(id);
}

void SingleChoicePage::setCheckedId(int id)
{
	page_content->setCheckedId(id);
}

void SingleChoicePage::okPressed()
{
	confirmSelection();
	emit Closed();
}

void SingleChoicePage::confirmSelection()
{
	bannerSelected(page_content->checkedId()); // update the current id and do custom actions
	setCheckedId(getCurrentId());
}
