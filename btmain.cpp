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
#include "thermalmenu.h"
#include "supervisionmenu.h"

#include <qwindowsystem_qws.h>
#include <qapplication.h>
#include <qobject.h>
#include <qcursor.h>
#include <qwindowdefs.h>
#include <qbitmap.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qfile.h>
#include <qxml.h>

#include <sys/sysinfo.h>

//#define SCREENSAVER_BALLS
#define SCREENSAVER_LINE

BtMain::BtMain(QWidget *parent, const char *name,QApplication* a) : QWidget(parent)
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

	client_monitor = new  Client("127.0.0.1",20000,1);
	client_comandi = new  Client("127.0.0.1",20000,0);
	client_richieste = new Client("127.0.0.1",20000,0, true);
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
#ifndef SCREENSAVER_LINE
	backcol = 0;
	y[0] = 0;
#else
	backcol = 10;
#endif
	tasti = NULL;
	linea = NULL;
	for (int idx = 0; idx < 12; idx++)
	{
		screensav[idx] = new BtLabel(this);
		screensav[idx]->setFrameStyle(QFrame::Panel | QFrame::Raised);
		screensav[idx]->hide();
		screensav[idx]->setText("");
		Sfondo[idx] = NULL;
	}
	for (int idx = 0; idx < 12; idx++)
	{
		ball[idx] = new BtLabel(this);
		ball[idx]->hide();
	}

	grab = NULL;
	pwdOn = 0;

	datiGen = new versio(NULL, "DG");
	struct sysinfo info;
	sysinfo(&info);
	qDebug("uptime: %d",(int)info.uptime);
	qDebug("touch ago: %d",(int)getTimePress());

	if ((QFile::exists("/etc/pointercal")) && ((info.uptime>200) || ((unsigned long)(info.uptime-1)<=(unsigned long)getTimePress())))
	{
		tempo1 = new QTimer(this);
		tempo1->start(200);
		connect(tempo1,SIGNAL(timeout()),this,SLOT(hom()));
		datiGen->show();
	}
	else
	{
		tempo1 = NULL;
		calib = new Calibrate(NULL, (unsigned char)1);
		calib->show();
		connect(calib, SIGNAL(fineCalib()), this,SLOT(hom()));
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
		connect(calib, SIGNAL(fineCalib()), datiGen,SLOT(showFullScreen()));
#endif
#if !defined (BTWEB) && !defined (BT_EMBEDDED)
		connect(calib, SIGNAL(fineCalib()), datiGen,SLOT(show()));
#endif
		alreadyCalibrated = TRUE;
	}
}

void BtMain::hom()
{
	if (tempo1)
		delete tempo1;
	else
	{
		tempo1 = new QTimer(this);
		tempo1->start(200);
		connect(tempo1,SIGNAL(timeout()),this,SLOT(hom()));
		return;
	}

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
		/*
		setBackgroundColor(*bg);
		for (int idx = 0; idx < 12; idx++)
		{
			ball[idx]->setBackgroundColor(*bg);
			ball[idx]->setBackgroundMode(Qt::NoBackground);
		}
		*/
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
		hide();
	}
	setGeometry(0,0,240,320);
	setCursor(QCursor(Qt::BlankCursor));
	setBackgroundColor(QColor(255,255,255));

	connect(client_monitor,SIGNAL(monitorSu()),this,SLOT(myMain()));
	client_monitor->connetti();
	qDebug("OPPI");
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
	disconnect(tempo1,SIGNAL(timeout()),this,SLOT(hom()));
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
		if  ((tiempo >= 30) && (getBacklight()))
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
		else if ((tiempo <= 5) && (bloccato))
		{
			qDebug("***** freeze(FALSE) ***** ");
			emit freeze(FALSE);
			bloccato = 0;
			tempo1->start(2000);
			pd_shown = false;
			freezed(FALSE);
		}
		if  ((tiempo >= 60) && (!svegliaIsOn) && (!calibrating))
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

			if  ((tiempo >= 65) && isHidden())
			{
				for (int idx = 0; idx < 12; idx++)
				{
					if (Sfondo[idx])
						delete(Sfondo[idx]);
					if (pagDefault)
					{
						Sfondo[idx] =  new QPixmap(QPixmap::grabWidget(pagDefault,(idx%3)*MAX_WIDTH/3,(int)(idx/3)*MAX_HEIGHT/4,MAX_WIDTH,MAX_HEIGHT/4));
					}
					else
					{
						Sfondo[idx] =  new QPixmap(QPixmap::grabWidget(Home,(idx%3)*MAX_WIDTH/3,(int)(idx/3)*MAX_HEIGHT/4,MAX_WIDTH,MAX_HEIGHT/4));
					}
					screensav[idx]->setGeometry((idx%3)*80,(int)(idx/3)*80,80,80);
					screensav[idx]->setPixmap(*Sfondo[idx]);
					screensav[idx]->hide();
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
					showFullScreen();
#else
					show();
#endif
				}
				if (grab)
					delete(grab);
				if (pagDefault)
					grab = new QPixmap(QPixmap::grabWidget(pagDefault,0,0,MAX_WIDTH,MAX_HEIGHT));
				else
					grab = new QPixmap(QPixmap::grabWidget(Home,0,0,MAX_WIDTH,MAX_HEIGHT));
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
				showFullScreen();
#else
				show();
#endif
				countScrSav = 0;
			}

			if (!isHidden())
			{
#ifdef SCREENSAVER_BALLS
				if (backcol < 3)
				{
					backcol = 4;
					for (int idx = 0; idx < 12; idx++)
					{
						screensav[idx]->hide();
					}
					setPaletteBackgroundPixmap(*grab);
					for (int idx = 0; idx < BALL_NUM; idx++)
					{
						x[idx] = (int)(200.0*rand() / (RAND_MAX+1.0));
						y[idx] = (int)(200.0*rand() / (RAND_MAX+1.0));
						vx[idx] = (int)(30.0*rand() / (RAND_MAX+1.0)) -15;
						vy[idx] = (int)(30.0*rand() / (RAND_MAX+1.0)) -15;
						if (!vy[idx])
							vy[idx] = 1;
						if (!vx[idx])
							vx[idx] = 1;
						dim[idx] = (int)(10.0*rand() / (RAND_MAX+1.0))+15;

						QBitmap Maschera = QBitmap(dim[idx],dim[idx],TRUE);
						QPainter p(&Maschera);
						p.setBrush(QBrush (Qt::color1, Qt::SolidPattern));
						for (int idy = 2; idy <= dim[idx]; idy++)
							p.drawEllipse((dim[idx]-idy)/2,(dim[idx]-idy)/2,idy,idy);
						ball[idx]->setMask(Maschera);
					}
					tempo1->start(100);
				}
				else
				{
					backcol++;
					if (backcol == 9)
					{
						backcol = 4;
						if (grab)
							delete grab;
						if (pagDefault)
							grab = new QPixmap(QPixmap::grabWidget(pagDefault,0,0,MAX_WIDTH,MAX_HEIGHT));
						else
							grab = new QPixmap(QPixmap::grabWidget(Home,0,0,MAX_WIDTH,MAX_HEIGHT));
						setPaletteBackgroundPixmap(*grab);
					}

					for (int idx = 0;idx < BALL_NUM; idx++)
					{
						x[idx] += vx[idx];
						y[idx] += vy[idx];

						if  (x[idx] <= 0)
						{
							vx[idx] = (int)(10.0*rand() / (RAND_MAX+1.0)) + 5;
							x[idx] = 0;
							ball[idx]->setPaletteBackgroundColor(QColor((int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150));
						}
						if  (y[idx] > (MAX_HEIGHT-dim[idx]))
						{
							vy[idx] = (int)(10.0*rand() / (RAND_MAX+1.0)) - 15;
							y[idx] = MAX_HEIGHT-dim[idx];
							ball[idx]->setPaletteBackgroundColor(QColor((int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150));
						}
						if   (y[idx] <= 0)
						{
							vy[idx] = (int)(10.0*rand() / (RAND_MAX+1.0)) + 5;
							if (!vy[idx])
								vy[idx] = 1;
							y[idx] = 0;
							ball[idx]->setPaletteBackgroundColor(QColor((int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150));
						}
						if  (x[idx] > (MAX_WIDTH-dim[idx]))
						{
							vx[idx] = (int) (10.0*rand() / (RAND_MAX+1.0)) - 15;
							if (!vx[idx])
								vx[idx] = 1;
							x[idx] = MAX_WIDTH-dim[idx];
							ball[idx]->setPaletteBackgroundColor(QColor((int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150));
						}
						ball[idx]->setGeometry(x[idx],y[idx],dim[idx],dim[idx]);
						ball[idx]->show();
					}
				}
#endif
#ifdef SCREENSAVER_LINE
				tempo1->start(100);
				if (backcol >= 5)
				{
					if (grab)
						delete grab;
					if (pagDefault)
						grab = new QPixmap(QPixmap::grabWidget(pagDefault,0,0,MAX_WIDTH,MAX_HEIGHT));
					else
						grab = new QPixmap(QPixmap::grabWidget(Home,0,0,MAX_WIDTH,MAX_HEIGHT));
					setPaletteBackgroundPixmap(*grab);
					backcol = 0;
				}
				backcol++;
				if (y[0] > MAX_HEIGHT)
				{
					y[0] = MAX_HEIGHT;
					y[1] = 1;
				}
				if (y[0] < 0)
				{
					y[0] = 0;
					y[1] = 0;
				}
				if (y[1] == 0)
					y[0] += 3;
				else
					y[0] -= 3;

				if (!linea)
					linea = new BtLabel(this,NULL,0);
				linea->setGeometry(0,y[0],MAX_WIDTH,6);
				if (y[1])
					linea->setPaletteBackgroundColor(QColor::QColor(Qt::black));
				else
					linea->setPaletteBackgroundColor(QColor::QColor(Qt::white));
				linea->show();
#endif
			}
		}
		else
			hide();
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
		hide();
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


palla::palla(QWidget*parent, const char* name,unsigned int f) : BtLabel(parent, name, (Qt::WindowFlags)f) { }

void palla::paintEvent(QPaintEvent *)
{
	QPainter paint(this);
	paint.setBrush(QBrush(foregroundColor(), Qt::SolidPattern));
	paint.drawEllipse(0,0,width(),height());
};

void palla ::clear()
{
	QPainter paint(this);
	paint.fillRect(0,0,width(),height(),QBrush (backgroundColor(), Qt::SolidPattern));
}


const QColor& palla::backgroundColor()
{
	return palette().color(backgroundRole());
}

const QColor& palla::foregroundColor()
{
	return palette().color(foregroundRole());
}
