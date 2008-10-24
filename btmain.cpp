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
#include <QPainter>
#include <QObject>
#include <QCursor>
#include <QBitmap>
#include <QPixmap>
#include <QTimer>
#include <QFile>

#include <sys/sysinfo.h>

//#define SCREENSAVER_BALLS
#define SCREENSAVER_LINE


BtMain::BtMain(QWidget *parent) : QWidget(parent)
{

	/*******************************************
	v** Socket
	*******************************************/
	qDebug("parte BtMain");

	// If not compiled for Desktop, turns OFF mouse pointer
	// QWSServer is in <qwindowsystem_qws.h>
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	QWSServer::setCursorVisible(FALSE);
#endif

	client_monitor = new Client(Client::MONITOR);
	client_comandi = new Client(Client::COMANDI);
	client_richieste = new Client(Client::RICHIESTE);
	btouch_device_cache.set_clients(client_comandi, client_monitor, client_richieste);
	connect(client_comandi, SIGNAL(frameToAutoread(char*)), client_monitor,SIGNAL(frameIn(char*)));

	setBacklight(TRUE);

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
	alreadyCalibrated = FALSE;
	svegliaIsOn = FALSE;
	tiempo_last_ev = 0;
	pd_shown = false;
#ifdef SCREENSAVER_LINE
	screensaver = getScreenSaver(ScreenSaver::LINES);
#else
	screensaver = getScreenSaver(ScreenSaver::BALLS);
#endif
	tasti = NULL;
	pwdOn = 0;

	datiGen = new versio(NULL, "DG");
	struct sysinfo info;
	sysinfo(&info);
	qDebug("uptime: %d",(int)info.uptime);
	qDebug("touch ago: %d",(int)getTimePress());

	if ((QFile::exists("/etc/pointercal")) && ((info.uptime>200) || ((unsigned long)(info.uptime-1)<=(unsigned long)getTimePress())))
	{
		waitBeforeInit();
		datiGen->show();
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
		alreadyCalibrated = TRUE;
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

void BtMain::hom()
{
	datiGen->inizializza();
	QColor *bg, *fg1, *fg2;
	readExtraConf(&bg, &fg1, &fg2);

	//-----------------------------------------------
	if (QFile::exists("cfg/conf.xml"))
	{ 
		xmlconfhandler *handler2 = new xmlconfhandler(this, &Home, &specPage, &scenari_evoluti, &videocitofonia, &illumino,
				&scenari, &carichi, &imposta, &automazioni, &termo, &difSon, &dm, &antintr, &supervisione, &pagDefault,
				client_comandi, client_monitor, client_richieste, datiGen, bg, fg1, fg2);
		// TODO: sistemare con i metodi qt4!

		// setBackgroundColor(*bg);
		
		QFile *xmlFile = new QFile("cfg/conf.xml");
		QXmlInputSource source2(xmlFile);
		QXmlSimpleReader reader2;
		qDebug("parte parsing");
		reader2.setContentHandler(handler2);
		reader2.parse(source2);
		qDebug("finito parsing");
		delete handler2;
		delete xmlFile;
		// TODO: verificare! Sembra che non sia piu' necessario e che l'applicazione venga
		// automaticamente chiusa quando tutte le finestre sono chiuse!
		//qApp->setMainWidget(Home);
		screensaver->hide();
		hide();
	}
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setCursor(QCursor(Qt::BlankCursor));
	setBackgroundColor(QColor(255,255,255));

	connect(client_monitor,SIGNAL(monitorSu()),this,SLOT(myMain()));
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
		alreadyCalibrated = TRUE;
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
			setBacklight(TRUE);
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
			setBacklight(TRUE);
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
			setBacklight(TRUE);
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
			setBacklight(TRUE);
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
				setBacklight(FALSE);
				qDebug("***** freeze(TRUE) ***** ");
				emit freeze(TRUE);
				bloccato = 01;
				tempo1->start(500);
#endif
			}
		}
		else if (tiempo <= 5 && bloccato)
		{
			qDebug("***** freeze(FALSE) ***** ");
			emit freeze(FALSE);
			bloccato = 0;
			tempo1->start(2000);
			pd_shown = false;
			freezed(FALSE);
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
		setBacklight(FALSE);
		qDebug("***** freeze(TRUE) ***** ");
		emit freeze(TRUE);
		tempo1->start(500);
#endif
		firstTime = 0;
		bloccato = 1;
	}
	else if (tiempo <= 5)
	{
		firstTime = 0;
		setBacklight(TRUE);
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
		setBacklight(TRUE);
		screensaver->hide();
		if (pwdOn)
		{
			if (!tasti)
			{
				tasti = new tastiera(NULL,"tast");
				tasti->setBGColor(Home->backgroundColor());
				tasti->setFGColor(Home->foregroundColor());
				tasti->setMode(tastiera::HIDDEN);
				tasti->showTastiera();
				connect(tasti, SIGNAL(Closed(char*)), this, SLOT(testPwd(char*)));
			}
		}
	}
}

void BtMain::setPwd(bool b, char* p)
{
	pwdOn = b;
	strcpy(&pwd[0], p);
	qDebug("BtMain nuova pwd = %s - %d",&pwd[0],pwdOn);
}

void BtMain::testPwd(char* p)
{
	if (p)
	{
		if (strcmp(p,&pwd[0]))
		{
			tasti->showTastiera();
			qDebug("pwd ko %s  doveva essere %s",p,&pwd[0]);
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

void BtMain::setBackgroundColor(const QColor &c)
{
	QPalette palette;
	palette.setColor(backgroundRole(), c);
	setPalette(palette);
}

void BtMain::setPaletteBackgroundPixmap(const QPixmap &pixmap)
{
	QPalette palette;
	palette.setBrush(backgroundRole(), QBrush(pixmap));
	setPalette(palette);
}
