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


#ifndef NAVIGATION_BAR_H
#define NAVIGATION_BAR_H

#include <QWidget>

class BtButton;

/*!
	\ingroup Core
	\brief Abstract base class for navigation bar.

	Defines the dimension and the positioning of its buttons.

	It provides the method \a createButton() which creates, moves to the right
	position and connect to the given slot a \a BtButton with the given icon.

	\sa createButton()
*/
class AbstractNavigationBar : public QWidget
{
protected:
	AbstractNavigationBar(QWidget *parent = 0);
	virtual QSize sizeHint() const;

	/*!
		\brief Creates a button into the NavigationBar.

		Returns a new BtButton instance with the given \a icon that will be emit
		the given signal when the user click the button and position it into the
		right position on the navigation bar.

		\sa BtButton
	*/
	BtButton *createButton(QString icon, const char *signal, int pos);
};

/*!
	\ingroup Core
	\brief Represents a bar used to navigate between pages.

	A navigation bar is composed by four buttons, usually with the following meaning:
	\li a back button, to close the page and turn back to the previous one;
	\li an up button, to scroll up the content of the page;
	\li a down button, to scroll down the content of the page;
	\li a forward button, which has a custom behaviour.

	\sa Page
 */
class NavigationBar : public AbstractNavigationBar
{
Q_OBJECT
public:
	/*!
		\brief Constructor

		Constructs a new NavigationBar and its default buttons if the corresponding
		tagnames are provided (tagnames are not empty).

		\sa AbstractNavigationBar::createButton()
	*/
	NavigationBar(QString forward_icon=QString(), QString down_icon="scroll_down",
		QString up_icon="scroll_up", QString back_icon="back", QWidget *parent = 0);

	// the forward button is a common customization of the NavigationBar, so we
	// make it public
	BtButton *forward_button;

public slots:
	/*!
		\brief Shows or hides the scroll buttons
	*/
	void displayScrollButtons(bool display);

signals:
	/*!
		\brief Signal emitted when the back button is clicked
	*/
	void backClick();

	/*!
		\brief Signal emitted when the up button is clicked
	*/
	void upClick();

	/*!
		\brief Signal emitted when the down button is clicked
	*/
	void downClick();

	/*!
		\brief Signal emitted when the forward button is clicked
	*/
	void forwardClick();

private:
	BtButton *back_button, *up_button, *down_button;
};


#endif // NAVIGATION_BAR_H
