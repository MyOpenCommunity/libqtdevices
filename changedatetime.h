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


#ifndef IMPOSTA_TIME_H
#define IMPOSTA_TIME_H

#include "page.h"

class BtDateEdit;
class BtTimeEdit;
class ChangeDate;
class PlatformDevice;


/*!
  \class ChangeTime
  \brief This is the class used to set time.

*/
class  ChangeTime : public Page
{
Q_OBJECT
public:
	ChangeTime();

public slots:
	virtual void showPage();

protected:
	void timerEvent(QTimerEvent *event);
	void showEvent(QShowEvent *event);
	void hideEvent(QHideEvent *event);

private slots:
	void acceptTime();

private:
	void startTimeUpdate();

private:
	BtTimeEdit *edit;
	ChangeDate *date; // next page
	int timer_id;
	PlatformDevice *dev;
};


/*!
  \class ChangeDate
  \brief This is the class used to set time.

*/
class  ChangeDate : public Page
{
Q_OBJECT
public:
	ChangeDate();

public slots:
	virtual void showPage();

private slots:
	void acceptDate();

private:
	BtDateEdit *edit;
	PlatformDevice *dev;
};


#endif // IMPOSTA_TIME_H
