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


#ifndef SECTION_PAGE_H
#define SECTION_PAGE_H

#include "iconpage.h"
#include "state_button.h"

class QDomNode;


/*!
	\ingroup Core
	\brief A base class for pages that contain sections.

	Using the getPage() factory function, this class creates an icon for each
	section defined under the configuration node passed at the constructor.
*/
class SectionPage : public IconPage
{
Q_OBJECT
protected:
	SectionPage(const QDomNode &config_node);
	SectionPage();

	void loadItems(const QDomNode &config_node);

	void addPage(Page *page, const QString &label, const QString &icon_name, const QString &icon_name_on = QString(),
		int x = 0, int y = 0);

protected slots:
	virtual void changeIconState(StateButton::Status st) {}
};

#endif // SECTION_PAGE_H
