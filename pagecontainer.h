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


#ifndef PAGECONTAINER_H
#define PAGECONTAINER_H

#include "page.h"

#include <QButtonGroup>
#include <QHash>

class QDomNode;

/**
 * \class PageContainer
 *
 * A container for other pages. Show a list of buttons, each of them is automatically
 * connected (in addPage method) with the correspondent page through the slot
 * 'showPage' and signal 'Closed'.
 */
class PageContainer : public Page
{
Q_OBJECT
public:
	PageContainer(const QDomNode &config_node);
	virtual void addBackButton();

protected:
	void addPage(Page *page, int id, QString iconName, int x, int y);

private:
	QButtonGroup buttons_group;
	QHash<int, Page*> page_list;
	void loadItems(const QDomNode &config_node);

private slots:
	void clicked(int id);
};

#endif // PAGECONTAINER_H
