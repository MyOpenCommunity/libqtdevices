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

#ifndef TEST_DISPLAYCONTROL_H
#define TEST_DISPLAYCONTROL_H

#include <QObject>

class DisplayControl;
class PageContainer;
class Page;
class Window;


class TestDisplayControl : public QObject
{
Q_OBJECT
public:
	TestDisplayControl();

private slots:
	void init();

	void testFreeze();
	void testScreensaver();
	void testScreensaverNone();
	void testForcedOperativeMode();
	void testScreensaverPostForcedOperativeMode();
	void testVdeCallActive();
	void testCalibrationActive();
	void testAlarmClockActive();
	void testScreenOffNoScreeensaver();
	void testScreenOff();
	void testMakeActive();
	void testMakeActivePassword();
	void testMakeActivePostScreensaver();
	void testScreensaverPostMakeActive();
	void testScreenOffPostMakeActive();

private:
	DisplayControl *display;
	QWidget *parent_window;
	PageContainer *page_container;
	Page *target_page, *exit_page;
	Window *target_window;
};

#endif
