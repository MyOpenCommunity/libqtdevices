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
#include "brightnesspage.h"
#include "btmain.h"
#include "main.h" // BTouch

#include <QTimer>
#include <QDebug>

#include <stdlib.h>


static const char *ICON_BRIGHTNESS = IMG_PATH "btlum.png";


impostaSveglia::impostaSveglia(QWidget *parent, contdiff *diso, QString hour, QString minute, QString icon1,
	QString icon2, QString icon3, int enabled, int freq, int tipo)
	: bann2But(parent)
{
	icon_on = icon1;
	icon_off = icon2;
	SetIcons(icon_off, icon3);
	svegliolina = new sveglia(NULL,(uchar) freq, (uchar) tipo,diso, 0, hour.toInt(), minute.toInt());
	svegliolina->hide();
	setAbil(enabled == 1);
	connect(this,SIGNAL(dxClick()),svegliolina,SLOT(mostra()));
	connect(this,SIGNAL(sxClick()),this,SLOT(toggleAbil()));

	connect(parentWidget() , SIGNAL(gestFrame(char*)),svegliolina,SLOT(gestFrame(char*)));
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


calibration::calibration(sottoMenu *parent, QString icon) : bannOnDx(parent)
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


impBeep::impBeep(sottoMenu *parent, QString val, QString icon1, QString icon2)
	: bannOnSx(parent)
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


impContr::impContr(sottoMenu *parent, QString val, QString icon1) : bannOnDx(parent)
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


machVers::machVers(sottoMenu *parent, versio *ver, QString icon1)
	: bannOnDx(parent)
{
	SetIcons(icon1, 1);
	connect(this,SIGNAL(click()),this,SLOT(showVers()));
	v = ver;
}

void machVers::showVers()
{
	v->showFullScreen();
	QTimer::singleShot(10000, this, SLOT(tiempout()));
}

void machVers::tiempout()
{
	v->hide();
}


impPassword::impPassword(QWidget *parent, QString icon1, QString icon2, QString icon3, QString pwd, int attiva)
	: bann2But(parent)
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


BannBrightness::BannBrightness(sottoMenu *parent) : bannOnDx(parent)
{
	SetIcons(ICON_BRIGHTNESS, 1);
	page = new BrightnessPage();
	page->hide();
	connect(this, SIGNAL(click()), page, SLOT(showFullScreen()));
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
