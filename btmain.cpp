#include "btmain.h"
#include "main.h" // bt_global::config
#include "homepage.h"
#include "hardware_functions.h" // rearmWDT, getTimePress, setOrientation, getBacklight
#include "xml_functions.h" // getPageNode, getElement, getChildWithId, getTextChild
#include "calibrate.h"
#include "genpage.h"
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
#include "mainwidget.h"

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


#define CFG_FILE MY_FILE_USER_CFG_DEFAULT

namespace
{
	void setConfigValue(QDomNode root, QString path, QString &dest)
	{
		QDomElement n = getElement(root, path);
		if (!n.isNull())
			dest = n.text();
	}
}


BtMain::BtMain()
{
	boot_time = new QTime();
	boot_time->start();
	difSon = 0;
	dm = 0;
	screensaver = 0;
	loadGlobalConfig();
	qDebug("parte BtMain");

	QString font_file = QString(MY_FILE_CFG_FONT).arg(bt_global::config[LANGUAGE]);
	bt_global::font = new FontManager(font_file);
	bt_global::skin = new SkinManager(SKIN_FILE);

	client_monitor = new Client(Client::MONITOR);
	client_comandi = new Client(Client::COMANDI);
	client_richieste = new Client(Client::RICHIESTE);
#if DEBUG
	client_supervisor = new Client(Client::SUPERVISOR);
	client_supervisor->forwardFrame(client_monitor);
#endif
	FrameReceiver::setClientMonitor(client_monitor);
	banner::setClients(client_comandi, client_richieste);
	Page::setClients(client_comandi, client_richieste);
	root_widget = new RootWidget(maxWidth(), maxHeight());
	page_container = root_widget->centralLayout();
	page_container->blockTransitions(true); // no transitions until homepage is showed
	connect(page_container, SIGNAL(currentPageChanged(Page*)), SLOT(currentPageChanged(Page*)));
	device::setClients(client_comandi, client_richieste);

	connect(client_monitor,SIGNAL(monitorSu()), SLOT(monitorReady()));

	monitor_ready = false;
	config_loaded = false;

	rearmWDT();

	calibrating = false;
	event_unfreeze = false;
	firstTime = true;
	pagDefault = NULL;
	prev_page = NULL;
	Home = NULL;
	screen = NULL;
	alreadyCalibrated = false;
	svegliaIsOn = false;
	tiempo_last_ev = 0;
	pd_shown = false;

	tasti = NULL;
	pwdOn = false;

	version = new Version;
	version->setModel(bt_global::config[MODEL]);

#if BT_EMBEDDED
	if (QFile::exists("/etc/pointercal"))
	{
		version->showPage();
		waitBeforeInit();
	}
	else
	{
#if !defined(BT_HARDWARE_X11)
		calib = new Calibrate(NULL, 1);
		calib->showFullScreen();
		connect(calib, SIGNAL(fineCalib()), this, SLOT(waitBeforeInit()));
		connect(calib, SIGNAL(fineCalib()), version, SLOT(showPage()));
#endif
		alreadyCalibrated = true;
	}
#else
	version->showPage();
	waitBeforeInit();
#endif
}

BtMain::~BtMain()
{
	delete screensaver;
	delete bt_global::skin;
	delete bt_global::font;
	delete client_comandi;
	delete client_monitor;
	delete client_richieste;
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
	setConfigValue(n, "language", config[LANGUAGE]);
	setConfigValue(n, "clock/dateformat", config[DATE_FORMAT]);
	setConfigValue(n, "modello", config[MODEL]);
	setConfigValue(n, "nome", config[NAME]);
}

void BtMain::waitBeforeInit()
{
	QTimer::singleShot(200, this, SLOT(hom()));
}

bool BtMain::loadConfiguration(QString cfg_file)
{
	if (QFile::exists(cfg_file))
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

		QDomNode display_node = getChildWithId(getPageNode(IMPOSTAZIONI), QRegExp("item\\d{1,2}"), DISPLAY);

		BrightnessLevel level = BRIGHTNESS_NORMAL; // default brightness
		if (!display_node.isNull())
		{
			QDomElement n = getElement(display_node, "brightness/level");
			if (!n.isNull())
				level = static_cast<BrightnessLevel>(n.text().toInt());
		}
		bt_global::display._setBrightness(level);

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

#ifdef CONFIG_BTOUCH
		QDomNode displaypages = getConfElement("displaypages");
		QDomNode pagemenu_home = getChildWithId(displaypages, QRegExp("pagemenu(\\d{1,2}|)"), 0);
		// homePage must be built after the loading of the configuration,
		// to ensure that values displayed (by homePage or its child pages)
		// is in according with saved values.
		Home = new HomePage(pagemenu_home);

		QDomNode home_node = getChildWithName(displaypages, "homepage");
		if (getTextChild(home_node, "isdefined").toInt())
		{
			int id_default = getTextChild(home_node, "id").toInt();
			pagDefault = !id_default ? Home : getPage(id_default);
		}

		QString orientation = getTextChild(displaypages, "orientation");
		if (!orientation.isNull())
			setOrientation(orientation);
#else
		QDomNode gui = getConfElement("gui");
		// TODO read the id from the <homepage> node
		QDomNode pagemenu_home = getHomepageNode();
		Home = new HomePage(pagemenu_home);
		connect(root_widget->mainWidget(), SIGNAL(showHomePage()), Home, SLOT(showPage()));
		connect(root_widget->mainWidget(), SIGNAL(showSectionPage(int)), Home, SLOT(showSectionPage(int)));

		pagDefault = Home;
#endif

		// TODO: read the transition effect from configuration
		page_container->installTransitionWidget(new BlendingTransition);
//		page_container->installTransitionWidget(new MosaicTransition);
		return true;
	}
	return false;
}

void BtMain::hom()
{
	version->inizializza();

	if (!loadConfiguration(CFG_FILE))
		qFatal("Unable to load configuration");

	if (pagDefault)
		connect(pagDefault, SIGNAL(Closed()), Home, SLOT(showPage()));
	// The stylesheet can contain some references to dynamic properties,
	// so loading of css must be done after setting these properties (otherwise
	// it might be necessary to force a stylesheet recomputation).
	QString style = bt_global::skin->getStyle();
	if (style.isNull())
		qWarning("Unable to load skin file!");
	else
		qApp->setStyleSheet(style);

	config_loaded = true;

	if (monitor_ready)
		myMain();
}

void BtMain::monitorReady()
{
	monitor_ready = true;
	if (config_loaded)
		myMain();
}

void BtMain::init()
{
	qDebug("BtMain::init()");

	Home->inizializza();
	if (version)
		version->inizializza();

	foreach (Page *p, page_list)
		p->inizializza();

#if BT_EMBEDDED
	if (static_cast<int>(getTimePress()) * 1000 <= boot_time->elapsed() && !alreadyCalibrated)
	{
#if !defined(BT_HARDWARE_X11)
		calib = new Calibrate(NULL, 1);
		calib->showFullScreen();
		connect(calib, SIGNAL(fineCalib()),Home,SLOT(showPage()));
#endif
		alreadyCalibrated = true;
	}
#endif
}

void BtMain::myMain()
{
	qDebug("entro MyMain");

	init();
	page_container->blockTransitions(false);
	Home->showPage();
	bt_global::devices_cache.init_devices();

	tempo1 = new QTimer(this);
	tempo1->start(2000);
	connect(tempo1,SIGNAL(timeout()),this,SLOT(gesScrSav()));

	tempo2 = new QTimer(this);
	tempo2->start(3000);
	connect(tempo2,SIGNAL(timeout()),this,SLOT(testFiles()));
}

void BtMain::testFiles()
{
	if (QFile::exists(FILE_TEST1))
	{
		if ((screen) && (tiposcreen != genPage::RED))
		{
			delete screen;
			screen = NULL;
		}
		else if (!screen)
		{
			tiposcreen = genPage::RED;
			screen = new genPage(NULL,genPage::RED);
			screen->show();
			qDebug("TEST1");
			bt_global::display.setState(DISPLAY_OPERATIVE);
			tempo1->stop();
		}
	}
	else if (QFile::exists(FILE_TEST2))
	{
		if ((screen) && (tiposcreen != genPage::GREEN))
		{
			delete screen;
			screen = NULL;
		}
		else if (!screen)
		{
			tiposcreen=genPage::GREEN;
			screen = new genPage(NULL,genPage::GREEN);
			screen->show();
			qDebug("TEST2");
			bt_global::display.setState(DISPLAY_OPERATIVE);
			tempo1->stop();
		}
	}
	else if (QFile::exists(FILE_TEST3))
	{
		if ((screen) && (tiposcreen != genPage::BLUE))
		{
			delete screen;
			screen = NULL;
		}
		else if (!screen)
		{
			tiposcreen = genPage::BLUE;
			screen = new genPage(NULL,genPage::BLUE);
			screen->show();
			qDebug("TEST3");
			bt_global::display.setState(DISPLAY_OPERATIVE);
			tempo1->stop();
		}
	}
	else if (QFile::exists(FILE_AGGIORNAMENTO))
	{
		if ((screen) && (tiposcreen != genPage::IMAGE))
		{
			delete screen;
			screen = NULL;
		}
		else if (!screen)
		{
			screen = new genPage(NULL,genPage::IMAGE, IMG_PATH "dwnpage.png");
			tiposcreen = genPage::IMAGE;
			screen->show();
			qDebug("AGGIORNAMENTO");
			bt_global::display.setState(DISPLAY_OPERATIVE);
			tempo1->stop();
		}
	}
	else
	{
		if (screen)
		{
			delete screen;
			screen = NULL;
			tiposcreen = genPage::NONE;
			tempo1->start(2000);
		}
	}
}

static unsigned long now()
{
	return time(NULL);
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

void BtMain::currentPageChanged(Page *p)
{
	if (screensaver && screensaver->isRunning())
		prev_page = p;
}

// TODO: this method will be called when a page has modified the order of pages and
// screensaver. We can do better if we use a more general way
// TODO: this won't work with screensaver DEFORM. I'm leaving it here as a placeholder for a bugfix,
// remove when a more general way to handle this case has been developed
void BtMain::showScreensaverIfNeeded()
{
	if (screenSaverRunning())
		screensaver->target()->showPage();
}

void BtMain::gesScrSav()
{
	unsigned long tiempo, tiempo_press;
	rearmWDT();

	if (bt_global::display.isForcedOperativeMode())
		return;

	tiempo_press = getTimePress();
	if (event_unfreeze)
	{
		tiempo_last_ev = now();
		event_unfreeze = false;
	}
	tiempo = qMin(tiempo_press, (now() - tiempo_last_ev));

	if (!firstTime)
	{
		if  (tiempo >= 30 && getBacklight())
		{
			if (!svegliaIsOn)
			{
				if (!bloccato)
					freeze(true);
				tempo1->start(500);
			}
		}
		else if (tiempo <= 5 && bloccato)
		{
			tempo1->start(2000);
			pd_shown = false;
		}
		if  (tiempo >= 60 && !svegliaIsOn && !calibrating)
		{
			if (pagDefault)
			{
				if (!pd_shown && bt_global::display.currentState() == DISPLAY_OPERATIVE)
				{
					pd_shown = true;
					if (pagDefault)
						pagDefault->showPage();
				}
			}

			if  (tiempo >= 65 && bt_global::display.currentState() == DISPLAY_FREEZED)
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

				Page *target = pagDefault ? pagDefault : Home;
				prev_page = page_container->currentPage();

				page_container->blockTransitions(true);
				if (target == pagDefault)
					unrollPages();

				target->showPage();
				page_container->blockTransitions(false);
				qDebug() << "start screensaver:" << target_screensaver << "on:" << page_container->currentPage();
				screensaver->start(target);
				emit startscreensaver(prev_page);
				bt_global::display.setState(DISPLAY_SCREENSAVER);
			}
		}
		else if (screensaver && screensaver->isRunning())
		{
			Page *target = screensaver->target();
			if (target != pagDefault)
				page_container->setCurrentPage(prev_page);

			screensaver->stop();
		}
	}
	else if (tiempo >= 120)
	{
		freeze(true);
		tempo1->start(500);
		firstTime = false;
	}
	else if (tiempo <= 5)
	{
		firstTime = false;
		tempo1->start(2000);
		bloccato = false;
	}
}

Page *BtMain::getPreviousPage()
{
	return prev_page;
}

bool BtMain::screenSaverRunning()
{
	return screensaver && screensaver->isRunning();
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
	bloccato = b;
	emit freezed(bloccato);

	if (!bloccato)
	{
		event_unfreeze = true;
		bt_global::display.setState(DISPLAY_OPERATIVE);
		if (screensaver && screensaver->isRunning())
		{
			Page *target = screensaver->target();
			if (target != pagDefault)
				page_container->setCurrentPage(prev_page);

			screensaver->stop();
		}
		if (pwdOn)
		{
			if (!tasti)
			{
				tasti = new Keypad();
				tasti->setMode(Keypad::HIDDEN);
				connect(tasti, SIGNAL(Closed()), this, SLOT(testPwd()));
			}
			if (page_container->currentPage() != tasti)
				tasti->showPage();
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
	QString p = tasti->getText();
	qDebug() << "testing password, input text is: " << p;
	if (!p.isEmpty())
	{
		if (p != pwd)
		{
			tasti->resetText();
			qDebug() << "pwd ko" << p << "doveva essere " << pwd;
		}
		else
		{
			qDebug() << "pwd ok!";
			if (pagDefault)
				pagDefault->showPage();
			else
				Home->showPage();
			tasti->disconnect();
			tasti->deleteLater();
			tasti = NULL;
		}
	}
}

void BtMain::svegl(bool b)
{
	qDebug("BtMain::svegl->%d",b);
	svegliaIsOn = b;
}

void BtMain::startCalib()
{
	qDebug("BtMain::startCalib()");
	calibrating = true;
}

void BtMain::endCalib()
{
	qDebug("BtMain::endCalib()");
	calibrating = false;
}

void BtMain::resetTimer()
{
	qDebug("BtMain::ResetTimer()");
	emit resettimer();
}

// The global definition of btmain pointer
BtMain *bt_global::btmain;

