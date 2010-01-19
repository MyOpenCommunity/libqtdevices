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
#include "btbutton.h"
#include "generic_functions.h" // getPressName, createMsgOpen
#include "devices_cache.h" // bt_global::devices_cache
#include "lighting_device.h"
#include "skinmanager.h"
#include "xml_functions.h"

#include <QDebug>


SingleActuator::SingleActuator(QWidget *parent, const QDomNode &config_node, QString address)
	: BannOnOffState(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	initBanner(bt_global::skin->getImage("off"), bt_global::skin->getImage("actuator_state"),
		bt_global::skin->getImage("on"), OFF, getTextChild(config_node, "descr"));

	// TODO: read pull mode from config
	dev = bt_global::add_device_to_cache(new LightingDevice(address));

	connect(left_button, SIGNAL(clicked()), SLOT(deactivate()));
	connect(right_button, SIGNAL(clicked()), SLOT(activate()));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
}

void SingleActuator::activate()
{
	dev->turnOn();
}

void SingleActuator::deactivate()
{
	dev->turnOff();
}

void SingleActuator::inizializza(bool forza)
{
	dev->requestStatus();
}

void SingleActuator::status_changed(const StatusList &status_list)
{
	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
	{
		switch (it.key())
		{
		case LightingDevice::DIM_DEVICE_ON:
			it.value().toBool() ? setState(ON) : setState(OFF);
			break;
		}
		++it;
	}
}



ButtonActuator::ButtonActuator(QWidget *parent, const QDomNode &config_node, int t) :
	BannSinglePuls(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	initBanner(bt_global::skin->getImage("on"), bt_global::skin->getImage("action_icon"),
		getTextChild(config_node, "descr"));
	setAddress(getTextChild(config_node, "where"));

	type = t;

	connect(right_button, SIGNAL(pressed()), SLOT(activate()));
	connect(right_button, SIGNAL(released()), SLOT(deactivate()));
}

// This banner should have a real device but 1) no vct devices are implemented, 2) three classes should be
// implemented, which really seems to me an overkill
void ButtonActuator::activate()
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

void ButtonActuator::deactivate()
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

