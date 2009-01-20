/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** BtMain.cpp
 **
 **Apertura pagina iniziale e associazione tasti-sottomenù
 **
 ****************************************************************/

#include "btmain.h"
#include "main.h"
#include "homepage.h"
#include "sottomenu.h"
#include "sounddiffusion.h"
#include "multisounddiff.h"
#include "videoentryphone.h"
#include "antintrusion.h"
#include "automation.h"
#include "lighting.h"
#include "scenario.h"
#include "settings.h"
#include "loads.h"
#include "generic_functions.h"
#include "xml_functions.h"
#include "calibrate.h"
#include "genpage.h"
#include "openclient.h"
#include "version.h"
#include "keypad.h"
#include "screensaver.h"
#include "thermalmenu.h"
#include "supervisionmenu.h"
#include "displaycontrol.h" // bt_global::display
#include "specialpage.h"
#include "page.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "device.h"


#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QWSServer>
#include <QObject>
#include <QBitmap>
#include <QPixmap>
#include <QString>
#include <QTimer>
#include <QFile>

#include <sys/sysinfo.h>

#define CFG_FILE MY_FILE_USER_CFG_DEFAULT


BtMain::BtMain(QWidget *parent) : QWidget(parent), screensaver(0)
{
	qDebug("parte BtMain");
	QWSServer::setCursorVisible(false);

	client_monitor = new Client(Client::MONITOR);
	client_comandi = new Client(Client::COMANDI);
	client_richieste = new Client(Client::RICHIESTE);
	banner::setClients(client_comandi, client_richieste);
	Page::setClients(client_comandi, client_richieste);
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
	Home = NULL;
	screen = NULL;
	alreadyCalibrated = false;
	svegliaIsOn = false;
	tiempo_last_ev = 0;
	pd_shown = false;

	tasti = NULL;
	pwdOn = 0;

	datiGen = new Version();
	struct sysinfo info;
	sysinfo(&info);
	qDebug("uptime: %d",(int)info.uptime);
	qDebug("touch ago: %d",(int)getTimePress());

	if ((QFile::exists("/etc/pointercal")) && ((info.uptime>200) || ((unsigned long)(info.uptime-1)<=(unsigned long)getTimePress())))
	{
		datiGen->showPage();
		waitBeforeInit();
	}
	else
	{
		calib = new Calibrate(NULL, 1);
		calib->showFullScreen();
		connect(calib, SIGNAL(fineCalib()), this, SLOT(waitBeforeInit()));
		connect(calib, SIGNAL(fineCalib()), datiGen, SLOT(showPage()));
		alreadyCalibrated = true;
	}
}

BtMain::~BtMain()
{
	if (screensaver)
		delete screensaver;
}

void BtMain::waitBeforeInit()
{
	QTimer::singleShot(200, this, SLOT(hom()));
}

bool BtMain::loadStyleSheet(QString filename)
{
	if (QFile::exists(filename))
	{
		QFile file(filename);
		if (file.open(QFile::ReadOnly))
		{
			qApp->setStyleSheet(file.readAll());
			file.close();
			return true;
		}
	}
	return false;
}

Page *BtMain::getPage(int id)
{
	QDomNode page_node = getPageNode(id);
	if (page_node.isNull())
		return 0;

	// A section page can be built only once.
	if (page_list.contains(id))
		return page_list[id];

	Page *page = 0;
	switch (id)
	{
	case AUTOMAZIONE:
	{
		Automation *p = new Automation(page_node);
		p->forceDraw();
		page = p;
		break;
	}
	case ILLUMINAZIONE:
	{
		Lighting *p = new Lighting(page_node);
		p->forceDraw();
		connect(p, SIGNAL(richStato(QString)), client_richieste, SLOT(richStato(QString)));
		page = p;
		break;
	}
	case ANTIINTRUSIONE:
	{
		Antintrusion *p = new Antintrusion(page_node);
		p->draw();
		connect(client_comandi, SIGNAL(openAckRx()), p, SIGNAL(openAckRx()));
		connect(client_comandi, SIGNAL(openNakRx()), p, SIGNAL(openNakRx()));
		connect(client_monitor, SIGNAL(frameIn(char *)), p, SLOT(gesFrame(char *)));
		page = p;
		break;
	}
	case CARICHI:
	{
		Loads *p = new Loads(page_node);
		p->forceDraw();
		page = p;
		break;
	}
	case TERMOREGOLAZIONE:
	case TERMOREG_MULTI_PLANT:
	{
		ThermalMenu *p = new ThermalMenu(page_node);
		p->forceDraw();
		connect(client_monitor, SIGNAL(frameIn(char *)), p, SIGNAL(gestFrame(char *)));
		page = p;
		break;
	}
	case DIFSON:
	{
		SoundDiffusion *p = new SoundDiffusion(page_node);
		p->draw();
		connect(client_monitor, SIGNAL(frameIn(char *)), p, SLOT(gestFrame(char *)));
		page = p;
		break;
	}
	case DIFSON_MULTI:
	{
		MultiSoundDiff *p = new MultiSoundDiff(page_node);
		p->forceDraw();
		connect(client_monitor, SIGNAL(frameIn(char *)), p, SLOT(gestFrame(char *)));
		page = p;
		break;
	}
	case SCENARI:
	case SCENARI_EVOLUTI:
	{
		Scenario *p = new Scenario(page_node);
		p->forceDraw();
		page = p;
		break;
	}
	case IMPOSTAZIONI:
	{
		Settings *p = new Settings(page_node);
		p->forceDraw();
		connect(client_monitor, SIGNAL(frameIn(char *)), p, SIGNAL(gestFrame(char *)));
		connect(p, SIGNAL(startCalib()), this, SLOT(startCalib()));
		connect(p, SIGNAL(endCalib()), this, SLOT(endCalib()));
		page = p;
		break;
	}
	case VIDEOCITOFONIA:
	{
		VideoEntryPhone *p = new VideoEntryPhone(page_node);
		p->forceDraw();
		page = p;
		break;
	}
	case SUPERVISIONE:
	{
		SupervisionMenu *p = new SupervisionMenu(page_node);
		p->forceDraw();
		connect(client_monitor, SIGNAL(frameIn(char *)), p, SIGNAL(gestFrame(char *)));
		connect(p, SIGNAL(richStato(QString)), client_richieste, SLOT(richStato(QString)));
		page = p;
		break;
	}
	case SPECIAL:
	{
		SpecialPage *p = new SpecialPage(page_node);
		connect(client_monitor, SIGNAL(frameIn(char *)), p, SLOT(gestFrame(char *)));
		connect(p, SIGNAL(Closed()), p, SLOT(hide()));
		page = p;
		break;
	}
	default:
		qFatal("Page %d not found on xml config file!", id);
	}

	page_list[id] = page;
	page->hide();
	return page;
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
				datiGen->setAddr(addr.text().toInt(&ok, 16) - 768);

			QDomElement model = getElement(setup, "generale/modello");
			if (!model.isNull())
				datiGen->setModel(model.text());
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
		bt_global::display.setState(DISPLAY_OPERATIVE);

		ScreenSaver::Type type = ScreenSaver::LINES; // default screensaver
		if (!display_node.isNull())
		{
			QDomElement n = getElement(display_node, "screensaver/type");
			if (!n.isNull())
				type = static_cast<ScreenSaver::Type>(n.text().toInt());
		}
		bt_global::display.current_screensaver = type;
		return true;
	}
	return false;
}

void BtMain::hom()
{
	datiGen->inizializza();

	if (loadConfiguration(CFG_FILE))
		hide();

	// The stylesheet can contain some references to dynamic properties,
	// so loading of css must be done after setting these properties (otherwise
	// it might be necessary to force a stylesheet recomputation).
	if (!loadStyleSheet(CSS_FILE))
		qWarning("Unable to load skin file!");

	config_loaded = true;
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);

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
	connect(client_monitor,SIGNAL(frameIn(char *)),datiGen,SLOT(gestFrame(char *))); 

	Home->inizializza();
	if (datiGen)
		datiGen->inizializza();

	foreach (Page *p, page_list)
		p->inizializza();

	struct sysinfo info;
	sysinfo(&info);
	if (info.uptime < 200 && ((unsigned long)(info.uptime - 1) > (unsigned long)getTimePress()) && !alreadyCalibrated)
	{
		Home->hide();
		calib = new Calibrate(NULL, 1);
		calib->showFullScreen();
		connect(calib, SIGNAL(fineCalib()),Home,SLOT(showFullScreen()));
		alreadyCalibrated = true;
	}
}

void BtMain::myMain()
{
	qDebug("entro MyMain");

	init();
	Home->showPage();
	datiGen->hide();
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
			freeze(false);
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
					screensaver->start(target);
					bt_global::display.setState(DISPLAY_SCREENSAVER);
				}
			}
		}
		else if (screensaver && screensaver->isRunning())
			screensaver->stop();
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
		bt_global::display.setState(DISPLAY_OPERATIVE);
		tempo1->start(2000);
		bloccato = false;
	}
}

bool BtMain::eventFilter(QObject *obj, QEvent *ev)
{
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
			screensaver->stop();
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
	if (!p.isEmpty())
	{
		if (p != pwd)
		{
			tasti->resetText();
			qDebug() << "pwd ko" << p << "doveva essere " << pwd;
		}
		else
		{
			tasti->disconnect();
			tasti->hide();
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

