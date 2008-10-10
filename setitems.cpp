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
#include "genericfunz.h"
#include "cleanscreen.h"

#include <QTimer>

#include <stdlib.h>


extern unsigned char tipoData;

setDataOra::setDataOra(sottoMenu *parent,const char *name) : bannOnDx(parent, name)
{
	SetIcons(ICON_INFO,1);
	settalora.setBGColor(backgroundColor());
	settalora.setFGColor(foregroundColor());
	connect(this,SIGNAL(click()),&settalora,SLOT(mostra()));
	connect(&settalora,SIGNAL(sendFrame(char*)), this, SIGNAL(sendFrame(char*)));
	connect(&settalora, SIGNAL(sendInit(char *)), this, SIGNAL(sendInit(char *)));
}

void setDataOra::hide()
{
	banner::hide();
	settalora.hide();
}


impostaSveglia::impostaSveglia(QWidget *parent,const char *name, contdiff* diso, char*h, char*m, char* icon1, char*icon2, int enabled , int freq , char* frame, int tipo)
	: bann2But(parent, name)
{

	strcpy(iconOn, icon1);
	strcpy(iconOff, icon2);
	SetIcons(iconOff ,ICON_INFO);
	svegliolina = new sveglia(NULL,"svegliolina",(uchar) freq, (uchar) tipo,diso, frame, h, m);
	svegliolina->setBGColor(backgroundColor());
	svegliolina->setFGColor(foregroundColor());
	svegliolina->hide();
	if (enabled == 1)
		setAbil(TRUE);
	else
		setAbil(FALSE);
	connect(this,SIGNAL(dxClick()),svegliolina,SLOT(mostra()));
	connect(this,SIGNAL(dxClick()),parentWidget(),SLOT(hide()));
	connect(this,SIGNAL(sxClick()),this,SLOT(toggleAbil()));

	connect(parentWidget() , SIGNAL(gestFrame(char*)),svegliolina,SLOT(gestFrame(char*)));
	connect(svegliolina,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*)));
	connect(svegliolina, SIGNAL(sendInit(char *)), this, SIGNAL(sendInit(char *)));
	connect(svegliolina,SIGNAL(ImClosed()),parentWidget() , SLOT(showFullScreen()));
	connect(svegliolina, SIGNAL(freeze(bool)),this , SIGNAL(freeze(bool)));
	connect(svegliolina, SIGNAL(svegl(bool)),this , SIGNAL(svegl(bool)));
	connect(this, SIGNAL(freezed(bool)), svegliolina,SLOT(spegniSveglia(bool)));
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

	svegliolina-> activateSveglia(b);
	show();
}

void impostaSveglia::show()
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
	QWidget::show();
	svegliolina->setSerNum(getSerNum());
}

void impostaSveglia::inizializza()
{
	svegliolina->inizializza();
}


calibration::calibration(sottoMenu  *parent,const char *name, const char* icon)
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


impBeep::impBeep(sottoMenu  *parent,const char *name ,char* val, const char * icon1, const char *icon2)
        : bannOnSx(parent, name)
{
	strncpy(iconOn, icon1, sizeof(iconOn));
	strncpy(iconOff, icon2, sizeof(iconOff));
	connect(this,SIGNAL(click()),this,SLOT(toggleBeep()));
	SetIcons(iconOff,1);
	SetIcons(iconOn,1);
	bool on = !strcmp(val, "1");
	if (!on)
	{
		setBeep(FALSE,FALSE);
		SetIcons(uchar(0), &iconOff[0]);
	}
	else
	{
	setBeep(TRUE,FALSE);
	SetIcons(uchar(1), &iconOn[0]);
	}
	Draw();
}

void impBeep::toggleBeep()
{
	if (getBeep())
	{
		setBeep(FALSE,TRUE);
		SetIcons(uchar(0), &iconOff[0]);
	}
	else
	{
		setBeep(TRUE,TRUE);
		SetIcons(uchar(0), &iconOn[0]);
		beep();
	}
	Draw();
}


impContr::impContr(sottoMenu  *parent,const char *name ,char* val, const char * icon1)
	: bannOnDx(parent, name)
{
	contrasto = NULL;
	SetIcons(icon1,1);
	connect(this,SIGNAL(click()),this,SLOT(showContr()));
	setContrast(atoi(val),FALSE);
}

void impContr::showContr()
{
	hide();
	contrasto = new contrPage(NULL,"contr");
	connect(contrasto, SIGNAL(Close()), this, SLOT(contrMade()));

	contrasto->setBGColor(backgroundColor());
	contrasto->setFGColor(foregroundColor());
	contrasto->show();
}

void impContr::contrMade()
{
	show();
	contrasto->hide();
	setContrast(getContrast(),TRUE);
	delete contrasto;
	contrasto = NULL;
}

void impContr::hide()
{
	banner::hide();
	if (contrasto)
		contrasto->hide();
}


machVers::machVers(sottoMenu  *parent,const char *name, versio *ver, const char* icon1)
	: bannOnDx(parent, name)
{
	SetIcons(icon1,1);
	connect(this,SIGNAL(click()),this,SLOT(showVers()));
	v=ver;
}

void machVers::showVers()
{
	v->setPaletteBackgroundColor(backgroundColor());
	v->setPaletteForegroundColor(foregroundColor());
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


impPassword::impPassword(QWidget *parent,const char *name, char* icon1, char*icon2,char* icon3, char* password,int attiva)
	: bann2But(parent, name)
{

	strncpy(iconOn, icon1, sizeof(iconOn));
	strncpy(iconOff, icon2, sizeof(iconOff));
	strncpy(paswd, password, sizeof(paswd));

	SetIcons(iconOff ,icon3);

	tasti = new tastiera(NULL,"tast");

	tasti->setBGColor(backgroundColor());
	tasti->setFGColor(foregroundColor());
	tasti->hide();
	tasti->setMode(tastiera::HIDDEN);
	connect(this,SIGNAL(dxClick()),tasti,SLOT(showTastiera()));
	connect(this,SIGNAL(sxClick()),this,SLOT(toggleActivation()));

	connect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));
	connect(this, SIGNAL(setPwd(bool,char*)), parentWidget(), SIGNAL(setPwd(bool,char*)));
	if (attiva == 1)
		active = TRUE;
	else
		active = FALSE;

	emit setPwd(active,paswd);
	starting = 1;
}


void impPassword::toggleActivation()
{
	if (active)
	{
		active = FALSE;
		setCfgValue(PROTEZIONE, "enabled","0",getSerNum());
	}
	else
	{
		active = TRUE;
		setCfgValue(PROTEZIONE, "enabled","1",getSerNum());
	}
	emit setPwd(active,paswd);
	show();
}

void impPassword::show()
{
	if (active)
		SetIcons(uchar(0),iconOn);
	else
		SetIcons(uchar(0),iconOff);
	qDebug("impPassword::show()");
	Draw();
	qDebug("passwd = %s %d", &paswd[0], paswd[0]);
	if ((paswd[0]=='\000') || (starting))
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
	QWidget::show();
}

void impPassword::reShow1(char* c)
{
	if (c == NULL)
	{
		show();
		return;
	}
	if (strcmp(&paswd[0],c))
	{
		show();
		qDebug("password errata doveva essere %s",&paswd[0]);
		sb = getBeep();
		setBeep(TRUE,FALSE); 
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

void impPassword::reShow2(char* c)
{
	if (c)
	{
		connect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));
		disconnect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow2(char*)));
		strcpy(&paswd[0],c);
		setCfgValue(PROTEZIONE, "value",&paswd[0],getSerNum());
		emit(setPwd(active,&paswd[0]));
	}
	show();
	tasti->setMode(tastiera::HIDDEN);
}

void impPassword::tiempout()
{
	setBeep(sb,FALSE);
}

void impPassword::setEnabled(bool  b)
{
	if (!b)
		starting = 0;
	QWidget::setEnabled(b);
}

void impPassword::hide()
{
	banner::hide();
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

void BannCleanScreen::hide()
{
	bannOnDx::hide();
	page->hide();
}
