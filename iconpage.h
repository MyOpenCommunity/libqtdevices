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


#ifndef ICONPAGE_H
#define ICONPAGE_H

#include "page.h"
#include "gridcontent.h"

#include <QButtonGroup>
#include <QHash>
#include <QList>

class IconContent;
class BtButton;

/**
 * \class IconPage
 *
 * Show a list of buttons, each of them is automatically connected (in addPage method)
 * with the correspondent page through the slot 'showPage' and signal 'Closed'.
 */
class IconPage : public Page
{
Q_OBJECT
public:
	// the type returned by page_content
	typedef IconContent ContentType;

	virtual void addBackButton();
	virtual void activateLayout();

protected:
	void buildPage(IconContent *content, NavigationBar *nav_bar, const QString &label = QString());
	void addPage(Page *page, const QString &label, const QString &iconName, int x = 0, int y = 0);
	BtButton *addButton(const QString &label, const QString &iconName, int x = 0, int y = 0);
};


/**
 * The IconContent class manages a grid of buttons.
 */
class IconContent : public GridContent
{
friend void IconPage::activateLayout();
Q_OBJECT
public:
	IconContent(QWidget *parent=0);
	void addButton(QWidget *button, const QString &label = QString());
	void addWidget(QWidget *widget);

protected:
	// The drawContent is the place where this widget is actually drawed. In order
	// to have a correct transition effect, this method is also called by the
	// Page _before_ that the Page is showed.
	virtual void drawContent();

private:
	QList<QWidget*> items;
};

#endif // ICONPAGE_H
