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
#include "hardware_functions.h" // rearmWDT, getTimePress, setOrientation, getBacklight
#include "xml_functions.h" // getPageNode, getElement, getChildWithId, getTextChild
#include "openclient.h"
#include "version.h"
#include "keypad.h"
#include "screensaver.h"
#include "displaycontrol.h" // bt_global::display
#include "devices_cache.h" // bt_global::devices_cache
#include "fontmanager.h" // bt_global::font
#include "skinmanager.h" // bt_global::skin
#include "audiostatemachine.h" // bt_global::audio_states
#include "page.h"
#include "device.h"
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
#include "videodoorentry.h"
#if !defined(BT_HARDWARE_X11)
#include "calibration.h"
#endif
#include "media_device.h" // AmplifierDevice::setVirtualAmplifierWhere
#include "mediaplayer.h" // bt_global::sound

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

// delay between two consecutive screensaver checks
#define SCREENSAVER_CHECK 2000
#define WD_THREAD_INTERVAL 5000

#if LAYOUT_TS_3_5
#define TS_NUM_BASE_ADDRESS 0x300
#else
#define TS_NUM_BASE_ADDRESS 0x700
#endif



namespace
{
	void setConfigValue(QDomNode root, QString path, QString &dest)
	{
		QDomElement n = getElement(root, path);
		if (!n.isNull())
			dest = n.text();
	}


#if defined(BT_HARDWARE_X11) || defined(BT_HARDWARE_TS_10)

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

	snSignal = new QSocketNotifier(signalfd[1], QSocketNotifier::Read, this);
	snSignal->setEnabled(true);
	connect(snSignal, SIGNAL(activated(int)), SLOT(handleSignal()));
}

SignalsHandler::~SignalsHandler()
{
	delete snSignal;
}

void SignalsHandler::handleSignal()
{
	snSignal->setEnabled(false);
	char signal_number;
	::read(signalfd[1], &signal_number, sizeof(signal_number));

	qDebug() << "Handling signal" << int(signal_number);
	emit signalReceived(int(signal_number));

	snSignal->setEnabled(true);
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
	screensaver = 0;
	// construct global objects
	bt_global::config = new QHash<GlobalFields, QString>();

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
	bt_global::audio_states = new AudioStateMachine;
	bt_global::sound = new SoundPlayer;
	bt_global::ringtones = new RingtonesManager(RINGTONE_FILE);

#if defined(BT_HARDWARE_X11) || defined(BT_HARDWARE_TS_10)
	// save last click time for the screen saver
	qApp->installEventFilter(new LastClickTime);
	// avoid calibration starting at every boot
	setTimePress(QDateTime::currentDateTime().addSecs(-60));
#endif

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
			clients[id].command = new Client(Client::COMMAND, host, port);
			clients[id].request = new Client(Client::REQUEST, host, port);
		}

	// If it is not defined a main openserver, the main openserver is the local openserver
	if (!clients.contains(MAIN_OPENSERVER))
	{
		monitors[MAIN_OPENSERVER] = new Client(Client::MONITOR);
		clients[MAIN_OPENSERVER].command = new Client(Client::COMMAND);
		clients[MAIN_OPENSERVER].request = new Client(Client::REQUEST);
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
		Client *client_supervisor = new Client(Client::SUPERVISOR);
		client_supervisor->forwardFrame(monitors[MAIN_OPENSERVER]);

		// The supervisor socket is opened only in two cases: when the GUI in in debug
		// mode in order to receive the frames sent using a client like PyCo
		// or when we have the videocall ip enabled. In both situations, the openserver
		// is the local one, so we manage only this case.
		clients[MAIN_OPENSERVER].supervisor = client_supervisor;
	}

	banner::setClients(clients[MAIN_OPENSERVER].command, clients[MAIN_OPENSERVER].request);
	Page::setClients(clients[MAIN_OPENSERVER].command, clients[MAIN_OPENSERVER].request);
	FrameReceiver::setClientsMonitor(monitors);
	device::setClients(clients);

	// When only the main openserver is defined the homepage is showed only when
	// the monitor of the openserver is up. Otherwise the homepage is showed as
	// soon as the configuration is loaded.
	if (clients.count() > 1)
		monitor_ready = true;
	else
	{
		OpenServerManager *manager = device::getManager(MAIN_OPENSERVER);

		connect(manager, SIGNAL(connectionUp()), SLOT(connectionReady()));
	}

	window_container = new WindowContainer(maxWidth(), maxHeight());
	page_container = window_container->centralLayout();
	page_container->blockTransitions(true); // no transitions until homepage is showed
	connect(page_container, SIGNAL(currentPageChanged(Page*)), &bt_global::page_stack, SLOT(currentPageChanged(Page *)));

	rearmWDT();

	calibrating = false;
	pagDefault = NULL;
	home = NULL;
	version = NULL;
	alreadyCalibrated = false;
	alarm_clock_on = false;
	vde_call_active = false;
	last_event_time = 0;
	frozen = false;

	passwordKeypad = NULL;
	pwdOn = false;

	Window *loading = NULL;

	freeze_time = 30;
	screensaver_time = 60;
	screenoff_time = 120;

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

		alreadyCalibrated = true;
#ifdef LAYOUT_TS_3_5
		Calibration *cal = new Calibration(true);
#else
		Calibration *cal = new Calibration;
#endif
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
	delete screensaver;
	delete bt_global::skin;
	delete bt_global::font;

	QMutableHashIterator<int,Clients> it(clients);
	while (it.hasNext())
	{
		it.next();
		delete it.value().command;
		delete it.value().request;
		delete it.value().supervisor;
	}

	QMutableHashIterator<int, Client*> mit(monitors);
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
	if (ok && (num - TS_NUM_BASE_ADDRESS >= 0))
		(*config)[TS_NUMBER] = QString::number(num - TS_NUM_BASE_ADDRESS);
	else
	{
		qWarning() << "Unable to read the serial number of the touchscreen";
		(*config)[TS_NUMBER] = QString::number(0);
	}

	// TS 10'' source and amplifier addresses
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

#ifdef CONFIG_TS_3_5
	QDomNode display_node = getChildWithId(getPageNode(SETTINGS), QRegExp("item\\d{1,2}"), DISPLAY);
#endif

#ifdef CONFIG_TS_3_5
	if (!display_node.isNull())
	{
		QDomElement n = getElement(display_node, "brightness/level");
		if (!n.isNull())
			bt_global::display->setInactiveBrightness(static_cast<BrightnessLevel>(n.text().toInt()));
	}
#endif

	ScreenSaver::Type type = ScreenSaver::LINES; // default screensaver
#ifdef CONFIG_TS_3_5
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
#endif
	bt_global::display->current_screensaver = type;

	window_container->homeWindow()->loadConfiguration();

#ifdef CONFIG_TS_3_5
	QDomNode gui_node = getConfElement("displaypages");
	QDomNode pagemenu_home = getChildWithId(gui_node, QRegExp("pagemenu(\\d{1,2}|)"), 0);
	// homePage must be built after the loading of the configuration,
	// to ensure that values displayed (by homePage or its child pages)
	// is in according with saved values.
	home = new HomePage(pagemenu_home);

	QString orientation = getTextChild(gui_node, "orientation");
	if (!orientation.isNull())
		setOrientation(orientation);
#else
	QDomNode gui_node = getConfElement("gui");
	// TODO read the id from the <homepage> node
	QDomNode pagemenu_home = getHomepageNode();
	home = new HomePage(pagemenu_home);

	QDomNode video_node = getPageNode(VIDEODOORENTRY);
	// Touch X can receive calls even if the videodoorentry section is not
	// configured (but the configuration specifies it as an internal place).
	// In this case, we create an "empty" version of the VideoDoorEntry page,
	// only to manage the ringtone and the loading of the VCTCallPage/IntercomPage.
	if (video_node.isNull() && !(*bt_global::config)[PI_ADDRESS].isEmpty())
		(void) new VideoDoorEntry;

#endif
	connect(window_container->homeWindow(), SIGNAL(showHomePage()), home, SLOT(showPage()));
	connect(window_container->homeWindow(), SIGNAL(showSectionPage(int)), home, SLOT(showSectionPage(int)));
	connect(home, SIGNAL(iconStateChanged(int,StateButton::Status)), window_container->homeWindow(),
		SLOT(iconStateChanged(int,StateButton::Status)));

	QDomNode home_node = getChildWithName(gui_node, "homepage");
	if (getTextChild(home_node, "isdefined").toInt())
	{
		int id_default = getTextChild(home_node, "id").toInt();
		pagDefault = !id_default ? home : getPage(id_default);
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
		connect(pagDefault, SIGNAL(Closed()), home, SLOT(showPage()));
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

	home->inizializza();
	if (version)
		version->inizializza();

	foreach (Page *p, page_list)
		p->inizializza();

	device::initDevices();

#if !defined(BT_HARDWARE_X11)
	if (static_cast<int>(getTimePress()) * 1000 <= boot_time->elapsed() && !alreadyCalibrated)
	{
		qDebug() << "Boot time" << boot_time->elapsed() << "last press" << static_cast<int>(getTimePress()) * 1000;

		alreadyCalibrated = true;
#ifdef LAYOUT_TS_3_5
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

	home->showPage();
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
	home->showPage();
}

Page *BtMain::homePage()
{
	return home;
}

void BtMain::unrollPages()
{
	int seq_pages = 0;
	if (page_container->currentPage() != pagDefault && page_container->currentPage() != version)
		while (page_container->currentPage() != home)
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

void BtMain::makeActive()
{
	qDebug() << "BtMain::makeActive";
	last_event_time = now();

	if (bt_global::display->currentState() == DISPLAY_OFF ||
		bt_global::display->currentState() == DISPLAY_SCREENSAVER ||
		bt_global::display->currentState() == DISPLAY_FREEZED)
	{
		if (pwdOn)
		{
			if (bt_global::display->currentState() != DISPLAY_FREEZED)
			{
				bt_global::audio_states->removeState(AudioStates::SCREENSAVER);
				emit stopscreensaver(); // emitted both in DISPLAY_OFF and DISPLAY_SCREENSAVER
				if (screensaver && screensaver->isRunning())
					screensaver->stop();
				bt_global::display->setState(DISPLAY_FREEZED);
				freeze(true);
			}
		}
		else
			freeze(false);
	}
}

void BtMain::setScreenSaverTimeouts(int screensaver_start, int blank_screen)
{
	qDebug() << "Screensaver time" << screensaver_start << "blank screen" << blank_screen;

	screenoff_time = blank_screen;
	screensaver_time = screensaver_start;
}

int BtMain::freezeTime()
{
	if (blankScreenTime())
		return qMin(qMin(freeze_time, screensaverTime()), blankScreenTime());
	else
		return qMin(freeze_time, screensaverTime());
}

int BtMain::screensaverTime()
{
	return screensaver_time;
}

int BtMain::blankScreenTime()
{
	ScreenSaver::Type target_screensaver = bt_global::display->currentScreenSaver();

	if (screenoff_time == 0)
		return 0;
	else if (target_screensaver == ScreenSaver::NONE)
		return screenoff_time - screensaver_time;
	else
		return screenoff_time;
}

void BtMain::checkScreensaver()
{
	rearmWDT();

#if defined(BT_HARDWARE_X11) || defined(BT_HARDWARE_TS_10)
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

	if (bt_global::display->isForcedOperativeMode())
		return;
	if (alarm_clock_on || calibrating || vde_call_active)
		return;

	ScreenSaver::Type target_screensaver = bt_global::display->currentScreenSaver();
#ifdef BT_HARDWARE_TS_3_5
	// When the brightness is set to off in the old hardware the display
	// is not really off, so it is required to use a screensaver to protect
	// the display, even if the screensaver is not visible.
	if (bt_global::display->inactiveBrightness() == BRIGHTNESS_OFF)
		target_screensaver = ScreenSaver::LINES;
#endif
	int time_press = getTimePress();
	int time = qMin(time_press, abs(int(now() - last_event_time)));

	if (blankScreenTime() != 0 &&
		((bt_global::display->currentState() == DISPLAY_SCREENSAVER && time >= blankScreenTime()) ||
		 (bt_global::display->currentState() == DISPLAY_FREEZED && target_screensaver == ScreenSaver::NONE && time >= blankScreenTime())))
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
		if (bt_global::display->currentState() != DISPLAY_SCREENSAVER)
			bt_global::audio_states->toState(AudioStates::SCREENSAVER);
		bt_global::display->setState(DISPLAY_OFF);
	}
	else if (time >= freezeTime() && getBacklight() && !frozen)
	{
		freeze(true);
	}
	else if (time >= screensaverTime() && target_screensaver != ScreenSaver::NONE)
	{
		if (bt_global::display->currentState() == DISPLAY_OPERATIVE &&
			pagDefault && page_container->currentPage() != pagDefault)
		{
			pagDefault->showPage();
		}

		if (bt_global::display->currentState() == DISPLAY_FREEZED)
		{
			if (screensaver && screensaver->type() != target_screensaver)
			{
				delete screensaver;
				screensaver = 0;
			}

			if (!screensaver)
				screensaver = getScreenSaver(target_screensaver);

			// TODO move the code until the end of the block to PageStack and/or ScreenSaver
			Page *prev_page = page_container->currentPage();

#ifdef LAYOUT_TS_3_5
			page_container->blockTransitions(true);
			if (pagDefault)
			{
				unrollPages();
				pagDefault->showPage();
			}
			else
			{
				home->showPage();
				// this makes the screen saver go back to prev_page
				// when exited
				bt_global::page_stack.currentPageChanged(prev_page);
			}

			window_container->homeWindow()->showWindow();
			page_container->blockTransitions(false);
#endif
			if (window_container->currentWindow() == window_container->homeWindow())
				qDebug() << "start screensaver:" << target_screensaver << "on:" << page_container->currentPage();
			else
				qDebug() << "start screensaver:" << target_screensaver << "on:" << window_container->currentWindow();
			screensaver->start(window_container->currentWindow());
			emit startscreensaver(prev_page);
			bt_global::display->setState(DISPLAY_SCREENSAVER);
			bt_global::audio_states->toState(AudioStates::SCREENSAVER);
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
	if (ev->type() == QEvent::MouseButtonPress || ev->type() == QEvent::MouseButtonDblClick ||
		ev->type() == QEvent::MouseMove || ev->type() == QEvent::Enter || ev->type() == QEvent::Leave)
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

	if (!frozen)
	{
		// set last_event_time first to avoid re-freezing if one of the actions
		// below re-enters the event loop
		last_event_time = now();

		// in this case the screeensaver is not running, to reduce power consumption,
		// but the state is as if it were
		if (bt_global::display->currentState() == DISPLAY_OFF)
		{
			bt_global::audio_states->removeState(AudioStates::SCREENSAVER);
			emit stopscreensaver();
		}

		if (screensaver && screensaver->isRunning())
		{
			bt_global::audio_states->removeState(AudioStates::SCREENSAVER);
			emit stopscreensaver();
			screensaver->stop();
		}

		bt_global::display->setState(DISPLAY_OPERATIVE);

		if (pwdOn)
		{
			if (!passwordKeypad)
			{
				passwordKeypad = new KeypadWindow(Keypad::HIDDEN);
				connect(passwordKeypad, SIGNAL(Closed()), SLOT(testPassword()));
			}
			bt_global::page_stack.showKeypad(passwordKeypad);
			passwordKeypad->showWindow();
		}
		qApp->removeEventFilter(this);
	}
	else
	{
		bt_global::display->setState(DISPLAY_FREEZED);
		qApp->installEventFilter(this);
	}
}

void BtMain::setPassword(bool enable, QString password)
{
	pwdOn = enable;
	pwd = password;
	qDebug() << "new password:" << pwd << "active:" << pwdOn;
}

void BtMain::testPassword()
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
			passwordKeypad = NULL;
			bt_global::page_stack.closeWindow(t);
			t->disconnect();
			t->deleteLater();
		}
	}
}

bool BtMain::calibrating = false;

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

bool BtMain::isCalibrating()
{
	return calibrating;
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

// The global definition of btmain pointer
BtMain *bt_global::btmain;

