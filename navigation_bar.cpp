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


#include "navigation_bar.h"
#include "btbutton.h"
#include "skinmanager.h" // bt_global::skin

#include <QFileInfo>

static const int buttons_dim = 60;


/*!
	\ingroup Core
	\class AbstractNavigationBar
	\brief Abstract base class for navigation bar.

	Defines the dimension and the positioning of its buttons.

	It provides the method \a createButton() which creates, moves to the right
	position and connect to the given slot a \a BtButton with the given icon.

	\sa createButton()
 */

/*!
	\brief Constructor
 */
AbstractNavigationBar::AbstractNavigationBar(QWidget *parent) : QWidget(parent)
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

/*!
	\brief Creates a button into the NavigationBar

	Returns a new BtButton instance with the given \a icon that will be emit
	the given signal when the user click the button and position it into the
	right position on the navigation bar.

	\sa BtButton
 */
BtButton *AbstractNavigationBar::createButton(QString icon, const char *signal, int pos)
{
	if (!icon.isNull())
	{
		BtButton *b = new BtButton(this);
		connect(b, SIGNAL(clicked()), signal);
		if (QFileInfo(icon).exists())
			b->setImage(icon); // for retrocompatibility
		else
			b->setImage(bt_global::skin->getImage(icon));
#ifdef LAYOUT_TS_3_5
		b->setGeometry(buttons_dim * pos, 0, buttons_dim, buttons_dim);
#else
		b->setGeometry(13, 60 + buttons_dim * pos, buttons_dim, buttons_dim);
#endif
		return b;
	}
	return 0;
}

QSize AbstractNavigationBar::sizeHint() const
{
#ifdef LAYOUT_TS_10
	return QSize(75, 355);
#else
	return QSize(buttons_dim * 4, buttons_dim);
#endif
}

/*!
	\ingroup Core
	\class NavigationBar
	\brief Represent a bar used to navigate between pages.

	A navigation bar is composed by four buttons, usually with the following meaning:
	\li a back button, to close the page and turn back to the previous one;
	\li an up button, to scroll up the content of the page;
	\li a down button, to scroll down the content of the page;
	\li a forward button, which has a custom behaviour.

	\sa Page
 */

/*!
	\brief Constructor

	Constructs a new NavigationBar and its default buttons if the corresponding
	tagnames are provided (tagnames are not empty).

	\sa AbstractNavigationBar::createButton()
 */
NavigationBar::NavigationBar(QString forward_icon, QString down_icon, QString up_icon, QString back_icon, QWidget *parent) :
	AbstractNavigationBar(parent)
{
#ifdef LAYOUT_TS_3_5
	back_button = createButton(back_icon, SIGNAL(backClick()), 0);
	down_button = createButton(down_icon, SIGNAL(downClick()), 1);
	up_button = createButton(up_icon, SIGNAL(upClick()), 2);
	forward_button = createButton(forward_icon, SIGNAL(forwardClick()), 3);
#else
	// TODO: TS 3.5'' code expects the forward button to be in the navigation
	// bar; in TS 10'' the button is mostly in the page (except for a delete
	// button in a couple of pages)
	forward_button = createButton(forward_icon, SIGNAL(forwardClick()), 0);
	up_button = createButton(up_icon, SIGNAL(upClick()), 1);
	down_button = createButton(down_icon, SIGNAL(downClick()), 2);
	back_button = createButton(back_icon, SIGNAL(backClick()), 3);
#endif
}

/*!
	\brief Shows or hides the scroll buttons
 */
void NavigationBar::displayScrollButtons(bool display)
{
	down_button->setVisible(display);
	up_button->setVisible(display);
}

/*!
	\fn void NavigationBar::backClick()
	\brief Signal emitted when the back button is clicked
 */

/*!
	\fn void NavigationBar::upClick()
	\brief Signal emitted when the up button is clicked
 */

/*!
	\fn void NavigationBar::downClick()
	\brief Signal emitted when the down button is clicked
 */

/*!
	\fn void NavigationBar::forwardClick()
	\brief Signal emitted when the forward button is clicked
 */
