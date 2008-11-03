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
#include "diffsonora.h"
#include "diffmulti.h"
#include "antintrusione.h"
#include "genericfunz.h"
#include "xmlconfhandler.h"
#include "calibrate.h"
#include "btlabel.h"
#include "genpage.h"
#include "device_cache.h"
#include "openclient.h"
#include "versio.h"
#include "tastiera.h"
#include "screensaver.h"
#include "thermalmenu.h"
#include "supervisionmenu.h"

#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QWSServer>
#include <QObject>
#include <QCursor>
#include <QBitmap>
#include <QPixmap>
#include <QTimer>
#include <QFile>

#include <sys/sysinfo.h>

//#define SCREENSAVER_BALLS
#define SCREENSAVER_LINE

#define CFG_FILE "cfg/conf.xml"


BtMain::BtMain(QWidget *parent) : QWidget(parent)
{
	qDebug("parte BtMain");

	// If not compiled for Desktop, turns OFF mouse pointer
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	QWSServer::setCursorVisible(false);
#endif

	client_monitor = new Client(Client::MONITOR);
	client_comandi = new Client(Client::COMANDI);
	client_richieste = new Client(Client::RICHIESTE);
	btouch_device_cache.set_clients(client_comandi, client_monitor, client_richieste);
	connect(client_comandi, SIGNAL(frameToAutoread(char*)), client_monitor,SIGNAL(frameIn(char*)));
	connect(client_monitor,SIGNAL(monitorSu()), SLOT(monitorReady()));

	monitor_ready = false;
	config_loaded = false;

	setBacklight(true);

	rearmWDT();

	calibrating = false;
	event_unfreeze = false;
	firstTime = 1;
	pagDefault = NULL;
	Home = specPage = NULL;
	illumino = scenari = carichi = imposta = automazioni = scenari_evoluti = videocitofonia = NULL;
	supervisione = NULL;
	termo = NULL;
	difSon = NULL;
	dm = NULL;
	antintr = NULL;
	screen = NULL;
	alreadyCalibrated = false;
	svegliaIsOn = false;
	tiempo_last_ev = 0;
	pd_shown = false;
#ifdef SCREENSAVER_LINE
	screensaver = getScreenSaver(ScreenSaver::LINES);
#else
	screensaver = getScreenSaver(ScreenSaver::BALLS);
#endif
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
		calib->show();
		connect(calib, SIGNAL(fineCalib()), this, SLOT(waitBeforeInit()));

#if defined (BTWEB) ||  defined (BT_EMBEDDED)
		connect(calib, SIGNAL(fineCalib()), datiGen,SLOT(showFullScreen()));
#endif
#if !defined (BTWEB) && !defined (BT_EMBEDDED)
		connect(calib, SIGNAL(fineCalib()), datiGen,SLOT(show()));
#endif
		alreadyCalibrated = true;
	}
}

BtMain::~BtMain()
{
	delete screensaver;
}

void BtMain::waitBeforeInit()
{
	QTimer::singleShot(200, this, SLOT(hom()));
}

bool BtMain::loadConfiguration(QString cfg_file)
{
	QColor *bg, *fg1, *fg2;
	readExtraConf(&bg, &fg1, &fg2);
	if (QFile::exists(cfg_file))
	{
		xmlconfhandler handler(this, &Home, &specPage, &scenari_evoluti, &videocitofonia, &illumino,
				&scenari, &carichi, &imposta, &automazioni, &termo, &difSon, &dm, &antintr, &supervisione, &pagDefault,
				client_comandi, client_monitor, client_richieste, datiGen, fg2);

		QFile xmlFile(cfg_file);
		QXmlSimpleReader reader;
		qDebug("parte parsing");
		reader.setContentHandler(&handler);
		reader.parse(QXmlInputSource(&xmlFile));
		qDebug("finito parsing");
		QString widget_style = QString(" QWidget {background-color:%1;color:%2;} ").arg(bg->name()).arg(fg1->name());
		QString no_style(" QWidget[noStyle=\"true\"] {background-color:#FFFFFF;color:#000000;} QWidget[noStyle=\"true\"] QWidget {background-color:#FFFFFF;color:#000000;} ");
		QString second_text_color = QString(" QWidget[SecondFgColor=\"true\"] {color:%1;} QWidget[SecondFgColor=\"true\"] QWidget {color:%1;}  ").arg(fg2->name());
		qApp->setStyleSheet(widget_style + no_style + second_text_color);
		return true;
	}
	return false;
}

void BtMain::hom()
{
	datiGen->inizializza();

	if (loadConfiguration(CFG_FILE))
	{
		// TODO: verificare! Sembra che non sia piu' necessario e che l'applicazione venga
		// automaticamente chiusa quando tutte le finestre sono chiuse!
		//qApp->setMainWidget(Home);
		screensaver->hide();
		hide();
	}
	else
	{
		qApp->setStyleSheet(QString("QWidget {background-color:#000000;color:#FFFFFF;}"));
	}

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
	connect(datiGen,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
	connect(datiGen,SIGNAL(sendInit(char *)),client_richieste,SLOT(ApriInviaFrameChiudi(char *)));
	connect(Home,SIGNAL(sendInit(char *)),client_richieste,SLOT(ApriInviaFrameChiudi(char *)));

	Home->inizializza();
	if (datiGen)
		datiGen->inizializza();
	if (illumino)
		illumino->inizializza();
	if (automazioni)
		automazioni->inizializza();
	if (antintr)
		antintr->inizializza();
	if (difSon)
		difSon->inizializza();
	if (dm)
		dm->inizializza();
	if (scenari)
		scenari->inizializza();
	if (imposta)
		imposta->inizializza();
	if (termo)
		termo->inizializza();
	if (scenari_evoluti)
		scenari_evoluti->inizializza();
	if (videocitofonia)
		videocitofonia->inizializza();
	if (imposta)
		imposta->inizializza();
	if (supervisione)
		supervisione->inizializza();

	struct sysinfo info;
	sysinfo(&info);
	if (info.uptime < 200 && ((unsigned long)(info.uptime - 1) > (unsigned long)getTimePress()) && !alreadyCalibrated)
	{
		calib = new Calibrate(NULL, 1);

#if defined (BTWEB) ||  defined (BT_EMBEDDED)
		Home->hide();
		calib->show();
		connect(calib, SIGNAL(fineCalib()),Home,SLOT(showFullScreen()));
#endif
#if !defined (BTWEB) && !defined (BT_EMBEDDED)
		calib->show();
#endif
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
			screen = new genPage(NULL,"red",genPage::RED);
			screen->show();
			qDebug("TEST1");
			setBacklight(true);
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
			screen = new genPage(NULL,"green",genPage::GREEN);
			screen->show();
			qDebug("TEST2");
			setBacklight(true);
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
			screen = new genPage(NULL,"blu",genPage::BLUE);
			screen->show();
			qDebug("TEST3");
			setBacklight(true);
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
			screen = new genPage(NULL,"blu",genPage::IMAGE, IMG_PATH "dwnpage.png");
			tiposcreen = genPage::IMAGE;
			screen->show();
			qDebug("AGGIORNAMENTO");
			setBacklight(true);
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

#ifndef min
# define min(a, b) ((a) < (b)) ? (a) : (b)
#endif

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
	tiempo = min(tiempo_press, (now() - tiempo_last_ev));

	if (!firstTime)
	{
		if  (tiempo >= 30 && getBacklight())
		{
			if (!svegliaIsOn)
			{
#ifndef BACKLIGHT_SEMPRE_ON
				setBacklight(false);
				qDebug("***** freeze(TRUE) ***** ");
				emit freeze(true);
				bloccato = 01;
				tempo1->start(500);
#endif
			}
		}
		else if (tiempo <= 5 && bloccato)
		{
			qDebug("***** freeze(FALSE) ***** ");
			emit freeze(false);
			bloccato = 0;
			tempo1->start(2000);
			pd_shown = false;
			freezed(false);
		}
		if  (tiempo >= 60 && !svegliaIsOn && !calibrating)
		{
			if (pagDefault)
			{
				if (!pd_shown)
				{
					pd_shown = true;
					if (illumino)
						illumino->hide();
					if (scenari)
						scenari->hide();
					if (carichi)
						carichi->hide();
					if (imposta)
						imposta->hide();
					if (automazioni)
						automazioni->hide();
					if (termo)
						termo->hide();
					if (difSon)
						difSon->hide();
					if (dm)
						dm->hide();
					if (antintr)
						antintr->hide();
					if (specPage)
						specPage->hide();
					if (scenari_evoluti)
						scenari_evoluti->hide();
					if (videocitofonia)
						videocitofonia->hide();
					if (supervisione)
						supervisione->hide();
					if (pagDefault)
						pagDefault->showFullScreen();
				}
			}

			if  (tiempo >= 65 && screensaver->isHidden())
			{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
				screensaver->showFullScreen();
#else
				screensaver->show();
#endif
			}

			if (!screensaver->isHidden())
			{
				QWidget *target = pagDefault ? pagDefault : Home;
				screensaver->refresh(QPixmap::grabWidget(target,0,0,MAX_WIDTH,MAX_HEIGHT));
				tempo1->start(100);
			}
		}
		else
			screensaver->hide();
	}
	else if (tiempo >= 120)
	{
#ifndef BACKLIGHT_SEMPRE_ON
		setBacklight(false);
		qDebug("***** freeze(TRUE) ***** ");
		emit freeze(true);
		tempo1->start(500);
#endif
		firstTime = 0;
		bloccato = 1;
	}
	else if (tiempo <= 5)
	{
		firstTime = 0;
		setBacklight(true);
		tempo1->start(2000);
		bloccato = 0;
	}
}

void BtMain::freezed(bool b)
{
	bloccato = 0;
	if (b)
		bloccato = 1;

	qDebug("BtMain::freezed(%d)", b);
	if  (!b)
	{
		event_unfreeze = 1;
		setBacklight(true);
		screensaver->hide();
		if (pwdOn)
		{
			if (!tasti)
			{
				tasti = new tastiera(NULL,"tast");
				tasti->setMode(tastiera::HIDDEN);
				tasti->showTastiera();
				connect(tasti, SIGNAL(Closed(char*)), this, SLOT(testPwd(char*)));
			}
		}
	}
}

void BtMain::setPwd(bool b, char* p)
{
	// TODO: cambiare la password in un QString! E' necessario cambiare il segnale
	// proveniente dalla classe impPassword (setitems) e che passa da sottomenu e
	// xmlconfhandler (sarebbe anche il caso di semplificare se possibile questo giro)
	pwdOn = b;
	strcpy(pwd, p);
	qDebug("BtMain nuova pwd = %s - %d", pwd, pwdOn);
}

void BtMain::testPwd(char* p)
{
	if (p)
	{
		if (strcmp(p, pwd))
		{
			tasti->showTastiera();
			qDebug("pwd ko %s doveva essere %s",p, pwd);
		}
		else
		{
			delete tasti;
			tasti = NULL;
		}
	}
	else
		tasti->showTastiera();
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

void BtMain::ResetTimer()
{
	qDebug("BtMain::ResetTimer()");
	emit resettimer();
}
