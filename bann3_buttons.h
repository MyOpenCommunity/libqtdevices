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

class QWidget;

class Bann3Buttons : public BannerNew
{
Q_OBJECT
protected:
	Bann3Buttons(QWidget *parent);
	void initBanner(const QString &left, const QString &center, const QString &right,
		const QString &banner_text);
	BtButton *right_button, *center_button, *left_button;

private:
	QLabel *text;
};

/*!
  \class bann3But
  \brief This is a class that describes a banner with a text between 2 buttons 
  \author Davide
  \date lug 2005
*/
class bann3But : public banner
{
Q_OBJECT
public:
	bann3But(QWidget *parent);

signals:
	void centerClick();
};


/*!
  \class bann3ButLab
  \brief This is a class that describes a banner with a label between 2 buttons and a text under them
  There are 2 alternative buttons on the left and only one is visible at a time.
  \author Davide
  \date lug 2005
*/
class bann3ButLab : public banner
{
Q_OBJECT
public:
	bann3ButLab(QWidget *parent);
};


#endif // BANN3_BUTTONS_H
