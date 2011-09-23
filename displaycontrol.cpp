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


#include "displaycontrol.h"
#include "hardware_functions.h" // setBrightnessLevel, setBacklight
#include "btmain.h" // bt_global::btmain, bt_global::status
#include "pagestack.h" // bt_global::page_stack
#include "pagecontainer.h"
#include "page.h"
#ifdef LAYOUT_TS_10
#include "audiostatemachine.h" // bt_global::audio_states
#endif

#include <QDebug>
#include <QEvent>
#include <QApplication>

#ifdef BT_HARDWARE_DM365
#define DELTA_OFF 8
#define DELTA_LOW 7
#define DELTA_NORMAL 6
#define DELTA_HIGH 0
#else
#define DELTA_OFF 8
#define DELTA_LOW 8
#define DELTA_NORMAL 5
#define DELTA_HIGH 1
#endif

static unsigned long now()
{
	return time(NULL);
}


DisplayControl::DisplayControl()
{
	forced_operative_mode = false;
	direct_screen_access = 0;

	freeze_time = 30;
	screensaver_time = 60;
	screenoff_time = 120;

	page_container = 0;
	screensaver = 0;
	last_event_time = 0;
	frozen = false;

#ifdef BT_HARDWARE_PXA270
	operative_brightness = 1; // a low brightness for the touch 10''
	setInactiveBrightness(BRIGHTNESS_LOW);
#else
	operative_brightness = 9; // an high brightness for the touch 3.5''
	setInactiveBrightness(BRIGHTNESS_NORMAL);
#endif

	setState(DISPLAY_OPERATIVE);
	current_screensaver = ScreenSaver::LINES;
	current_transition_effects = TransitionWidget::NONE;
}

DisplayControl::~DisplayControl()
{
	delete screensaver;
}

void DisplayControl::setPageContainer(PageContainer *c)
{
	page_container = c;
}

void DisplayControl::startTime()
{
	// start the screensaver countdown
	last_event_time = now();
}

void DisplayControl::updateBrightnessData()
{
	switch (inactive_brightness)
	{
	case BRIGHTNESS_OFF:
		data[DISPLAY_FREEZED].brightness = qMax(operative_brightness - DELTA_OFF, 1);
		data[DISPLAY_FREEZED].backlight = false;
		break;

	case BRIGHTNESS_LOW:
		data[DISPLAY_FREEZED].brightness = qMax(operative_brightness - DELTA_LOW, 1);
		data[DISPLAY_FREEZED].backlight = true;
		break;

	case BRIGHTNESS_NORMAL:
		data[DISPLAY_FREEZED].brightness = qMax(operative_brightness - DELTA_NORMAL, 1);
		data[DISPLAY_FREEZED].backlight = true;
		break;

	case BRIGHTNESS_HIGH:
		data[DISPLAY_FREEZED].brightness = qMax(operative_brightness - DELTA_HIGH, 1);
		data[DISPLAY_FREEZED].backlight = true;
		break;

	default:
		qFatal("Unknown level for inactive brightness");
	}

	// The screensaver status has the same values than the freezed one.
	data[DISPLAY_SCREENSAVER].brightness = data[DISPLAY_FREEZED].brightness;
	data[DISPLAY_SCREENSAVER].backlight = data[DISPLAY_FREEZED].backlight;

	// Off and operative status have the same values for all levels
	data[DISPLAY_OFF].brightness = operative_brightness;
	data[DISPLAY_OFF].backlight = false;

	data[DISPLAY_OPERATIVE].brightness = operative_brightness;
	data[DISPLAY_OPERATIVE].backlight = true;
}

void DisplayControl::setInactiveBrightness(BrightnessLevel level)
{
	inactive_brightness = level;
	updateBrightnessData();
}

void DisplayControl::setOperativeBrightness(int brightness)
{
	if (brightness > 10 || brightness < 1)
	{
		qWarning() << "Operative brightness out of range 1 - 10";
		brightness = qMax(qMin(brightness, 10), 1);
	}

	operative_brightness = brightness;
	updateBrightnessData();
	setBrightnessLevel(data[current_state].brightness);
}

int DisplayControl::operativeBrightness()
{
	return operative_brightness;
}

BrightnessLevel DisplayControl::inactiveBrightness()
{
	return inactive_brightness;
}

void DisplayControl::forceOperativeMode(bool enable)
{
	forced_operative_mode = enable;
	if (enable)
		setState(DISPLAY_OPERATIVE);
}

bool DisplayControl::isForcedOperativeMode()
{
	return forced_operative_mode;
}

void DisplayControl::setDirectScreenAccess(bool status)
{
	if (!status && !direct_screen_access)
		return;
	direct_screen_access += status ? 1 : -1;
	if (direct_screen_access == 1 && status)
		emit directScreenAccessStarted();
	else if (direct_screen_access == 0)
		emit directScreenAccessStopped();
}

bool DisplayControl::isDirectScreenAccess()
{
	return direct_screen_access != 0;
}

void DisplayControl::changeBrightness(DisplayStatus status)
{
	setBacklight(data[status].backlight);
	setBrightnessLevel(data[status].brightness);
}

void DisplayControl::setState(DisplayStatus status)
{
	if (!forced_operative_mode || (forced_operative_mode && status == DISPLAY_OPERATIVE))
		changeBrightness(status);

	current_state = status;
}

void DisplayControl::setScreenSaver(ScreenSaver::Type t)
{
	current_screensaver = t;
}

ScreenSaver::Type DisplayControl::currentScreenSaver()
{
	return current_screensaver;
}

bool DisplayControl::eventFilter(QObject *obj, QEvent *ev)
{
	// Discard the mouse press and mouse double click
	if (ev->type() == QEvent::MouseButtonPress || ev->type() == QEvent::MouseButtonDblClick ||
		ev->type() == QEvent::MouseMove || ev->type() == QEvent::Enter || ev->type() == QEvent::Leave)
		return true;

	if (ev->type() != QEvent::MouseButtonRelease)
		return false;

	freeze(false);
	return true;
}

void DisplayControl::setScreenSaverTimeouts(int screensaver_start, int blank_screen)
{
	qDebug() << "Screensaver time" << screensaver_start << "blank screen" << blank_screen;

	screenoff_time = blank_screen;
	screensaver_time = screensaver_start;
}

int DisplayControl::freezeTime()
{
	if (blankScreenTime())
		return qMin(qMin(freeze_time, screensaverTime()), blankScreenTime());
	else
		return qMin(freeze_time, screensaverTime());
}

int DisplayControl::screensaverTime()
{
	return screensaver_time;
}

int DisplayControl::blankScreenTime()
{
	if (screenoff_time == 0)
		return 0;
	else if (current_screensaver == ScreenSaver::NONE)
		return screenoff_time - screensaver_time;
	else
		return screenoff_time;
}

bool DisplayControl::canScreensaverStart()
{
	return !(bt_global::status.alarm_clock_on || bt_global::status.calibrating ||
			 bt_global::status.vde_call_active);
}

TransitionWidget::Type DisplayControl::currentTransitionEffects()
{
	return current_transition_effects;
}

void DisplayControl::installTransitionEffects(TransitionWidget::Type t)
{
	Q_ASSERT_X(page_container, "DisplayControl::installTransitionEffects", "Page container not set!");

	if (t != current_transition_effects)
	{
		TransitionWidget *w = 0;
		switch (t)
		{
		case TransitionWidget::BLENDING:
			w = new BlendingTransition;
			break;
		case TransitionWidget::MOSAIC:
			w = new MosaicTransition;
			break;
		case TransitionWidget::ENLARGE:
			w = new EnlargeTransition;
			break;
		case TransitionWidget::NONE:
			break;
		default:
			Q_ASSERT_X(false, "DisplayControl::installTransitionEffects",
				qPrintable(QString("Unknown transition effects type: %1").arg(t)));
		}

		page_container->installTransitionWidget(w);
		current_transition_effects = t;
	}
}

void DisplayControl::turnOff()
{
	qDebug() << "Turning screen off";
	// the stopscreensaver() event is emitted when the user clicks on screen
	if (screensaver && screensaver->isRunning())
		screensaver->stop();
	else
	{
		// Some pages do things when the screensaver starts. For example the
		// RDS radio stop the RDS updates. We want the same behaviour when
		// the screen turn off.
		emit startscreensaver(page_container->currentPage());
	}
#ifdef LAYOUT_TS_10
	if (current_state != DISPLAY_SCREENSAVER)
		bt_global::audio_states->toState(AudioStates::SCREENSAVER);
#endif
	setState(DISPLAY_OFF);
}

void DisplayControl::startScreensaver(Page *target_page, Window *target_window, Page *exit_page)
{
	Page *current_page = page_container->currentPage();
	if (current_state == DISPLAY_OPERATIVE && current_page != target_page)
	{
		target_page->showPage();
	}

	if (current_state == DISPLAY_FREEZED)
	{
		if (screensaver && screensaver->type() != current_screensaver)
		{
			delete screensaver;
			screensaver = 0;
		}

		if (!screensaver)
			screensaver = getScreenSaver(current_screensaver);

		page_container->blockTransitions(true);

		if (current_page != exit_page)
			emit unrollPages();

		target_page->showPage();
		target_window->showWindow();

		// this makes the screen saver go back to exit_page when exited (always required
		// because the target page is removed from the stack when calling screensaver->stop())
		bt_global::page_stack.currentPageChanged(exit_page);

		page_container->blockTransitions(false);

		qDebug() << "start screensaver:" << current_screensaver << "on:" << target_page << target_window;
		screensaver->start(target_window);
		emit startscreensaver(exit_page);

		setState(DISPLAY_SCREENSAVER);
#ifdef LAYOUT_TS_10
		bt_global::audio_states->toState(AudioStates::SCREENSAVER);
#endif
	}
}

void DisplayControl::checkScreensaver(Page *target_page, Window *target_window, Page *exit_page)
{
	Q_ASSERT_X(page_container, "DisplayControl::checkScreensaver", "Page container not set!");
	if (isForcedOperativeMode())
		return;

	if (!canScreensaverStart())
		return;

#ifdef BT_HARDWARE_PXA255
	// When the brightness is set to off in the old hardware the display
	// is not really off, so it is required to use a screensaver to protect
	// the display, even if the screensaver is not visible.
	if (inactiveBrightness() == BRIGHTNESS_OFF)
		current_screensaver = ScreenSaver::LINES;
#endif
	int time_press = getTimePress();
	int time = qMin(time_press, abs(int(now() - last_event_time)));
	int blank_time = blankScreenTime();

	if (blank_time != 0 &&
		((current_state == DISPLAY_SCREENSAVER && time >= blank_time) ||
		 (current_state == DISPLAY_FREEZED && current_screensaver == ScreenSaver::NONE && time >= blank_time)))
	{
		turnOff();
	}
	else if (time >= freezeTime() && getBacklight() && !frozen)
	{
		freeze(true);
	}
	else if (time >= screensaverTime() && current_screensaver != ScreenSaver::NONE)
	{
		startScreensaver(target_page, target_window, exit_page);
	}
}

void DisplayControl::freeze(bool b)
{
	qDebug("DisplayControl::freeze(%d)", b);
	frozen = b;

	if (!frozen)
	{
		// set last_event_time first to avoid re-freezing if one of the actions
		// below re-enters the event loop
		last_event_time = now();

		// in this case the screeensaver is not running, to reduce power consumption,
		// but the state is as if it were
		if (current_state == DISPLAY_OFF)
		{
#ifdef LAYOUT_TS_10
			bt_global::audio_states->removeState(AudioStates::SCREENSAVER);
#endif
			emit stopscreensaver();
		}

		if (screensaver && screensaver->isRunning())
		{
#ifdef LAYOUT_TS_10
			bt_global::audio_states->removeState(AudioStates::SCREENSAVER);
#endif
			emit stopscreensaver();
			screensaver->stop();
		}

		setState(DISPLAY_OPERATIVE);
		qApp->removeEventFilter(bt_global::display);
		emit unfreezed();
	}
	else
	{
		if (current_state == DISPLAY_OPERATIVE)
			qApp->installEventFilter(bt_global::display);
		else if (current_state == DISPLAY_SCREENSAVER)
		{
			if (screensaver && screensaver->isRunning())
			{
	#ifdef LAYOUT_TS_10
				bt_global::audio_states->removeState(AudioStates::SCREENSAVER);
	#endif
				emit stopscreensaver();
				screensaver->stop();
			}
		}
		if (current_state != DISPLAY_FREEZED)
		{
			setState(DISPLAY_FREEZED);
			emit freezed();
		}
	}
}

void DisplayControl::makeActive()
{
	qDebug() << "DisplayControl::makeActive";
	last_event_time = now();

	if (current_state != DISPLAY_OPERATIVE)
	{
		// If the user has protected its touch with the password, we have to do
		// all the things to show the event that calls the makeActive leaving
		// the tounch in the freeze state so when the user clicks on the screen
		// the keypad window (for inserting the password) is shown.
		freeze(bt_global::status.check_password == true);
	}
}

// The global definition of display
DisplayControl *bt_global::display = 0;
