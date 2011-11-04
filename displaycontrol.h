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


#ifndef DISPLAYCONTROL_H
#define DISPLAYCONTROL_H

#include "screensaver.h" // ScreenSaver::Type
#include "transitionwidget.h" // TransitionWidget::Type

#include <QMap>

class PageContainer;
class QEvent;
class TransitionWidget;


/*!
	\ingroup Core
	\brief Defines the standard levels of the brightness.
*/
enum BrightnessLevel
{
	BRIGHTNESS_OFF,
	BRIGHTNESS_LOW,
	BRIGHTNESS_NORMAL,
	BRIGHTNESS_HIGH
};


/*!
	\ingroup Core
	\brief The status of the display.
*/
enum DisplayStatus
{
	DISPLAY_OPERATIVE,    /*!< The normal status, when the user interact with the screen. */
	DISPLAY_FREEZED,      /*!< The 'freeze' mode, when the touchscreen is disabled until a touch is performed. */
	DISPLAY_SCREENSAVER,  /*!< During the screensaver. */
	DISPLAY_OFF           /*!< The display is switch off. */
};


/*!
	\ingroup Core
	\brief The global controller of the properties of the display.

	Controls the status of the display, setting the status using the setState() method.
	Normally, the freezed status enter after a while that the screen is not touched,
	but you can prevent this setting the forceOperativeMode(). The method
	isForcedOperativeMode() can be used to know if the screen was forced.

	There are also methods to set the level of the brightness, during the operative
	state or one of the inative states, and methods to set or retrieve the current
	screensaver type.
	Finally, the method setDirectScreenAccess() can be used when an external
	program has to write directly to the screen (to ensure that only one program
	at a given time do that) while the methods isDirectScreenAccess() and the
	signals directScreenAccessStarted(), directScreenAccessStopped() can be
	used to know if any program had request the control of the screen.
*/
class DisplayControl : public QObject
{
Q_OBJECT
friend class BtMain;
friend class TestDisplayControl;
public:
	DisplayControl();
	~DisplayControl();

	void setPageContainer(PageContainer *c);

	/*!
	  \brief Change the brightness without changing the display state.
	*/
	void changeBrightness(DisplayStatus status);

	/*!
		\brief Set the brightness to be used in operative mode.

		Valid values are from 1 (min) to 10 (max).
		\sa operativeBrightness()
	*/
	void setOperativeBrightness(int brightness);

	/*!
		\brief Retrieve the brightess used in the operative mode.
		\sa setOperativeBrightness()
	*/
	int operativeBrightness();

	/*!
		\brief Set the brightness used when the touchscreen is inactive (freezed or screensaver)
		\sa inactiveBrightness()
	*/

	void setInactiveBrightness(BrightnessLevel level);

	/*!
		\brief Retrieve the brightness used when the touchscreen is inactive.
		\sa setInactiveBrightness()
	*/
	BrightnessLevel inactiveBrightness();

	/*!
		\brief Set the screensaver type.
		\sa currentScreenSaver()
	*/
	void setScreenSaver(ScreenSaver::Type t);

	/*!
		\brief Retrieve the current screensaver type.
		\sa setScreenSaver()
	*/
	ScreenSaver::Type currentScreenSaver();

	/*!
		\brief Force the display (or remove the forcing) to run in the operative mode.
		\sa DisplayStatus, isForcedOperativeMode()
	*/
	void forceOperativeMode(bool enable);

	/*!
		\brief Return true if the display is forced to run in the operative mode.
		\sa forceOperativeMode(), DisplayStatus
	*/
	bool isForcedOperativeMode();

	/*!
		\brief Set that some process is writing directly to the screen.
		This method is used to notify watchers that some process is writing
		to the screen; typically there can only be one such program active at
		a give time.
		\sa isDirectScreenAccess(), directScreenAccessStarted(), directScreenAccessStopped()
	*/
	void setDirectScreenAccess(bool status);

	/*!
		\brief Return true if there is some program that is writing to the screen.
		\sa directScreenAccessStarted(), directScreenAccessStopped(), setDirectScreenAccess()
	*/
	bool isDirectScreenAccess();

	// set the freezze and blank screen timeouts in seconds (the screensaver starts 3 seconds after the freeze)
	void setScreenSaverTimeouts(int freeze, int blank_screen);

	/*!
		\brief Restore the 'operative' status
		This method should be used when an action that change the current page
		is triggered by a spontaneous event and stops the screensaver if running
		or exit from the blank screen (keeping the screen frozen if password
		protection is active).
	*/
	void makeActive();

	/// Freeze or unfreeze the application
	void freeze(bool freeze);

	void checkScreensaver(Page *target_page, Window *target_window, Page *exit_page);

	void startTime();

	/// Set the state of the display
	void setState(DisplayStatus status);

	/*!
		\brief Install the widget argument as the one which manages the transition effects.
	*/
	void installTransitionEffects(TransitionWidget::Type t);

	/// Return the current transition effects installed
	TransitionWidget::Type currentTransitionEffects();

	/// Sets/unsets the locked status of the screen
	void setScreenLocked(bool is_locked);

signals:
	/*!
		\brief Notifies that some program is writing directly to the screen.
		\sa directScreenAccessStopped(), setDirectScreenAccess(), isDirectScreenAccess()
	*/
	void directScreenAccessStarted();

	/*!
		\brief Notifies that some program is not anymore writing directly to the screen.
		\sa directScreenAccessStarted(), setDirectScreenAccess(), isDirectScreenAccess()
	*/
	void directScreenAccessStopped();

	void startscreensaver(Page*);
	void stopscreensaver();

	void unfreezed();
	void freezed();

	void unrollPages();

	/*!
		\brief Requests to unlock the screen
		The signal is emitted when exiting the DISPLAY_FREEZED status with the screen
		locked.  The signal must be handled by showing some sort of password prompt,
		and calling \c setScreenLocked(false) if the user enters the correct password.
	*/
	void unlockScreen();

protected:
	virtual bool eventFilter(QObject *obj, QEvent *ev);

private:
	void updateBrightnessData();
	bool canScreensaverStart();

	// take into account selected screensaver when computing freeze/blank screen times
	//
	// if the user is idle for this number of seconds, freeze the screen
	int freezeTime();
	// if the user is idle for this number of seconds, start the screen saver
	int screensaverTime();
	// if the user is idle for this number of seconds, turn off the screen
	int blankScreenTime();

	// called when we turn off the screen.
	void turnOff(Page *exit_page);

	void startScreensaver(Page *target_page, Window *target_window, Page *exit_page);

	struct DisplayData
	{
		int brightness;
		bool backlight;
	};

	QMap<DisplayStatus, DisplayData> data;
	BrightnessLevel inactive_brightness;
	ScreenSaver::Type current_screensaver;
	TransitionWidget::Type current_transition_effects;
	DisplayStatus current_state;
	bool forced_operative_mode;
	int operative_brightness, direct_screen_access;

	// the three values below are in seconds; screenoff_time can be 0
	// it must always be screensaver_time < screenoff_time
	int freeze_time;
	int screensaver_time;
	int screenoff_time;

	ScreenSaver *screensaver;
	bool frozen, locked;
	int last_event_time;
	PageContainer *page_container;
};

namespace bt_global { extern DisplayControl *display; }

#endif //DISPLAYCONTROL_H
