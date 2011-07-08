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


#ifndef _ALARM_PAGE_H_
#define _ALARM_PAGE_H_

#include "page.h"

class QLabel;
class QDateTime;


/*!
	\ingroup Antintrusion
	\brief The popup page that represents a burglar alarm.
*/
class AlarmPage : public Page
{
Q_OBJECT
public:

	AlarmPage(const QString &icon, const QString &description, const QString &zone, int id);
	virtual int sectionId() const;
	int alarmId() const;

public slots:
	virtual void showPage();
	virtual void cleanUp();

signals:
#ifdef LAYOUT_TS_3_5
	void nextAlarm();
	void prevAlarm();
#endif
	void deleteAlarm();
	void showHomePage();
	void showAlarmList();

private:
	QLabel *image;
	QLabel *description;
	int alarm_id;
};

#endif // _ALARM_PAGE_H_
