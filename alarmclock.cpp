#include "alarmclock.h"
#include "generic_functions.h" // setCfgValue
#include "hardware_functions.h" // getBeep, setBeep, beep
#include "multisounddiff.h" // contdiff
#include "btbutton.h"
#include "openclient.h"
#include "datetime.h"
#include "fontmanager.h" // bt_global::font
#include "displaycontrol.h" // bt_global::display
#include "btmain.h" // bt_global::btmain
#include "sounddiffusion.h" // declare SoundDiffusion*
#include "singlechoicecontent.h"
#include "skinmanager.h"
#include "alarmsounddiff_device.h"

#include <openmsg.h>

#include <QApplication>
#include <QDateTime>
#include <QWidget>
#include <QLabel>
#include <QMap>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>


// Alarmnavigation implementation

AlarmNavigation::AlarmNavigation(bool forwardButton, QWidget *parent)
		: QWidget(parent)
{
	QHBoxLayout *l = new QHBoxLayout(this);

	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);

	BtButton *ok = new BtButton();
	connect(ok, SIGNAL(clicked()), SIGNAL(okClicked()));
	ok->setImage(bt_global::skin->getImage("ok"));
	l->addWidget(ok);
	l->addStretch(1);

	if (forwardButton)
	{
		BtButton *forward = new BtButton();
		connect(forward, SIGNAL(clicked()), SIGNAL(forwardClicked()));
		forward->setImage(bt_global::skin->getImage("forward"));
		l->addWidget(forward);
	}
}

// AlarmClock implementation

AlarmClock::AlarmClock(Type t, Freq f, int hour, int minute)
{
	aumVolTimer = NULL;
	alarmTime = QTime(hour, minute);
	minuTimer = NULL;
	freq = f;
	type = t;

	for (uchar idx = 0; idx < AMPLI_NUM; idx++)
		volSveglia[idx] = -1;

	alarm_time = new AlarmClockTime(this);
	alarm_type = new AlarmClockFreq(this);
	if (type == DI_SON)
		alarm_sound_diff = new AlarmClockSoundDiff(this);
	else
		alarm_sound_diff = NULL;

	connect(bt_global::btmain, SIGNAL(freezed(bool)), SLOT(freezed(bool)));
	connect(alarm_type, SIGNAL(selectionChanged(AlarmClock::Freq)), SLOT(setFreq(AlarmClock::Freq)));

	dev = new AlarmSoundDiffDevice();
	connect(dev, SIGNAL(status_changed(const StatusList &)),
		SLOT(status_changed(const StatusList &)));
}

void AlarmClock::showPage()
{
	alarm_time->showPage();
}

void AlarmClock::showTypePage()
{
	alarm_type->showPage();
}

void AlarmClock::handleClose()
{
	dev->setReceiveFrames(false);
	setActive(true);
	emit Closed();
	alarmTime = alarm_time->getAlarmTime();

	QMap<QString, QString> data;
	data["hour"] = alarmTime.toString("hh");
	data["minute"] = alarmTime.toString("mm");
	data["alarmset"] = QString::number(freq);

	setCfgValue(data, SET_SVEGLIA, serNum);

	if (aggiornaDatiEEprom)
		setAlarmVolumes(serNum-1, volSveglia, sorgente, stazione);
}

void AlarmClock::showSoundDiffPage()
{
	aggiornaDatiEEprom = 1;
	sorgente = 101;
	stazione = 0;
	dev->setReceiveFrames(true);
	for (unsigned idx = 0; idx < AMPLI_NUM; idx++)
		volSveglia[idx] = -1;

	alarm_sound_diff->showPage();
}

void AlarmClock::setActive(bool a)
{
	_setActive(a);
	setCfgValue("enabled", active ? "1" : "0", SET_SVEGLIA, serNum);
}

void AlarmClock::_setActive(bool a)
{
	active = a;
	if (active)
	{
		if (!minuTimer)
		{
			minuTimer = new QTimer(this);
			minuTimer->start(200);
			connect(minuTimer,SIGNAL(timeout()), this,SLOT(verificaSveglia()));
		}
	}
	else
	{
		if (minuTimer)
		{
			minuTimer->stop();
			disconnect(minuTimer,SIGNAL(timeout()), this,SLOT(verificaSveglia()));
			delete minuTimer;
			minuTimer = NULL;
		}
	}
}

void AlarmClock::status_changed(const StatusList &sl)
{
	if (sl.contains(AlarmSoundDiffDevice::DIM_STATUS))
	{
		bool status = sl[AlarmSoundDiffDevice::DIM_STATUS].toBool();
		int amplifier = sl[AlarmSoundDiffDevice::DIM_AMPLIFIER].toInt();

		if (status)
		{
			int volume = sl[AlarmSoundDiffDevice::DIM_VOLUME].toInt();

			volSveglia[amplifier] = volume;
		}
		else
			volSveglia[amplifier] = -1;
	}

	if (sl.contains(AlarmSoundDiffDevice::DIM_SOURCE))
		sorgente = sl[AlarmSoundDiffDevice::DIM_SOURCE].toInt();
	if (sl.contains(AlarmSoundDiffDevice::DIM_RADIO_STATION))
		stazione = sl[AlarmSoundDiffDevice::DIM_RADIO_STATION].toInt();
}

bool AlarmClock::eventFilter(QObject *obj, QEvent *ev)
{
	// Discard the mouse press and mouse double click
	if (ev->type() == QEvent::MouseButtonPress || ev->type() == QEvent::MouseButtonDblClick)
		return true;

	if (ev->type() != QEvent::MouseButtonRelease)
		return false;

	// We stop the alarm and restore the normal behaviour
	qApp->removeEventFilter(this);
	spegniSveglia(false);
	bt_global::display.forceOperativeMode(false);
	return true;
}

void AlarmClock::verificaSveglia()
{
	if (!active)
		return;

	QDateTime actualDateTime = QDateTime::currentDateTime();

	if (freq == SEMPRE || freq == ONCE ||
		(freq == FERIALI && actualDateTime.date().dayOfWeek() < 6) ||
		(freq == FESTIVI && actualDateTime.date().dayOfWeek() > 5))
	{
		qDebug("secsTo: %d",alarmTime.secsTo(actualDateTime.time()));
		if ((actualDateTime.time() >= alarmTime) && (alarmTime.secsTo(actualDateTime.time())<60))
		{
			if (type == BUZZER)
			{
				aumVolTimer = new QTimer(this);
				aumVolTimer->start(100);
				connect(aumVolTimer,SIGNAL(timeout()),this,SLOT(buzzerAlarm()));
				contaBuzzer = 0;
				conta2min = 0;
				bt_global::btmain->freeze(true);
				bt_global::btmain->svegl(true);
			}
			else if (type == DI_SON)
			{
				aumVolTimer = new QTimer(this);
				aumVolTimer->start(3000);
				connect(aumVolTimer,SIGNAL(timeout()),this,SLOT(aumVol()));
				conta2min = 0;
				// When the alarm ring we have to put the light on (like in the
				// operative mode) but with a screen "locked" (like in the freezed
				// mode). We do that with an event filter.
				bt_global::btmain->freeze(false); // To stop a screensaver, if running
				bt_global::display.forceOperativeMode(true); // Prevent the screeensaver start
				qApp->installEventFilter(this);
				bt_global::btmain->svegl(true);
			}
			else
				qFatal("Unknown sveglia type!");

			qDebug("PARTE LA SVEGLIA");

		if (freq == ONCE)
			setActive(false);
		}
	}
	if (active)
		minuTimer->start((60-actualDateTime.time().second())*1000);
}

bool AlarmClock::isActive()
{
	return active;
}

void AlarmClock::aumVol()
{
	if (conta2min == 0)
	{
		dev->startAlarm(sorgente, stazione, volSveglia);
		conta2min = 9;
	}

	conta2min++;
	if (conta2min < 32)
	{
		for (int idx = 0; idx < AMPLI_NUM; idx++)
		{
			if (volSveglia[idx] >= conta2min)
				dev->setVolume(idx, conta2min);
		}
	}
	else if (conta2min > 49)
	{
		qDebug("SPENGO LA SVEGLIA per timeout");
		aumVolTimer->stop();
		delete aumVolTimer;
		aumVolTimer = NULL;

		dev->stopAlarm(sorgente, volSveglia);

		bt_global::btmain->freeze(false);
		bt_global::btmain->svegl(false);
		bt_global::display.forceOperativeMode(false);
		emit alarmClockFired();
	}
}

void AlarmClock::buzzerAlarm()
{
	if (contaBuzzer == 0)
	{
		buzAbilOld = getBeep();
		setBeep(true,false);
	}
	if  (contaBuzzer % 2 == 0)
	{
		if (((contaBuzzer + 2) % 12) && (contaBuzzer % 12))
			beep(10);
	}

	if (contaBuzzer % 8 == 0)
		bt_global::display.setState(DISPLAY_OPERATIVE);
	else
		bt_global::display.setState(DISPLAY_FREEZED);

	contaBuzzer++;
	if (contaBuzzer >= 10*60*2)
	{
		qDebug("SPENGO LA SVEGLIA");
		aumVolTimer->stop();
		setBeep(buzAbilOld,false);
		delete aumVolTimer;
		aumVolTimer = NULL;
		bt_global::btmain->freeze(false);
		bt_global::btmain->svegl(false);
		emit alarmClockFired();
	}
}

void AlarmClock::freezed(bool b)
{
	// We use freeze only for the buzzer (for the difson we use an event Filter)
	// TODO: use event filter even for the first.
	if (!b && type == BUZZER)
		spegniSveglia(b);
}

void AlarmClock::spegniSveglia(bool b)
{
	if (!b && aumVolTimer)
	{
		if (aumVolTimer->isActive())
		{
			qDebug("SPENGO LA SVEGLIA");
			aumVolTimer->stop();
			if (type == BUZZER)
				setBeep(buzAbilOld,false);
			else
				dev->stopAlarm(sorgente, volSveglia);

			delete aumVolTimer;
			aumVolTimer = NULL;
			bt_global::btmain->svegl(false);
			emit alarmClockFired();
		}
	}
	else if (b)
	{
		if (isVisible())
			handleClose();
	}
}

void AlarmClock::setSerNum(int s)
{
	serNum = s;
}

void AlarmClock::setFreq(AlarmClock::Freq f)
{
	freq = f;
}

void AlarmClock::inizializza()
{
	getAlarmVolumes(serNum-1, volSveglia, &sorgente, &stazione);
}

// AlarmClockTime implementation

AlarmClockTime::AlarmClockTime(AlarmClock *alarm_page)
{
	AlarmNavigation *navigation = new AlarmNavigation(true);

	QLabel *icon = new QLabel;
	icon->setPixmap(bt_global::skin->getImage("alarm_icon"));

	edit = new BtTimeEdit(this);
	edit->setTimeWithSeconds(alarm_page->alarmTime);

	QWidget *content = new QWidget;
	QVBoxLayout *l = new QVBoxLayout(content);
	QHBoxLayout *r = new QHBoxLayout;

	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	r->setContentsMargins(0, 0, 0, 0);
	r->setSpacing(0);

	r->addSpacing(40);
	r->addWidget(edit);
	r->addSpacing(40);

	// top level layout
	l->addWidget(icon, 0, Qt::AlignHCenter);
	l->addSpacing(10);
	l->addLayout(r);

	connect(navigation, SIGNAL(forwardClicked()), alarm_page, SLOT(showTypePage()));
	connect(navigation, SIGNAL(okClicked()), alarm_page, SLOT(handleClose()));

	buildPage(content, navigation);
}

QTime AlarmClockTime::getAlarmTime() const
{
	BtTime t = edit->time();

	return QTime(t.hour(), t.minute());
}

// AlarmClockFreq implementation

AlarmClockFreq::AlarmClockFreq(AlarmClock *alarm_page)
{
	AlarmNavigation *navigation = new AlarmNavigation(alarm_page->type == AlarmClock::DI_SON);

	content = new SingleChoiceContent(this);
	content->addBanner(tr("once"), AlarmClock::ONCE);
	content->addBanner(tr("always"), AlarmClock::SEMPRE);
	content->addBanner(tr("mon-fri"), AlarmClock::FERIALI);
	content->addBanner(tr("sat-sun"), AlarmClock::FESTIVI);

	connect(content, SIGNAL(bannerSelected(int)),
		SLOT(setSelection(int)));

	connect(navigation,SIGNAL(forwardClicked()),alarm_page,SLOT(showSoundDiffPage()));
	connect(navigation, SIGNAL(okClicked()), alarm_page, SLOT(handleClose()));

	content->setCheckedId(alarm_page->freq);

	buildPage(content, navigation);
}

void AlarmClockFreq::setSelection(int freq)
{
	emit selectionChanged(AlarmClock::Freq(freq));
}

// AlarmClockSoundDiff

AlarmClockSoundDiff::AlarmClockSoundDiff(AlarmClock *alarm_page)
{
	if (bt_global::btmain->difSon)
		difson = bt_global::btmain->difSon;
	if (bt_global::btmain->dm)
		difson = bt_global::btmain->dm;

	connect(this, SIGNAL(Closed()), alarm_page, SLOT(handleClose()));
}

void AlarmClockSoundDiff::showPage()
{
	Page::showPage();

	// reparent only if we have a multichannel sound diffusion
	if (bt_global::btmain->dm)
		difson->setParent(this);
	difson->forceDraw();
	difson->showPage();

	connect(difson, SIGNAL(Closed()), SLOT(handleClose()));
}

void AlarmClockSoundDiff::handleClose()
{
	disconnect(difson, SIGNAL(Closed()), this, SLOT(handleClose()));
	emit Closed();
}
