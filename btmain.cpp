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


#include "btmain.h"
#include "main.h" // bt_global::config
#include "homepage.h"
#include "hardware_functions.h" // rearmWDT, getTimePress, setOrientation, getBacklight, initScreen
#include "xml_functions.h" // getPageNode, getElement, getChildWithId, getTextChild
#include "openclient.h"
#include "version.h"
#include "keypad.h"
#include "screensaver.h"
#include "displaycontrol.h" // bt_global::display
#include "page.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "device.h"
#include "fontmanager.h" // bt_global::font
#include "skinmanager.h" // bt_global::skin
#include "pagefactory.h" // getPage
#include "banner.h"
#include "transitionwidget.h"
#include "frame_receiver.h"
#include "pagecontainer.h"
#include "homewindow.h"
#include "iconwindow.h"
#include "windowcontainer.h"
#include "ringtonesmanager.h"
#include "pagestack.h"
#include "videoentryphone.h"
#if !defined(BT_HARDWARE_X11)
#include "calibration.h"
#endif

#include <QMutableHashIterator>
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QApplication>
#include <QObject>
#include <QBitmap>
#include <QPixmap>
#include <QString>
#include <QTimer>
#include <QFile>
#include <QTime>


// delay between two consecutive screensaver checks
#define SCREENSAVER_CHECK 2000

namespace
{
	void setConfigValue(QDomNode root, QString path, QString &dest)
	{
		QDomElement n = getElement(root, path);
		if (!n.isNull())
			dest = n.text();
	}
}


#if defined(BT_HARDWARE_X11) || defined(BT_HARDWARE_TOUCHX)

// used to store the time of the last click; used by the screen saver code
// on x86
class LastClickTime : public QObject
{
public:
	LastClickTime();

protected:
	bool eventFilter(QObject *obj, QEvent *ev);
};

LastClickTime::LastClickTime()
{
}

bool LastClickTime::eventFilter(QObject *obj, QEvent *ev)
{
	// Save last click time
	if (ev->type() == QEvent::MouseButtonPress || ev->type() == QEvent::MouseButtonDblClick)
		setTimePress(QDateTime::currentDateTime());

	return false;
}

#endif


BtMain::BtMain(int openserver_reconnection_time)
{
	boot_time = new QTime;
	boot_time->start();
	difSon = 0;
	dm = 0;
	screensaver = 0;
	loadGlobalConfig();
	qDebug("BtMain::BtMain");

	QString font_file = QString(MY_FILE_CFG_FONT).arg(bt_global::config[LANGUAGE]);
	bt_global::font = new FontManager(font_file);
	bt_global::skin = new SkinManager(SKIN_FILE);
	bt_global::ringtones = new RingtonesManager;

#if defined(BT_HARDWARE_X11) || defined(BT_HARDWARE_TOUCHX)
	// save last click time for the screen saver
	qApp->installEventFilter(new LastClickTime);
	// avoid calibration starting at every boot
	setTimePress(QDateTime::currentDateTime().addSecs(-60));
#endif

	initScreen();
	monitor_ready = false;
	config_loaded = false;

	OpenServerManager::reconnection_time = openserver_reconnection_time;

	// The configuration is usually loaded in the "init()" method, but in this
	// case (that should be an exception) we need to know the list of openserver.
	QDomNode openservers_node = getConfElement("setup/openserver");
	if (!openservers_node.isNull())
		foreach (const QDomNode &item, getChildren(openservers_node, "item"))
		{
			int id = getTextChild(item, "id").toInt();
			QString host = getTextChild(item, "address");
			int port = getTextChild(item, "port").toInt();
			monitors[id] = new Client(Client::MONITOR, host, port);
			clients[id].first = new Client(Client::COMMAND, host, port);
			clients[id].second = new Client(Client::REQUEST, host, port);
		}

	// If it is not defined a main openserver, the main openserver is the local openserver
	if (!clients.contains(MAIN_OPENSERVER))
	{
		monitors[MAIN_OPENSERVER] = new Client(Client::MONITOR);
		clients[MAIN_OPENSERVER].first = new Client(Client::COMMAND);
		clients[MAIN_OPENSERVER].second = new Client(Client::REQUEST);
	}

#if DEBUG
	client_supervisor = new Client(Client::SUPERVISOR);
	client_supervisor->forwardFrame(monitors[MAIN_OPENSERVER]);
#endif

	// When only the main openserver is defined the homepage is showed only when
	// the monitor of the openserver is up. Otherwise the homepage is showed as
	// soon as the configuration is loaded.
	if (clients.count() > 1)
		monitor_ready = true;
	else
		connect(monitors[MAIN_OPENSERVER], SIGNAL(connectionUp()), SLOT(monitorReady()));

	banner::setClients(clients[MAIN_OPENSERVER].first, clients[MAIN_OPENSERVER].second);
	Page::setClients(clients[MAIN_OPENSERVER].first, clients[MAIN_OPENSERVER].second);
	FrameReceiver::setClientsMonitor(monitors);
	device::setClients(clients);

	window_container = new WindowContainer(maxWidth(), maxHeight());
	page_container = window_container->centralLayout();
	page_container->blockTransitions(true); // no transitions until homepage is showed
	connect(page_container, SIGNAL(currentPageChanged(Page*)), &bt_global::page_stack, SLOT(currentPageChanged(Page *)));

	rearmWDT();

	calibrating = false;
	pagDefault = NULL;
	Home = NULL;
	version = NULL;
	alreadyCalibrated = false;
	alarmClockIsOn = false;
	last_event_time = 0;
	frozen = false;

	passwordKeypad = NULL;
	pwdOn = false;

	Window *loading = NULL;

	// TODO these must be read from configuration
	freeze_time = 30;
	screensaver_time = 60;
	screenoff_time = 120;

#ifdef LAYOUT_BTOUCH
	version = new Version;
	version->setModel(bt_global::config[MODEL]);
#else
	// the stylesheet on QApplication must be set later (see comment in init())
	loading = new IconWindow("splash_image", bt_global::skin->getStyle());
#endif

	initMultimedia();

#if !defined(BT_HARDWARE_X11)
	if (!Calibration::exists())
	{
		alreadyCalibrated = true;
#ifdef LAYOUT_BTOUCH
		Calibration *cal = new Calibration(true);
#else
		Calibration *cal = new Calibration;
#endif
		cal->showWindow();
		connect(cal, SIGNAL(Closed()), SLOT(waitBeforeInit()));

#ifdef LAYOUT_BTOUCH
		connect(cal, SIGNAL(Closed()), version, SLOT(showPage()));
#else
		connect(cal, SIGNAL(Closed()), loading, SLOT(showWindow()));
#endif
		return;
	}
#endif

	if (version)
		version->showPage();
	if (loading)
		loading->showWindow();
	waitBeforeInit();
}

BtMain::~BtMain()
{
	delete screensaver;
	delete bt_global::skin;
	delete bt_global::font;

	QMutableHashIterator<int, QPair<Client*, Client*> > it(clients);
	while (it.hasNext())
	{
		it.next();
		delete it.value().first;
		delete it.value().second;
	}

	QMutableHashIterator<int, Client*> mit(monitors);
	while (mit.hasNext())
	{
		mit.next();
		delete mit.value();
	}
#if DEBUG
	delete client_supervisor;
#endif
}

void BtMain::loadGlobalConfig()
{
	using bt_global::config;

	// Load the default values
	config[TEMPERATURE_SCALE] = QString::number(CELSIUS);
	config[LANGUAGE] = DEFAULT_LANGUAGE;
	config[DATE_FORMAT] = QString::number(EUROPEAN_DATE);

	QDomNode n = getConfElement("setup/generale");

	// Load the current values
	setConfigValue(n, "temperature/format", config[TEMPERATURE_SCALE]);
	Q_ASSERT_X((config[TEMPERATURE_SCALE] == QString::number(CELSIUS)) ||
		(config[TEMPERATURE_SCALE] == QString::number(FAHRENHEIT)), "BtMain::loadGlobalConfig",
		"Temperature scale is invalid.");
	setConfigValue(n, "language", config[LANGUAGE]);
	setConfigValue(n, "clock/dateformat", config[DATE_FORMAT]);
	setConfigValue(n, "modello", config[MODEL]);
	setConfigValue(n, "nome", config[NAME]);

	QDomNode scs_node = getConfElement("setup/scs");

	setConfigValue(scs_node, "coordinate_scs/my_piaddress", config[PI_ADDRESS]);
	// transform address into internal address
	if (!config[PI_ADDRESS].isNull())
		config[PI_ADDRESS].prepend("1");
}

void BtMain::waitBeforeInit()
{
	QTimer::singleShot(200, this, SLOT(init()));
}

void BtMain::loadConfiguration()
{
	if (version)
	{
		QDomNode setup = getConfElement("setup");
		if (!setup.isNull())
		{
			QDomElement addr = getElement(setup, "scs/coordinate_scs/diag_addr");
			bool ok;
			if (!addr.isNull())
				version->setAddr(addr.text().toInt(&ok, 16) - 768);
		}
		else
			qWarning("setup node not found on xml config file!");
	}

	QDomNode display_node = getChildWithId(getPageNode(IMPOSTAZIONI), QRegExp("item\\d{1,2}"), DISPLAY);

#ifdef BT_HARDWARE_TOUCHX
	// on TouchX there is no way to control the screensaver brightness
	BrightnessLevel level = BRIGHTNESS_HIGH;
#else
	BrightnessLevel level = BRIGHTNESS_NORMAL; // default brightness
#endif
	if (!display_node.isNull())
	{
		QDomElement n = getElement(display_node, "brightness/level");
		if (!n.isNull())
			level = static_cast<BrightnessLevel>(n.text().toInt());
	}
	bt_global::display.setBrightness(level);

	ScreenSaver::Type type = ScreenSaver::LINES; // default screensaver
	if (!display_node.isNull())
	{
		QDomElement screensaver_node = getElement(display_node, "screensaver");
		QDomElement n = getElement(screensaver_node, "type");
		if (!n.isNull())
			type = static_cast<ScreenSaver::Type>(n.text().toInt());
		ScreenSaver::initData(screensaver_node);
		if (type == ScreenSaver::DEFORM) // deform is for now disabled!
			type = ScreenSaver::LINES;
	}
	bt_global::display.current_screensaver = type;

	window_container->homeWindow()->loadConfiguration();

#ifdef CONFIG_BTOUCH
	QDomNode gui_node = getConfElement("displaypages");
	QDomNode pagemenu_home = getChildWithId(gui_node, QRegExp("pagemenu(\\d{1,2}|)"), 0);
	// homePage must be built after the loading of the configuration,
	// to ensure that values displayed (by homePage or its child pages)
	// is in according with saved values.
	Home = new HomePage(pagemenu_home);

	QString orientation = getTextChild(gui_node, "orientation");
	if (!orientation.isNull())
		setOrientation(orientation);
#else
	QDomNode gui_node = getConfElement("gui");
	// TODO read the id from the <homepage> node
	QDomNode pagemenu_home = getHomepageNode();
	Home = new HomePage(pagemenu_home);

	QDomNode video_node = getPageNode(VIDEOCITOFONIA);
	// Touch X can receive calls even if the videoentryphone section is not
	// configured (but the configuration specifies it as an internal place).
	if (video_node.isNull() && !bt_global::config[PI_ADDRESS].isEmpty())
		VideoEntryPhone::loadHiddenPages();

#endif
	connect(window_container->homeWindow(), SIGNAL(showHomePage()), Home, SLOT(showPage()));
	connect(window_container->homeWindow(), SIGNAL(showSectionPage(int)), Home, SLOT(showSectionPage(int)));

	QDomNode home_node = getChildWithName(gui_node, "homepage");
	if (getTextChild(home_node, "isdefined").toInt())
	{
		int id_default = getTextChild(home_node, "id").toInt();
		pagDefault = !id_default ? Home : getPage(id_default);
	}

	// Transition effects are for now disabled!
//	TransitionWidget *tr = new BlendingTransition;
//	window_container->installTransitionWidget(tr);
//	page_container->installTransitionWidget(tr);
}

void BtMain::init()
{
	if (version)
		version->inizializza();

	loadConfiguration();

	if (pagDefault)
		connect(pagDefault, SIGNAL(Closed()), Home, SLOT(showPage()));
	// The stylesheet can contain some references to dynamic properties,
	// so loading of css must be done after setting these properties (otherwise
	// it might be necessary to force a stylesheet recomputation).
	QString style = bt_global::skin->getStyle();
	if (style.isNull())
		qWarning("Unable to load skin file!");
	else
	{
		// setStyleSheet may be slow, try to avoid the watchdog timeout
		rearmWDT();
		qApp->setStyleSheet(style);
		rearmWDT();
	}

	config_loaded = true;
	qDebug("Initialization complete, from now on will write to configuration");
	bt_global::config[INIT_COMPLETE] = "1"; // maybe change config to contain QVariant?

	if (monitor_ready)
		myMain();
}

void BtMain::monitorReady()
{
	monitor_ready = true;
	if (config_loaded)
		myMain();
}

void BtMain::myMain()
{
	// Called when both the connection is up and config is loaded.
	qDebug("BtMain::MyMain");

	Home->inizializza();
	if (version)
		version->inizializza();

	foreach (Page *p, page_list)
		p->inizializza();

	device::initDevices();

#if !defined(BT_HARDWARE_X11)
	if (static_cast<int>(getTimePress()) * 1000 <= boot_time->elapsed() && !alreadyCalibrated)
	{
		alreadyCalibrated = true;
#ifdef LAYOUT_BTOUCH
		Calibration *cal = new Calibration(true);
#else
		Calibration *cal = new Calibration;
#endif
		cal->showWindow();
		connect(cal, SIGNAL(Closed()), SLOT(startGui()));
		return;
	}
#endif
	startGui();
}

static unsigned long now()
{
	return time(NULL);
}

void BtMain::startGui()
{
	// start the screensaver countdown when the home page is shown
	last_event_time = now();

	Home->showPage();
	// this needs to be after the showPage, and will be a no-op until transitions
	// between windows are implemented
	page_container->blockTransitions(false);
	window_container->homeWindow()->showWindow();

	screensaver_timer = new QTimer(this);
	screensaver_timer->start(SCREENSAVER_CHECK);
	connect(screensaver_timer, SIGNAL(timeout()), SLOT(checkScreensaver()));
}

void BtMain::showHomePage()
{
	Home->showPage();
}

void BtMain::unrollPages()
{
	int seq_pages = 0;
	if (page_container->currentPage() != pagDefault && page_container->currentPage() != version)
		while (page_container->currentPage() != Home)
		{
			page_container->currentPage()->forceClosed();
			++seq_pages;
			// To avoid infinite loop, we assume that the application
			// can have a maximum number of sequential pages in the
			// navigation equal to 50.
			if (seq_pages > 50)
			{
				qWarning() << "Maximum number of sequential pages reached with" << page_container->currentPage();
				break;
			}
		}
}

void BtMain::makeActiveAndFreeze()
{
	if (screensaver && screensaver->isRunning())
	{
		screensaver->stop();
		bt_global::display.setState(DISPLAY_FREEZED);
		last_event_time = now();

		if (pwdOn)
			freeze(true);
	}

	if (passwordKeypad)
	{
		bt_global::page_stack.closeWindow(passwordKeypad);
		freeze(true);
	}
}

void BtMain::checkScreensaver()
{
	rearmWDT();

	if (bt_global::display.isForcedOperativeMode())
		return;
	if (alarmClockIsOn || calibrating)
		return;

	int time_press = getTimePress();
	int time = qMin(time_press, int(now() - last_event_time));

	if (screenoff_time != 0 && time >= screenoff_time &&
		 bt_global::display.currentState() == DISPLAY_SCREENSAVER)
	{
		qDebug() << "Turning screen off";
		screensaver->stop();
		bt_global::display.setState(DISPLAY_OFF);
	}
	else if (time >= freeze_time && getBacklight() && !frozen)
	{
		freeze(true);
	}
	else if (time >= screensaver_time)
	{
		if (bt_global::display.currentState() == DISPLAY_OPERATIVE &&
		    pagDefault && page_container->currentPage() != pagDefault)
		{
			pagDefault->showPage();
		}

		// TODO discover if the "+ 5" is a fudge-factor
		if (time >= screensaver_time + 5 && bt_global::display.currentState() == DISPLAY_FREEZED)
		{
			ScreenSaver::Type target_screensaver = bt_global::display.currentScreenSaver();
			// When the brightness is set to off in the old hardware the display
			// is not really off, so it is required to use a screensaver to protect
			// the display, even if the screensaver is not visible.
			if (bt_global::display.currentBrightness() == BRIGHTNESS_OFF)
				target_screensaver = ScreenSaver::LINES;

			if (screensaver && screensaver->type() != target_screensaver)
			{
				delete screensaver;
				screensaver = 0;
			}

			if (!screensaver)
				screensaver = getScreenSaver(target_screensaver);

			// TODO move the code until the end of the block to PageStack and/or ScreenSaver
			Page *prev_page = page_container->currentPage();

			page_container->blockTransitions(true);
			if (pagDefault)
			{
				unrollPages();
				pagDefault->showPage();
			}
			else
			{
				Home->showPage();
				// this makes the screen saver go back to prev_page
				// when exited
				bt_global::page_stack.currentPageChanged(prev_page);
			}

			window_container->homeWindow()->showWindow();
			page_container->blockTransitions(false);
			qDebug() << "start screensaver:" << target_screensaver << "on:" << page_container->currentPage();
			screensaver->start(window_container->homeWindow());
			emit startscreensaver(prev_page);
			bt_global::display.setState(DISPLAY_SCREENSAVER);
		}
	}
}

TrayBar *BtMain::trayBar()
{
	return window_container->homeWindow()->tray_bar;
}

Window *BtMain::homeWindow()
{
	return window_container->homeWindow();
}

bool BtMain::eventFilter(QObject *obj, QEvent *ev)
{
	// Discard the mouse press and mouse double click
	if (ev->type() == QEvent::MouseButtonPress || ev->type() == QEvent::MouseButtonDblClick)
		return true;

	if (ev->type() != QEvent::MouseButtonRelease)
		return false;

	freeze(false);
	return true;
}

void BtMain::freeze(bool b)
{
	qDebug("BtMain::freeze(%d)", b);
	frozen = b;
	emit freezed(frozen);

	if (!frozen)
	{
		last_event_time = now();
		bt_global::display.setState(DISPLAY_OPERATIVE);
		if (screensaver && screensaver->isRunning())
		{
			screensaver->stop();
		}

		if (pwdOn)
		{
			if (!passwordKeypad)
			{
				passwordKeypad = new KeypadWindow(Keypad::HIDDEN);
				connect(passwordKeypad, SIGNAL(Closed()), SLOT(testPwd()));
			}
			bt_global::page_stack.showKeypad(passwordKeypad);
			passwordKeypad->showWindow();
		}
		qApp->removeEventFilter(this);
	}
	else
	{
		bt_global::display.setState(DISPLAY_FREEZED);
		qApp->installEventFilter(this);
	}
}

void BtMain::setPwd(bool b, QString p)
{
	pwdOn = b;
	pwd = p;
	qDebug() << "new password:" << pwd << "active:" << pwdOn;
}

void BtMain::testPwd()
{
	QString p = passwordKeypad->getText();
	qDebug() << "testing password, input text is: " << p;
	if (!p.isEmpty())
	{
		if (p != pwd)
		{
			passwordKeypad->resetText();
			qDebug() << "pwd ko" << p << "doveva essere " << pwd;
		}
		else
		{
			qDebug() << "pwd ok!";
			Window *t = passwordKeypad;
			// set to NULL to avoid freezing again in makeActiveAndFreeze
			passwordKeypad = NULL;
			bt_global::page_stack.closeWindow(t);
			t->disconnect();
			t->deleteLater();
		}
	}
}

void BtMain::svegl(bool b)
{
	qDebug("BtMain::svegl->%d",b);
	alarmClockIsOn = b;
}

void BtMain::calibrationStarted()
{
	qDebug("BtMain::calibrationStarted()");
	calibrating = true;
}

void BtMain::calibrationEnded()
{
	qDebug("BtMain::calibrationEnded()");
	calibrating = false;
}

void BtMain::resetTimer()
{
	qDebug("BtMain::ResetTimer()");
	emit resettimer();
}

// The global definition of btmain pointer
BtMain *bt_global::btmain;

