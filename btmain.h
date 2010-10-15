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


#ifndef BTMAIN_H
#define BTMAIN_H

#include <QObject>
#include <QHash>
#include <QPair>
#include <QDateTime>

#include "device.h" // Clients

class Version;
class HomePage;
class Client;
class KeypadWindow;
class ScreenSaver;
class Page;
class PageContainer;
class WindowContainer;
class Window;
class TrayBar;

class QPixmap;
class QString;
class QTimer;
class QTime;
class QSocketNotifier;



class SignalsHandler : public QObject
{
Q_OBJECT
public:
	SignalsHandler();
	~SignalsHandler();

	static void signalHandler(int signal_number);

signals:
	void signalReceived(int signal_number);

private slots:
	void handleSignal();

private:
	static long parent_pid;
	static int signalfd[2];
	QSocketNotifier *snSignal;
};


class BtMain : public QObject
{
Q_OBJECT
friend Page *getPage(int id);
friend class HomePage;

public:
	BtMain(int openserver_reconnection_time);
	~BtMain();
	Version *version;

	void resetTimer();
	/// Freeze or unfreeze the application
	void freeze(bool freeze);

	void setPassword(bool enable, QString password);

	Window *homeWindow();
	TrayBar *trayBar();
	void showHomePage();
	Page *homePage();

	// set the screensaver and blank screen timeouts in seconds
	void setScreenSaverTimeouts(int screensaver_start, int blank_screen);

	// stop the screen saver but keep the screen frozen if password protection is active.
	void makeActive();

	static bool isCalibrating();
	static void calibrationStarted();
	static void calibrationEnded();

	bool alarm_clock_on;
	bool vde_call_active;

signals:
	void resettimer();
	void startscreensaver(Page*);
	void stopscreensaver();

protected:
	virtual bool eventFilter(QObject *obj, QEvent *ev);

private slots:
	void init();
	void checkScreensaver();
	void testPassword();
	void waitBeforeInit();
	void connectionReady();
	void startGui();
	void handleSignal(int signal_number);

private:
	QHash<int, Clients> clients;
	QHash<int, Client*> monitors;

	QHash<int, Page*> page_list;
	QTime *boot_time;
	HomePage *Home;
	Page *pagDefault;

	QTimer *screensaver_timer;
	QDateTime last_date_time;
	QString pwd;
	bool pwdOn, alreadyCalibrated;
	KeypadWindow *passwordKeypad;
	bool frozen;
	int last_event_time;

	// take into account selected screensaver when computing freeze/blank screen times
	//
	// if the user is idle for this number of seconds, freeze the screen
	int freezeTime();
	// if the user is idle for this number of seconds, start the screen saver
	int screensaverTime();
	// if the user is idle for this number of seconds, turn off the screen
	int blankScreenTime();

	// the three values below are in seconds; screenoff_time can be 0
	// it must always be screensaver_time < screenoff_time

	int freeze_time;
	int screensaver_time;
	int screenoff_time;

	static bool calibrating;
	ScreenSaver *screensaver;
	PageContainer *page_container;
	WindowContainer *window_container;

	// A flag that is set when the client monitor socket is ready
	bool monitor_ready;
	// A flag that is set when the configuration has loaded
	bool config_loaded;

	void myMain();
	/// Load the main configuration
	void loadConfiguration();

	// Load the global configuration (the item in "generale" section of xml config file)
	void loadGlobalConfig();
	// Unroll all the pages until homepage
	void unrollPages();
};

namespace bt_global { extern BtMain *btmain; }

#endif// BTMAIN_H
