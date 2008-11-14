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
#include <QDebug>

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


impostaSveglia::impostaSveglia(QWidget *parent, const char *name, contdiff *diso, QString hour, QString minute, QString icon1,
	QString icon2, int enabled, int freq, QString frame, int tipo)
	: bann2But(parent, name)
{
	icon_on = icon1;
	icon_off = icon2;
	SetIcons(icon_off, ICON_INFO);
	QByteArray buf_frame = frame.toAscii();
	svegliolina = new sveglia(NULL,"svegliolina",(uchar) freq, (uchar) tipo,diso, buf_frame.data(), hour.toInt(), minute.toInt());
	svegliolina->hide();
	setAbil(enabled == 1);
	connect(this,SIGNAL(dxClick()),svegliolina,SLOT(mostra()));
	connect(this,SIGNAL(sxClick()),this,SLOT(toggleAbil()));

	connect(parentWidget() , SIGNAL(gestFrame(char*)),svegliolina,SLOT(gestFrame(char*)));
	connect(svegliolina,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*)));
	connect(svegliolina, SIGNAL(sendInit(char *)), this, SIGNAL(sendInit(char *)));
	connect(svegliolina,SIGNAL(ImClosed()), svegliolina, SLOT(hide()));
	connect(svegliolina,SIGNAL(ImClosed()), this, SLOT(forceDraw()));
}

void impostaSveglia::gestFrame(char* frame)
{
}

void impostaSveglia::setAbil(bool b)
{
	svegliolina->setActive(b);
	forceDraw();
}

void impostaSveglia::toggleAbil()
{
	svegliolina->setActive(!svegliolina->isActive());
	forceDraw();
}

void impostaSveglia::forceDraw()
{
	SetIcons(0, svegliolina->isActive() ? icon_on : icon_off);
	Draw();
	svegliolina->setSerNum(getSerNum());
}

void impostaSveglia::inizializza()
{
	svegliolina->inizializza();
}


calibration::calibration(sottoMenu *parent, const char *name, QString icon)
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


impBeep::impBeep(sottoMenu *parent, const char *name, QString val, QString icon1, QString icon2)
	: bannOnSx(parent, name)
{
	icon_on = icon1;
	icon_off = icon2;
	connect(this,SIGNAL(click()),this,SLOT(toggleBeep()));
	SetIcons(0, icon_on);

	bool on = (val.toInt() == 1);
	setBeep(on, false);
	if (on)
		SetIcons(1, icon_on);
	else
		SetIcons(0, icon_off);
	Draw();
}

void impBeep::toggleBeep()
{
	if (getBeep())
	{
		setBeep(false, true);
		SetIcons(0, icon_off);
	}
	else
	{
		setBeep(true, true);
		SetIcons(0, icon_on);
		beep();
	}
	Draw();
}


impContr::impContr(sottoMenu *parent, const char *name, QString val, QString icon1)
	: bannOnDx(parent, name)
{
	contrasto = NULL;
	SetIcons(icon1, 1);
	connect(this,SIGNAL(click()),this,SLOT(showContr()));
	setContrast(val.toInt(), false);
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


machVers::machVers(sottoMenu *parent, const char *name, versio *ver, QString icon1)
	: bannOnDx(parent, name)
{
	SetIcons(icon1, 1);
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


impPassword::impPassword(QWidget *parent, const char *name, QString icon1, QString icon2, QString icon3, QString pwd, int attiva)
	: bann2But(parent, name)
{
	icon_on = icon1;
	icon_off = icon2;
	password = pwd;

	SetIcons(icon_off, icon3);

	tasti = new tastiera();
	tasti->hide();
	tasti->setMode(tastiera::HIDDEN);
	connect(this,SIGNAL(dxClick()),tasti,SLOT(showFullScreen()));
	connect(this,SIGNAL(sxClick()),this,SLOT(toggleActivation()));

	connect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));
	// TODO: e' brutto fare una connessione con il parent.. se il banner viene messo in un'altra gerarchia non funziona
	// piu' niente!
	connect(this, SIGNAL(setPwd(bool, QString)), parentWidget(), SIGNAL(setPwd(bool, QString)));
	active = (attiva == 1);

	emit setPwd(active, password);
	starting = true;
}

void impPassword::toggleActivation()
{
	active = !active;
	setCfgValue("enabled", QString::number(active), PROTEZIONE, getSerNum());
	emit setPwd(active, password);
	SetIcons(0, active ? icon_on : icon_off);
	Draw();
}

void impPassword::showEvent(QShowEvent *event)
{
	Draw();
	qDebug() << "password = " << password;
	if (password.isEmpty() || starting)
	{
		qDebug("password = ZERO");
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
	qDebug("impPassword::reShow1");
	if (c == NULL)
	{
		show();
		return;
	}
	if (password != c)
	{
		show();
		qDebug() << "password errata doveva essere " << password;
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
		tasti->showFullScreen();
		qDebug("password giusta");
	}
}

void impPassword::reShow2(char *c)
{
	qDebug("impPassword::reShow2");
	if (c)
	{
		connect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));
		disconnect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow2(char*)));
		password = c;
		setCfgValue("value", password, PROTEZIONE, getSerNum());
		emit setPwd(active, password);
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
		starting = false;
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
