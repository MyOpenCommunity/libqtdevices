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

#include "scrollablepage.h"

#include <QButtonGroup>
#include <QHash>
#include <QList>

class IconContent;
class BtButton;
class StateButton;
class NavigationBar;


#define DIM_BUT 80

/*!
	\ingroup Core
	\brief Shows a list of buttons.

	The %IconPage is a ScrollablePage subclass, so if the content is to big
	to be visualized in a single screen, it is paginated.

	As in ScrollablePage, all the logic about banner arranging into the single
	page is delegate to the content class, in this case the IconContent class.

	There are two methods for adding a button into the IconPage:
	- by the addButton() method, which creates the button attach it to the
		page and returns it
	- by the addPage() method, which creates the button, attach it to the page
		and connect it to the corresponding page through the slot showPage() and
		the Closed() signal.
 */
class IconPage : public ScrollablePage
{
Q_OBJECT
public:
	// the type returned by page_content
	typedef IconContent ContentType;

protected:
	/*!
		\brief Build an IconPage using the given \a content and \a nav_bar.

		If \a label is not null, a PageTitleWidget is created and placed in the
		right place.
	*/
	void buildPage(IconContent *content, NavigationBar *nav_bar, const QString &label = QString());

	/*!
		\brief Creates a button, puts it on the IconPage and connects it to \a page.
	*/
	void addPage(Page *page, const QString &label, const QString &iconName, int x = 0, int y = 0);

	/*!
		\brief Creates a button, puts it on the IconPage, and returns it.
	*/
	BtButton *addButton(const QString &label, const QString &iconName, int x = 0, int y = 0);
};


/*!
	\ingroup Core
	\brief Manages a grid of buttons.

	This class extends the ScrollableContent class, inhering all the logic about
	pagination.

	The drawContent() method is reimplemented to correctly arrange buttons inside
	the content.

	To add a button to the IconContent you can use the addButton() method.
	The addWidget() method permits to add a generic widget to the content.
*/
class IconContent : public ScrollableContent
{
Q_OBJECT
public:
	/*!
		\brief Constructor.

		Creates a new IconContent with the given \a parent.
	*/
	IconContent(QWidget *parent = 0);

	/*!
		\brief Puts the \a button inside the content.

		If label is not empty, creates a new ScrollingLabel as description for
		the button placed below it.
	*/
	void addButton(QWidget *button, const QString &label = QString());

	/*!
		\brief Puts \a widget inside the content.
	*/
	void addWidget(QWidget *widget);

	virtual void drawContent();

private:
	QList<QWidget*> items;
};


/*!
	\ingroup Core
	\brief Base class for IconPage buttons that aren't just links to pages but have some
	special behaviour. Provides an uniform layout.
*/
class IconPageButton : public QWidget
{
Q_OBJECT
public:
	/*!
		\brief Constructor.

		Construct a new IconPageButton with the given \a label.
	*/
	IconPageButton(const QString &label);

protected:
	StateButton *button;
};


#endif // ICONPAGE_H
