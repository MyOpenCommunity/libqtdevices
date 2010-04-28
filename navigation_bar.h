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


/**
 * The abstract version of the navigation bar, that defines only the dimension
 * and the positioning of its buttons.
 */
class AbstractNavigationBar : public QWidget
{
protected:
	AbstractNavigationBar(QWidget *parent = 0);
	virtual QSize sizeHint() const;
	BtButton *createButton(QString icon, const char *signal, int pos);
};


/**
 * The navigation bar, used to navigate through pages
 */
class NavigationBar : public AbstractNavigationBar
{
Q_OBJECT
public:
	NavigationBar(QString forward_icon=QString(), QString down_icon="scroll_down",
		QString up_icon="scroll_up", QString back_icon="back", QWidget *parent = 0);

	// the forward button is a common customization of the NavigationBar, so we
	// do it public
	BtButton *forward_button;

public slots:
	void displayScrollButtons(bool display);

signals:
	void backClick();
	void upClick();
	void downClick();
	void forwardClick();

private:
	BtButton *back_button, *up_button, *down_button;
};


#endif // NAVIGATION_BAR_H
