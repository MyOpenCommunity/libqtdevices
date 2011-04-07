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

#include "frame_classes.h" // Clients

class Version;
class HomePage;
class Client;
class Page;
class PageContainer;
class WindowContainer;
class Window;
class TrayBar;
class KeypadWindow;

class QPixmap;
class QString;
class QTimer;
class QTime;
class QSocketNotifier;


/*!
	\defgroup Core Core

	Contains constants, functions and classes that are not related to a specific
	section.

	\sa \ref page-gui for an overview of the main classes of the Core module.
*/


/*!
	\namespace bt_global
	\brief Encapsulates all the global objects.
*/

/*!
	\ingroup Core
	\brief Manages the unix signals used on the BTicino hardware.
*/
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
	QSocketNotifier *sn_signal;
};


struct BtStatus
{
	bool calibrating;
	bool vde_call_active;
	QString password;
	bool check_password;
	bool alarm_clock_on;

	BtStatus()
	{
		calibrating = vde_call_active = check_password = alarm_clock_on = false;
	}
};


/*!
	\ingroup Core
	\brief The main class of the BTouch interface.

	It loads the configuration and loads corresponding objects, establishes
	the connection with the openserver(s) and manages the global objects.
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

	Window *homeWindow();
	TrayBar *trayBar();
	void showHomePage();
	Page *homePage();

	bool alarm_clock_on;

	// Unroll all the pages until homepage
	void unrollPages();

signals:
	void resettimer();

private slots:
	void init();
	void checkActivities();
	void waitBeforeInit();
	void connectionReady();
	void startGui();
	void handleSignal(int signal_number);
	void testPassword();
	void showKeypadIfNeeded();

private:
	QHash<int, Clients> clients;
	QHash<int, ClientReader*> monitors;

	QHash<int, Page*> page_list;
	QTime *boot_time;
	HomePage *home;
	Page *page_default;

	QTimer *activities_timer;
	QDateTime last_date_time;
	bool already_calibrated;

	PageContainer *page_container;
	WindowContainer *window_container;

	// A flag that is set when the client monitor socket is ready
	bool monitor_ready;
	// A flag that is set when the configuration has loaded
	bool config_loaded;

	KeypadWindow *password_keypad;

	void myMain();
	/// Load the main configuration
	void loadConfiguration();

	// Load the global configuration (the item in "generale" section of xml config file)
	void loadGlobalConfig();

	void showPasswordKeypad();

	Page *screensaverExitPage();
	Page *screensaverTargetPage();
	Window *screensaverTargetWindow();
};

namespace bt_global { extern BtMain *btmain; }

namespace bt_global { extern BtStatus status; }

#endif// BTMAIN_H
