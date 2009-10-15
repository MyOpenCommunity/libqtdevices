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
#include "icondispatcher.h"
#include "btbutton.h"
#include "fontmanager.h"

#include <QDebug>
#include <QLabel>


BannOnOffState::BannOnOffState(QWidget *parent) :
	banner(parent)
{
	const int BANONOFF_BUT_DIM = 60;
	const int BUTONOFF_ICON_DIM_X = 120;
	const int BUTONOFF_ICON_DIM_Y = 60;

	sx_button = new BtButton(this);
	sx_button->setGeometry(0, 0, BANONOFF_BUT_DIM , BANONOFF_BUT_DIM);

	dx_button = new BtButton(this);
	dx_button->setGeometry(banner_width-BANONOFF_BUT_DIM , 0 , BANONOFF_BUT_DIM , BANONOFF_BUT_DIM);

	text = new QLabel(this);
	text->setGeometry(0, BANONOFF_BUT_DIM, banner_width , banner_height-BANONOFF_BUT_DIM);
	text->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	text->setFont(bt_global::font->get(FontManager::TEXT));

	center_icon = new QLabel(this);
	center_icon->setGeometry(BANONOFF_BUT_DIM, 0, BUTONOFF_ICON_DIM_X , BUTONOFF_ICON_DIM_Y);
}

void BannOnOffState::loadIcons(QString l, QString c, QString r)
{
	qDebug() << l << c << r;
	left = l;
	center = c;
	right = r;

	sx_button->setImage(left);
	dx_button->setImage(right);
}

void BannOnOffState::setPrimaryText(const QString &t)
{
	text->setText(t);
}

void BannOnOffState::setState(States new_state)
{
	switch (new_state)
	{
	case ON:
	{
		int pos = center.indexOf(".");
		QString center_on = center.left(pos) + "on" + center.mid(pos);
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(center_on));
	}
		break;
	case OFF:
	{
		int pos = center.indexOf(".");
		QString center_off = center.left(pos) + "off" + center.mid(pos);
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(center_off));
	}
		break;
	}
}


SingleActuator::SingleActuator(QWidget *parent, const QDomNode &config_node, QString address)
	: BannOnOffState(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	loadIcons(bt_global::skin->getImage("off"), bt_global::skin->getImage("actuator_state"),
		bt_global::skin->getImage("on"));
	setPrimaryText(getTextChild(config_node, "descr"));
	setState(OFF);

	// TODO: read pull mode from config
	dev = bt_global::add_device_to_cache(new LightingDevice(address));

	connect(sx_button, SIGNAL(clicked()), SLOT(deactivate()));
	connect(dx_button, SIGNAL(clicked()), SLOT(activate()));
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

