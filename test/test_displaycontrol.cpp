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
#include "pagestack.h"

#include <QTest>
#include <QSignalSpy>


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
	bt_global::page_stack.setHomePage(new Page);

	qRegisterMetaType<Page*>("Page*");
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
	display->screenoff_time = 3;

	// Reset the audio state machine states
	bt_global::audio_states->deleteLater();
	bt_global::audio_states = new AudioStateMachine;
	// Reset the page stack
	bt_global::page_stack.clear();
}

void TestDisplayControl::testFreeze()
{
	// We can't set an interval smaller than this because the time elapsed
	// in the screensaver is calculated in seconds, using the time() unix function.
	int small_interval = 200;
	QSignalSpy spy(display, SIGNAL(freezed()));
	display->freeze_time = 2;
	display->startTime();

	testSleep(display->freeze_time * 1000 - small_interval);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);
	QCOMPARE(spy.count(), 0);

	testSleep(small_interval);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_FREEZED);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::IDLE);
	QCOMPARE(spy.count(), 1);
}

void TestDisplayControl::testScreensaver()
{
	QSignalSpy spy(display, SIGNAL(startscreensaver(Page*)));
	display->startTime();

	sleepSecs(display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_FREEZED);
	QVERIFY(display->screensaver == 0);
	QCOMPARE(spy.count(), 0);

	sleepSecs(display->screensaver_time - display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_SCREENSAVER);
	QCOMPARE(display->screensaver->isRunning(), true);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::SCREENSAVER);
	QCOMPARE(spy.count(), 1);
}

void TestDisplayControl::testScreensaverNone()
{
	QSignalSpy spy(display, SIGNAL(startscreensaver(Page*)));
	display->current_screensaver = ScreenSaver::NONE;
	display->startTime();

	sleepSecs(display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_FREEZED);

	sleepSecs(display->screensaver_time - display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	// display can be in freeze or off mode depending on the screenoff var value.
	QVERIFY(display->current_state != DISPLAY_SCREENSAVER);
	QCOMPARE(spy.count(), display->current_state == DISPLAY_FREEZED ? 0 : 1);
}

void TestDisplayControl::testForcedOperativeMode()
{
	QSignalSpy spy(display, SIGNAL(freezed()));
	display->forceOperativeMode(true);
	display->startTime();

	sleepSecs(display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::IDLE);
	QCOMPARE(spy.count(), 0);
}

void TestDisplayControl::testScreensaverPostForcedOperativeMode()
{
	QSignalSpy spy(display, SIGNAL(startscreensaver(Page*)));
	display->forceOperativeMode(true);
	display->startTime();

	sleepSecs(display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);
	display->forceOperativeMode(false);
	QCOMPARE(spy.count(), 0);

	sleepSecs(display->screensaver_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_SCREENSAVER);
	QCOMPARE(display->screensaver->isRunning(), true);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::SCREENSAVER);
	QCOMPARE(spy.count(), 1);
}

void TestDisplayControl::testVdeCallActive()
{
	bt_global::status.vde_call_active = true;
	display->startTime();

	sleepSecs(display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::IDLE);

	bt_global::status.vde_call_active = false;
}

void TestDisplayControl::testCalibrationActive()
{
	bt_global::status.calibrating = true;
	display->startTime();

	sleepSecs(display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::IDLE);

	bt_global::status.calibrating = false;
}

void TestDisplayControl::testAlarmClockActive()
{
	bt_global::status.alarm_clock_on = true;
	display->startTime();

	sleepSecs(display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::IDLE);

	bt_global::status.alarm_clock_on = false;
}

void TestDisplayControl::testScreenOffNoScreeensaver()
{
	QSignalSpy spy(display, SIGNAL(startscreensaver(Page*)));
	display->current_screensaver = ScreenSaver::NONE;
	display->startTime();

	sleepSecs(display->screenoff_time - display->screensaver_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_OFF);
	QVERIFY(display->screensaver == 0);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::SCREENSAVER);
	QCOMPARE(spy.count(), 1);
}

void TestDisplayControl::testScreenOff()
{
	QSignalSpy spy(display, SIGNAL(startscreensaver(Page*)));
	display->screenoff_time = 1;
	display->startTime();

	sleepSecs(display->screensaver_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_SCREENSAVER);
	QCOMPARE(spy.count(), 1);
	spy.clear();

	sleepSecs(display->screenoff_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_OFF);
	QCOMPARE(display->screensaver->isRunning(), false);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::SCREENSAVER);
	QCOMPARE(spy.count(), 0);
}

void TestDisplayControl::testMakeActive()
{
	QSignalSpy spy(display, SIGNAL(unfreezed()));
	display->freeze(true);
	QCOMPARE(display->current_state, DISPLAY_FREEZED);
	QCOMPARE(spy.count(), 0);

	display->makeActive();
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);
	QCOMPARE(spy.count(), 1);
}


void TestDisplayControl::testMakeActivePassword()
{
	QSignalSpy spy(display, SIGNAL(unfreezed()));
	bt_global::status.check_password = true;
	display->freeze(true);
	QCOMPARE(display->current_state, DISPLAY_FREEZED);
	QCOMPARE(spy.count(), 0);

	display->makeActive();
	QCOMPARE(display->current_state, DISPLAY_FREEZED);
	bt_global::status.check_password = false;
	QCOMPARE(spy.count(), 0);
}

void TestDisplayControl::testMakeActivePostScreensaver()
{
	QSignalSpy spy(display, SIGNAL(stopscreensaver()));
	display->startTime();

	sleepSecs(display->screensaver_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_SCREENSAVER);
	QCOMPARE(display->screensaver->isRunning(), true);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::SCREENSAVER);
	QCOMPARE(spy.count(), 0);

	display->makeActive();
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);
	QCOMPARE(display->screensaver->isRunning(), false);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::IDLE);
	QCOMPARE(spy.count(), 1);
}

void TestDisplayControl::testMakeActivePostScreensaverPassword()
{
	bt_global::status.check_password = true;
	QSignalSpy spy(display, SIGNAL(stopscreensaver()));
	display->startTime();

	sleepSecs(display->screensaver_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_SCREENSAVER);
	QCOMPARE(display->screensaver->isRunning(), true);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::SCREENSAVER);
	QCOMPARE(spy.count(), 0);

	display->makeActive();
	QCOMPARE(display->current_state, DISPLAY_FREEZED);
	QCOMPARE(display->screensaver->isRunning(), false);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::IDLE);
	QCOMPARE(spy.count(), 1);
	bt_global::status.check_password = false;
}

void TestDisplayControl::testMakeActivePostScreenOff()
{
	display->startTime();

	sleepSecs(display->screenoff_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	display->checkScreensaver(target_page, target_window, exit_page);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_OFF);
	QCOMPARE(display->screensaver->isRunning(), false);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::SCREENSAVER);

	display->makeActive();
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);
	QCOMPARE(display->screensaver->isRunning(), false);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::IDLE);
}

void TestDisplayControl::testScreensaverPostMakeActive()
{
	QSignalSpy spy(display, SIGNAL(startscreensaver(Page*)));
	display->startTime();

	sleepSecs(display->screensaver_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_SCREENSAVER);
	QCOMPARE(spy.count(), 1);
	spy.clear();

	display->makeActive();
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);
	QCOMPARE(spy.count(), 0);

	sleepSecs(display->screensaver_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_SCREENSAVER);
	QCOMPARE(display->screensaver->isRunning(), true);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::SCREENSAVER);
	QCOMPARE(spy.count(), 1);
}

void TestDisplayControl::testScreenOffPostMakeActive()
{
	QSignalSpy spy(display, SIGNAL(startscreensaver(Page*)));
	display->startTime();

	display->freeze(true);
	sleepSecs(display->freeze_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_FREEZED);

	display->makeActive();
	QCOMPARE(display->current_state, DISPLAY_OPERATIVE);
	QCOMPARE(spy.count(), 0);

	sleepSecs(display->screenoff_time);
	display->checkScreensaver(target_page, target_window, exit_page);
	display->checkScreensaver(target_page, target_window, exit_page);
	display->checkScreensaver(target_page, target_window, exit_page);
	QCOMPARE(display->current_state, DISPLAY_OFF);
	QCOMPARE(display->screensaver->isRunning(), false);
	QVERIFY(bt_global::audio_states->currentState() == AudioStates::SCREENSAVER);
	QCOMPARE(spy.count(), 1);
}

void TestDisplayControl::testScreensaverExitPage()
{
	// Without page default
	Page *target = page_container->currentPage(); // the homepage
	Page *exit = new Page;
	page_container->setCurrentPage(exit);

	QSignalSpy spy(display, SIGNAL(unrollPages()));
	display->startTime();
	sleepSecs(display->screensaver_time);
	display->checkScreensaver(target, target_window, exit);
	display->checkScreensaver(target, target_window, exit);
	QCOMPARE(display->current_state, DISPLAY_SCREENSAVER);
	QCOMPARE(spy.count(), 0);
	display->makeActive();
	QCOMPARE(page_container->currentPage(), exit);
}

void TestDisplayControl::testScreensaverExitPageDefault()
{
	// With page default
	Page *target = new Page;
	Page *exit = target;
	Page *other = new Page;
	page_container->setCurrentPage(other);

	QSignalSpy spy(display, SIGNAL(unrollPages()));
	display->startTime();
	sleepSecs(display->screensaver_time);
	display->checkScreensaver(target, target_window, exit);
	display->checkScreensaver(target, target_window, exit);
	QCOMPARE(display->current_state, DISPLAY_SCREENSAVER);
	QCOMPARE(spy.count(), 1);
	display->makeActive();
	QCOMPARE(page_container->currentPage(), exit);
}
