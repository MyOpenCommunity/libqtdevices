/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** setItems.cpp
**
**
**definizione dei vari items
****************************************************************/

#include "versio.h"
#include "sveglia.h"
#include "setitems.h"
#include "tastiera.h"
#include "diffmulti.h"
#include "calibrate.h"
#include "contrpage.h"
#include "generic_functions.h" // setBeep, getBeep, beep, setContrast, getContrast, setCfgValue
#include "cleanscreen.h"
#include "brightnesspage.h"

#include <QTimer>

#include <stdlib.h>


extern unsigned char tipoData;
static const char *ICON_BRIGHTNESS = IMG_PATH "btlum.png";

setDataOra::setDataOra(sottoMenu *parent,const char *name) : bannOnDx(parent, name)
{
	SetIcons(ICON_INFO,1);
	connect(this,SIGNAL(click()),&settalora,SLOT(mostra()));
	connect(&settalora,SIGNAL(sendFrame(char*)), this, SIGNAL(sendFrame(char*)));
	connect(&settalora, SIGNAL(sendInit(char *)), this, SIGNAL(sendInit(char *)));
}

void setDataOra::hideEvent(QHideEvent *event)
{
	settalora.hide();
}


impostaSveglia::impostaSveglia(QWidget *parent,const char *name, contdiff *diso, char *h, char *m, char *icon1,
	char *icon2, int enabled, int freq, char *frame, int tipo)
	: bann2But(parent, name)
{

	strcpy(iconOn, icon1);
	strcpy(iconOff, icon2);
	SetIcons(iconOff ,ICON_INFO);
	svegliolina = new sveglia(NULL,"svegliolina",(uchar) freq, (uchar) tipo,diso, frame, h, m);
	svegliolina->hide();
	if (enabled == 1)
		setAbil(true);
	else
		setAbil(false);
	connect(this,SIGNAL(dxClick()),svegliolina,SLOT(mostra()));
	connect(this,SIGNAL(dxClick()),parentWidget(),SLOT(hide()));
	connect(this,SIGNAL(sxClick()),this,SLOT(toggleAbil()));

	connect(parentWidget() , SIGNAL(gestFrame(char*)),svegliolina,SLOT(gestFrame(char*)));
	connect(svegliolina,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*)));
	connect(svegliolina, SIGNAL(sendInit(char *)), this, SIGNAL(sendInit(char *)));
	connect(svegliolina,SIGNAL(ImClosed()),parentWidget() , SLOT(showFullScreen()));
	connect(svegliolina, SIGNAL(svegl(bool)),this , SIGNAL(svegl(bool)));
}

void impostaSveglia::gestFrame(char* frame)
{
}

void impostaSveglia::setAbil(bool b)
{
	svegliolina->activateSveglia(b);
	show();
}

void impostaSveglia::toggleAbil()
{
	bool b;
	b = svegliolina->getActivation()^1;

	svegliolina->activateSveglia(b);
	show();
}

void impostaSveglia::showEvent(QShowEvent *event)
{
	if (svegliolina->getActivation())
	{
		qDebug("impostaSveglia mette icona ON (%p)", iconOn);
		SetIcons(uchar(0),iconOn);
	}
	else
	{
		qDebug("impostaSveglia mette icona OFF");
		SetIcons(uchar(0),iconOff);
	}
	Draw();
	svegliolina->setSerNum(getSerNum());
}

void impostaSveglia::inizializza()
{
	svegliolina->inizializza();
}


calibration::calibration(sottoMenu *parent,const char *name, const char *icon)
	: bannOnDx(parent, name)
{
	SetIcons(ICON_INFO,1);
	connect(this,SIGNAL(click()),this,SLOT(doCalib()));
}

void calibration::doCalib()
{
	calib = new Calibrate();
	calib->show();
	hide();
	connect(calib,SIGNAL(fineCalib()), this, SLOT(fineCalib()));
	emit startCalib();
}

void calibration::fineCalib()
{
	show();
	delete calib;
	emit endCalib();
}


impBeep::impBeep(sottoMenu *parent, const char *name, char *val, const char *icon1, const char *icon2)
	: bannOnSx(parent, name)
{
	strncpy(iconOn, icon1, sizeof(iconOn));
	strncpy(iconOff, icon2, sizeof(iconOff));
	connect(this,SIGNAL(click()),this,SLOT(toggleBeep()));
	//TODO: che ci fanno tutte queste chiamate alla seticons??
	SetIcons(iconOff,1);
	SetIcons(iconOn,1);
	bool on = !strcmp(val, "1");
	setBeep(on, false);
	if (on)
		SetIcons(uchar(1), iconOn);
	else
		SetIcons(uchar(0), iconOff);
	Draw();
}

void impBeep::toggleBeep()
{
	if (getBeep())
	{
		setBeep(false, true);
		SetIcons(uchar(0), iconOff);
	}
	else
	{
		setBeep(true, true);
		SetIcons(uchar(0), iconOn);
		beep();
	}
	Draw();
}


impContr::impContr(sottoMenu *parent, const char *name, char *val, const char *icon1)
	: bannOnDx(parent, name)
{
	contrasto = NULL;
	SetIcons(icon1,1);
	connect(this,SIGNAL(click()),this,SLOT(showContr()));
	setContrast(atoi(val),false);
}

void impContr::showContr()
{
	hide();
	contrasto = new contrPage(NULL,"contr");
	connect(contrasto, SIGNAL(Close()), this, SLOT(contrMade()));
	contrasto->show();
}

void impContr::contrMade()
{
	show();
	contrasto->hide();
	setContrast(getContrast(),true);
	delete contrasto;
	contrasto = NULL;
}

void impContr::hideEvent(QHideEvent *event)
{
	if (contrasto)
		contrasto->hide();
}


machVers::machVers(sottoMenu *parent, const char *name, versio *ver, const char *icon1)
	: bannOnDx(parent, name)
{
	SetIcons(icon1,1);
	connect(this,SIGNAL(click()),this,SLOT(showVers()));
	v = ver;
}

void machVers::showVers()
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	v->showFullScreen();
#else
	v->show();
#endif
	QTimer::singleShot(10000, this, SLOT(tiempout()));
}

void machVers::tiempout()
{
	v->hide();
}


impPassword::impPassword(QWidget *parent, const char *name, char *icon1, char *icon2,char *icon3, char *password, int attiva)
	: bann2But(parent, name)
{

	strncpy(iconOn, icon1, sizeof(iconOn));
	strncpy(iconOff, icon2, sizeof(iconOff));
	strncpy(paswd, password, sizeof(paswd));

	SetIcons(iconOff ,icon3);

	tasti = new tastiera(NULL,"tast");
	tasti->hide();
	tasti->setMode(tastiera::HIDDEN);
	connect(this,SIGNAL(dxClick()),tasti,SLOT(showTastiera()));
	connect(this,SIGNAL(sxClick()),this,SLOT(toggleActivation()));

	connect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));
	connect(this, SIGNAL(setPwd(bool,char*)), parentWidget(), SIGNAL(setPwd(bool,char*)));
	if (attiva == 1)
		active = true;
	else
		active = false;

	emit setPwd(active,paswd);
	starting = 1;
}


void impPassword::toggleActivation()
{
	if (active)
	{
		active = false;
		setCfgValue(PROTEZIONE, "enabled","0",getSerNum());
	}
	else
	{
		active = true;
		setCfgValue(PROTEZIONE, "enabled","1",getSerNum());
	}
	emit setPwd(active,paswd);
	show();
}

void impPassword::showEvent(QShowEvent *event)
{
	if (active)
		SetIcons(uchar(0),iconOn);
	else
		SetIcons(uchar(0),iconOff);
	Draw();
	qDebug("passwd = %s %d", paswd, paswd[0]);
	if ((paswd[0] == '\000') || starting)
	{
		qDebug("passwd = ZERO");
		disconnect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));
		disconnect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow2(char*)));
		tasti->setMode(tastiera::CLEAN);
		connect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow2(char*)));
	}
	else
	{
		disconnect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));
		disconnect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow2(char*)));
		tasti->setMode(tastiera::HIDDEN);
		connect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));
	}
}

void impPassword::reShow1(char *c)
{
	if (c == NULL)
	{
		show();
		return;
	}
	if (strcmp(paswd,c))
	{
		show();
		qDebug("password errata doveva essere %s",paswd);
		sb = getBeep();
		setBeep(true,false);
		beep(1000);
		QTimer::singleShot(1100, this, SLOT(tiempout()));
	}
	else
	{
		connect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow2(char*)));
		disconnect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));
		tasti->setMode(tastiera::CLEAN);
		tasti->showTastiera();
		qDebug("password giusta");
	}
}

void impPassword::reShow2(char *c)
{
	if (c)
	{
		connect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));
		disconnect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow2(char*)));
		strcpy(paswd,c);
		setCfgValue(PROTEZIONE, "value",paswd,getSerNum());
		emit(setPwd(active,paswd));
	}
	show();
	tasti->setMode(tastiera::HIDDEN);
}

void impPassword::tiempout()
{
	setBeep(sb,false);
}

// TODO: capire l'esatto funzionamento di "starting" ed eliminare la setEnabled!
void impPassword::setEnabled(bool  b)
{
	if (!b)
		starting = 0;
	QWidget::setEnabled(b);
}

void impPassword::hideEvent(QHideEvent *event)
{
	if (tasti)
		tasti->hide();
}


BannCleanScreen::BannCleanScreen(sottoMenu *parent, const char *name) : bannOnDx(parent, name)
{
	SetIcons(ICON_INFO,1);
	page = new CleanScreen();
	page->hide();

#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	connect(this, SIGNAL(click()), page, SLOT(showFullScreen()));
#else
	connect(this, SIGNAL(click()), page, SLOT(show()));
#endif

	connect(page, SIGNAL(Closed()), page, SLOT(hide()));
}

BannCleanScreen::~BannCleanScreen()
{
	delete page;
}

void BannCleanScreen::hideEvent(QHideEvent *event)
{
	page->hide();
}

BannBrightness::BannBrightness(sottoMenu *parent, const char *name) : bannOnDx(parent, name)
{
	SetIcons(ICON_BRIGHTNESS, 1);
	page = new BrightnessPage();
	page->hide();
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	connect(this, SIGNAL(click()), page, SLOT(showFullScreen()));
#else
	connect(this, SIGNAL(click()), page, SLOT(show()));
#endif
	connect(page, SIGNAL(Closed()), page, SLOT(hide()));
}

void BannBrightness::hideEvent(QHideEvent *event)
{
	page->hide();
}

BannBrightness::~BannBrightness()
{
	delete page;
}
