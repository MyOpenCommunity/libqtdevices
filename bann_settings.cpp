#include "bann_settings.h"
#include "version.h"
#include "alarmclock.h"
#include "keypad.h"
#include "multisounddiff.h" // contdiff
#include "calibrate.h"
#include "contrast.h"
#include "generic_functions.h" // setBeep, getBeep, beep, setContrast, getContrast, setCfgValue
#include "btmain.h" // bt_global::btmain
#include "btbutton.h"

#include <QTimer>
#include <QDebug>


bannAlarmClock::bannAlarmClock(QWidget *parent, contdiff *diso, int hour, int minute, QString icon1,
	QString icon2, QString icon3, int enabled, int tipo, int freq)
	: bann2But(parent)
{
	sxButton->setCheckable(true);
	SetIcons(0, icon2, icon1);
	SetIcons(1, icon3);
	Draw(); // Draw must be called before setAbil.. see impBeep

	alarm_clock = new AlarmClock(static_cast<AlarmClock::Type>(tipo), static_cast<AlarmClock::Freq>(freq), diso, hour, minute);
	alarm_clock->setSerNum(getSerNum());
	alarm_clock->hide();
	setAbil(enabled == 1);
	connect(this, SIGNAL(dxClick()), alarm_clock, SLOT(showPage()));
	connect(this, SIGNAL(sxClick()), this, SLOT(toggleAbil()));

	connect(alarm_clock,SIGNAL(Closed()), alarm_clock, SLOT(hide()));
}

void bannAlarmClock::gestFrame(char* frame)
{
	alarm_clock->gestFrame(frame);
}

void bannAlarmClock::setAbil(bool b)
{
	sxButton->setChecked(b);
	alarm_clock->setActive(b);
}

void bannAlarmClock::toggleAbil()
{
	setAbil(!alarm_clock->isActive());
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
	connect(this, SIGNAL(click()), this, SLOT(toggleBeep()));

	bool on = (val.toInt() == 1);
	sxButton->setCheckable(true);
	setBeep(on, false);

	SetIcons(0, icon2, icon1);
	Draw(); // Draw must be called before setChecked (because it calls the setPixmap function)
	sxButton->setChecked(on);
}

void impBeep::toggleBeep()
{
	if (getBeep())
	{
		setBeep(false, true);
		sxButton->setChecked(false);
	}
	else
	{
		setBeep(true, true);
		sxButton->setChecked(true);
		beep();
	}
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
	password = pwd;

	tasti = new Keypad();
	tasti->hide();
	tasti->setMode(Keypad::HIDDEN);
	connect(this,SIGNAL(dxClick()),tasti,SLOT(showPage()));
	connect(this,SIGNAL(sxClick()),this,SLOT(toggleActivation()));

	connect(tasti, SIGNAL(Closed()), this, SLOT(reShow1()));
	connect(tasti, SIGNAL(Closed()), tasti, SLOT(hide()));

	SetIcons(1, icon3);
	SetIcons(0, icon2, icon1);
	Draw();
	sxButton->setCheckable(true);

	active = (attiva == 1);
	bt_global::btmain->setPwd(active, password);

	sxButton->setChecked(active);
}

void impPassword::toggleActivation()
{
	active = !active;
	setCfgValue("enabled", QString::number(active), PROTEZIONE, getSerNum());
	bt_global::btmain->setPwd(active, password);
	sxButton->setChecked(active);
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
		bt_global::btmain->setPwd(active, password);
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

