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


#include "test_displaycontrol.h"
#include "test_functions.h"

#include "page.h"
#include "pagecontainer.h"
#include "displaycontrol.h"
#include "windowcontainer.h"
#include "audiostatemachine.h"
#include "btmain.h"

#include <QTest>


static void sleepSecs(int seconds)
{
	testSleep(seconds * 1000);
}


TestDisplayControl::TestDisplayControl()
{
	bt_global::status.alarm_clock_on = false;
	bt_global::status.calibrating = false;
	bt_global::status.vde_call_active = false;
	bt_global::status.check_password = false;

	bt_global::audio_states = new AudioStateMachine;
	parent_window = new QWidget;
	WindowContainer *w = new WindowContainer(100, 100, parent_window);
	page_container = new PageContainer(w);
	Page::setPageContainer(page_container);
	page_container->setContainerWindow(new Window);

	target_page = new Page;
	exit_page = target_page;
	target_window = new Window;
}

void TestDisplayControl::init()
{
	display = new DisplayControl;
	display->setPageContainer(page_container);
	display->freeze_time = 1;
	display->screensaver_time = 2;
}

void TestDisplayControl::testFreeze()
{
	// We can't set an interval smaller than this because the time elapsed
	// in the screensaver is calculated in seconds, using the time() unix function.
	int small_interval = 100;
	display->freeze_time = 2;
	display->startTime();

	testSleep(display->freeze_time * 1000 - small_interval);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);

	testSleep(small_interval);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_FREEZED);
}

void TestDisplayControl::testScreensaver()
{
	display->startTime();

	sleepSecs(display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_FREEZED);
	QVERIFY(display->screensaver == 0);

	sleepSecs(display->screensaver_time - display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_SCREENSAVER);
	QCOMPARE(display->screensaver->isRunning(), true);
}

void TestDisplayControl::testScreensaverNone()
{
	display->current_screensaver = ScreenSaver::NONE;
	display->startTime();

	sleepSecs(display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_FREEZED);

	sleepSecs(display->screensaver_time - display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_FREEZED);
}

void TestDisplayControl::testForcedOperativeMode()
{
	display->forceOperativeMode(true);
	display->startTime();

	sleepSecs(display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);
}

void TestDisplayControl::testVdeCallActive()
{
	bt_global::status.vde_call_active = true;
	display->startTime();

	sleepSecs(display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);

	bt_global::status.vde_call_active = false;
}

void TestDisplayControl::testCalibrationActive()
{
	bt_global::status.calibrating = true;
	display->startTime();

	sleepSecs(display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);

	bt_global::status.calibrating = false;
}

void TestDisplayControl::testAlarmClockActive()
{
	bt_global::status.alarm_clock_on = true;
	display->startTime();

	sleepSecs(display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);

	bt_global::status.alarm_clock_on = false;
}

void TestDisplayControl::testMakeActive()
{
	display->freeze(true);
	QCOMPARE(display->current_state, DISPLAY_FREEZED);
	display->makeActive();
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);
}


void TestDisplayControl::testMakeActivePassword()
{
	bt_global::status.check_password = true;
	display->freeze(true);
	QCOMPARE(display->current_state, DISPLAY_FREEZED);
	display->makeActive();
	QCOMPARE(display->current_state, DISPLAY_FREEZED);
	bt_global::status.check_password = false;
}

void TestDisplayControl::testMakeActiveScreensaver()
{
	display->startTime();

	sleepSecs(display->screensaver_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_SCREENSAVER);
	QCOMPARE(display->screensaver->isRunning(), true);

	display->makeActive();
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);
	QCOMPARE(display->screensaver->isRunning(), false);
}

