#include "bann_settings.h"
#include "version.h"
#include "alarmclock.h"
#include "keypad.h"
#include "multisounddiff.h" // contdiff
#include "calibrate.h"
#include "contrast.h"
#include "generic_functions.h" // setBeep, getBeep, beep, setContrast, getContrast, setCfgValue
#include "global.h"

#include <QTimer>
#include <QDebug>


bannAlarmClock::bannAlarmClock(QWidget *parent, contdiff *diso, int hour, int minute, QString icon1,
	QString icon2, QString icon3, int enabled, int tipo, int freq)
	: bann2But(parent)
{
	icon_on = icon1;
	icon_off = icon2;
	SetIcons(icon_off, icon3);
	alarm_clock = new AlarmClock(static_cast<AlarmClock::Type>(tipo), static_cast<AlarmClock::Freq>(freq), diso, hour, minute);
	alarm_clock->hide();
	setAbil(enabled == 1);
	connect(this, SIGNAL(dxClick()), alarm_clock, SLOT(showPage()));
	connect(this, SIGNAL(sxClick()), this, SLOT(toggleAbil()));

	connect(alarm_clock,SIGNAL(Closed()), alarm_clock, SLOT(hide()));
	connect(alarm_clock,SIGNAL(Closed()), this, SLOT(forceDraw()));
}

void bannAlarmClock::gestFrame(char* frame)
{
	alarm_clock->gestFrame(frame);
}

void bannAlarmClock::setAbil(bool b)
{
	alarm_clock->setActive(b);
	forceDraw();
}

void bannAlarmClock::toggleAbil()
{
	alarm_clock->setActive(!alarm_clock->isActive());
	forceDraw();
}

void bannAlarmClock::forceDraw()
{
	SetIcons(0, alarm_clock->isActive() ? icon_on : icon_off);
	Draw();
	alarm_clock->setSerNum(getSerNum());
}

void bannAlarmClock::inizializza()
{
	alarm_clock->inizializza();
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


bannContrast::bannContrast(sottoMenu *parent, QString val, QString icon) :
	bannOnDx(parent, icon, new Contrast())
{
	setContrast(val.toInt(), false);
	connect(linked_page, SIGNAL(Closed()), SLOT(done()));
}

void bannContrast::done()
{
	setContrast(getContrast(), true);
}


bannVersion::bannVersion(sottoMenu *parent, QString icon, Version *ver)
	: bannOnDx(parent, icon)
{
	connect(this, SIGNAL(click()), this, SLOT(showVers()));
	v = ver;
}

void bannVersion::showVers()
{
	v->showPage();
	QTimer::singleShot(10000, this, SLOT(tiempout()));
}

void bannVersion::tiempout()
{
	v->hide();
}


impPassword::impPassword(QWidget *parent, QString icon1, QString icon2, QString icon3, QString pwd, int attiva)
	: bann2But(parent)
{
	icon_on = icon1;
	icon_off = icon2;
	password = pwd;

	tasti = new Keypad();
	tasti->hide();
	tasti->setMode(Keypad::HIDDEN);
	connect(this,SIGNAL(dxClick()),tasti,SLOT(showPage()));
	connect(this,SIGNAL(sxClick()),this,SLOT(toggleActivation()));

	connect(tasti, SIGNAL(Closed()), this, SLOT(reShow1()));
	connect(tasti, SIGNAL(Closed()), tasti, SLOT(hide()));

	active = (attiva == 1);
	BTouch->setPwd(active, password);
	SetIcons(1, icon3);
	SetIcons(0, active ? icon_on : icon_off);
}

void impPassword::toggleActivation()
{
	active = !active;
	setCfgValue("enabled", QString::number(active), PROTEZIONE, getSerNum());
	BTouch->setPwd(active, password);
	SetIcons(0, active ? icon_on : icon_off);
	Draw();
}

void impPassword::showEvent(QShowEvent *event)
{
	Draw();
	qDebug() << "password = " << password;
	if (password.isEmpty())
	{
		qDebug("password = ZERO");
		disconnect(tasti, SIGNAL(Closed()),this , SLOT(reShow1()));
		disconnect(tasti, SIGNAL(Closed()),this , SLOT(reShow2()));
		tasti->setMode(Keypad::CLEAN);
		connect(tasti, SIGNAL(Closed()),this , SLOT(reShow2()));
	}
	else
	{
		disconnect(tasti, SIGNAL(Closed()),this , SLOT(reShow1()));
		disconnect(tasti, SIGNAL(Closed()),this , SLOT(reShow2()));
		tasti->setMode(Keypad::HIDDEN);
		connect(tasti, SIGNAL(Closed()), this, SLOT(reShow1()));
	}
}

void impPassword::reShow1()
{
	QString c = tasti->getText();
	if (c.isEmpty())
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
		connect(tasti, SIGNAL(Closed()), this, SLOT(reShow2()));
		disconnect(tasti, SIGNAL(Closed()), this, SLOT(reShow1()));
		tasti->setMode(Keypad::CLEAN);
		tasti->showPage();
		qDebug("password giusta");
	}
}

void impPassword::reShow2()
{
	QString c = tasti->getText();
	if (!c.isEmpty())
	{
		connect(tasti, SIGNAL(Closed()), this, SLOT(reShow1()));
		disconnect(tasti, SIGNAL(Closed()), this, SLOT(reShow2()));
		password = c;
		setCfgValue("value", password, PROTEZIONE, getSerNum());
		BTouch->setPwd(active, password);
	}
	show();
}

void impPassword::tiempout()
{
	setBeep(sb,false);
}

void impPassword::hideEvent(QHideEvent *event)
{
	if (tasti)
		tasti->hide();
}

