
/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**sveglia.cpp
**
**Sottomenù imposta sveglia
**
****************************************************************/


#include "sveglia.h"
#include "generic_functions.h" // setCfgValue, setBacklight, getBeep, setBeep, beep, setBacklight
#include "diffmulti.h"
#include "btbutton.h"
#include "openclient.h"
#include "timescript.h"
#include "bannfrecce.h"
#include "fontmanager.h"
#include "brightnesscontrol.h"
#include "btmain.h"
#include "main.h" // BTouch

#include <openwebnet.h>

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


sveglia::sveglia(QWidget *parent, uchar t, uchar freq, contdiff *diso, char *f, int hour, int minute)
	: QWidget(parent)
{
	// TODO: rimuovere gestione frame!
	bannNavigazione = new bannFrecce(this,9);
	bannNavigazione->setGeometry(0 , MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE ,MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);
	aumVolTimer = NULL;
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

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

	QFont aFont;
	FontManager::instance()->getFont(font_sveglia_scelta, aFont);
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
	difson = diso;
	if (difson)
		difson->hide();

	for (uchar idx = 0; idx < AMPLI_NUM; idx++)
		volSveglia[idx] = -1;
	minuTimer = NULL;
	tipoSveglia = freq;
	tipo = t;
	qDebug("tipoSveglia= %d - tipo= %d",tipoSveglia, tipo);
	if (tipo == FRAME)
		frame = f;

	connect(BTouch, SIGNAL(freezed(bool)), SLOT(spegniSveglia(bool)));
}

void sveglia::okTime()
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
		choice[idx]->setChecked(tipoSveglia == idx);
	}

	dataOra->hide();
	Immagine->hide();
	
	if (tipo != DI_SON)
		bannNavigazione->nascondi(banner::BUT2);
}

void sveglia::mostra()
{
	for (uchar idx = 0; idx < 4; idx++)
		but[idx]->show();
	dataOra->show();
	Immagine->show();

	bannNavigazione->show();
	for (uchar idx = 0; idx < 4; idx++)
	{
		choice[idx]->hide();
		testiChoice[idx]->hide();
	}

	showFullScreen();

	disconnect(but[0],SIGNAL(clicked()),dataOra,SLOT(aumOra()));
	disconnect(but[1],SIGNAL(clicked()),dataOra,SLOT(aumMin()));
	disconnect(but[2],SIGNAL(clicked()),dataOra,SLOT(diminOra()));
	disconnect(but[3],SIGNAL(clicked()),dataOra,SLOT(diminMin()));

	connect(but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumOra()));
	connect(but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumMin()));
	connect(but[2] ,SIGNAL(clicked()),dataOra,SLOT(diminOra()));
	connect(but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminMin()));
	connect(bannNavigazione  ,SIGNAL(forwardClick()),this,SLOT(okTime()));
	disconnect(bannNavigazione ,SIGNAL(forwardClick()),this,SLOT(okTipo()));
	disconnect(bannNavigazione ,SIGNAL(forwardClick()),this,SLOT(Closed()));

	disconnect(bannNavigazione  ,SIGNAL(backClick()),this,SLOT(Closed()));
	connect(bannNavigazione  ,SIGNAL(backClick()),this,SLOT(Closed()));

	if (difson)
		difson->connectClosed(this);

	connect(choice[0],SIGNAL(toggled(bool)),this,SLOT(sel1(bool)));
	connect(choice[1],SIGNAL(toggled(bool)),this,SLOT(sel2(bool)));
	connect(choice[2],SIGNAL(toggled(bool)),this,SLOT(sel3(bool)));
	connect(choice[3],SIGNAL(toggled(bool)),this,SLOT(sel4(bool)));
	aggiornaDatiEEprom = 0;
	if (tipo != DI_SON)
		bannNavigazione->mostra(banner::BUT2);
}

void sveglia::drawSelectPage()
{
	for (uchar idx = 0; idx < 4; idx++)
		choice[idx]->setChecked(idx == tipoSveglia);
}

void sveglia::sel1(bool isOn)
{
	if (isOn)
		tipoSveglia = ONCE;
	drawSelectPage();
}

void sveglia::sel2(bool isOn)
{
	if (isOn)
		tipoSveglia = SEMPRE;
	drawSelectPage();
}

void sveglia::sel3(bool isOn)
{
	if (isOn)
		tipoSveglia = FERIALI;
	drawSelectPage();
}

void sveglia::sel4(bool isOn)
{
	if (isOn)
		tipoSveglia = FESTIVI;
	drawSelectPage();
}

void sveglia::Closed()
{
	qDebug("Sveglia Closed");
	//imposta la sveglia in
	if (difson)
	{
		disconnect(difson,SIGNAL(Closed()),this,SLOT(Closed()));
		difson->disconnectClosed(this);
		if (aggiornaDatiEEprom)
		{
			difson->reparent(NULL,0,QPoint(0,0),false);
			difson->setNavBarMode(3);
			difson->ripristinaRighe();
			difson->restorewindows();
			difson->setGeom(0,0,MAX_WIDTH,MAX_HEIGHT);
			difson->forceDraw();

#if defined (BTWEB) || defined (BT_EMBEDDED)
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
	emit ImClosed();
	delete oraSveglia;
	oraSveglia = new QDateTime(dataOra->getDataOra());

	QMap<QString, QString> data;
	data["hour"] = oraSveglia->time().toString("hh");
	data["minute"] = oraSveglia->time().toString("mm");
	data["alarmset"] = QString::number(tipoSveglia);

	setCfgValue(data, SET_SVEGLIA, serNum);
}

void sveglia::okTipo()
{
	disconnect(bannNavigazione ,SIGNAL(forwardClick()),this,SLOT(okTipo()));
	connect(bannNavigazione ,SIGNAL(forwardClick()),this,SLOT(Closed()));
	for (uchar idx = 0; idx < 4; idx++)
	{
		choice[idx]->hide();
		testiChoice[idx]->hide();
	}
	Immagine->show();
	if (tipo != DI_SON)
		Closed();
	else if (difson)
	{
		this->bannNavigazione->hide();
		difson->setNumRighe((uchar)3);
		difson->setGeom(0,80,240,240);
		difson->setNavBarMode(6);
		difson->reparent((QWidget*)this,(int)0,QPoint(0,80),(bool)true);
		difson->resizewindows();
		difson->forceDraw();

		aggiornaDatiEEprom = 1;
		gesFrameAbil = true;
		sorgente = 101;
		stazione = 0;
		for (unsigned int idx = 0; idx < AMPLI_NUM; idx++)
			volSveglia[idx] = -1;
		difson->show();
	}
}

void sveglia::setActive(bool a)
{
	active = a;
	if (active)
	{
		if (!minuTimer)
		{
			minuTimer = new QTimer(this);
			minuTimer->start(200);
			connect(minuTimer,SIGNAL(timeout()), this,SLOT(verificaSveglia()));
			setCfgValue("enabled","1", SET_SVEGLIA, serNum);
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
			setCfgValue("enabled", "0", SET_SVEGLIA, serNum);
		}
	}
}

void sveglia::gestFrame(char* f)
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
					BTouch->sendFrame("*#16*" + QString::number(deviceAddr) + "*1##");
				}
			}
			else
			{
				if (!strcmp(msg_open.Extract_grandezza(),"1"))
				{
					int vol;
					vol=atoi(msg_open.Extract_valori(0))&0x1F;
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

void sveglia::verificaSveglia()
{
	if (!active)
		return;

	QDateTime actualDateTime = QDateTime::currentDateTime();

	if (tipoSveglia == SEMPRE || tipoSveglia == ONCE ||
		(tipoSveglia == FERIALI && actualDateTime.date().dayOfWeek() < 6) ||
		(tipoSveglia == FESTIVI && actualDateTime.date().dayOfWeek() > 5))
	{
		qDebug("secsTo: %d",oraSveglia->time().secsTo(actualDateTime.time()));
		if ((actualDateTime.time() >= (oraSveglia->time())) && ((oraSveglia->time().secsTo(actualDateTime.time())<60)))
		{
			if (tipo == BUZZER)
			{
				aumVolTimer = new QTimer(this);
				aumVolTimer->start(100);
				connect(aumVolTimer,SIGNAL(timeout()),this,SLOT(buzzerAlarm()));
				contaBuzzer = 0;
				conta2min = 0;
				BTouch->freeze(true);
				BTouch->svegl(true);
			}
			else if (tipo == DI_SON)
			{
				aumVolTimer = new QTimer(this);
				aumVolTimer->start(3000);
				connect(aumVolTimer,SIGNAL(timeout()),this,SLOT(aumVol()));
				conta2min = 0;
				BrightnessControl::instance()->setState(ON);
				BTouch->freeze(true);
				BTouch->svegl(true);
			}
			else if (tipo == FRAME)
			{
				qDebug() << "mando la frame:" << frame;
				BTouch->sendFrame(frame);
			}

			qDebug("PARTE LA SVEGLIA");

		if (tipoSveglia == ONCE)
			setActive(false);
		}
	}
	if (active)
		minuTimer->start((60-actualDateTime.time().second())*1000);
}

bool sveglia::isActive()
{
	return active;
}

void sveglia::aumVol()
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
		BTouch->sendFrame("*16*3*" + QString::number(sorgente) + "##");
		BTouch->sendFrame("*#16*" + QString::number(sorgente) + "*#7*" + QString::number(stazione) + "##");

		for (int idx = 0; idx < AMPLI_NUM; idx++)
		{
			if (volSveglia[idx] > -1)
			{
				if (idx >= 10 && idx <= 19)
				{
					if (!amb1)
					{
						amb1 = true;
						BTouch->sendFrame("*16*3*" + QString::number(sorgente + 10) + "##");
					}
				}
				if (idx >= 20 && idx <= 29)
				{
					if (!amb2)
					{
						amb2 = true;
						BTouch->sendFrame("*16*3*" + QString::number(sorgente + 20) + "##");
					}
				}
				if (idx >= 30 && idx <= 39)
				{
					if (!amb3)
					{
						amb3 = true;
						BTouch->sendFrame("*16*3*" + QString::number(sorgente + 30) + "##");
					}
				}
				if (idx >= 40 && idx <= 49)
				{
					if (!amb4)
					{
						amb4 = true;
						BTouch->sendFrame("*16*3*" + QString::number(sorgente + 40) + "##");
					}
				}
				if (idx >= 50 && idx <= 59)
				{
					if (!amb5)
					{
						amb5 = true;
						BTouch->sendFrame("*16*3*" + QString::number(sorgente + 50) + "##");
					}
				}
				if (idx >= 60 && idx <= 69)
				{
					if (!amb6)
					{
						amb6 = true;
						BTouch->sendFrame("*16*3*" + QString::number(sorgente + 60) + "##");
					}
				}
				if (idx >= 70 && idx <= 79)
				{
					if (!amb7)
					{
						amb7 = true;
						BTouch->sendFrame("*16*3*" + QString::number(sorgente + 70) + "##");
					}
				}
				if (idx >= 80 && idx <= 89)
				{
					if (!amb8)
					{
						amb8 = true;
						BTouch->sendFrame("*16*3*" + QString::number(sorgente + 80) + "##");
					}
				}
				if (volSveglia[idx] < 10)
				{
					QString f;
					BTouch->sendFrame(f.sprintf("*#16*%02d*#1*%d##", idx, volSveglia[idx]));
				}
				else
				{
					QString f;
					BTouch->sendFrame(f.sprintf("*#16*%02d*#1*8##", idx));
				}

				QString f;
				BTouch->sendFrame(f.sprintf("*16*3*%02d##", idx));
			}
		}
		conta2min = 9;
	}

	conta2min++;
	if (conta2min < 32)
	{
		for (int idx = 0; idx < AMPLI_NUM; idx++)
		{
			if (volSveglia[idx] >= conta2min)
			{
				QString f;
				BTouch->sendFrame(f.sprintf("*#16*%02d*#1*%d##", idx, conta2min));
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
				BTouch->sendFrame(f.sprintf("*16*13*%02d##", idx));
			}
		}
		BTouch->freeze(false);
		BTouch->svegl(false);
	}
}

void sveglia::buzzerAlarm()
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
		BrightnessControl::instance()->setState(ON);
	else
		BrightnessControl::instance()->setState(OFF);

	contaBuzzer++;
	if (contaBuzzer >= 10*60*2)
	{
		qDebug("SPENGO LA SVEGLIA");
		aumVolTimer->stop();
		setBeep(buzAbilOld,false);
		delete aumVolTimer;
		aumVolTimer = NULL;
		BTouch->freeze(false);
		BTouch->svegl(false);
	}
}

void sveglia::spegniSveglia(bool b)
{
	if (!b && aumVolTimer)
	{
		if (aumVolTimer->isActive())
		{
			qDebug("SPENGO LA SVEGLIA");
			aumVolTimer->stop();
			if (tipo == BUZZER)
				setBeep(buzAbilOld,false);

			delete aumVolTimer;
			aumVolTimer = NULL;
			BTouch->svegl(false);
		}
	}
	else if (b)
	{
		if (isVisible())
			Closed();
	}
}

void sveglia::setSerNum(int s)
{
	serNum = s;
}

void sveglia::inizializza()
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	if (tipo == DI_SON)
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
