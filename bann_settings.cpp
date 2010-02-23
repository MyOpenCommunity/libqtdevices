/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


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


bannAlarmClock::bannAlarmClock(QWidget *parent, int hour, int minute, QString icon1,
	QString icon2, QString icon3, int enabled, int tipo, int freq)
	: bann2But(parent)
{
	sxButton->setOnOff();
	sxButton->setStatus(enabled == 1);

	SetIcons(0, icon2, icon1);
	SetIcons(1, icon3);
	Draw(); // Draw must be called before setAbil.. see impBeep

	alarm_clock = new AlarmClock(static_cast<AlarmClock::Type>(tipo), static_cast<AlarmClock::Freq>(freq), hour, minute);
	alarm_clock->setSerNum(getSerNum());
	alarm_clock->hide();

	alarm_clock->_setActive(enabled == 1);
	connect(this, SIGNAL(dxClick()), alarm_clock, SLOT(showPage()));
	connect(this, SIGNAL(sxClick()), this, SLOT(toggleAbil()));

	connect(alarm_clock,SIGNAL(Closed()), SLOT(handleClose()));
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

void bannAlarmClock::gestFrame(char* frame)
{
	alarm_clock->gestFrame(frame);
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

void bannAlarmClock::inizializza()
{
	alarm_clock->inizializza();
}


calibration::calibration(sottoMenu *parent, QString icon) : bannOnDx(parent)
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


impBeep::impBeep(sottoMenu *parent, QString val, QString icon1, QString icon2)
	: bannOnSx(parent)
{
	connect(this, SIGNAL(click()), this, SLOT(toggleBeep()));

	bool on = (val.toInt() == 1);
	sxButton->setOnOff();
	setBeep(on, false);

	SetIcons(0, icon2, icon1);
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


bannContrast::bannContrast(sottoMenu *parent, QString val, QString icon) :
	bannOnDx(parent, icon, new Contrast())
{
	setContrast(val.toInt(), false);
	connect(linked_dx_page, SIGNAL(Closed()), SLOT(done()));
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
	QTimer::singleShot(10000, this, SIGNAL(pageClosed()));
}


impPassword::impPassword(QWidget *parent, QString icon1, QString icon2, QString icon3, QString pwd, int attiva)
	: bann2But(parent)
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

	connect(this, SIGNAL(dxClick()), tasti, SLOT(showPage()));
	connect(this, SIGNAL(sxClick()), this, SLOT(toggleActivation()));
	connect(tasti, SIGNAL(Closed()), this, SLOT(checkPasswd()));

	SetIcons(1, icon3);
	SetIcons(0, icon2, icon1);
	Draw();

	active = (attiva == 1);
	bt_global::btmain->setPwd(active, password);

	sxButton->setOnOff();
	sxButton->setStatus(active);
}

void impPassword::toggleActivation()
{
	active = !active;
	setCfgValue("enabled", QString::number(active), PROTEZIONE, getSerNum());
	bt_global::btmain->setPwd(active, password);
	sxButton->setStatus(active);
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
			QTimer::singleShot(1100, this, SLOT(tiempout()));
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

void impPassword::tiempout()
{
	setBeep(sb,false);
}

