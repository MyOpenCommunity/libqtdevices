#include "bann_settings.h"
#include "version.h"
#include "alarmclock.h"
#include "keypad.h"
#include "multisounddiff.h" // contdiff
#include "calibrate.h"
#include "contrast.h"
#include "generic_functions.h" // setCfgValue
#include "hardware_functions.h" // setBeep, getBeep, beep, setContrast, getContrast
#include "btmain.h" // bt_global::btmain
#include "btbutton.h"
#include "fontmanager.h"

#include <QTimer>
#include <QDebug>
#include <QLabel>


bannAlarmClock::bannAlarmClock(QWidget *parent, int hour, int minute, QString icon_on,
	QString icon_off, QString icon_label, int enabled, int tipo, int freq)
	: bann2But(parent)
{
	sxButton->setOnOff();
	sxButton->setStatus(enabled == 1);

	SetIcons(0, icon_off, icon_on);
	SetIcons(1, icon_label);
	Draw(); // Draw must be called before setAbil.. see impBeep

	alarm_clock = new AlarmClock(SET_SVEGLIA,
				     static_cast<AlarmClock::Type>(tipo),
				     static_cast<AlarmClock::Freq>(freq), QList<bool>(),
				     hour, minute);
	alarm_clock->setSerNum(getSerNum());
	alarm_clock->hide();

	alarm_clock->_setActive(enabled == 1);
	connect(this, SIGNAL(dxClick()), alarm_clock, SLOT(showPage()));
	connect(this, SIGNAL(sxClick()), this, SLOT(toggleAbil()));

	connect(alarm_clock, SIGNAL(Closed()), SLOT(handleClose()));
	connect(alarm_clock, SIGNAL(alarmClockFired()), SLOT(setButtonIcon()));
}

void bannAlarmClock::setSerNum(int num)
{
	banner::setSerNum(num);
	alarm_clock->setSerNum(num);
}

void bannAlarmClock::handleClose()
{
	// When the page of the alarmclock is closed, the alarm is always set as 'on'.
	sxButton->setStatus(true);
	emit pageClosed();
}

void bannAlarmClock::setButtonIcon()
{
	if (!alarm_clock->isActive())
		sxButton->setStatus(false);
}

void bannAlarmClock::setAbil(bool b)
{
	sxButton->setStatus(b);
	alarm_clock->setActive(b);
}

void bannAlarmClock::toggleAbil()
{
	setAbil(!alarm_clock->isActive());
}

void bannAlarmClock::inizializza(bool forza)
{
	bann2But::inizializza(forza);
	alarm_clock->inizializza();
}


bannAlarmClockIcon::bannAlarmClockIcon(int hour, int minute, QString icon_on,
	QString icon_off, QString icon_state, QString icon_edit, QString text, int enabled, int tipo, QList<bool> days)
	: BannOnOffState(0)
{
	initBanner(icon_on, icon_state, icon_edit, enabled ? ON : OFF, text);

	left_button->setOnOff();
	left_button->setPressedImage(icon_off);
	left_button->setStatus(enabled == 1);

	alarm_clock = new AlarmClock(SET_SVEGLIA_SINGLEPAGE,
				     static_cast<AlarmClock::Type>(tipo),
				     AlarmClock::NESSUNO,
				     days, hour, minute);
	alarm_clock->setSerNum(getSerNum());
	alarm_clock->hide();

	alarm_clock->_setActive(enabled == 1);
	connect(right_button, SIGNAL(clicked()), alarm_clock, SLOT(showPage()));
	connect(left_button, SIGNAL(clicked()), SLOT(toggleAbil()));

	connect(alarm_clock,SIGNAL(Closed()), SLOT(handleClose()));
	connect(alarm_clock, SIGNAL(alarmClockFired()), SLOT(setButtonIcon()));
}

void bannAlarmClockIcon::setSerNum(int num)
{
	banner::setSerNum(num);
	alarm_clock->setSerNum(num);
}

void bannAlarmClockIcon::handleClose()
{
	// When the page of the alarmclock is closed, the alarm is always set as 'on'.
	left_button->setStatus(true);
	setState(ON);
	emit pageClosed();
}

void bannAlarmClockIcon::setButtonIcon()
{
	if (!alarm_clock->isActive())
		left_button->setStatus(false);
}

void bannAlarmClockIcon::setAbil(bool b)
{
	setState(b ? ON : OFF);
	left_button->setStatus(b);
	alarm_clock->setActive(b);
}

void bannAlarmClockIcon::toggleAbil()
{
	setAbil(!alarm_clock->isActive());
}

void bannAlarmClockIcon::inizializza(bool forza)
{
	BannOnOffState::inizializza(forza);
	alarm_clock->inizializza();
}


calibration::calibration(QWidget *parent, QString icon) : bannOnDx(parent)
{
	SetIcons(icon, 1);
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


impBeep::impBeep(QWidget *parent, QString val, QString icon_on, QString icon_off)
	: bannOnSx(parent)
{
	connect(this, SIGNAL(click()), this, SLOT(toggleBeep()));

	bool on = (val.toInt() == 1);
	sxButton->setOnOff();
	setBeep(on, false);

	SetIcons(0, icon_off, icon_on);
	Draw(); // Draw must be called before setStatus (because it calls the setPixmap function)
	sxButton->setStatus(on);
}

void impBeep::toggleBeep()
{
	if (getBeep())
	{
		setBeep(false, true);
		sxButton->setStatus(false);
	}
	else
	{
		setBeep(true, true);
		sxButton->setStatus(true);
		beep();
	}
}


bannContrast::bannContrast(QWidget *parent, QString val, QString icon) :
	bannOnDx(parent, icon, new Contrast())
{
	setContrast(val.toInt(), false);
	connect(linked_dx_page, SIGNAL(Closed()), SLOT(done()));
}

void bannContrast::done()
{
	setContrast(getContrast(), true);
}


bannVersion::bannVersion(QWidget *parent, QString icon, Version *ver)
	: bannOnDx(parent, icon)
{
	connect(this, SIGNAL(click()), this, SLOT(showVers()));
	v = ver;
}

void bannVersion::showVers()
{
	v->showPage();
	QTimer::singleShot(10000, this, SIGNAL(pageClosed()));
}


impPassword::impPassword(QWidget *parent, QString icon_on, QString icon_off, QString icon_label, QString descr, QString pwd, int attiva)
	: Bann2Buttons(parent)
{
	password = pwd;
	tasti = new Keypad();
	if (password.isEmpty())
	{
		status = PASSWD_NOT_SET;
		tasti->setMode(Keypad::CLEAN);
	}
	else
	{
		status = PASSWD_SET;
		tasti->setMode(Keypad::HIDDEN);
	}
	initBanner(icon_off, icon_label, descr);

	connect(right_button, SIGNAL(clicked()), tasti, SLOT(showPage()));
	connect(left_button, SIGNAL(clicked()), SLOT(toggleActivation()));
	connect(tasti, SIGNAL(Closed()), SLOT(checkPasswd()));

	active = (attiva == 1);
	bt_global::btmain->setPwd(active, password);

	left_button->setOnOff();
	left_button->setPressedImage(icon_on);
	left_button->setStatus(active);
}

void impPassword::toggleActivation()
{
	active = !active;
	setCfgValue("enabled", QString::number(active), PROTEZIONE, getSerNum());
	bt_global::btmain->setPwd(active, password);
	left_button->setStatus(active);
}

void impPassword::showEvent(QShowEvent *event)
{
	Draw();
	qDebug() << "password = " << password;
	if (password.isEmpty())
	{
		qDebug("password = ZERO");
		status = PASSWD_NOT_SET;
	}
	else
	{
		status = PASSWD_SET;
		tasti->setMode(Keypad::HIDDEN);
		qDebug("password not ZERO");
	}
}

void impPassword::checkPasswd()
{
	QString c = tasti->getText();
	tasti->resetText();
	if (status == PASSWD_NOT_SET)
	{
		if (!c.isEmpty())
		{
			password = c;
			setCfgValue("value", password, PROTEZIONE, getSerNum());
			bt_global::btmain->setPwd(active, password);
			status = PASSWD_SET;
		}
		emit pageClosed();
	}
	else // status == PASSWD_SET
	{
		if (c.isEmpty())
		{
			emit pageClosed();
			return;
		}
		if (password != c)
		{
			qDebug() << "password errata doveva essere " << password;
			sb = getBeep();
			setBeep(true,false);
			beep(1000);
			QTimer::singleShot(1100, this, SLOT(restoreBeepState()));
			emit pageClosed();
		}
		else //password is correct
		{
			tasti->setMode(Keypad::CLEAN);
			qDebug("password giusta");
			status = PASSWD_NOT_SET;
			tasti->showPage();
		}
	}
}

void impPassword::restoreBeepState()
{
	setBeep(sb,false);
}

