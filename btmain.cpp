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
#include "main.h" // (*bt_global::config)
#include "homepage.h"
#include "homewindow.h"
#include "hardware_functions.h" // rearmWDT, getTimePress, getBacklight
#include "xml_functions.h" // getPageNode, getElement, getChildWithId, getTextChild
#include "openclient.h"
#include "version.h"
#include "pagestack.h" // bt_global::page_stack
#include "displaycontrol.h" // bt_global::display
#include "devices_cache.h" // bt_global::devices_cache
#include "fontmanager.h" // bt_global::font
#include "skinmanager.h" // bt_global::skin
#include "page.h"
#include "device.h"
#include "pagefactory.h" // getPage
#include "banner.h"
#include "transitionwidget.h"
#include "pagecontainer.h"
#include "homewindow.h"
#include "iconwindow.h"
#include "windowcontainer.h"
#include "videodoorentry.h"
#include "keypad.h" // KeypadWindow
#if !defined(BT_HARDWARE_X11)
#include "calibration.h"
#endif
#include "media_device.h" // AmplifierDevice::setVirtualAmplifierWhere
#include "mediaplayer.h" // bt_global::sound
#include "ringtonesmanager.h"

#ifdef LAYOUT_TS_10
#include "audiostatemachine.h" // bt_global::audio_states
#include "mount_watcher.h" // bt_global::mount_watcher
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
#include <QThreadPool>
#include <QSocketNotifier>
#include <QtConcurrentRun>
#include <QMouseEvent>

#include <sys/types.h>
#include <sys/socket.h> // socketpair
#include <unistd.h> // write
#include <signal.h> // SIGUSR2, SIGTERM

#define DEBUG_MOUSEPRESS 0

#define ACTIVITIES_CHECK 2000
#define WD_THREAD_INTERVAL 5000

#define TS_NUM_BASE_ADDRESS 0x700


namespace
{
	void setConfigValue(QDomNode root, QString path, QString &dest)
	{
		QDomElement n = getElement(root, path);
		if (!n.isNull())
			dest = n.text();
	}

#if defined(BT_HARDWARE_X11) || defined(BT_HARDWARE_PXA270) || defined(BT_HARDWARE_DM365)

	// used to store the time of the last click; used by the screen saver code
	// on x86
	class LastClickTime : public QObject
	{
	public:
		LastClickTime();

		static bool isPressed() { return pressed; }

	protected:
		bool eventFilter(QObject *obj, QEvent *ev);

	private:
		static bool pressed;
	};

	LastClickTime::LastClickTime()
	{
	}

	bool LastClickTime::pressed = false;

	bool LastClickTime::eventFilter(QObject *obj, QEvent *ev)
	{

#if DEBUG_MOUSEPRESS
		if (ev->type() == QEvent::MouseButtonPress)
		{
			QWidget *content = new QWidget;
			content->setWindowFlags(Qt::FramelessWindowHint);
			content->show();
			content->raise();
			content->setFixedSize(3, 3);
			content->setStyleSheet("*{background-color: red}");
			QMouseEvent *me = static_cast<QMouseEvent*>(ev);
			content->move(me->globalPos());
			QTimer::singleShot(10000, content, SLOT(deleteLater()));
		}
#endif

		// Save last click time
		if (ev->type() == QEvent::MouseButtonPress || ev->type() == QEvent::MouseButtonDblClick)
		{
			setTimePress(QDateTime::currentDateTime());
			pressed = true;
		}

		if (ev->type() == QEvent::MouseButtonRelease)
		{
			setTimePress(QDateTime::currentDateTime());
			pressed = false;
		}

		return false;
	}
#endif

	volatile bool stop_watch_dog;

	void updateWatchDog()
	{
		while (!stop_watch_dog)
		{
			qDebug() << "Rearming watchdog from thread";
			rearmWDT();
			usleep(WD_THREAD_INTERVAL * 1000);
		}
	}

	void startUpdateWatchDog()
	{
		stop_watch_dog = false;

		QtConcurrent::run(&updateWatchDog);
	}

	void stopUpdateWatchDog()
	{
		stop_watch_dog = true;
	}
}


int SignalsHandler::signalfd[2];
long SignalsHandler::parent_pid = 0;


SignalsHandler::SignalsHandler()
{
	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, signalfd))
		qWarning() << "Cannot create SignalsHandler socketpair";
	parent_pid = getpid();

	sn_signal = new QSocketNotifier(signalfd[1], QSocketNotifier::Read, this);
	sn_signal->setEnabled(true);
	connect(sn_signal, SIGNAL(activated(int)), SLOT(handleSignal()));
}

SignalsHandler::~SignalsHandler()
{
	delete sn_signal;
}

void SignalsHandler::handleSignal()
{
	sn_signal->setEnabled(false);
	char signal_number;
	::read(signalfd[1], &signal_number, sizeof(signal_number));

	qDebug() << "Handling signal" << int(signal_number);
	emit signalReceived(int(signal_number));

	sn_signal->setEnabled(true);
}

void SignalsHandler::signalHandler(int signal_number)
{
	// ignore signals sent to childs; this handles the
	// time interval between fork() and exec() when using QProcess
	if (getpid() != parent_pid)
		return;

	char tmp = signal_number;
	::write(signalfd[0], &tmp, sizeof(tmp)); // write something, in order to "activate" the notifier
}


BtMain::BtMain(int openserver_reconnection_time)
{
	boot_time = new QTime;
	boot_time->start();

	// construct global objects
	bt_global::config = new QHash<GlobalField, QString>();

	loadGlobalConfig();
	qDebug("BtMain::BtMain");

	// for unknown reason, when launched by bt_processi, the thread pool does not
	// work (and a simple GUI restart from the shell does the trick); forcing the max
	// thread count works
	QThreadPool::globalInstance()->setMaxThreadCount(6);

	QString font_file = QString(MY_FILE_CFG_FONT).arg((*bt_global::config)[LANGUAGE]);
	bt_global::font = new FontManager(font_file);
	bt_global::display = new DisplayControl;
	bt_global::skin = new SkinManager(SKIN_FILE);
	bt_global::sound = new SoundPlayer;
	bt_global::ringtones = new RingtonesManager(RINGTONE_FILE);
#ifdef LAYOUT_TS_10
	bt_global::audio_states = new AudioStateMachine;
	bt_global::mount_watcher = new MountWatcher;
#endif

#if defined(BT_HARDWARE_X11) || defined(BT_HARDWARE_PXA270) || defined(BT_HARDWARE_DM365)
	// save last click time for the screen saver
	qApp->installEventFilter(new LastClickTime);
	// avoid calibration starting at every boot
	setTimePress(QDateTime::currentDateTime().addSecs(-60));
#endif

	monitor_ready = false;
	config_loaded = false;

	OpenServerManager::reconnection_time = openserver_reconnection_time;
	ClientWriter::setDelay((*bt_global::config)[TS_NUMBER].toInt() * TS_NUMBER_FRAME_DELAY);

	// The configuration is usually loaded in the "init()" method, but in this
	// case (that should be an exception) we need to know the list of openserver.
	QDomNode openservers_node = getConfElement("setup/openserver");
	if (!openservers_node.isNull())
		foreach (const QDomNode &item, getChildren(openservers_node, "item"))
		{
			int id = getTextChild(item, "id").toInt();
			QString host = getTextChild(item, "address");
			int port = getTextChild(item, "port").toInt();
			monitors[id] = new ClientReader(Client::MONITOR, host, port);
			clients[id].command = new ClientWriter(Client::COMMAND, host, port);
			clients[id].request = new ClientWriter(Client::REQUEST, host, port);
		}

	// If it is not defined a main openserver, the main openserver is the local openserver
	if (!clients.contains(MAIN_OPENSERVER))
	{
		monitors[MAIN_OPENSERVER] = new ClientReader(Client::MONITOR);
		clients[MAIN_OPENSERVER].command = new ClientWriter(Client::COMMAND);
		clients[MAIN_OPENSERVER].request = new ClientWriter(Client::REQUEST);
	}

#if DEBUG
	bool debug = true;
#else
	bool debug = false;
#endif

	if (debug || (*bt_global::config)[PI_MODE].toInt() == 1) // debug or vct full ip
	{
		// All the devices are connected to the client MONITOR, but sometimes we can
		// receive frames from the client SUPERVISOR. To mantain the code clean (and
		// because we have not to distinguish the input channel) we forward the frame
		// received from the SUPERVISOR to the MONITOR.
		ClientReader *client_supervisor = new ClientReader(Client::SUPERVISOR);
		client_supervisor->forwardFrame(monitors[MAIN_OPENSERVER]);

		// The supervisor socket is opened only in two cases: when the GUI in in debug
		// mode in order to receive the frames sent using a client like PyCo
		// or when we have the videocall ip enabled. In both situations, the openserver
		// is the local one, so we manage only this case.
		clients[MAIN_OPENSERVER].supervisor = client_supervisor;
	}

	Banner::setClients(clients[MAIN_OPENSERVER].command, clients[MAIN_OPENSERVER].request);
	Page::setClients(clients[MAIN_OPENSERVER].command, clients[MAIN_OPENSERVER].request);
	FrameReceiver::setClientsMonitor(monitors);
	FrameSender::setClients(clients);

	// When only the main openserver is defined the homepage is shown only when
	// the monitor of the openserver is up. Otherwise the homepage is shown as
	// soon as the configuration is loaded.
	if (clients.count() > 1)
		monitor_ready = true;
	else
	{
		OpenServerManager *manager = device::getManager(MAIN_OPENSERVER);

		connect(manager, SIGNAL(connectionUp()), SLOT(connectionReady()));
	}

	window_container = new WindowContainer(maxWidth(), maxHeight());

	home_window = new HomeWindow;
	page_container = home_window->centralLayout();
	Page::setPageContainer(page_container);

	page_container->blockTransitions(true); // no transitions until homepage is showed
	connect(page_container, SIGNAL(currentPageChanged(Page*)), &bt_global::page_stack, SLOT(currentPageChanged(Page *)));

	bt_global::display->setPageContainer(page_container);
	connect(bt_global::display, SIGNAL(unfreezed()), SLOT(showKeypadIfNeeded()));
	connect(bt_global::display, SIGNAL(unrollPages()), SLOT(unrollPages()));

	rearmWDT();

	password_keypad = 0;

	page_default = NULL;
	home_page = NULL;
	version = NULL;
	already_calibrated = false;

	Window *loading = NULL;

#ifdef LAYOUT_TS_3_5
	// We want to set the stylesheet for the version page, but we have to wait
	// after all the pages are built in order to set the dynamic properties _before_
	// applying the styles. As an exception, we set the styles for the version page
	// here and only after the styles for all the pages.
	version = new Version;
	version->setModel((*bt_global::config)[MODEL]);
	QString style = bt_global::skin->getStyle();
	if (!style.isNull())
		version->setStyleSheet(style);
#else
	// the stylesheet on QApplication must be set later (see comment in init())
	loading = new IconWindow("splash_image", bt_global::skin->getStyle());
#endif

	initMultimedia();

#if !defined(BT_HARDWARE_X11)
	if (!Calibration::exists())
	{
		qDebug() << "No pointer calibration file, calibrating";

		// We have to force the skin manager context for the calibration
		bt_global::skin->addToContext(CALIBRATION_CONTEXT);
		already_calibrated = true;
#ifdef LAYOUT_TS_3_5
		Calibration *cal = new Calibration(true);
#else
		Calibration *cal = new Calibration;
#endif
		bt_global::skin->removeFromContext();
		cal->showWindow();
		connect(cal, SIGNAL(Closed()), SLOT(waitBeforeInit()));

#ifdef LAYOUT_TS_3_5
		connect(cal, SIGNAL(Closed()), version, SLOT(showPage()));
#else
		connect(cal, SIGNAL(Closed()), loading, SLOT(showWindow()));
#endif
		return;
	}

	if (qApp->arguments().contains("-testcalib"))
	{
		CalibrationTest *cal = new CalibrationTest;

		cal->showWindow();
		connect(cal, SIGNAL(Closed()), SLOT(waitBeforeInit()));

#ifdef LAYOUT_TS_3_5
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
	delete bt_global::skin;
	delete bt_global::font;
#ifdef LAYOUT_TS_10
	delete bt_global::mount_watcher;
	delete bt_global::audio_states;
#endif
	delete bt_global::ringtones;
	delete bt_global::sound;
	delete bt_global::display;
	bt_global::devices_cache.clear();
	DevicesCachePrivate::devices_cache_no_init.clear();

	QMutableHashIterator<int,Clients> it(clients);
	while (it.hasNext())
	{
		it.next();
		delete it.value().command;
		delete it.value().request;
		delete it.value().supervisor;
	}

	QMutableHashIterator<int, ClientReader*> mit(monitors);
	while (mit.hasNext())
	{
		mit.next();
		delete mit.value();
	}
}

void BtMain::loadGlobalConfig()
{
	using bt_global::config;

	// Load the default values
	(*config)[TEMPERATURE_SCALE] = QString::number(CELSIUS);
	(*config)[LANGUAGE] = DEFAULT_LANGUAGE;
	(*config)[DATE_FORMAT] = QString::number(EUROPEAN_DATE);

	QDomNode n = getConfElement("setup/generale");

	// Load the current values
	setConfigValue(n, "temperature/format", (*config)[TEMPERATURE_SCALE]);
	Q_ASSERT_X(((*config)[TEMPERATURE_SCALE] == QString::number(CELSIUS)) ||
		((*config)[TEMPERATURE_SCALE] == QString::number(FAHRENHEIT)), "BtMain::loadGlobalConfig",
		"Temperature scale is invalid.");
	setConfigValue(n, "language", (*config)[LANGUAGE]);
	setConfigValue(n, "clock/dateformat", (*config)[DATE_FORMAT]);
	setConfigValue(n, "modello", (*config)[MODEL]);
	setConfigValue(n, "nome", (*config)[NAME]);

	QDomNode vde_node = getConfElement("setup/vdes");
	QString guard_addr;
	setConfigValue(vde_node, "guardunits/item", guard_addr);
	if (!guard_addr.isEmpty())
		(*config)[GUARD_UNIT_ADDRESS] = "3" + guard_addr;

	QDomNode vde_pi_node = getChildWithName(vde_node, "communication");
	if (!vde_pi_node.isNull())
	{
		QString address = getTextChild(vde_pi_node, "address");
		QString dev = getTextChild(vde_pi_node, "dev");
		if (!address.isNull())
		{
			if (address == "-1")
				(*config)[PI_ADDRESS] = "--";
			else
				(*config)[PI_ADDRESS] = dev + address;
		}

		(*config)[PI_MODE] = getTextChild(vde_pi_node, "mode");
	}
	else
		(*config)[PI_MODE] = QString();

	QDomNode scs_node = getConfElement("setup/scs");

	bool ok;
	int num = getElement(scs_node, "coordinate_scs/diag_addr").text().toInt(&ok, 16);
	if (ok && (num - TS_NUM_BASE_ADDRESS >= 0 && num - TS_NUM_BASE_ADDRESS < 100))
		(*config)[TS_NUMBER] = QString::number(num - TS_NUM_BASE_ADDRESS);
	else
	{
		qWarning() << "Unable to read the serial number of the touchscreen";
		(*config)[TS_NUMBER] = QString::number(0);
	}

	// source and amplifier addresses
	setConfigValue(scs_node, "coordinate_scs/my_mmaddress", (*config)[SOURCE_ADDRESS]);
	setConfigValue(scs_node, "coordinate_scs/my_maaddress", (*config)[AMPLIFIER_ADDRESS]);

	if ((*config)[SOURCE_ADDRESS] == "-1")
		(*config)[SOURCE_ADDRESS] = "";
	if ((*config)[AMPLIFIER_ADDRESS] == "-1")
		(*config)[AMPLIFIER_ADDRESS] = "";

	AmplifierDevice::setVirtualAmplifierWhere((*config)[AMPLIFIER_ADDRESS]);
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
	home_window->loadConfiguration();

	QDomNode gui_node = getConfElement("gui");
	home_page = static_cast<HomePage*>(getSectionPageFromId(HOME_PAGE));

#ifdef LAYOUT_TS_10
	QDomNode video_node = getPageNode(VIDEODOORENTRY);
	// Touch X can receive calls even if the videodoorentry section is not
	// configured (but the configuration specifies it as an internal place).
	// In this case, we create an "empty" version of the VideoDoorEntry page,
	// only to manage the ringtone and the loading of the VCTCallPage/IntercomPage.
	if (video_node.isNull() && !(*bt_global::config)[PI_ADDRESS].isEmpty())
		(void) new VideoDoorEntry;
#endif // LAYOUT_TS_10

	bt_global::page_stack.setHomePage(home_page);
	connect(home_window, SIGNAL(showHomePage()), home_page, SLOT(showPage()));
	connect(home_window, SIGNAL(showSectionPage(int)), home_page, SLOT(showSectionPage(int)));
	connect(home_page, SIGNAL(iconStateChanged(int,StateButton::Status)), home_window,
		SLOT(iconStateChanged(int,StateButton::Status)));

	QDomNode home_node = getChildWithName(gui_node, "homepage");
	if (getTextChild(home_node, "isdefined").toInt())
	{
		int id_default = getTextChild(home_node, "id").toInt();
		page_default = id_default == HOME_PAGE ? home_page : getSectionPageFromId(id_default);
	}
}

void BtMain::init()
{
	if (version)
		version->inizializza();

	loadConfiguration();

	if (page_default)
		connect(page_default, SIGNAL(Closed()), home_page, SLOT(showPage()));
	// The stylesheet can contain some references to dynamic properties,
	// so loading of css must be done after setting these properties (otherwise
	// it might be necessary to force a stylesheet recomputation).
	QString style = bt_global::skin->getStyle();
	if (style.isNull())
		qWarning("Unable to load skin file!");
	else
	{
		// setStyleSheet may be slow, try to avoid the watchdog timeout
		// by calling it in a secondary thread
		startUpdateWatchDog();
		qApp->setStyleSheet(style);
		stopUpdateWatchDog();
	}

	createFlagFile(FILE_CONF_LOADED);
	config_loaded = true;
	qDebug("Initialization complete, from now on will write to configuration");
	(*bt_global::config)[INIT_COMPLETE] = "1"; // maybe change config to contain QVariant?

	if (monitor_ready)
		myMain();
}

void BtMain::connectionReady()
{
	monitor_ready = true;
	if (config_loaded)
		myMain();
}

void BtMain::myMain()
{
	// Called when both the connection with the main openserver is up and config is loaded.
	qDebug("BtMain::MyMain");

	// disconnect the connectionReady slot: myMain must be called only once even
	// in the case of openserver reconnection
	{
		OpenServerManager *manager = device::getManager(MAIN_OPENSERVER);

		disconnect(manager, 0, this, 0);
	}

	home_page->inizializza();
	if (version)
		version->inizializza();

	foreach (Page *p, page_list)
		p->inizializza();

	device::initDevices();

#if !defined(BT_HARDWARE_X11)
	if (static_cast<int>(getTimePress()) * 1000 <= boot_time->elapsed() && !already_calibrated)
	{
		qDebug() << "Boot time" << boot_time->elapsed() << "last press" << static_cast<int>(getTimePress()) * 1000;

		already_calibrated = true;
		// We have to force the skin manager context for the calibration
		bt_global::skin->addToContext(CALIBRATION_CONTEXT);
#ifdef LAYOUT_TS_3_5
		Calibration *cal = new Calibration(true);
#else
		Calibration *cal = new Calibration;
#endif
		bt_global::skin->removeFromContext();
		cal->showWindow();
		connect(cal, SIGNAL(Closed()), SLOT(startGui()));
		return;
	}
#endif
	startGui();
}

void BtMain::startGui()
{
	// start counting the time elapsed to manage freeze/screensaver..
	bt_global::display->startTime();

	home_page->showPage();
	// this needs to be after the showPage, and will be a no-op until transitions
	// between windows are implemented
	page_container->blockTransitions(false);
	home_window->showWindow();

	activities_timer = new QTimer(this);
	activities_timer->start(ACTIVITIES_CHECK);
	connect(activities_timer, SIGNAL(timeout()), SLOT(checkActivities()));
}

void BtMain::showHomePage()
{
	home_page->showPage();
}

Page *BtMain::homePage()
{
	return home_page;
}

void BtMain::unrollPages()
{
	int seq_pages = 0;
	if (page_container->currentPage() != page_default && page_container->currentPage() != version)
		while (page_container->currentPage() != home_page)
		{
			Page *curr = page_container->currentPage();
			if (curr)
			{
				curr->cleanUp();
				curr->forceClosed();
			}
			++seq_pages;
			// To avoid infinite loop, we assume that the application
			// can have a maximum number of sequential pages in the
			// navigation equal to 50.
			if (seq_pages > 50)
			{
				qWarning() << "Maximum number of sequential pages reached with" << curr;
				break;
			}
		}
}

// The logics for the screensaver are the following:
// on ts10, the screensaver runs in the current page/window with the exception of
// the slideshow, that runs in a dedicated window;
// on ts3, the screensaver runs in the homepage and turns back to the current page
// if the default page is not set, otherwise it runs in the default page where
// stays also after the exit from the screensaver.
Page *BtMain::screensaverTargetPage()
{
#ifdef LAYOUT_TS_3_5
	return page_default ? page_default : home_page;
#else
	return page_container->currentPage();
#endif
}

Page *BtMain::screensaverExitPage()
{
#ifdef LAYOUT_TS_3_5
	return page_default ? page_default : page_container->currentPage();
#else
	return page_container->currentPage();
#endif
}

Window *BtMain::screensaverTargetWindow()
{
#ifdef LAYOUT_TS_3_5
	return home_window;
#else
	return window_container->currentWindow();
#endif
}

void BtMain::checkActivities()
{
	rearmWDT();

#if defined(BT_HARDWARE_X11) || defined(BT_HARDWARE_PXA270) || defined(BT_HARDWARE_DM365)
	// detect when the user adjusts date/time
	// TODO add frame parsing to PlatformDevice to detect when date/time really changes
	QDateTime curr = QDateTime::currentDateTime();

	// TODO the correct handling would require to send a signal with the time delta and
	//      have hardware_functions (and others) adjust the saved date/time; this works
	//      for now
	if (abs(last_date_time.secsTo(curr)) > 30)
		// we assume that the user just clicked "OK" to change the date,
		setTimePress(curr);
	last_date_time = curr;

	if (LastClickTime::isPressed())
		return;
#endif

	Page *target_page = screensaverTargetPage();
	Window *target_window = screensaverTargetWindow();
	Page *exit_page = screensaverExitPage();

	bt_global::display->checkScreensaver(target_page, target_window, exit_page);
}

TrayBar *BtMain::trayBar()
{
	return home_window->tray_bar;
}

Window *BtMain::homeWindow()
{
	return home_window;
}

void BtMain::resetTimer()
{
	qDebug("BtMain::ResetTimer()");
	emit resettimer();
}

void BtMain::handleSignal(int signal_number)
{
	if (signal_number == SIGUSR2)
		resetTimer();
	else if (signal_number == SIGTERM)
	{
		qDebug("Terminating on SIGTERM");
		qApp->quit();
	}
}

void BtMain::testPassword()
{
	QString text = password_keypad->getText();
	if (!text.isEmpty())
	{
		if (text != bt_global::status.password)
		{
			password_keypad->resetText();
			qDebug() << "BtMain::testPassword the input text" << text
				<< "doesn't match the password" << bt_global::status.password;
		}
		else
		{
			qDebug() << "BtMain::testPassword password ok";
			bt_global::page_stack.closeWindow(password_keypad);
			password_keypad->disconnect();
			password_keypad->deleteLater();
			password_keypad = 0;
		}
	}
}

void BtMain::showPasswordKeypad()
{
	if (!password_keypad)
	{
		password_keypad = new KeypadWindow(Keypad::HIDDEN);
		connect(password_keypad, SIGNAL(Closed()), SLOT(testPassword()));
	}
	bt_global::page_stack.showKeypad(password_keypad);
	password_keypad->showWindow();
}

void BtMain::showKeypadIfNeeded()
{
	if (bt_global::status.check_password)
		showPasswordKeypad();
}

// The global definition of btmain pointer
BtMain *bt_global::btmain;

// The global BTouch status
BtStatus bt_global::status;

