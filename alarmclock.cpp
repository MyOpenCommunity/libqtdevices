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


#include "alarmclock.h"
#include "generic_functions.h" // setCfgValue, getBeep, setBeep, beep
#include "multisounddiff.h" // contdiff
#include "btbutton.h"
#include "openclient.h"
#include "timescript.h"
#include "bannfrecce.h"
#include "fontmanager.h" // bt_global::font
#include "displaycontrol.h" // (*bt_global::display)
#include "btmain.h" // bt_global::btmain
#include "sounddiffusion.h" // declare SoundDiffusion*

#include <openwebnet.h>

#include <QApplication>
#include <QDateTime>
#include <QPixmap>
#include <QWidget>
#include <QLabel>
#include <QDir>
#include <QFile>
#include <QMap>

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>


AlarmClock::AlarmClock(Type t, Freq f, int hour, int minute)
{
	bannNavigazione = new bannFrecce(this,9);
	bannNavigazione->setGeometry(0 , MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE ,MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);
	aumVolTimer = NULL;

	for (uchar idx = 0; idx < 2; idx++)
	{
		but[idx] = new BtButton(this);

		but[idx]->setGeometry(idx*80 + 50,80,60,60);
		but[idx]->setAutoRepeat(true);
		but[idx]->setImage(ICON_FRECCIA_SU);
	}

	for (uchar idx = 2; idx < 4; idx++)
	{
		but[idx] = new BtButton(this);
		but[idx]->setGeometry((idx-2)*80 + 50,190,60,60);
		but[idx]->setAutoRepeat(true);
		but[idx]->setImage(ICON_FRECCIA_GIU);
	}

	QPixmap Icon(ICON_SVEGLIA_ON);

	Immagine = new QLabel(this);
	if (!Icon.isNull())
		Immagine->setPixmap(Icon);

	Immagine->setGeometry(90,0,80,80);

	QFont aFont = bt_global::font->get(FontManager::TEXT);

	for (uchar idx = 0; idx < 4; idx++)
	{
		choice[idx] = new BtButton(this);
		choice[idx]->setGeometry(10,idx*60,60,60);
		choice[idx]->setCheckable(true);
		choice[idx]->setImage(ICON_VUOTO);
		choice[idx]->hide();

		testiChoice[idx] = new QLabel(this);
		testiChoice[idx]->setGeometry(80,idx*60,120,60);
		testiChoice[idx]->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		testiChoice[idx]->setFont(aFont);
		testiChoice[idx]->hide();
    }

	dataOra = NULL;
	testiChoice[0]->setText(tr("once"));
	testiChoice[1]->setText(tr("always"));
	testiChoice[2]->setText(tr("mon-fri"));
	testiChoice[3]->setText(tr("sat-sun"));

	oraSveglia = new QDateTime();
	oraSveglia->setTime(QTime(hour, minute));
	oraSveglia->setDate(QDate::currentDate());
	dataOra = new timeScript(this,2,oraSveglia);
	dataOra->setGeometry(40,140,160,50);
	dataOra->setFrameStyle(QFrame::Plain);
	dataOra->setLineWidth(0);

	for (uchar idx = 0; idx < AMPLI_NUM; idx++)
		volSveglia[idx] = -1;
	minuTimer = NULL;
	freq = f;
	type = t;

	difson = 0;
	connect(bt_global::btmain, SIGNAL(freezed(bool)), SLOT(freezed(bool)));
}

void AlarmClock::okTime()
{
	disconnect(but[0], SIGNAL(clicked()), dataOra, SLOT(aumOra()));
	disconnect(but[1], SIGNAL(clicked()), dataOra, SLOT(aumMin()));
	disconnect(but[2], SIGNAL(clicked()), dataOra, SLOT(diminOra()));
	disconnect(but[3], SIGNAL(clicked()), dataOra, SLOT(diminMin()));
	disconnect(bannNavigazione,SIGNAL(forwardClick()),this,SLOT(okTime()));
	connect(bannNavigazione,SIGNAL(forwardClick()),this,SLOT(okTipo()));

	for (uchar idx = 0; idx < 4; idx++)
		but[idx]->hide();

	for (uchar idx = 0; idx < 4; idx++)
	{
		choice[idx]->show();
		testiChoice[idx]->show();
		choice[idx]->setChecked(freq == idx);
	}

	dataOra->hide();
	Immagine->hide();

	if (type != DI_SON)
		bannNavigazione->nascondi(banner::BUT2);
}

void AlarmClock::showPage()
{
	for (uchar idx = 0; idx < 4; idx++)
		but[idx]->show();
	dataOra->show();
	Immagine->show();
	if (!difson)
		if (type == DI_SON)
		{
			if (bt_global::btmain->difSon)
				difson = bt_global::btmain->difSon;
			if (bt_global::btmain->dm)
				difson = bt_global::btmain->dm;
		}

	if (difson)
		difson->hide();

	bannNavigazione->show();
	for (uchar idx = 0; idx < 4; idx++)
	{
		choice[idx]->hide();
		testiChoice[idx]->hide();
	}

	disconnect(but[0],SIGNAL(clicked()),dataOra,SLOT(aumOra()));
	disconnect(but[1],SIGNAL(clicked()),dataOra,SLOT(aumMin()));
	disconnect(but[2],SIGNAL(clicked()),dataOra,SLOT(diminOra()));
	disconnect(but[3],SIGNAL(clicked()),dataOra,SLOT(diminMin()));

	connect(but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumOra()));
	connect(but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumMin()));
	connect(but[2] ,SIGNAL(clicked()),dataOra,SLOT(diminOra()));
	connect(but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminMin()));
	connect(bannNavigazione, SIGNAL(forwardClick()), this, SLOT(okTime()));
	disconnect(bannNavigazione,SIGNAL(forwardClick()), this, SLOT(okTipo()));
	disconnect(bannNavigazione,SIGNAL(forwardClick()), this, SLOT(handleClose()));

	disconnect(bannNavigazione, SIGNAL(backClick()), this, SLOT(handleClose()));
	connect(bannNavigazione, SIGNAL(backClick()), this, SLOT(handleClose()));

	connect(choice[0],SIGNAL(toggled(bool)),this,SLOT(sel1(bool)));
	connect(choice[1],SIGNAL(toggled(bool)),this,SLOT(sel2(bool)));
	connect(choice[2],SIGNAL(toggled(bool)),this,SLOT(sel3(bool)));
	connect(choice[3],SIGNAL(toggled(bool)),this,SLOT(sel4(bool)));
	aggiornaDatiEEprom = 0;
	if (type != DI_SON)
		bannNavigazione->mostra(banner::BUT2);

	Page::showPage();
}

void AlarmClock::drawSelectPage()
{
	for (uchar idx = 0; idx < 4; idx++)
		choice[idx]->setChecked(idx == freq);
}

void AlarmClock::sel1(bool isOn)
{
	if (isOn)
		freq = ONCE;
	drawSelectPage();
}

void AlarmClock::sel2(bool isOn)
{
	if (isOn)
		freq = SEMPRE;
	drawSelectPage();
}

void AlarmClock::sel3(bool isOn)
{
	if (isOn)
		freq = FERIALI;
	drawSelectPage();
}

void AlarmClock::sel4(bool isOn)
{
	if (isOn)
		freq = FESTIVI;
	drawSelectPage();
}

void AlarmClock::handleClose()
{
	//imposta la sveglia in
	if (difson)
	{
		disconnect(difson, SIGNAL(Closed()), this, SLOT(handleClose()));
		if (aggiornaDatiEEprom)
		{
#if defined (BT_EMBEDDED)
			int eeprom;
			eeprom = open("/dev/nvram", O_RDWR | O_SYNC, 0666);
			lseek(eeprom,BASE_EEPROM + (serNum-1)*(AMPLI_NUM+KEY_LENGTH+SORG_PAR) + KEY_LENGTH, SEEK_SET);
			for (unsigned int idx = 0; idx < AMPLI_NUM; idx++)
				write(eeprom,&volSveglia[idx],1);
			write(eeprom,&sorgente,1);
			write(eeprom,&stazione,1);
			::close(eeprom);
#endif
		}
	}

	gesFrameAbil = false;
	setActive(true);
	emit Closed();
	delete oraSveglia;
	oraSveglia = new QDateTime(dataOra->getDataOra());

	QMap<QString, QString> data;
	data["hour"] = oraSveglia->time().toString("hh");
	data["minute"] = oraSveglia->time().toString("mm");
	data["alarmset"] = QString::number(freq);

	setCfgValue(data, SET_SVEGLIA, serNum);
}

void AlarmClock::okTipo()
{
	disconnect(bannNavigazione ,SIGNAL(forwardClick()),this,SLOT(okTipo()));
	connect(bannNavigazione ,SIGNAL(forwardClick()),this,SLOT(handleClose()));
	for (uchar idx = 0; idx < 4; idx++)
	{
		choice[idx]->hide();
		testiChoice[idx]->hide();
	}
	Immagine->show();
	if (type != DI_SON)
		handleClose();
	else if (difson)
	{
		bannNavigazione->hide();
		// reparent only if we have a multichannel sound diffusion
		if (bt_global::btmain->dm)
			difson->setParent(this);
		connect(difson, SIGNAL(Closed()), SLOT(handleClose()));
		difson->forceDraw();

		aggiornaDatiEEprom = 1;
		gesFrameAbil = true;
		sorgente = 101;
		stazione = 0;
		for (unsigned idx = 0; idx < AMPLI_NUM; idx++)
			volSveglia[idx] = -1;
		difson->showPage();
	}
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

void AlarmClock::gestFrame(char* f)
{
	if (gesFrameAbil == false)
		return;

	openwebnet msg_open;
	int deviceAddr;

	msg_open.CreateMsgOpen(f, strstr(f, "##") - f + 2);

	if (!strcmp(msg_open.Extract_chi(),"16"))
	{
		deviceAddr = atoi(msg_open.Extract_dove());
		if (deviceAddr >= 0 && deviceAddr <= AMPLI_NUM)
		{
			if (!msg_open.IsMeasureFrame())
			{
				if (!strcmp(msg_open.Extract_cosa(),"13"))
				{
					volSveglia[deviceAddr] = -1;
					if (deviceAddr == 0)
						for (uchar idx = 0; idx < AMPLI_NUM; idx++)
							volSveglia[idx] = -1;

					if (deviceAddr < 10)
						for (uchar idx = 0;idx < 10; idx++)
							volSveglia[deviceAddr * 10 + idx] = -1;
				}
				if (!strcmp(msg_open.Extract_cosa(),"3"))
				{
					qDebug("ho visto un ampli acceso!");
					sendFrame("*#16*" + QString::number(deviceAddr) + "*1##");
				}
			}
			else
			{
				if (!strcmp(msg_open.Extract_grandezza(),"1"))
				{
					int vol;
					vol = atoi(msg_open.Extract_valori(0)) & 0x1F;
					volSveglia[deviceAddr] = vol;
					qDebug("o visto un volume di %d pari a %d",deviceAddr, vol);
				}
			}
		}
		else
		{
			if (!strcmp(msg_open.Extract_cosa(),"3"))
			{
				sorgente = deviceAddr;
				if (sorgente > 109)
					sorgente = sorgente - ((sorgente-100)/10)*10;
				qDebug("Sorgente %d", sorgente);
			}
			if (msg_open.IsMeasureFrame() && (!strcmp(msg_open.Extract_grandezza(),"7")))
			{
				stazione = atoi(msg_open.Extract_valori(1))&0x1F;
				qDebug("Stazione %d",stazione);
				sorgente = deviceAddr;
				if (sorgente > 109)
					sorgente = sorgente-((sorgente-100)/10)*10;
				qDebug("Sorgente %d", sorgente);
			}
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
	(*bt_global::display).forceOperativeMode(false);
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
		qDebug("secsTo: %d",oraSveglia->time().secsTo(actualDateTime.time()));
		if ((actualDateTime.time() >= (oraSveglia->time())) && ((oraSveglia->time().secsTo(actualDateTime.time())<60)))
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
				(*bt_global::display).forceOperativeMode(true); // Prevent the screeensaver start
				qApp->installEventFilter(this);
				bt_global::btmain->svegl(true);
			}
			else
				assert(!"Unknown sveglia type!");

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
		(*bt_global::display).forceOperativeMode(false);
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
		(*bt_global::display).setState(DISPLAY_OPERATIVE);
	else
		(*bt_global::display).setState(DISPLAY_FREEZED);

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

void AlarmClock::inizializza()
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	if (type == DI_SON)
	{
		int eeprom;
		char chiave[6];

		memset(chiave,'\000',sizeof(chiave));
		eeprom = open("/dev/nvram", O_RDWR | O_SYNC, 0666);
		lseek(eeprom, BASE_EEPROM+(serNum-1)*(AMPLI_NUM+KEY_LENGTH+SORG_PAR),SEEK_SET);
		read(eeprom, chiave, 5);

		if (strcmp(chiave,AL_KEY))
		{
			lseek(eeprom, BASE_EEPROM+(serNum-1)*(AMPLI_NUM+KEY_LENGTH+SORG_PAR), SEEK_SET);
			write(eeprom,AL_KEY,5);
			for (unsigned int idx = 0; idx < AMPLI_NUM; idx++)
			{
				write(eeprom,"\000",1);
				volSveglia[idx]=-1;
			}
		}
		else
		{
			int ploffete = BASE_EEPROM + (serNum-1)*(AMPLI_NUM+KEY_LENGTH+SORG_PAR) + KEY_LENGTH;
			lseek(eeprom,ploffete, SEEK_SET);
			for (unsigned int idx = 0; idx < AMPLI_NUM; idx++)
			{
				read(eeprom,&volSveglia[idx],1);
				volSveglia[idx]&=0x1F;
			}
			read(eeprom,&sorgente,1);
			read(eeprom,&stazione,1);
		}
	::close(eeprom);    // servono i:: se no fa la close() di QWidget
	}
#endif
}
