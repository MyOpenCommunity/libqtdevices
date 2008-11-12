/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** actuators.cpp
 **
 **definizioni dei vari attuatori implementati
 **
 ****************************************************************/

#include "actuators.h"
#include "device_cache.h" // btouch_device_cache
#include "device.h"
#include "btbutton.h"
#include "generic_functions.h" // void getPressName(...)
#include "fontmanager.h"

#include <openwebnet.h> // class openwebnet

#include <QFile>
#include <QLabel>
#include <QTimer>
#include <QDebug>

#include <stdlib.h>

/*****************************************************************
 **attuatAutom
 ****************************************************************/

attuatAutom::attuatAutom(QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *pressedIcon ,int period,int number)
	: bannOnOff(parent, name)
{
	SetIcons(IconaSx,IconaDx ,icon, pressedIcon,period ,number);
	setAddress(indirizzo);
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_light(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void attuatAutom::status_changed(QList<device_status*> sl)
{
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("attuatAutom::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
			case device_status::LIGHTS:
				qDebug("attuatAutom status variation");
				ds->read(device_status_light::ON_OFF_INDEX, curr_status);
				qDebug("status = %d", curr_status.get_val());
				qDebug("status = %d", curr_status.get_val());
				if (!curr_status.get_val() && isActive())
				{
					aggiorna = true;
					impostaAttivo(0);
				}
				else if (curr_status.get_val() && !isActive())
				{
					aggiorna = true;
					impostaAttivo(1);
				}
				break;
			default:
				qDebug("attuatAutom variation, ignored");
				break;
		}
	}

	if (aggiorna)
		Draw();
}


void attuatAutom::Attiva()
{
	openwebnet msg_open;

	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen("1", "1",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}

void attuatAutom::Disattiva()
{
	openwebnet msg_open;

	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen("1", "0",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}

void attuatAutom::inizializza(bool forza)
{ 
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)pippo,strlen((char*)pippo));
	if (!forza)
		emit richStato(msg_open.frame_open);
	else
		dev->sendInit(msg_open.frame_open);
}

/*****************************************************************
 **gruppo di attuatAutom
 ****************************************************************/

grAttuatAutom::grAttuatAutom(QWidget *parent,const char *name,void *indirizzi, char* IconaDx,char* IconaSx,char *icon ,int period,int number)
	: bannOnOff(parent, name)
{
	SetIcons(IconaDx, IconaSx,NULL,icon,period ,number);
	// TODO: togliere questo cast da void*!!!! (bisogna intervenire su xmlconfhandler)
	elencoDisp = *((QList<QString*>*)indirizzi);
	dev = btouch_device_cache.get_device(getAddress());
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
}

void grAttuatAutom::Attiva()
{
	openwebnet msg_open;

	for (int i = 0; i < elencoDisp.size();++i)
	{
		msg_open.CreateNullMsgOpen();
		QByteArray buf = elencoDisp.at(i)->toAscii();
		msg_open.CreateMsgOpen("1", "1", buf.data(), "");
		dev->sendFrame(msg_open.frame_open);
	}
}

void grAttuatAutom::Disattiva()
{
	openwebnet msg_open;

	for (int i = 0; i < elencoDisp.size();++i)
	{
		msg_open.CreateNullMsgOpen();
		QByteArray buf = elencoDisp.at(i)->toAscii();
		msg_open.CreateMsgOpen("1", "0", buf.data(), "");
		dev->sendFrame(msg_open.frame_open);
	}
}


/*****************************************************************
 **attuatAutomInt
 ****************************************************************/

attuatAutomInt::attuatAutomInt(QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *StopIcon ,int period,int number)
	: bannOnOff(parent, name)
{
	memset(nomeFile1,'\000',sizeof(nomeFile1));
	memset(nomeFile2,'\000',sizeof(nomeFile2));
	strncpy(nomeFile1,icon,strstr(icon,".")-icon);
	strncpy(nomeFile2,icon,strstr(icon,".")-icon);
	strcat(nomeFile1,"o");
	strcat(nomeFile2,"c");
	strcat(nomeFile1,strstr(icon,"."));
	strcat(nomeFile2,strstr(icon,"."));
	SetIcons(IconaSx, IconaDx , icon,nomeFile1,nomeFile2);
	strncpy(nomeFile3,StopIcon,sizeof(nomeFile3));
	strncpy(nomeFile1,IconaSx,sizeof(nomeFile3));
	strncpy(nomeFile2,IconaDx,sizeof(nomeFile3));

	setAddress(indirizzo);
	connect(this,SIGNAL(sxClick()),this,SLOT(analizzaUp()));
	connect(this,SIGNAL(dxClick()),this,SLOT(analizzaDown()));

	uprunning = dorunning = 0;
	dev = btouch_device_cache.get_autom_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void attuatAutomInt::status_changed(QList<device_status*> sl)
{
	stat_var curr_status(stat_var::STAT);
	bool aggiorna = false;
	qDebug("attuatAutomInt::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::AUTOM:
			qDebug("Autom status variation");
			ds->read(device_status_autom::STAT_INDEX, curr_status);
			int v = curr_status.get_val();
			qDebug("status = %d", v);
			qDebug("isActive = %d", isActive());
			switch (v)
			{
			case 0:
				if (isActive())
				{
					impostaAttivo(0);
					uprunning = dorunning = 0;
					aggiorna = 1;
					SetIcons((uchar)0,nomeFile1);
					SetIcons((uchar)1,nomeFile2);
					sxButton->setEnabled(1);
					dxButton->setEnabled(1);
				}
				break;
			case 1:
				if (!isActive() || (isActive() == 2))
				{
					impostaAttivo(1);
					dorunning = 0;
					uprunning = 1;
					aggiorna = 1;
					SetIcons((uchar)0,nomeFile3);
					SetIcons((uchar)1,nomeFile2);
					dxButton->setDisabled(1);
					sxButton->setEnabled(1);
				}
				break;
			case 2:
				if (!isActive() || (isActive() == 1))
				{
					impostaAttivo(2);
					dorunning = 1;
					uprunning = 0;
					aggiorna = 1;
					SetIcons((uchar)0,nomeFile1);
					SetIcons((uchar)1,nomeFile3);
					sxButton->setDisabled(1);
					dxButton->setEnabled(1);
				}
				break;
			default:
				qDebug("Unknown status in autom. message");
				break;
			}
			break;
		default:
			qDebug("Unknown device status type");
			break;
		}
	}

	if (aggiorna)
		Draw();
}

void attuatAutomInt::analizzaUp()
{
	if (!dorunning)
	{
		openwebnet msg_open;
		msg_open.CreateNullMsgOpen();
		if (uprunning)
			msg_open.CreateMsgOpen("2", "0", getAddress(),"");
		else
			msg_open.CreateMsgOpen("2", "1", getAddress(),"");
		dev->sendFrame(msg_open.frame_open);
	}
}

void attuatAutomInt::analizzaDown()
{
	if (!uprunning)
	{
		openwebnet msg_open;
		msg_open.CreateNullMsgOpen();
		if (uprunning)
			msg_open.CreateMsgOpen("2", "0",getAddress(),"");
		else
			msg_open.CreateMsgOpen("2", "2",getAddress(),"");
		dev->sendFrame(msg_open.frame_open);
	}
}

void attuatAutomInt::inizializza(bool forza)
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)pippo,strlen((char*)pippo));
	dev->sendInit(msg_open.frame_open);
}

/*****************************************************************
 **attuatAutomIntSic
 ****************************************************************/

attuatAutomIntSic::attuatAutomIntSic(QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *StopIcon ,int period,int number)
: bannOnOff(parent, name)
{
	memset(nomeFile1,'\000',sizeof(nomeFile1));
	memset(nomeFile2,'\000',sizeof(nomeFile2));
	strncpy(nomeFile1,icon,strstr(icon,".")-icon);
	strncpy(nomeFile2,icon,strstr(icon,".")-icon);
	strcat(nomeFile1,"o");
	strcat(nomeFile2,"c");
	strcat(nomeFile1,strstr(icon,"."));
	strcat(nomeFile2,strstr(icon,"."));
	SetIcons(IconaSx, IconaDx , icon,nomeFile1,nomeFile2);
	strncpy(nomeFile3,StopIcon,sizeof(nomeFile3));
	strncpy(nomeFile1,IconaSx,sizeof(nomeFile3));
	strncpy(nomeFile2,IconaDx,sizeof(nomeFile3));

	setAddress(indirizzo);
	connect(this,SIGNAL(sxPressed()),this,SLOT(upPres()));
	connect(this,SIGNAL(dxPressed()),this,SLOT(doPres()));
	connect(this,SIGNAL(sxReleased()),this,SLOT(upRil()));
	connect(this,SIGNAL(dxReleased()),this,SLOT(doRil()));

	uprunning = dorunning = 0;
	dev = btouch_device_cache.get_autom_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void attuatAutomIntSic::status_changed(QList<device_status*> sl)
{
	stat_var curr_status(stat_var::STAT);
	bool aggiorna = false;
	qDebug("attuatAutomInt::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::AUTOM:
			qDebug("Autom status variation");
			ds->read(device_status_autom::STAT_INDEX, curr_status);
			int v = curr_status.get_val();
			qDebug("status = %d", v);
			switch(v)
			{
			case 0:
				if (isActive())
				{
					impostaAttivo(0);
					uprunning=dorunning=0;
					aggiorna=1;
					SetIcons((uchar)0,nomeFile1);
					SetIcons((uchar)1,nomeFile2);
					sxButton->setEnabled(1);
					dxButton->setEnabled(1);
				}
				break;
			case 1:
				if (!isActive() || (isActive() == 2))
				{
					impostaAttivo(1);
					dorunning = 0;
					uprunning = 1;
					aggiorna = 1;
					SetIcons((uchar)0,nomeFile3);
					SetIcons((uchar)1,nomeFile2);
					dxButton->setDisabled(1);
					sxButton->setEnabled(1);
				}
				break;
			case 2:
				if (!isActive() || (isActive() == 1))
				{
					impostaAttivo(2);
					dorunning = 1;
					uprunning = 0;
					aggiorna = 1;
					SetIcons((uchar)0,nomeFile1);
					SetIcons((uchar)1,nomeFile3);
					sxButton->setDisabled(1);
					dxButton->setEnabled(1);
				}
				break;
			default:
				qDebug("Unknown status in autom. message");
				break;
			}
		default:
			qDebug("Unknown device status type");
			break;
		}
	}

	if (aggiorna)
		Draw();
}

void attuatAutomIntSic::upPres()
{
	if (!dorunning && !isActive())
	{
		openwebnet msg_open;
		msg_open.CreateNullMsgOpen();
		msg_open.CreateMsgOpen("2", "1",getAddress(),"");
		dev->sendFrame(msg_open.frame_open);
	} 
}

void attuatAutomIntSic::doPres()
{
	if (!uprunning && !isActive())
	{
		openwebnet msg_open;

		msg_open.CreateNullMsgOpen();
		msg_open.CreateMsgOpen("2", "2",getAddress(),"");
		dev->sendFrame(msg_open.frame_open);
	} 
}

void attuatAutomIntSic::upRil()
{
	if (!dorunning)
		QTimer::singleShot(500, this, SLOT(sendStop()));
}

void attuatAutomIntSic::doRil()
{
	if (!uprunning)
		QTimer::singleShot(500, this, SLOT(sendStop()));
}

void attuatAutomIntSic::sendStop()
{
	openwebnet msg_open;

	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen("2", "0",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}

void attuatAutomIntSic::inizializza(bool forza)
{ 
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)pippo,strlen((char*)pippo));
	dev->sendInit(msg_open.frame_open);
}


/*****************************************************************
 **attuatAutomTemp
 ****************************************************************/

attuatAutomTemp::attuatAutomTemp(QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *pressedIcon ,int period,int number, QList<QString*> *lt)
: bannOnOff2scr(parent, name)
{
	SetIcons(IconaDx, IconaSx ,icon, pressedIcon,period ,number);
	setAddress(indirizzo);
	cntTempi = 0;
	static const char *t[] =  { "1'", "2'", "3'", "4'", "5'", "15'", "30''" };
	int nt = lt->size() ? lt->size() : sizeof(t) / sizeof(char *);
	for (int i = 0; i < nt; i++)
	{
		QString *s;
		if (i < lt->size())
			s = lt->at(i);
		else
			s = new QString(t[i]);
		tempi.append(s);
	}

	assegna_tempo_display();
	SetSecondaryTextU(tempo_display);
	dev = btouch_device_cache.get_light(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
	connect(this, SIGNAL(dxClick()), this, SLOT(Attiva()));
	connect(this, SIGNAL(sxClick()), this, SLOT(CiclaTempo()));
}

QString attuatAutomTemp::leggi_tempo()
{
	if (cntTempi >= ntempi())
		return "";

	return *tempi.at(cntTempi);
}

uchar attuatAutomTemp::ntempi()
{
	return tempi.size();
}

void attuatAutomTemp::status_changed(QList<device_status*> sl)
{
	stat_var curr_hh(stat_var::HH);
	stat_var curr_mm(stat_var::MM);
	stat_var curr_ss(stat_var::SS);
	bool aggiorna = false;
	qDebug("attuatAutomTemp::status_changed()");
	stat_var curr_status(stat_var::ON_OFF);

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::LIGHTS:
			qDebug("Light status variation");
			ds->read(device_status_light::ON_OFF_INDEX, curr_status);
			qDebug("status = %d", curr_status.get_val());
			aggiorna = true;
			if ((isActive() && !curr_status.get_val()) || (!isActive() && curr_status.get_val())) // Update
				impostaAttivo(curr_status.get_val() != 0);
			break;
		case device_status::DIMMER:
			qDebug("dimmer status variation, ignored");
			break;
		case device_status::DIMMER100:
			qDebug("dimmer 100 status variation, ignored");
			break;
		case device_status::NEWTIMED:
			qDebug("new timed device status variation, ignored");
			ds->read(device_status_new_timed::HH_INDEX, curr_hh);
			ds->read(device_status_new_timed::MM_INDEX, curr_mm);
			ds->read(device_status_new_timed::SS_INDEX, curr_ss);
			if (!curr_hh.get_val() && !curr_mm.get_val() && !curr_ss.get_val())
			{
				qDebug("hh == mm == ss == 0, ignoring");
				if (isActive())
				{
					impostaAttivo(false);
					aggiorna = true;
				}
				break;
			}

			if (curr_hh.get_val()==255 && curr_mm.get_val()==255 && curr_ss.get_val()==255)
			{
				qDebug("hh == mm == ss == 255, ignoring");
				break;
			}
			uchar hnow, mnow, snow;
			hnow = curr_hh.get_val();
			mnow = curr_mm.get_val();
			snow = curr_ss.get_val();
			int tmpval = (hnow * 3600) + (mnow * 60) + snow;
			if ((isActive() && tmpval) || (!isActive() && !tmpval))
			{
				qDebug("already active, ignoring");
				break;
			}
			impostaAttivo((tmpval != 0));
			qDebug("tempo = %d %d %d", hnow, mnow, snow);
			aggiorna = true;
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}

	if (aggiorna)
		Draw();
}

void attuatAutomTemp::Attiva()
{
	openwebnet msg_open;
	char cosa[15];

	memset(cosa,'\000',sizeof(cosa));
	sprintf(cosa,"%d",(11+cntTempi));

	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen("1",cosa,getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}

void attuatAutomTemp::CiclaTempo()
{
	cntTempi = (cntTempi+1) % ntempi();
	qDebug("ntempi = %d", ntempi());
	qDebug("cntTempi = %d", cntTempi);
	assegna_tempo_display();
	SetSecondaryTextU(tempo_display);
	Draw();
}

void attuatAutomTemp::inizializza(bool forza)
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)pippo,strlen((char*)pippo));
	dev->sendInit(msg_open.frame_open);
}

void attuatAutomTemp::assegna_tempo_display()
{
	tempo_display = leggi_tempo();
}

attuatAutomTemp::~attuatAutomTemp()
{
	while (!tempi.isEmpty())
		delete tempi.takeFirst();
}

/*****************************************************************
 **attuatAutomTempNuovoN
 ****************************************************************/

attuatAutomTempNuovoN::attuatAutomTempNuovoN(QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *pressedIcon ,int period,int number , QList<QString*> *lt)
	: attuatAutomTemp(parent, name, indirizzo, IconaSx, IconaDx, icon, pressedIcon, period, number, lt)
{
	assegna_tempo_display();
	stato_noto = false;
	SetSecondaryTextU(tempo_display);
	disconnect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(attuatAutomTemp::status_changed(QList<device_status*>)));
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void attuatAutomTempNuovoN::status_changed(QList<device_status*> sl)
{
	bool aggiorna = false;
	stat_var curr_hh(stat_var::HH);
	stat_var curr_mm(stat_var::MM);
	stat_var curr_ss(stat_var::SS);
	qDebug("attuatAutomTempNuovoN::status_changed()");
	stat_var curr_status(stat_var::ON_OFF);

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::LIGHTS:
			qDebug("Light status variation");
			ds->read(device_status_light::ON_OFF_INDEX, curr_status);
			qDebug("status = %d", curr_status.get_val());
			aggiorna = true;
			if ((isActive() && !curr_status.get_val()) || (!isActive() && curr_status.get_val()))
			{
				// Update
				impostaAttivo(curr_status.get_val() != 0);
			}
			break;
		case device_status::DIMMER:
			qDebug("dimmer status variation, ignored");
			break;
		case device_status::DIMMER100:
			qDebug("dimmer 100 status variation, ignored");
			break;
		case device_status::NEWTIMED:
			qDebug("new timed device status variation");
			ds->read(device_status_new_timed::HH_INDEX, curr_hh);
			ds->read(device_status_new_timed::MM_INDEX, curr_mm);
			ds->read(device_status_new_timed::SS_INDEX, curr_ss);
			if (!curr_hh.get_val() && !curr_mm.get_val() && !curr_ss.get_val())
			{
				qDebug("hh == mm == ss == 0, ignoring");
				if (isActive())
				{
					impostaAttivo(false);
					aggiorna = true;
				}
				break;
			}
			if (curr_hh.get_val()==255 && curr_mm.get_val()==255 && curr_ss.get_val()==255)
			{
				qDebug("hh == mm == ss == 255, ignoring");
				break;
			}
			uchar hnow, mnow, snow;
			hnow = curr_hh.get_val();
			mnow = curr_mm.get_val();
			snow = curr_ss.get_val();
			int tmpval = (hnow * 3600) + (mnow * 60) + snow;
			if ((isActive() && tmpval) || (!isActive() && !tmpval))
			{
				qDebug("already active, ignoring");
				break;
			}
			impostaAttivo((tmpval != 0));
			qDebug("tempo = %d %d %d", hnow, mnow, snow);
			aggiorna = true;
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}

	qDebug("aggiorna = %d", aggiorna);
	if (aggiorna)
		Draw();
}

void attuatAutomTempNuovoN::Attiva()
{
	openwebnet msg_open;
	char frame[100];
	QByteArray buf = leggi_tempo().toAscii();
	sprintf(frame, "*#1*%s*#2*%s##", getAddress(), buf.constData());
	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen(frame, strlen(frame));
	dev->sendFrame(msg_open.frame_open);
}

// *#1*dove*2## 
void attuatAutomTempNuovoN::inizializza(bool forza)
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"*2##");
	msg_open.CreateMsgOpen((char*)pippo,strlen((char*)pippo));
	dev->sendInit(msg_open.frame_open);
}

void attuatAutomTempNuovoN::assegna_tempo_display()
{
	char *ptr;
	char tmp[50];
	int hh , mm, ss;
	// TODO: riscrivere utilizzando c++ e qt!!
	QByteArray buf = leggi_tempo().toAscii();
	strcpy(tmp, buf.constData());

	// Prende solo hh e min
	ptr = strtok(tmp, "*");
	hh = strtol(ptr, NULL, 10);
	ptr = strtok(NULL, "*");
	mm = strtol(ptr, NULL, 10);
	ptr = strtok(NULL, "*");
	ss = strtol(ptr, NULL, 10);
	qDebug("tempo = %d %d %d", hh, mm, ss);

	if (!hh && !mm) // Time in secs
		tempo_display.sprintf("%d\"", ss);
	else if (!hh) // Time in mins'
		tempo_display.sprintf("%d'", mm);
	else if (hh < 10) // Time in hh:mm
		tempo_display.sprintf("%d:%02d", hh, mm);
	else// Time in hh h
		tempo_display.sprintf("%dh", hh);
}


/*****************************************************************
 **attuatAutomTempNuovoF
 ****************************************************************/

#define NTIMEICONS 9

attuatAutomTempNuovoF::attuatAutomTempNuovoF(QWidget *parent,const char *name,char* indirizzo,char* IconaCentroSx,char* IconaCentroDx,char *IconaDx, QString t)
	: bannOn2scr(parent, name)
{
	attuatAutomTempNuovoF::SetIcons(IconaCentroSx, IconaCentroDx, IconaDx);
	setAddress(indirizzo);
	SetSecondaryTextU("????");

	// TODO: riscrivere utilizzando qt e c++!!
	strncpy(tempo, t.isEmpty() ? t.toAscii().constData() : "0*0*0", sizeof(tempo));
	char *ptr;
	char tmp1[50];
	strcpy(tmp1, tempo);
	ptr = strtok(tmp1, "*");
	h = strtol(ptr, NULL, 10);
	ptr = strtok(NULL, "*");
	m = strtol(ptr, NULL, 10);
	ptr = strtok(NULL, "*");
	s = strtol(ptr, NULL, 10);
	val = 0;
	qDebug("tempo = %d %d %d", h, m, s);
	char tmp[50];
	if (!h && !m)
	{
		// Time in secs
		sprintf(tmp, "%d\"", s);
	}
	else if (!h)
	{
		// Time in mins'
		sprintf(tmp, "%d'", m);
	}
	else if (h < 10)
	{
		// Time in hh:mm
		sprintf(tmp, "%d:%02d", h, m);
	}
	else
	{
		// Time in hh h
		sprintf(tmp, "%dh", h);
	}

	myTimer = new QTimer(this);
	stato_noto = false;
	temp_nota = false;
	connect(myTimer,SIGNAL(timeout()),this,SLOT(update()));
	update_ok = false;
	tentativi_update = 0;
	SetSecondaryTextU(tmp);
	connect(this,SIGNAL(dxClick()),this,SLOT(Attiva())); 
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_newtimed(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
	dev = btouch_device_cache.get_dimmer(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void attuatAutomTempNuovoF::status_changed(QList<device_status*> sl)
{
	stat_var curr_hh(stat_var::HH);
	stat_var curr_mm(stat_var::MM);
	stat_var curr_ss(stat_var::SS);
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("attuatAutomTempNuovoF::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::LIGHTS:
			qDebug("Light status variation");
			aggiorna = true;
			ds->read(device_status_light::ON_OFF_INDEX, curr_status);
			qDebug("status = %d", curr_status.get_val());
			if ((isActive() && !curr_status.get_val()) || (!isActive() && curr_status.get_val()))
			{
				impostaAttivo(curr_status.get_val() != 0);
				if (!isActive())
					myTimer->stop();
				val = 0;
			}
			break;
		case device_status::DIMMER:
			qDebug("dimmer status variation, ignored");
			break;
		case device_status::DIMMER100:
			qDebug("dimmer 100 status variation, ignored");
			break;
		case device_status::NEWTIMED:
			qDebug("new timed device status variation");
			ds->read(device_status_new_timed::HH_INDEX, curr_hh);
			ds->read(device_status_new_timed::MM_INDEX, curr_mm);
			ds->read(device_status_new_timed::SS_INDEX, curr_ss);
			if (!curr_hh.get_val() && !curr_mm.get_val() && !curr_ss.get_val())
			{
				qDebug("hh == mm == ss == 0, ignoring");
				myTimer->stop();
				val = 0;
				break;
			}
			if (curr_hh.get_val()==255 && curr_mm.get_val()==255 && curr_ss.get_val()==255)
			{
				qDebug("hh == mm == ss == 255, ignoring");
				break;
			}
			uchar hnow, mnow, snow;
			hnow = curr_hh.get_val();
			mnow = curr_mm.get_val();
			snow = curr_ss.get_val();
			int tmpval = (hnow * 3600) + (mnow * 60) + snow;
			if (tmpval == val)
				return;
			val = tmpval;
			impostaAttivo((val != 0));
			update_ok = true;
			if (!val)
				myTimer->stop();
			else if (!myTimer->isActive())
				myTimer->start((1000 * val) / NTIMEICONS);
			qDebug("tempo = %d %d %d", hnow, mnow, snow);
			aggiorna = true;
			break;
		default:
			qDebug("WARNING: tipo non previsto");
		}
	}

	if (aggiorna)
	{
		qDebug("invoco Draw con value = %d", value);
		Draw();
	}
}

void attuatAutomTempNuovoF::update()
{
	if (!update_ok)
	{
		if (tentativi_update++ > 2)
		{
			qDebug("Esauriti i tentativi di aggiornamento\n");
			myTimer->stop();
			return;
		}
		update_ok = false;
	}
	openwebnet msg_open;
	char frame[100];
	sprintf(frame, "*#1*%s*2##", getAddress());
	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen(frame, strlen(frame));
	dev->sendInit(msg_open.frame_open);
	// imposto il timer corretto
	if (!temp_nota)
	{
		int v = h * 3600 + m * 60 + s;
		myTimer->start((1000 * v) / NTIMEICONS);
		temp_nota = true;
	}
}

void attuatAutomTempNuovoF::Attiva()
{
	openwebnet msg_open;
	char frame[100];
	sprintf(frame, "*#1*%s*#2*%s##", getAddress(), tempo);
	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen(frame, strlen(frame));
	dev->sendFrame(msg_open.frame_open);
	tentativi_update = 0;
	update_ok = false;
	//programma un aggiornamento immediato
	temp_nota = false;
	myTimer->start(100);
	Draw();
}

// *#1*dove*2## 
void attuatAutomTempNuovoF::inizializza(bool forza)
{ 
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"*2##");
	msg_open.CreateMsgOpen((char*)pippo,strlen((char*)pippo));
	dev->sendInit(msg_open.frame_open);
}

// Chiede lo stato dell'attuatore con una frame vecchia
void attuatAutomTempNuovoF::chiedi_stato()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)pippo,strlen((char*)pippo));
	emit richStato(msg_open.frame_open);
}

void attuatAutomTempNuovoF::SetIcons(char *i1, char *i2, char *i3)
{
	qDebug("attuatAutomTempNuovoF::SetIcons");
	char tmp[MAX_PATH], tmp1[MAX_PATH];
	char *ptr;
	if (!Icon[0])
		Icon[0] = new QPixmap();
	strcpy(tmp1, i2);
	ptr = strtok(tmp1, ".");
	sprintf(tmp, "%soff.png", ptr);
	Icon[0]->load(tmp);
	qDebug("Icon[0] <- %s", tmp);

	if (!Icon[1])
		Icon[1] = new QPixmap();
	strcpy(tmp1, i2);
	ptr = strtok(tmp1, ".");
	sprintf(tmp, "%son.png", ptr);
	Icon[1]->load(tmp);
	qDebug("Icon[1] <- %s", tmp);

	if (!Icon[2])
		Icon[2] = new QPixmap();
	Icon[2]->load(i3);
	qDebug("Icon[2] <- %s", i3);
	QString pressIconName = getPressName(i3);
	if (QFile::exists(pressIconName))
	{
		if (!pressIcon[2])
			pressIcon[2] = new QPixmap();
		pressIcon[2]->load(pressIconName);
		qDebug() << "pressIcon[2] <- " << pressIconName;
	}
	for (int i = 0; i < NTIMEICONS; i++)
	{
		if (!Icon[3 + i])
			Icon[3 + i] = new QPixmap();
		strcpy(tmp1, i1);
		ptr = strtok(tmp1, ".");
		sprintf(tmp, "%s%d.png", ptr, i);
		Icon[3 + i]->load(tmp);	
		qDebug("Icon[%d] <- %s", 3+i, tmp);
	}
}

void attuatAutomTempNuovoF::Draw()
{
	qDebug("attuatAutomTempNuovoF::Draw(), attivo = %d, value = %d", attivo, val);
	if (attivo == 1)
	{
		int index = ((10 * val * (NTIMEICONS-1))/((h * 3600) + (m * 60) + s));
		index = (index % 10) >= 5 ? index/10 + 1 : index/10;
		if (index >= NTIMEICONS)
			index = NTIMEICONS - 1;
		qDebug("index = %d", index);
		if (Icon[3 + index] && BannerIcon2)
		{
			BannerIcon->setPixmap(*Icon[3 + index]);
			qDebug("* Icon[%d]", 3 + index);
		}
		if (Icon[1] && BannerIcon)
		{
			BannerIcon2->setPixmap(*Icon[1]);
			qDebug("** Icon[%d]", 1);
		}
	}
	else
	{
		if (Icon[0] && BannerIcon)
		{
			BannerIcon->setPixmap(*Icon[3]);
			qDebug("*** Icon[3]");
		}
		if (Icon[3] && BannerIcon2)
		{
			BannerIcon2->setPixmap(*Icon[0]);
			qDebug("**** Icon[0]");
		}
	}
	if ((dxButton) && (Icon[2]))
	{
		dxButton->setPixmap(*Icon[2]);
		if (pressIcon[2])
			dxButton->setPressedPixmap(*pressIcon[2]);
	}
	if (BannerText)
	{
		QFont aFont;
		FontManager::instance()->getFont(font_items_bannertext, aFont);
		BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		BannerText->setFont(aFont);
		BannerText->setText(qtesto);
	}
	if (SecondaryText)
	{
		QFont aFont;
		FontManager::instance()->getFont(font_items_secondarytext, aFont);
		SecondaryText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		SecondaryText->setFont(aFont);
		SecondaryText->setText(qtestoSecondario);
	}
}


/*****************************************************************
 **gruppo di attuatInt
 ****************************************************************/

grAttuatInt::grAttuatInt(QWidget *parent,const char *name,void *indirizzi,char* IconaSx,char* IconaDx, char *icon ,int period,int number)
	: bann3But(parent, name)
{
	SetIcons(IconaDx,IconaSx ,NULL,icon,period ,number);
	// TODO: togliere questo cast da void*!!!! (bisogna intervenire su xmlconfhandler)
	elencoDisp = *((QList<QString*>*)indirizzi);
	dev = btouch_device_cache.get_device(getAddress());
	connect(this,SIGNAL(dxClick()),this,SLOT(Alza()));
	connect(this,SIGNAL(sxClick()),this,SLOT(Abbassa()));
	connect(this,SIGNAL(centerClick()),this,SLOT(Ferma()));
}

void grAttuatInt::sendFrame(char *msg)
{
	openwebnet msg_open;

	for (int i = 0; i < elencoDisp.size();++i)
	{
		msg_open.CreateNullMsgOpen();
		QByteArray buf = elencoDisp.at(i)->toAscii();
		msg_open.CreateMsgOpen("2", msg, buf.data(), "");
		dev->sendFrame(msg_open.frame_open);
	}
}

void grAttuatInt::Alza()
{
	sendFrame("1");
}

void grAttuatInt::Abbassa()
{
	sendFrame("2");
}

void grAttuatInt::Ferma()
{
	sendFrame("0");
}

void grAttuatInt::inizializza(bool forza)
{
}

/*****************************************************************
 **attuatPuls
 ****************************************************************/

attuatPuls::attuatPuls(QWidget *parent,const char *name,char* indirizzo,char* IconaSx,/*char* IconaDx,*/char *icon ,char tipo ,int period,int number)
: bannPuls(parent, name)
{
	SetIcons(IconaSx,NULL,icon,NULL,period ,number);
	setAddress(indirizzo);
	dev = btouch_device_cache.get_device(getAddress());
	connect(this,SIGNAL(sxPressed()),this,SLOT(Attiva()));
	connect(this,SIGNAL(sxReleased()),this,SLOT(Disattiva()));
	type=tipo;
	impostaAttivo(1);
}

void attuatPuls::Attiva()
{
	openwebnet msg_open;
	switch (type)
	{
	case  AUTOMAZ:
		msg_open.CreateNullMsgOpen();
		msg_open.CreateMsgOpen("1", "1",getAddress(),"");
		dev->sendFrame(msg_open.frame_open);
		break;
	case  VCT_SERR:
		msg_open.CreateNullMsgOpen();
		msg_open.CreateMsgOpen("6", "10",getAddress(),"");
		dev->sendFrame(msg_open.frame_open);
		break;
	case  VCT_LS:
		msg_open.CreateNullMsgOpen();
		msg_open.CreateMsgOpen("6", "12",getAddress(),"");
		dev->sendFrame(msg_open.frame_open);
		break;
	}
}

void attuatPuls::Disattiva()
{
	openwebnet msg_open;
	switch (type)
	{
	case  AUTOMAZ:
		msg_open.CreateNullMsgOpen();
		msg_open.CreateMsgOpen("1", "0",getAddress(),"");
		dev->sendFrame(msg_open.frame_open);
		break;
	case  VCT_SERR:
		break;
	case  VCT_LS:
		msg_open.CreateNullMsgOpen();
		msg_open.CreateMsgOpen("6", "11",getAddress(),"");
		dev->sendFrame(msg_open.frame_open);
		break;
	}
}

void attuatPuls::inizializza(bool forza)
{
}
