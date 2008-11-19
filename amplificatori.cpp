/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** amplificatori.cpp
 **
 **definizioni dei vari amplificatori implementati
 **
 ****************************************************************/

#include "amplificatori.h"
#include "device_cache.h" // btouch_device_cache
#include "device_status.h"
#include "device.h"
#include "btmain.h"
#include "main.h" // BTouch

#include <openwebnet.h> // class openwebnet

#include <QString>

#include <stdio.h> //sprintf


/*****************************************************************
 **amplificatore
 ****************************************************************/

amplificatore::amplificatore(QWidget *parent, char *indirizzo, QString IconaSx, QString IconaDx, QString icon, QString inactiveIcon)
	: bannRegolaz(parent)
{
	qDebug("amplificatore::amplificatore()");
	setRange(1,9);
	SetIcons(IconaSx, IconaDx, icon, inactiveIcon, true);
	setAddress(indirizzo);
	connect(this,SIGNAL(sxClick()),this,SLOT(Accendi()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Spegni()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
	connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
	setValue(1);
	impostaAttivo(0);
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_sound_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

int trasformaVol(int vol)
{
	if (vol<0)
		return(-1);

	if (vol<=3) 
		return(1);
	if (vol<=7) 
		return(2);
	if (vol<=11) 
		return(3);
	if (vol<=14) 
		return(4);
	if (vol<=17) 
		return(5);
	if (vol<=20) 
		return(6);
	if (vol<=23) 
		return(7);
	if (vol<=27) 
		return(8);
	if (vol<=31) 
		return(9);
	return(-1);
}

void amplificatore::status_changed(QList<device_status*> sl)
{
	stat_var curr_lev(stat_var::LEV);
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("amplificatore::status_changed");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::AMPLIFIER :
			qDebug("Ampli status variation");
			ds->read(device_status_amplifier::ON_OFF_INDEX, curr_status);
			ds->read(device_status_amplifier::AUDIO_LEVEL_INDEX, curr_lev);
			qDebug("status = %d, lev = %d", curr_status.get_val(),
					curr_lev.get_val());
			if ((isActive() && !curr_status.get_val()) || (!isActive() && curr_status.get_val()))
			{
				impostaAttivo(curr_status.get_val() != 0);
				aggiorna = true;
			}
			if (getValue() != curr_lev.get_val())
			{
				setValue(trasformaVol(curr_lev.get_val()));
				aggiorna = true ;
			}
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}

	if (aggiorna)
		Draw();
}

void amplificatore::Accendi()
{
	char pippo[50];
	char ind[3];

	qDebug("amplificatore::Accendi()");
	memset(pippo,'\000',sizeof(pippo));
	sprintf(ind, "%s", getAddress());
	sprintf(pippo,"*22*34#4#%c*3#%c#%c##",ind[0], ind[0], ind[1]);
	dev->sendFrame(pippo);
}

void amplificatore::Spegni()
{
	char pippo[50];
	char ind[3];

	qDebug("amplificatore::Spegni()");
	memset(pippo,'\000',sizeof(pippo));
	sprintf(ind, "%s", getAddress());
	sprintf(pippo,"*22*0#4#%c*3#%c#%c##",ind[0], ind[0], ind[1]);
	dev->sendFrame(pippo);
}

void amplificatore::Aumenta()
{
	openwebnet msg_open;

	qDebug("amplificatore::Aumenta()");
	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen("16", "1001",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}

void amplificatore::Diminuisci()
{
	openwebnet msg_open;

	qDebug("amplificatore::Diminuisci()");
	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen("16", "1101",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}

void amplificatore::inizializza(bool forza)
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#16*");
	strcat(pippo,getAddress());
	strcat(pippo,"*1##");
	msg_open.CreateMsgOpen((char*)pippo,strlen((char*)pippo));
	dev->sendInit(msg_open.frame_open);

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#16*");
	strcat(pippo,getAddress());
	strcat(pippo,"*5##");
	msg_open.CreateMsgOpen((char*)pippo,strlen((char*)pippo));
	dev->sendInit(msg_open.frame_open);
}

/*****************************************************************
 **gruppo di amplificatori
 ****************************************************************/

grAmplificatori::grAmplificatori(QWidget *parent, QList<QString> indirizzi, QString IconaSx,
	QString IconaDx, QString iconsx, QString icondx) : bannRegolaz(parent), elencoDisp(indirizzi)
{
	SetIcons(IconaSx, IconaDx, icondx, iconsx);
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
	connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
}

void grAmplificatori::sendActivationFrame(QString argm)
{
	for (int i = 0; i < elencoDisp.size(); ++i)
	{
		QString ind = elencoDisp.at(i);
		QString f;
		if (ind == "0")
			f = QString("*22*%2#4#%1*5#3#%1##").arg(ind.at(0)).arg(argm);
		else if (ind.at(0) == '#')
			f = QString("*22*%2#4#%1*4#%1##").arg(ind.at(1)).arg(argm);
		else
			f = QString("*22*%3#4#%1*3#%1#%2##").arg(ind.at(0)).arg(ind.at(1)).arg(argm);

		BTouch->sendFrame(f);
	}
}

void grAmplificatori::Attiva()
{
	sendActivationFrame("34");
}

void grAmplificatori::Disattiva()
{
	sendActivationFrame("0");
}

void grAmplificatori::Aumenta()
{
	openwebnet msg_open;

	for (int i = 0; i < elencoDisp.size(); ++i)
	{
		msg_open.CreateNullMsgOpen();
		QByteArray buf = elencoDisp.at(i).toAscii();
		msg_open.CreateMsgOpen("16", "1001",buf.data(),"");
		BTouch->sendFrame(msg_open.frame_open);
	}
}

void grAmplificatori::Diminuisci()
{
	openwebnet msg_open;

	for (int i = 0; i < elencoDisp.size(); ++i)
	{
		msg_open.CreateNullMsgOpen();
		QByteArray buf = elencoDisp.at(i).toAscii();
		msg_open.CreateMsgOpen("16", "1101",buf.data(),"");
		BTouch->sendFrame(msg_open.frame_open);
	}
}

void grAmplificatori::inizializza(bool forza)
{
}
