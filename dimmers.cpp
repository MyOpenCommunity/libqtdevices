/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** dimmers.cpp
 **
 **
 **definizioni dei vari dimmers implementati
 ****************************************************************/

#include "dimmers.h"
#include "device_cache.h" // btouch_device_cache
#include "device.h"
#include "btbutton.h"
#include "fontmanager.h"

#include <openwebnet.h> // class openwebnet

#include <QLabel>

#include <stdlib.h> // atoi
#include <stdio.h> //sprintf

/*****************************************************************
 **dimmer
 ****************************************************************/

dimmer::dimmer(QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *inactiveIcon,char* breakIcon, bool to_be_connect)
	: bannRegolaz(parent, name)
{

	setRange(20, 100);
	setStep(10);
	SetIcons(IconaSx,IconaDx,icon, inactiveIcon,breakIcon,(char)0);
	setAddress(indirizzo);
	connect(this,SIGNAL(sxClick()),this,SLOT(Accendi()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Spegni()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
	connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
	if (to_be_connect)
	{
		// Crea o preleva il dispositivo dalla cache
		dev = btouch_device_cache.get_dimmer(getAddress());
		// Get status changed events back
		connect(dev, SIGNAL(status_changed(QList<device_status*>)),
				this, SLOT(status_changed(QList<device_status*>)));
	}
}

void dimmer::Draw()
{
	if (getValue() > 100)
		setValue(100);
	qDebug("dimmer::Draw(), attivo = %d, value = %d", attivo, getValue());
	if ((sxButton) && (Icon[0]))
	{
		sxButton->setPixmap(*Icon[0]);
		if (pressIcon[0])
			sxButton->setPressedPixmap(*pressIcon[0]);
	}

	if ((dxButton) && (Icon[1]))
	{
		dxButton->setPixmap(*Icon[1]);
		if (pressIcon[1])
			dxButton->setPressedPixmap(*pressIcon[1]);
	}
	if (attivo == 1)
	{
		if ((Icon[4+((getValue()-minValue)/step)*2]) && (csxButton))
		{
			csxButton->setPixmap(*Icon[4+((getValue()-minValue)/step)*2]);
			qDebug("* Icon[%d]", 4+((getValue()-minValue)/step)*2);
		}
		if ((cdxButton) && (Icon[5+((getValue()-minValue)/step)*2]))
		{
			cdxButton->setPixmap(*Icon[5+((getValue()-minValue)/step)*2]);
			qDebug("** Icon[%d]", 5+((getValue()-minValue)/step)*2);
		}
	}
	else if (attivo == 0)
	{
		if ((Icon[2]) && (csxButton))
		{
			csxButton->setPixmap(*Icon[2]);
			qDebug("*** Icon[%d]", 2);
		}
		if ((cdxButton) && (Icon[3]))
		{
			cdxButton->setPixmap(*Icon[3]);
			qDebug("**** Icon[%d]", 3);
		}
	}
	else if (attivo == 2)
	{
		if ((Icon[44]) && (csxButton))
		{
			csxButton->setPixmap(*Icon[44]);
			qDebug("******* Icon[%d]", 44);
		}

		if ((cdxButton) && (Icon[45]))
		{
			cdxButton->setPixmap(*Icon[45]);
			qDebug("******* Icon[%d]", 45);
		}
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

void dimmer::status_changed(QList<device_status*> sl)
{
	stat_var curr_lev(stat_var::LEV);
	stat_var curr_speed(stat_var::SPEED);
	stat_var curr_status(stat_var::ON_OFF);
	stat_var curr_fault(stat_var::FAULT);
	bool aggiorna = false;
	qDebug("dimmer10::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::LIGHTS:
			qDebug("Light status variation");
			ds->read(device_status_light::ON_OFF_INDEX, curr_status);
			qDebug("status = %d", curr_status.get_val());
			impostaAttivo(curr_status.get_val() != 0);
			if (!curr_status.get_val())
			{
				// Update
				aggiorna = true;
				impostaAttivo(0);
			}
			else
				impostaAttivo(1);
			break;
		case device_status::DIMMER:
			ds->read(device_status_dimmer::LEV_INDEX, curr_lev);
			ds->read(device_status_dimmer::FAULT_INDEX, curr_fault);
			if (curr_fault.get_val())
			{
				qDebug("DIMMER FAULT !!");
				impostaAttivo(2);
			}
			else
			{
				qDebug("dimmer status variation");
				qDebug("level = %d", curr_lev.get_val());
				setValue(curr_lev.get_val());
				if ((curr_lev.get_val() == 0))
					impostaAttivo(0);
			}
			aggiorna = true;
			break;
		case device_status::DIMMER100:
			ds->read(device_status_dimmer100::LEV_INDEX, curr_lev);
			ds->read(device_status_dimmer100::SPEED_INDEX, curr_speed);
			ds->read(device_status_dimmer::FAULT_INDEX, curr_fault);
			qDebug("dimmer 100 status variation, ignored");
			break;
		case device_status::NEWTIMED:
			qDebug("new timed device status variation, ignored");
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}

	if (aggiorna)
		Draw();
}

void dimmer::Accendi()
{
	openwebnet msg_open;

	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen("1", "1",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}

void dimmer::Spegni()
{
	openwebnet msg_open;

	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen("1", "0",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}

void dimmer::Aumenta()
{
	openwebnet msg_open;

	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen("1", "30",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}

void dimmer::Diminuisci()
{
	openwebnet msg_open;
	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen("1", "31",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}

void dimmer::inizializza(bool forza)
{
	openwebnet msg_open;
	char    pippo[50];
	qDebug("dimmer::inizializza");

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
 **dimmer 100 livelli
 ****************************************************************/

dimmer100::dimmer100(QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,
	char *inactiveIcon,char* breakIcon,int sstart, int sstop)
	: dimmer(parent, name, indirizzo, IconaSx, IconaDx, icon,inactiveIcon, breakIcon, false)
{
	qDebug("costruttore dimmer100, name = %s", name);
	softstart = sstart;
	qDebug("softstart = %d", softstart);
	softstop = sstop;
	qDebug("softstop = %d", softstop);
	setRange(5,100);
	setStep(5);
	setValue(0);
	qDebug("IconaSx = %s", IconaSx);
	qDebug("IconaDx = %s", IconaDx);
	qDebug("icon = %s", icon);
	qDebug("inactiveIcon = %s", inactiveIcon);
	qDebug("breakIcon = %s", breakIcon);
	SetIcons(IconaSx,IconaDx,icon, inactiveIcon,breakIcon,(char)0);
	dev = btouch_device_cache.get_dimmer100(getAddress());
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
		this, SLOT(status_changed(QList<device_status*>)));
}


bool dimmer100::decCLV(openwebnet& msg, char& code, char& lev, char& speed,
		char& h, char &m, char &s)
{
	// Message is a new one if it has the form:
	// *#1*where*1*lev*speed##
	// which is a measure frame
	bool out = msg.IsMeasureFrame();
	if (!out)
		return out;
	code = atoi(msg.Extract_grandezza());
	qDebug("dimmer100::decCLV, code = %d", code);
	if (code == 2)
	{
		h = atoi(msg.Extract_valori(0));
		m = atoi(msg.Extract_valori(1));
		s = atoi(msg.Extract_valori(2));
	}
	else if (code == 1)
	{
		lev = atoi(msg.Extract_valori(0)) - 100;
		speed = atoi(msg.Extract_valori(1));
	}
	return true;
}

void dimmer100::Accendi()
{
	qDebug("dimmer100::Accendi()");
	if (isActive())
		return;
	//*#1*where*#1*lev*speed
	openwebnet msg_open;
	msg_open.CreateNullMsgOpen();
	char s[100];
	//*1*0#velocita*dove## 
	sprintf(s, "*1*1#%d*%s##", softstart, getAddress());
	msg_open.CreateMsgOpen(s, strlen(s));
	dev->sendFrame(msg_open.frame_open);
}

void dimmer100::Spegni()
{
	qDebug("dimmer100::Spegni()");
	if (!isActive())
		return;
	openwebnet msg_open;
	msg_open.CreateNullMsgOpen();
	char s[100];
	last_on_lev = getValue();
	//*1*0#velocita*dove## 
	sprintf(s, "*1*0#%d*%s##", softstop, getAddress());
	msg_open.CreateMsgOpen(s, strlen(s));
	dev->sendFrame(msg_open.frame_open);
}

void dimmer100::Aumenta()
{
	qDebug("dimmer100::Aumenta()");
	openwebnet msg_open;
	msg_open.CreateNullMsgOpen();
	char cosa[100];
	// Simone agresta il 4/4/2006
	// per l'incremento e il decremento prova ad usare il valore di velocit? di
	// default 255.
	sprintf(cosa, "30#5#255");
	msg_open.CreateMsgOpen("1", cosa, getAddress(), "");
	dev->sendFrame(msg_open.frame_open);
}

void dimmer100::Diminuisci()
{
	qDebug("dimmer100::Diminuisci()");
	openwebnet msg_open;
	char cosa[100];
	sprintf(cosa, "31#5#255");
	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen("1", cosa ,getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}

void dimmer100::status_changed(QList<device_status*> sl)
{
	stat_var curr_lev(stat_var::LEV);
	stat_var curr_speed(stat_var::SPEED);
	stat_var curr_status(stat_var::ON_OFF);
	stat_var curr_fault(stat_var::FAULT);
	bool aggiorna = false;
	qDebug("dimmer100::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::LIGHTS:
			qDebug("Light status variation");
			ds->read(device_status_light::ON_OFF_INDEX, curr_status);
			qDebug("status = %d", curr_status.get_val());
			if (!curr_status.get_val())
			{
				// Only update on OFF
				aggiorna = true;
				impostaAttivo(0);
			}
			else
				impostaAttivo(1);
			break;
		case device_status::DIMMER:
			ds->read(device_status_dimmer::LEV_INDEX, curr_lev);
			qDebug("dimmer status variation, ignored");
		case device_status::DIMMER100:
			ds->read(device_status_dimmer100::LEV_INDEX, curr_lev);
			ds->read(device_status_dimmer100::SPEED_INDEX, curr_speed);
			ds->read(device_status_dimmer100::FAULT_INDEX, curr_fault);
			if (curr_fault.get_val())
			{
				qDebug("DIMMER 100 FAULT !!");
				impostaAttivo(2);
			}
			else
			{
				qDebug("dimmer 100 status variation");
				qDebug("level = %d, speed = %d", curr_lev.get_val(), curr_speed.get_val());
				if ((curr_lev.get_val() == 0))
					impostaAttivo(0);
				if ((curr_lev.get_val() <= 5))
					setValue(5);
				else
					setValue(curr_lev.get_val());
				qDebug("value = %d", getValue());
			}
			aggiorna = true;
			break;
		case device_status::NEWTIMED:
			qDebug("new timed device status variation, ignored");
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}

	if (aggiorna)
		Draw();
}

void dimmer100::inizializza(bool forza)
{
	openwebnet msg_open;
	char    pippo[50];
	qDebug("dimmer100::inizializza");

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"*1##");
	msg_open.CreateMsgOpen((char*)pippo,strlen((char*)pippo));
	if (!forza)
		emit richStato(msg_open.frame_open);
	else
		dev->sendInit(msg_open.frame_open);
}

/*****************************************************************
 **gruppo di dimmer
 ****************************************************************/

grDimmer::grDimmer(QWidget *parent,const char *name,void *indirizzi, char* IconaSx,char* IconaDx,char *iconsx ,char* icondx,
	int period,int number) : bannRegolaz(parent, name)
{
	SetIcons(IconaSx, IconaDx ,icondx,iconsx);
	setAddress(indirizzi);
	dev = btouch_device_cache.get_device(getAddress());
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
	connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
}

void grDimmer::setAddress(void*indirizzi)
{
	elencoDisp = *((QList<QString*>*)indirizzi);
}

void grDimmer::sendFrame(char *msg)
{
	openwebnet msg_open;
	for (int i = 0; i < elencoDisp.size(); ++i)
	{
		msg_open.CreateNullMsgOpen();
		QByteArray buf = elencoDisp.at(i)->toAscii();
		msg_open.CreateMsgOpen("1", msg, buf.data(), "");
		dev->sendFrame(msg_open.frame_open);
	}
}

void grDimmer::Attiva()
{
	sendFrame("1");
}

void grDimmer::Disattiva()
{
	sendFrame("0");
}

void grDimmer::Aumenta()
{
	sendFrame("30");
}

void grDimmer::Diminuisci()
{
	sendFrame("31");
}

void grDimmer::inizializza(bool forza){}

/*****************************************************************
 **gruppo di dimmer100
 ****************************************************************/

grDimmer100::grDimmer100(QWidget *parent,const char *name,void *indirizzi, char* IconaSx,char* IconaDx,char *iconsx ,char* icondx,int period,
	int number, QList<int>sstart, QList<int>sstop) : grDimmer(parent, name, indirizzi, IconaSx, IconaDx, iconsx,icondx, period, number)
{
	qDebug("grDimmer100::grDimmer100()");
	qDebug("sstart[0] = %d", sstart[0]);
	setAddress(indirizzi);
	dev = btouch_device_cache.get_device(getAddress());
	soft_start = sstart;
	soft_stop = sstop;
}

void grDimmer100::Attiva()
{
	openwebnet msg_open;

	for (uchar idx = 0; idx < elencoDisp.count(); idx++)
	{
		msg_open.CreateNullMsgOpen();
		char s[100];
		QByteArray buf = elencoDisp.at(idx)->toAscii();
		sprintf(s, "*1*1#%d*%s##", soft_start[idx], buf.constData());
		msg_open.CreateMsgOpen(s, strlen(s));
		dev->sendFrame(msg_open.frame_open);
	}
}

void grDimmer100::Disattiva()
{
	openwebnet msg_open;

	for (uchar idx = 0; idx < elencoDisp.count(); idx++)
	{
		msg_open.CreateNullMsgOpen();
		char s[100];
		QByteArray buf = elencoDisp.at(idx)->toAscii();
		sprintf(s, "*1*0#%d*%s##", soft_stop[idx], buf.constData());
		msg_open.CreateMsgOpen(s, strlen(s));
		dev->sendFrame(msg_open.frame_open);
	}
}

void grDimmer100::Aumenta()
{
	openwebnet msg_open;
	for (uchar idx = 0; idx < elencoDisp.count(); idx++)
	{
		msg_open.CreateNullMsgOpen();
		QByteArray buf = elencoDisp.at(idx)->toAscii();
		msg_open.CreateMsgOpen("1", "30#5#255",buf.data(),"");
		dev->sendFrame(msg_open.frame_open);
	}
}

void grDimmer100::Diminuisci()
{
	openwebnet msg_open;
	for (uchar idx = 0; idx < elencoDisp.count(); idx++)
	{
		msg_open.CreateNullMsgOpen();
		QByteArray buf = elencoDisp.at(idx)->toAscii();
		msg_open.CreateMsgOpen("1", "31#5#255",buf.data(),"");
		dev->sendFrame(msg_open.frame_open);
	}
}
