#include "btmain.h"
#include "main.h" // bt_global::config
#include "homepage.h"
#include "generic_functions.h" // rearmWDT, getTimePress, setOrientation, getBacklight
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
	bt_global::display._setBrightness(BRIGHTNESS_NORMAL);
	bt_global::display.setState(DISPLAY_OPERATIVE);
	boot_time = new QTime();
	boot_time->start();
	difSon = 0;
	dm = 0;
	screensaver = 0;
	loadGlobalConfig();
	qDebug("parte BtMain");

	QString font_file = QString(MY_FILE_CFG_FONT).arg(bt_global::config[LANGUAGE]);
	bt_global::font = new FontManager(font_file);

	client_monitor = new Client(Client::MONITOR);
	client_comandi = new Client(Client::COMANDI);
	client_richieste = new Client(Client::RICHIESTE);
	banner::setClients(client_comandi, client_richieste);
	Page::setClients(client_comandi, client_richieste);
	Page::setMainWindow(&main_window);
	main_window.showFullScreen();
	main_window.setFixedSize(MAX_WIDTH, MAX_HEIGHT);
	// TODO: this ugly workaround is needed because the QStackedWidget in some ways
	// invalidate the first widget inserted. FIX it asap!
	main_window.addWidget(new QWidget);
	device::setClients(client_comandi, client_richieste, client_monitor);
	connect(client_comandi, SIGNAL(frameToAutoread(char*)), client_monitor,SIGNAL(frameIn(char*)));
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
	pwdOn = 0;
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
		calib = new Calibrate(NULL, 1);
		calib->showFullScreen();
		connect(calib, SIGNAL(fineCalib()), this, SLOT(waitBeforeInit()));
		connect(calib, SIGNAL(fineCalib()), version, SLOT(showPage()));
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

		QDomNode displaypages = getConfElement("displaypages");
		if (!displaypages.isNull())
		{
			QDomNode pagemenu_home = getChildWithId(displaypages, QRegExp("pagemenu(\\d{1,2}|)"), 0);
			Home = new homePage(pagemenu_home);

			QDomNode home_node = getChildWithName(displaypages, "homepage");
			if (getTextChild(home_node, "isdefined").toInt())
			{
				int id_default = getTextChild(home_node, "id").toInt();
				pagDefault = !id_default ? Home : getPage(id_default);
			}

			QString orientation = getTextChild(displaypages, "orientation");
			if (!orientation.isNull())
				setOrientation(orientation);
		}
		else
			qFatal("displaypages node not found on xml config file!");


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
		}
		bt_global::display.current_screensaver = type;

		// TODO: read the transition effect from configuration
		Page::installTransitionWidget(new BlendingTransition(&main_window));
		return true;
	}
	return false;
}

void BtMain::hom()
{
	version->inizializza();
	bt_global::skin = new SkinManager(SKIN_FILE);

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
	connect(client_monitor,SIGNAL(frameIn(char *)),version,SLOT(gestFrame(char *)));

	Home->inizializza();
	if (version)
		version->inizializza();

	foreach (Page *p, page_list)
		p->inizializza();

#if BT_EMBEDDED
	if (static_cast<int>(getTimePress()) * 1000 <= boot_time->elapsed() && !alreadyCalibrated)
	{
		calib = new Calibrate(NULL, 1);
		calib->showFullScreen();
		connect(calib, SIGNAL(fineCalib()),Home,SLOT(showPage()));
		alreadyCalibrated = true;
	}
#endif
}

void BtMain::myMain()
{
	qDebug("entro MyMain");

	init();
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

void BtMain::gesScrSav()
{
	unsigned long tiempo, tiempo_press;
	rearmWDT();

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
				if (!pd_shown)
				{
					pd_shown = true;
					if (pagDefault)
						pagDefault->showPage();
				}
			}

			if  (tiempo >= 65 && (!screensaver || !screensaver->isRunning()) && bt_global::display.screenSaverActive())
			{
				ScreenSaver::Type current_screensaver = bt_global::display.currentScreenSaver();
				if (current_screensaver != ScreenSaver::NONE)
				{
					if (screensaver && screensaver->type() != current_screensaver)
					{
						delete screensaver;
						screensaver = 0;
					}

					if (!screensaver)
						screensaver = getScreenSaver(current_screensaver);

					Page *target = pagDefault ? pagDefault : Home;
					if (target != pagDefault)
					{
						prev_page = static_cast<Page *>(main_window.currentWidget());
						// don't use showPage() because transition doesn't make sense here
						main_window.setCurrentWidget(target);
					}
					screensaver->start(target);
					bt_global::display.setState(DISPLAY_SCREENSAVER);
				}
			}
		}
		else if (screensaver && screensaver->isRunning())
		{
			Page *target = screensaver->target();
			if (target != pagDefault)
				main_window.setCurrentWidget(prev_page);
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
				main_window.setCurrentWidget(prev_page);
			screensaver->stop();
		}
		if (pwdOn)
		{
			if (!tasti)
			{
				tasti = new Keypad();
				tasti->setMode(Keypad::HIDDEN);
				tasti->showPage();
				connect(tasti, SIGNAL(Closed()), this, SLOT(testPwd()));
			}
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
	qDebug() << "BtMain nuova pwd = " << pwd << "-" << pwdOn;
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

