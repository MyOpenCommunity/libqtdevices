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
#include "generic_functions.h" // rearmWDT, getTimePress, setOrientation, getBacklight
#include "xml_functions.h" // getPageNode, getElement, getChildWithId, getTextChild
#include "calibrate.h"
#include "genpage.h"
#include "openclient.h"
#include "version.h"
#include "keypad.h"
#include "screensaver.h"
#include "displaycontrol.h" // (*bt_global::display)
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
	boot_time = new QTime();
	boot_time->start();
	difSon = 0;
	dm = 0;
	screensaver = 0;
	// construct global objects
	bt_global::config = new QHash<GlobalFields, QString>();

	loadGlobalConfig();
	qDebug("parte BtMain");

	QString font_file = QString(MY_FILE_CFG_FONT).arg((*bt_global::config)[LANGUAGE]);
	bt_global::font = new FontManager(font_file);
	bt_global::display = new DisplayControl;

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
	pwdOn = false;
	version = new Version;
	version->setModel((*bt_global::config)[MODEL]);

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
		(*bt_global::display)._setBrightness(level);

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
		(*bt_global::display).current_screensaver = type;

		QDomNode displaypages = getConfElement("displaypages");
		if (!displaypages.isNull())
		{
			QDomNode pagemenu_home = getChildWithId(displaypages, QRegExp("pagemenu(\\d{1,2}|)"), 0);
			// homePage must be built after the loading of the configuration,
			// to ensure that values displayed (by homePage or its child pages)
			// is in according with saved values.
			Home = new homePage(pagemenu_home);
			QObject::connect(bt_global::btmain->client_monitor, SIGNAL(frameIn(char *)), Home, SLOT(gestFrame(char *)));

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

		// TODO: read the transition effect from configuration
		//Page::installTransitionWidget(new BlendingTransition(&main_window));
		//Page::installTransitionWidget(new MosaicTransition(&main_window));
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
	{
		// setStyleSheet may be slow, try to avoid the watchdog timeout
		rearmWDT();
		qApp->setStyleSheet(style);
		rearmWDT();
	}

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
}

static unsigned long now()
{
	return time(NULL);
}

void BtMain::unrollPages()
{
	int seq_pages = 0;
	if (Page::currentPage() != pagDefault && Page::currentPage() != version)
		while (Page::currentPage() != Home)
		{
			Page::currentPage()->forceClosed();
			++seq_pages;
			// To avoid infinite loop, we assume that the application
			// can have a maximum number of sequential pages in the
			// navigation equal to 50.
			if (seq_pages > 50)
			{
				qWarning() << "Maximum number of sequential pages reached with" << Page::currentPage();
				break;
			}
		}
}

void BtMain::setPreviousPage(Page *page)
{
	prev_page = page;
}

void BtMain::showScreensaverIfNeeded()
{
	if (screenSaverRunning())
		screensaver->target()->showPage();
}

void BtMain::gesScrSav()
{
	unsigned long tiempo, tiempo_press;
	rearmWDT();

	if ((*bt_global::display).isForcedOperativeMode())
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
				if (!pd_shown && (*bt_global::display).currentState() == DISPLAY_OPERATIVE)
				{
					pd_shown = true;
					if (pagDefault)
						pagDefault->showPage();
				}
			}

			if  (tiempo >= 65 && (*bt_global::display).currentState() == DISPLAY_FREEZED)
			{
				ScreenSaver::Type target_screensaver = (*bt_global::display).currentScreenSaver();
				// When the brightness is set to off in the old hardware the display
				// is not really off, so it is required to use a screensaver to protect
				// the display, even if the screensaver is not visible.
				if ((*bt_global::display).currentBrightness() == BRIGHTNESS_OFF)
					target_screensaver = ScreenSaver::LINES;

				if (screensaver && screensaver->type() != target_screensaver)
				{
					delete screensaver;
					screensaver = 0;
				}

				if (!screensaver)
					screensaver = getScreenSaver(target_screensaver);

				Page *target = pagDefault ? pagDefault : Home;
				prev_page = Page::currentPage();

				Page::blockTransitions(true);
				if (target == pagDefault)
				{
					unrollPages();
					prev_page = target;
				}

				target->showPage();
				Page::blockTransitions(false);
				qDebug() << "start screensaver:" << target_screensaver << "on:" << main_window.currentWidget();
				screensaver->start(target);
				emit startscreensaver(prev_page);
				(*bt_global::display).setState(DISPLAY_SCREENSAVER);
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
		(*bt_global::display).setState(DISPLAY_OPERATIVE);
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
				connect(tasti, SIGNAL(Closed()), this, SLOT(testPwd()));
			}
			if (main_window.currentWidget() != tasti)
				tasti->showPage();
		}
		qApp->removeEventFilter(this);
	}
	else
	{
		(*bt_global::display).setState(DISPLAY_FREEZED);
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

