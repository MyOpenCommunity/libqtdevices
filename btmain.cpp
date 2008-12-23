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
#include "carico.h"
#include "generic_functions.h"
#include "xml_functions.h"
#include "calibrate.h"
#include "genpage.h"
#include "device_cache.h"
#include "openclient.h"
#include "versio.h"
#include "tastiera.h"
#include "screensaver.h"
#include "thermalmenu.h"
#include "supervisionmenu.h"
#include "brightnesscontrol.h"
#include "specialpage.h"
#include "page.h"

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
	btouch_device_cache.set_clients(client_comandi, client_monitor, client_richieste);
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

	datiGen = new versio();
	struct sysinfo info;
	sysinfo(&info);
	qDebug("uptime: %d",(int)info.uptime);
	qDebug("touch ago: %d",(int)getTimePress());

	if ((QFile::exists("/etc/pointercal")) && ((info.uptime>200) || ((unsigned long)(info.uptime-1)<=(unsigned long)getTimePress())))
	{
		datiGen->show();
		waitBeforeInit();
	}
	else
	{
		calib = new Calibrate(NULL, 1);
		calib->showFullScreen();
		connect(calib, SIGNAL(fineCalib()), this, SLOT(waitBeforeInit()));
		connect(calib, SIGNAL(fineCalib()), datiGen,SLOT(showFullScreen()));
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

bool BtMain::loadSkin(QString xml_file)
{
	if (QFile::exists(xml_file))
	{
		QFile extra(xml_file);
		QDomDocument dom;
		if (dom.setContent(&extra))
		{
			QDomElement style = getElement(dom, "extra/stylesheet");
			if (!style.isNull())
			{
				qApp->setStyleSheet(style.text());
				return true;
			}
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
		Carico *p = new Carico(page_node);
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

	page->hide();
	return page;
}

bool BtMain::loadConfiguration(QString cfg_file)
{
	if (QFile::exists(cfg_file))
	{
		QDomNode pagemenu_home = getChildWithId(getConfElement("displaypages"),
			QRegExp("pagemenu(\\d{1,2}|)"), 0);
		Home = new homePage(pagemenu_home);

		QDomNode home_node = getConfElement("displaypages/homepage");
		if (getTextChild(home_node, "isdefined").toInt())
		{
			int id_default = getTextChild(home_node, "id").toInt();
			pagDefault = !id_default ? Home : getPage(id_default);
		}

		QDomElement addr = getConfElement("setup/scs/coordinate_scs/diag_addr");
		bool ok;
		if (!addr.isNull())
			datiGen->setAddr(addr.text().toInt(&ok, 16) - 768);

		QDomElement model = getConfElement("setup/generale/modello");
		if (!model.isNull())
			datiGen->setModel(model.text());

		QDomElement orientation = getConfElement("displaypages/orientation");
		if (!orientation.isNull())
			setOrientation(orientation.text());

		// read screensaver type from config file
		QDomElement screensaver_type_node = getConfElement("displaypages/screensaver/type");
		int screensaver_type = ScreenSaver::LINES;
		if (screensaver_type_node.isNull())
			qWarning("Type of screeensaver not found!");
		else
			screensaver_type = screensaver_type_node.text().toInt();
		screensaver = getScreenSaver(static_cast<ScreenSaver::Type>(screensaver_type));

		// read configuration for brightness
		BrightnessControl::DefautPolicy conf_brightness_policy = BrightnessControl::POLICY_HIGH;
		if (screensaver_type == ScreenSaver::NONE)
			conf_brightness_policy = BrightnessControl::POLICY_OFF;
		else
		{
			// brightness conf is in IMPOSTAZIONI page
			QDomNode conf_page_root = getPageNode(IMPOSTAZIONI);
			QDomNode bright_item_node = getChildWithId(conf_page_root, QRegExp("item\\d{1,2}"), BRIGHTNESS);
			if (!bright_item_node.isNull())
			{
				QDomNode policy = bright_item_node.namedItem("liv").toElement();
				if (!policy.isNull())
					conf_brightness_policy = static_cast<BrightnessControl::DefautPolicy>(
							policy.toElement().text().toInt());
			}
		}
		BrightnessControl::instance()->setBrightnessPolicy(conf_brightness_policy);
		BrightnessControl::instance()->setState(ON);

		return true;
	}
	return false;
}

void BtMain::hom()
{
	datiGen->inizializza();

	if (loadConfiguration(CFG_FILE))
	{
		if (screensaver)
			screensaver->hide();
		hide();
	}

	if (!loadSkin(EXTRA_FILE))
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

	Page *p;
	foreach (p, page_list)
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
	Home->showFullScreen();
	datiGen->hide();
	btouch_device_cache.init_devices();

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
			BrightnessControl::instance()->setState(ON);
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
			BrightnessControl::instance()->setState(ON);
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
			BrightnessControl::instance()->setState(ON);
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
			BrightnessControl::instance()->setState(ON);
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

			if  (tiempo >= 65 && screensaver && screensaver->isHidden())
			{
				screensaver->showFullScreen();
				BrightnessControl::instance()->setState(SCREENSAVER);
			}

			// FIXME: do we need to change tempo1 if there's no screensaver?
			if (screensaver && !screensaver->isHidden())
			{
				QWidget *target = pagDefault ? pagDefault : Home;
				screensaver->refresh(QPixmap::grabWidget(target,0,0,MAX_WIDTH,MAX_HEIGHT));
				tempo1->start(100);
			}
		}
		else
			if (screensaver)
				screensaver->hide();
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
		BrightnessControl::instance()->setState(ON);
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
		BrightnessControl::instance()->setState(ON);
		if (screensaver)
			screensaver->hide();
		if (pwdOn)
		{
			if (!tasti)
			{
				tasti = new tastiera(NULL);
				tasti->setMode(tastiera::HIDDEN);
				tasti->showFullScreen();
				connect(tasti, SIGNAL(Closed(char*)), this, SLOT(testPwd(char*)));
			}
		}
		qApp->removeEventFilter(this);
	}
	else
	{
		BrightnessControl::instance()->setState(OFF);
		qApp->installEventFilter(this);
	}
}

void BtMain::setPwd(bool b, QString p)
{
	pwdOn = b;
	pwd = p;
	qDebug() << "BtMain nuova pwd = " << pwd << "-" << pwdOn;
}

void BtMain::testPwd(char* p)
{
	if (p)
	{
		if (p != pwd)
		{
			tasti->showFullScreen();
			qDebug() << "pwd ko" << p << "doveva essere " << pwd;
		}
		else
		{
			delete tasti;
			tasti = NULL;
		}
	}
	else
		tasti->showFullScreen();
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

QPixmap* BtMain::getIcon(QString name, const char *format, Qt::ImageConversionFlags flags)
{
	return icons_library.getIcon(name, format, flags);
}

void BtMain::sendFrame(QString frame)
{
	QByteArray buf = frame.toAscii();
	client_comandi->ApriInviaFrameChiudi(buf.constData());
}

void BtMain::sendInit(QString frame)
{
	QByteArray buf = frame.toAscii();
	client_richieste->ApriInviaFrameChiudi(buf.constData());
}
