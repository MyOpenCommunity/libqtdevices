/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** actuators.cpp
 **
 ** definizioni dei vari attuatori implementati
 **
 ****************************************************************/

#include "actuators.h"
#include "device_status.h"
#include "device.h"
#include "btbutton.h"
#include "generic_functions.h" // getPressName, createMsgOpen
#include "devices_cache.h" // bt_global::devices_cache
#include "lighting_device.h"
#include "skinmanager.h"
#include "xml_functions.h"

#include <QDebug>


SingleLight::SingleLight(QWidget *parent, const QDomNode &config_node, QString address)
	: bannOnOff(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	// TODO: verify the correct skins
	SetIcons(bt_global::skin->getImage("on"), bt_global::skin->getImage("off"),
		bt_global::skin->getImage("lamp_on"), bt_global::skin->getImage("lamp_off"));
	setText(getTextChild(config_node, "descr"));
	// TODO: read pull mode from config
	dev = bt_global::add_device_to_cache(new LightingDevice(address));

	connect(this, SIGNAL(dxClick()), SLOT(lightOff()));
	connect(this, SIGNAL(sxClick()), SLOT(lightOn()));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
}

void SingleLight::lightOn()
{
	dev->turnOn();
}

void SingleLight::lightOff()
{
	dev->turnOff();
}

void SingleLight::inizializza(bool forza)
{
	dev->requestStatus();
	banner::inizializza(forza);
}

void SingleLight::status_changed(const StatusList &status_list)
{
	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
	{
		switch (it.key())
		{
		case LightingDevice::DIM_DEVICE_ON:
			impostaAttivo(it.value().toBool());
			break;
		}
		++it;
	}
	Draw();
}



#if 1
attuatAutom::attuatAutom(QWidget *parent,QString where,QString IconaSx, QString IconaDx, QString icon,
	QString pressedIcon) : bannOnOff(parent)
{
	SetIcons(IconaSx, IconaDx, icon, pressedIcon);
	setAddress(where);
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
	// Crea o preleva il dispositivo dalla cache
	dev = bt_global::devices_cache.get_light(getAddress());
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
				ds->read(device_status_light::ON_OFF_INDEX, curr_status);
				qDebug("status = %d", curr_status.get_val());
				// this avoids changing state when the same state arrives
				// eg when in ACTIVE arrives ON, we must stay in ACTIVE
				if ((!curr_status.get_val() && isActive()) ||
					(curr_status.get_val() && !isActive()))
				{
					aggiorna = true;
					impostaAttivo(isActive() ? 0 : 1);
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


attuatPuls::attuatPuls(QWidget *parent, QString where, QString IconaSx, QString icon, char tipo)
	: bannPuls(parent)
{
	SetIcons(IconaSx, QString(), icon, QString());
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
		sendFrame(createMsgOpen("1", "1", getAddress()));
		break;
	case  VCT_SERR:
		sendFrame(createMsgOpen("6", "10", getAddress()));
		break;
	case  VCT_LS:
		sendFrame(createMsgOpen("6", "12", getAddress()));
		break;
	}
}

void attuatPuls::Disattiva()
{
	switch (type)
	{
	case  AUTOMAZ:
		sendFrame(createMsgOpen("1", "0", getAddress()));
		break;
	case  VCT_SERR:
		break;
	case  VCT_LS:
		sendFrame(createMsgOpen("6", "11", getAddress()));
		break;
	}
}
#endif

