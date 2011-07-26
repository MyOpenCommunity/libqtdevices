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


#ifndef BANN4_BUTTONS_H
#define BANN4_BUTTONS_H

#include "banner.h"

#include <QTimer>


class BtButton;
class ScrollingLabel;
class QHBoxLayout;
class QWidget;

/*
 * This is a class that describes a banner with 4 buttons a label and a text under them.
 */
class Bann4ButtonsIcon : public Banner
{
Q_OBJECT
protected:
	enum States
	{
		LOCKED,               // device is locked, no edit is possible (no right button visible)
		UNLOCKED,             // scenario edit is possible (pencil icon on right)
		EDIT_VIEW,            // edit buttons visible (right arrow icon on right)
	};
	enum EditingStates
	{
		EDIT_ACTIVE,          // editing is enabled (stop icon on left)
		EDIT_INACTIVE,        // editing is disabled (on icon on left)
	};

	Bann4ButtonsIcon(QWidget *parent);
	void initBanner(const QString &_right, const QString &_right_alt, const QString &_center,
		const QString &_c_left, const QString &_c_right, const QString &_left,
		const QString &_left_alt, States init_state, const QString &banner_text);
	void setState(States new_state);
	void setEditingState(EditingStates edit_state);
	BtButton *left_button, *center_left_button, *center_right_button, *right_button;

private:
	QLabel *center_icon, *text;
	QString left_icon, left_alt, right_icon, right_alt;
};


// Banner with 4 buttons and a descriptive label
class Bann4Buttons : public Banner
{
Q_OBJECT
public:
	Bann4Buttons(QWidget *parent = 0);
	void initBanner(const QString &right, const QString &center_right, const QString &center_left,
		const QString &left, const QString &banner_text);

	/*!
		\brief Squash together central buttons
	 */
	void setCentralSpacing(bool spaced);

	// Connect the left button to the page argument, shown when the button is clicked.
	void connectLeftButton(Page *p);

	// Connect the right button to the page argument, shown when the button is clicked.
	void connectRightButton(Page *p);

protected:
	BtButton *right_button, *center_right_button, *center_left_button, *left_button;

private:
	ScrollingLabel *text;
	QHBoxLayout *center_layout;
};


/*
 * A class to control the rate of signals emitted
 */
// TODO use Bann4Buttons as the base class
class BannLevel : public Banner
{
Q_OBJECT
protected:
	BannLevel(QWidget *parent);
	// to be used by derived classes which are still graphics (eg. AdjustDimmer)
	void initBanner(const QString &banner_text);
	// use this when no state is required
	void initBanner(const QString &left, const QString &center_left, const QString &center_right,
		const QString &right, const QString &banner_text);
	void setCenterLeftIcon(const QString &image);
	void setCenterRightIcon(const QString &image);
	BtButton *left_button, *right_button;

private slots:
	void startLeftTimer();
	void startRightTimer();

private:
	BtButton *center_left_button, *center_right_button;
	ScrollingLabel *text;
	QTimer timer;

signals:
	void center_left_clicked();
	void center_right_clicked();
};

#endif // BANN4_BUTTONS_H
