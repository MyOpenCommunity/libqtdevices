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


/**
 * This is the main class of the touchscreen interface. It loads the configuration,
 * establishes the connection with the openserver(s) and manage the global objects.
 */
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
	void freeze(bool);

	void setPwd(bool, QString);

	Window *homeWindow();
	TrayBar *trayBar();
	void showHomePage();
	Page *homePage();

	// set the screensaver and balcnk screen timeouts in seconds
	void setScreenSaverTimeouts(int screensaver_start, int blank_screen);

	// stop the screen saver and hide the password keypad but keep the
	// screen frozen if password protection is active
	void makeActiveAndFreeze();

	static bool isCalibrating();
	static void calibrationStarted();
	static void calibrationEnded();

	bool alarm_clock_on;

signals:
	void resettimer();
	void startscreensaver(Page*);

protected:
	virtual bool eventFilter(QObject *obj, QEvent *ev);

private slots:
	void init();
	void checkScreensaver();
	void testPwd();
	void waitBeforeInit();
	void connectionReady();
	void startGui();

private:
	QHash<int, QPair<Client*, Client*> > clients;
	QHash<int, Client*> monitors;

#if DEBUG
	Client *client_supervisor;
#endif
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

	// the four values below are in seconds; screenoff_time can be 0
	// it must always be freeze_time < screensaver_time < screenoff_time

	// if the user is idle for this number of seconds, freeze the screen
	int freeze_time;
	// if the user is idle for this number of seconds, start the screen saver
	int screensaver_time;
	// if the user is idle for this number of seconds, turn off the screen
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
