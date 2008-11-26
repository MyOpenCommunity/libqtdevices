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
#include "device_status.h"
#include "device.h"
#include "btbutton.h"
#include "generic_functions.h" // getPressName, createMsgOpen
#include "fontmanager.h"
#include "btmain.h"
#include "main.h" // BTouch

#include <QDebug>


attuatAutom::attuatAutom(QWidget *parent,QString where,QString IconaSx, QString IconaDx, QString icon, QString pressedIcon, int period, int number)
	: bannOnOff(parent)
{
	SetIcons(IconaSx, IconaDx, icon, pressedIcon, period, number);
	setAddress(where);
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
	dev->sendFrame(createMsgOpen("1", "1", getAddress()));
}

void attuatAutom::Disattiva()
{
	dev->sendFrame(createMsgOpen("1", "0", getAddress()));
}

void attuatAutom::inizializza(bool forza)
{
	QString f = "*#1*" + getAddress() + "##";
	if (!forza)
		emit richStato(f);
	else
		dev->sendInit(f);
}


attuatPuls::attuatPuls(QWidget *parent, QString where, QString IconaSx, QString icon, char tipo, int period, int number)
: bannPuls(parent)
{
	SetIcons(IconaSx, QString(), icon, QString(), period,number);
	setAddress(where);
	connect(this,SIGNAL(sxPressed()),this,SLOT(Attiva()));
	connect(this,SIGNAL(sxReleased()),this,SLOT(Disattiva()));
	type = tipo;
	impostaAttivo(1);
}

void attuatPuls::Attiva()
{
	switch (type)
	{
	case  AUTOMAZ:
		BTouch->sendFrame(createMsgOpen("1", "1", getAddress()));
		break;
	case  VCT_SERR:
		BTouch->sendFrame(createMsgOpen("6", "10", getAddress()));
		break;
	case  VCT_LS:
		BTouch->sendFrame(createMsgOpen("6", "12", getAddress()));
		break;
	}
}

void attuatPuls::Disattiva()
{
	switch (type)
	{
	case  AUTOMAZ:
		BTouch->sendFrame(createMsgOpen("1", "0", getAddress()));
		break;
	case  VCT_SERR:
		break;
	case  VCT_LS:
		BTouch->sendFrame(createMsgOpen("6", "11", getAddress()));
		break;
	}
}

void attuatPuls::inizializza(bool forza)
{
}

