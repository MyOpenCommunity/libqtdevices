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


#ifndef BANN3_BUTTONS_H
#define BANN3_BUTTONS_H

#include "banner.h"


class ScrollingLabel;


class QWidget;


// Banner with 3 buttons left, center and right + description below
class Bann3Buttons : public BannerNew
{
Q_OBJECT
protected:
	Bann3Buttons(QWidget *parent = 0);

	/*
	 * \param left Left button icon
	 * \param center Center button icon
	 * \param right Right button icon
	 */
	void initBanner(const QString &left, const QString &center, const QString &right,
		const QString &banner_text);
	BtButton *right_button, *center_button, *left_button;

private:
	ScrollingLabel *text;
};


// A banner with 2 (optional) buttons on both sides, a button + a label on the center, description below
class Bann3ButtonsLabel : public BannerNew
{
Q_OBJECT
protected:
	enum State
	{
		DISABLED,
		ENABLED,
	};

	enum Forced
	{
		NOT_FORCED,
		FORCED,
	};

	Bann3ButtonsLabel(QWidget *parent=0);
	void initBanner(const QString &_left_forced, const QString &_left_not_forced, const QString &center_but, const QString &center_lab,
		const QString &right, State init_state, Forced init_forced, const QString &banner_text);
	void setState(State new_state);
	void setForced(Forced is_forced);

	// TODO: add states (active, not active...)
	BtButton *right_button, *center_button, *left_button;

private:
	QLabel *center_label;
	ScrollingLabel *text;
	QString left_forced, left_not_forced;
};


#endif // BANN3_BUTTONS_H
