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

	subscribe_monitor(16);
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
	gesFrameAbil = false;
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
	gesFrameAbil = true;
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

void AlarmClock::manageFrame(OpenMsg &msg)
{
	if (gesFrameAbil == false)
		return;

	int deviceAddr = atoi(msg.Extract_dove());
	if (deviceAddr >= 0 && deviceAddr <= AMPLI_NUM)
	{
		if (!msg.IsMeasureFrame())
		{
			if (!strcmp(msg.Extract_cosa(),"13"))
			{
				volSveglia[deviceAddr] = -1;
				if (deviceAddr == 0)
					for (uchar idx = 0; idx < AMPLI_NUM; idx++)
						volSveglia[idx] = -1;

				if (deviceAddr < 10)
					for (uchar idx = 0;idx < 10; idx++)
						volSveglia[deviceAddr * 10 + idx] = -1;
			}
			if (!strcmp(msg.Extract_cosa(),"3"))
			{
				qDebug("ho visto un ampli acceso!");
				sendFrame("*#16*" + QString::number(deviceAddr) + "*1##");
			}
		}
		else
		{
			if (!strcmp(msg.Extract_grandezza(),"1"))
			{
				int vol;
				vol = atoi(msg.Extract_valori(0)) & 0x1F;
				volSveglia[deviceAddr] = vol;
				qDebug("o visto un volume di %d pari a %d",deviceAddr, vol);
			}
		}
	}
	else
	{
		if (!strcmp(msg.Extract_cosa(),"3"))
		{
			sorgente = deviceAddr;
			if (sorgente > 109)
				sorgente = sorgente - ((sorgente-100)/10)*10;
			qDebug("Sorgente %d", sorgente);
		}
		if (msg.IsMeasureFrame() && (!strcmp(msg.Extract_grandezza(),"7")))
		{
			stazione = atoi(msg.Extract_valori(1))&0x1F;
			qDebug("Stazione %d",stazione);
			sorgente = deviceAddr;
			if (sorgente > 109)
				sorgente = sorgente-((sorgente-100)/10)*10;
			qDebug("Sorgente %d", sorgente);
		}
	}
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
	bool amb1 = false;
	bool amb2 = false;
	bool amb3 = false;
	bool amb4 = false;
	bool amb5 = false;
	bool amb6 = false;
	bool amb7 = false;
	bool amb8 = false;

	if (conta2min == 0)
	{
		QString f = QString("*22*35#4#0#%2*3#1#0##").arg(sorgente-100);
		sendFrame(f);
		for (int idx = 0; idx < AMPLI_NUM; idx++)
		{
			if (volSveglia[idx] > -1)
			{
				if (idx >= 10 && idx <= 19)
				{
					if (!amb1)
					{
						amb1 = true;
						f = QString("*22*35#4#1#%2*3#1#0##").arg(sorgente-100);
						sendFrame(f);
					}
				}
				if (idx >= 20 && idx <= 29)
				{
					if (!amb2)
					{
						amb2 = true;
						f = QString("*22*35#4#2#%2*3#2#0##").arg(sorgente-100);
						sendFrame(f);
					}
				}
				if (idx >= 30 && idx <= 39)
				{
					if (!amb3)
					{
						amb3 = true;
						f = QString("*22*35#4#3#%2*3#3#0##").arg(sorgente-100);
						sendFrame(f);
					}
				}
				if (idx >= 40 && idx <= 49)
				{
					if (!amb4)
					{
						amb4 = true;
						f = QString("*22*35#4#4#%2*3#4#0##").arg(sorgente-100);
						sendFrame(f);
					}
				}
				if (idx >= 50 && idx <= 59)
				{
					if (!amb5)
					{
						amb5 = true;
						f = QString("*22*35#4#5#%2*3#5#0##").arg(sorgente-100);
						sendFrame(f);
					}
				}
				if (idx >= 60 && idx <= 69)
				{
					if (!amb6)
					{
						amb6 = true;
						f = QString("*22*35#4#6#%2*3#6#0##").arg(sorgente-100);
						sendFrame(f);
					}
				}
				if (idx >= 70 && idx <= 79)
				{
					if (!amb7)
					{
						amb7 = true;
						f = QString("*22*35#4#7#%2*3#7#0##").arg(sorgente-100);
						sendFrame(f);
					}
				}
				if (idx >= 80 && idx <= 89)
				{
					if (!amb8)
					{
						amb8 = true;
						f = QString("*22*35#4#8#%2*3#8#0##").arg(sorgente-100);
						sendFrame(f);
					}
				}
				if (volSveglia[idx] < 10)
				{
					QString f;
					sendFrame(f.sprintf("*#16*%02d*#1*%d##", idx, volSveglia[idx]));
				}
				else
				{
					QString f;
					sendFrame(f.sprintf("*#16*%02d*#1*8##", idx));
				}

				QString f;
				sendFrame(f.sprintf("*16*3*%02d##", idx));
			}
		}
		conta2min = 9;
		sendFrame("*#16*" + QString::number(sorgente) + "*#7*" + QString::number(stazione) + "##");
	}

	conta2min++;
	if (conta2min < 32)
	{
		for (int idx = 0; idx < AMPLI_NUM; idx++)
		{
			if (volSveglia[idx] >= conta2min)
			{
				QString f;
				sendFrame(f.sprintf("*#16*%02d*#1*%d##", idx, conta2min));
			}
		}
	}
	else if (conta2min > 49)
	{
		qDebug("SPENGO LA SVEGLIA per timeout");
		aumVolTimer->stop();
		delete aumVolTimer;
		aumVolTimer = NULL;

		openwebnet msg_open;
		for (int idx = 0; idx < AMPLI_NUM; idx++)
		{
			if (volSveglia[idx]>-1)
			{
				QString f;
				sendFrame(f.sprintf("*16*13*%02d##", idx));
			}
		}
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
	edit->setTime(alarm_page->alarmTime);

	QVBoxLayout *l = new QVBoxLayout(this);
	QHBoxLayout *r = new QHBoxLayout;

	r->addSpacing(40);
	r->addWidget(edit);
	r->addSpacing(40);

	// top level layout
	l->addWidget(icon, 0, Qt::AlignHCenter);
	l->addSpacing(10);
	l->addLayout(r);
	l->addWidget(navigation);

	l->setContentsMargins(0, 10, 0, 10);
	l->setSpacing(0);

	connect(navigation, SIGNAL(forwardClicked()), alarm_page, SLOT(showTypePage()));
	connect(navigation, SIGNAL(okClicked()), alarm_page, SLOT(handleClose()));
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

	QVBoxLayout *l = new QVBoxLayout(this);

	l->addWidget(content, 1);
	l->addWidget(navigation);
	l->setContentsMargins(0, 0, 0, 10);
	l->setSpacing(0);
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
