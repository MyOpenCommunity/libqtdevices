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
#include "generic_functions.h" // setCfgValue
#include "hardware_functions.h" // setBrightnessLevel, setBacklight
#include "btmain.h" // bt_global::btmain
#include "keypad.h" // KeypadWindow
#include "pagestack.h" // bt_global::page_stack

#include <QEvent>


DisplayControl::DisplayControl()
{
	forced_operative_mode = false;
	direct_screen_access = 0;
	check_password = false;
	password_keypad = 0;

#ifdef BT_HARDWARE_TS_10
	operative_brightness = 1; // a low brightness for the touch 10''
	setInactiveBrightness(BRIGHTNESS_LOW);
#else
	operative_brightness = 9; // an high brightness for the touch 3.5''
	setInactiveBrightness(BRIGHTNESS_NORMAL);
#endif

	setState(DISPLAY_OPERATIVE);
}

void DisplayControl::updateBrightnessData()
{
	switch (inactive_brightness)
	{
	case BRIGHTNESS_OFF:
		data[DISPLAY_FREEZED].brightness = qMax(operative_brightness - 8, 1);
		data[DISPLAY_FREEZED].backlight = false;
		break;

	case BRIGHTNESS_LOW:
		data[DISPLAY_FREEZED].brightness = qMax(operative_brightness - 8, 1);
		data[DISPLAY_FREEZED].backlight = true;
		break;

	case BRIGHTNESS_NORMAL:
		data[DISPLAY_FREEZED].brightness = qMax(operative_brightness - 5, 1);
		data[DISPLAY_FREEZED].backlight = true;
		break;

	case BRIGHTNESS_HIGH:
		data[DISPLAY_FREEZED].brightness = qMax(operative_brightness - 1, 1);
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

#ifdef CONFIG_TS_3_5
	setCfgValue("brightness/level", level, DISPLAY);
#endif
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

void DisplayControl::setState(DisplayStatus status)
{
	if (!forced_operative_mode || (forced_operative_mode && status == DISPLAY_OPERATIVE))
	{
		setBacklight(data[status].backlight);
		setBrightnessLevel(data[status].brightness);
	}

	current_state = status;
}

DisplayStatus DisplayControl::currentState()
{
	return current_state;
}

void DisplayControl::setScreenSaver(ScreenSaver::Type t)
{
	// TODO find the correct place to save the information
	// in TS 10'' it's saved inside ScreensaverPage, probably it can be done also on TS 3.5''
#ifdef CONFIG_TS_3_5
	setCfgValue("screensaver/type", t, DISPLAY);
#endif
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

	bt_global::btmain->freeze(false);
	return true;
}

void DisplayControl::setPassword(bool enable, QString pwd)
{
	check_password = enable;
	password = pwd;
	qDebug() << "DisplayControl::setPassword new password:" << pwd << "check:" << enable;
}

void DisplayControl::testPassword()
{
	QString text = password_keypad->getText();
	if (!text.isEmpty())
	{
		if (text != password)
		{
			password_keypad->resetText();
			qDebug() << "DisplayControl::testPassword the input text" << text
				<< "doesn't match the password" << password;
		}
		else
		{
			qDebug() << "DisplayControl::testPassword password ok";
			bt_global::page_stack.closeWindow(password_keypad);
			password_keypad->disconnect();
			password_keypad->deleteLater();
			password_keypad = 0;
		}
	}
}

void DisplayControl::showPasswordKeypad()
{
	if (!password_keypad)
	{
		password_keypad = new KeypadWindow(Keypad::HIDDEN);
		connect(password_keypad, SIGNAL(Closed()), SLOT(testPassword()));
	}
	bt_global::page_stack.showKeypad(password_keypad);
	password_keypad->showWindow();
}

// The global definition of display
DisplayControl *bt_global::display = 0;
