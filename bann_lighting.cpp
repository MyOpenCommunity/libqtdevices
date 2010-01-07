#include "bann_lighting.h"
#include "device_status.h"
#include "device.h"
#include "btbutton.h"
#include "fontmanager.h" // bt_global::font
#include "devices_cache.h" // bt_global::devices_cache
#include "generic_functions.h" // createMsgOpen
#include "icondispatcher.h" //bt_global::icons_cache
#include "skinmanager.h" // skincontext
#include "xml_functions.h" // getTextChild
#include "lighting_device.h"


#include <openwebnet.h> // class openwebnet

#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QDomNode>

#include <stdlib.h> // atoi

namespace
{
	QString formatTime(const BtTime &t)
	{
		QString str;
		int h = t.hour();
		int m = t.minute();
		int s = t.second();
		if (h == 0 && m == 0)  // time in secs
			str = QString("%1''").arg(s);
		else if (h == 0) // time in mins
			str = QString("%1'").arg(m);
		else if (h < 10)   // time in hh:mm
			str = QString("%1:%2").arg(h).arg(m, 2, 10, QChar('0'));
		else
			str = QString("%1h").arg(h);
		return str;
	}
}


LightGroup::LightGroup(QWidget *parent, const QDomNode &config_node, const QList<QString> &addresses)
	: BannOnOffNew(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	initBanner(bt_global::skin->getImage("off"), bt_global::skin->getImage("lamp_group_on"),
		bt_global::skin->getImage("on"), getTextChild(config_node, "descr"));

	foreach (const QString &address, addresses)
		// since we don't care about status changes, use PULL mode to analyze fewer frames
		devices << bt_global::add_device_to_cache(new LightingDevice(address, PULL));

	connect(left_button, SIGNAL(clicked()), SLOT(lightOff()));
	connect(right_button, SIGNAL(clicked()), SLOT(lightOn()));
}

void LightGroup::lightOff()
{
	foreach (LightingDevice *l, devices)
		l->turnOff();
}

void LightGroup::lightOn()
{
	foreach (LightingDevice *l, devices)
		l->turnOn();
}



AdjustDimmer::AdjustDimmer(QWidget *parent) :
	BannLevel(parent)
{
	current_level = 10;
}

void AdjustDimmer::initBanner(const QString &left, const QString &_center_left, const QString &_center_right,
		const QString &right, const QString &_broken, States init_state, int init_level,
		const QString &banner_text)
{
	BannLevel::initBanner(banner_text);

	left_button->setImage(left);
	right_button->setImage(right);

	center_left = _center_left;
	center_right = _center_right;
	broken = _broken;

	setState(init_state);
	setLevel(init_level);
}

void AdjustDimmer::setState(States new_state)
{
	switch (new_state)
	{
	case ON:
		setOnIcons();
		break;
	case OFF:
		setCenterLeftIcon(getBostikName(center_left, "sxl0"));
		setCenterRightIcon(getBostikName(center_right, "dxl0"));
		break;
	case BROKEN:
		setCenterLeftIcon(getBostikName(broken, "sx"));
		setCenterRightIcon(getBostikName(broken, "dx"));
		break;
	}
	current_state = new_state;
}

void AdjustDimmer::setOnIcons()
{
	setCenterLeftIcon(getBostikName(center_left, QString("sxl") + QString::number(current_level)));
	setCenterRightIcon(getBostikName(center_right, QString("dxl") + QString::number(current_level)));
}

void AdjustDimmer::setLevel(int level)
{
	current_level = level;
	if (current_state == ON)
		setOnIcons();
}



DimmerNew::DimmerNew(QWidget *parent, const QDomNode &config_node, QString where) :
	AdjustDimmer(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	light_value = 20;
	initBanner(bt_global::skin->getImage("off"), bt_global::skin->getImage("dimmer"),
		bt_global::skin->getImage("dimmer"), bt_global::skin->getImage("on"),
		bt_global::skin->getImage("dimmer_broken"), OFF, light_value, getTextChild(config_node, "descr"));

	dev = bt_global::add_device_to_cache(new DimmerDevice(where));
	connect(right_button, SIGNAL(clicked()), SLOT(lightOn()));
	connect(left_button, SIGNAL(clicked()), SLOT(lightOff()));
	connect(this, SIGNAL(center_left_clicked()), SLOT(decreaseLevel()));
	connect(this, SIGNAL(center_right_clicked()), SLOT(increaseLevel()));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
}

void DimmerNew::lightOn()
{
	dev->turnOn();
}

void DimmerNew::lightOff()
{
	dev->turnOff();
}

void DimmerNew::increaseLevel()
{
	dev->increaseLevel();
}

void DimmerNew::decreaseLevel()
{
	dev->decreaseLevel();
}

void DimmerNew::inizializza(bool forza)
{
	dev->requestStatus();
}

void DimmerNew::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case LightingDevice::DIM_DEVICE_ON:
			setState(it.value().toBool() ? ON : OFF);
			setLevel(light_value);
			break;
		case LightingDevice::DIM_DIMMER_LEVEL:
			setState(ON);
			light_value = it.value().toInt();
			setLevel(light_value);
			break;
		case LightingDevice::DIM_DIMMER_PROBLEM:
			// TODO: what happens if we are in dimmer fault state?
			setState(BROKEN);
			break;
		}
		++it;
	}
}



DimmerGroup::DimmerGroup(QWidget *parent, const QDomNode &config_node, QList<QString> addresses) :
	BannLevel(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	initBanner(bt_global::skin->getImage("off"), bt_global::skin->getImage("dimmer_grp_sx"),
		bt_global::skin->getImage("dimmer_grp_dx"),bt_global::skin->getImage("on"),
		getTextChild(config_node, "descr"));

	foreach (const QString &address, addresses)
		// since we don't care about status changes, use PULL mode to analyze fewer frames
		devices << bt_global::add_device_to_cache(new DimmerDevice(address, PULL));

	connect(right_button, SIGNAL(clicked()), SLOT(lightOn()));
	connect(left_button, SIGNAL(clicked()), SLOT(lightOff()));
	connect(this, SIGNAL(center_right_clicked()), SLOT(increaseLevel()));
	connect(this, SIGNAL(center_left_clicked()), SLOT(decreaseLevel()));
}

void DimmerGroup::lightOn()
{
	foreach (DimmerDevice *l, devices)
		l->turnOn();
}

void DimmerGroup::lightOff()
{
	foreach (DimmerDevice *l, devices)
		l->turnOff();
}

void DimmerGroup::increaseLevel()
{
	foreach (DimmerDevice *l, devices)
		l->increaseLevel();
}

void DimmerGroup::decreaseLevel()
{
	foreach (DimmerDevice *l, devices)
		l->decreaseLevel();
}


enum
{
	DIMMER100_STEP = 5,
	DIMMER100_SPEED = 255,
};

Dimmer100New::Dimmer100New(QWidget *parent, const QDomNode &config_node) :
	AdjustDimmer(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	light_value = 5;
	initBanner(bt_global::skin->getImage("off"), bt_global::skin->getImage("dimmer"),
		bt_global::skin->getImage("dimmer"), bt_global::skin->getImage("on"),
		bt_global::skin->getImage("dimmer_broken"), OFF, light_value, getTextChild(config_node, "descr"));

	QString where = getTextChild(config_node, "where");
	dev = bt_global::add_device_to_cache(new Dimmer100Device(where));

	start_speed = getTextChild(config_node, "softstart").toInt();
	stop_speed = getTextChild(config_node, "softstop").toInt();

	connect(right_button, SIGNAL(clicked()), SLOT(lightOn()));
	connect(left_button, SIGNAL(clicked()), SLOT(lightOff()));
	connect(this, SIGNAL(center_right_clicked()), SLOT(increaseLevel()));
	connect(this, SIGNAL(center_left_clicked()), SLOT(decreaseLevel()));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
}

void Dimmer100New::lightOn()
{
	dev->turnOn(start_speed);
}

void Dimmer100New::lightOff()
{
	dev->turnOff(stop_speed);
}

void Dimmer100New::increaseLevel()
{
	dev->increaseLevel100(DIMMER100_STEP, DIMMER100_SPEED);
}

void Dimmer100New::decreaseLevel()
{
	dev->decreaseLevel100(DIMMER100_STEP, DIMMER100_SPEED);
}

void Dimmer100New::inizializza(bool forza)
{
	dev->requestDimmer100Status();
}

void Dimmer100New::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case LightingDevice::DIM_DEVICE_ON:
			setState(it.value().toBool() ? ON : OFF);
			setLevel(light_value);
			break;
		case LightingDevice::DIM_DIMMER_LEVEL:
			setState(ON);
			light_value = it.value().toInt();
			// values too low are not registered by UI
			if (light_value < 5)
				light_value = 5;
			setLevel(light_value);
			break;
		case LightingDevice::DIM_DIMMER_PROBLEM:
			setState(BROKEN);
			break;
		case LightingDevice::DIM_DIMMER100_LEVEL:
		{
			// light values are between 0 (min) and 100 (max)
			light_value = roundTo5(it.value().toInt());
			setLevel(light_value);
			setState(ON);
		}
			break;
		}
		++it;
	}
}

// Round a value to the nearest greater multiple of 5, eg. numbers 1-5 map to 5, 6-10 to 10 and so on.
int Dimmer100New::roundTo5(int value)
{
	// I really hope the compiler optimizes this case!
	int div = value / 5;
	int mod = value % 5;
	if (mod == 0)
		return value;
	else
		return (div + 1) * 5;
}


Dimmer100Group::Dimmer100Group(QWidget *parent, const QDomNode &config_node) :
	BannLevel(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	initBanner(bt_global::skin->getImage("off"), bt_global::skin->getImage("dimmer_grp_sx"),
		bt_global::skin->getImage("dimmer_grp_dx"),bt_global::skin->getImage("on"),
		getTextChild(config_node, "descr"));

	// load all devices with relative start and stop speed
	QList<QDomNode> elements = getChildren(config_node, "element");
	foreach (const QDomNode &el, elements)
	{
		devices << bt_global::add_device_to_cache(new Dimmer100Device(getTextChild(el, "where"), PULL));
		start_speed << getTextChild(el, "softstart").toInt();
		stop_speed << getTextChild(el, "softstop").toInt();
	}
	Q_ASSERT_X(devices.size() == start_speed.size(), "Dimmer100Group::Dimmer100Group",
		"Device number and softstart number are different");
	Q_ASSERT_X(devices.size() == stop_speed.size(), "Dimmer100Group::Dimmer100Group",
		"Device number and softstop number are different");

	connect(right_button, SIGNAL(clicked()), SLOT(lightOn()));
	connect(left_button, SIGNAL(clicked()), SLOT(lightOff()));
	connect(this, SIGNAL(center_right_clicked()), SLOT(increaseLevel()));
	connect(this, SIGNAL(center_left_clicked()), SLOT(decreaseLevel()));
}

void Dimmer100Group::lightOff()
{
	for (int i = 0; i < devices.size(); ++i)
		devices[i]->turnOff(stop_speed[i]);
}

void Dimmer100Group::lightOn()
{
	for (int i = 0; i < devices.size(); ++i)
		devices[i]->turnOn(start_speed[i]);
}

void Dimmer100Group::increaseLevel()
{
	for (int i = 0; i < devices.size(); ++i)
		devices[i]->increaseLevel100(DIMMER100_STEP, DIMMER100_SPEED);
}

void Dimmer100Group::decreaseLevel()
{
	for (int i = 0; i < devices.size(); ++i)
		devices[i]->decreaseLevel100(DIMMER100_STEP, DIMMER100_SPEED);
}


TempLight::TempLight(QWidget *parent, const QDomNode &config_node) :
	BannOnOff2Labels(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	QString descr = getTextChild(config_node, "descr");
	initBanner(bt_global::skin->getImage("lamp_cycle"), bt_global::skin->getImage("lamp_time_on"),
		bt_global::skin->getImage("on"), OFF, descr, QString());

	QString where = getTextChild(config_node, "where");
	dev = bt_global::add_device_to_cache(new LightingDevice(where));

	time_index = 0;
	readTimes(config_node);
	updateTimeLabel();
	connect(left_button, SIGNAL(clicked()), SLOT(cycleTime()));
	connect(right_button, SIGNAL(clicked()), SLOT(activate()));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
}

void TempLight::inizializza(bool forza)
{
	dev->requestStatus();
}

void TempLight::readTimes(const QDomNode &node)
{
	Q_UNUSED(node);
	times << BtTime(0, 1, 0); // 1 min
	times << BtTime(0, 2, 0);
	times << BtTime(0, 3, 0);
	times << BtTime(0, 4, 0);
	times << BtTime(0, 5, 0);
	times << BtTime(0, 15, 0);
	times << BtTime(0, 0, 30);
	Q_ASSERT_X(times.size() <= 7, "TempLight::readTimes",
		"times length must be <= 7, otherwise activation will fail");
}

void TempLight::cycleTime()
{
	time_index = (time_index + 1) % times.size();
	updateTimeLabel();
}

void TempLight::updateTimeLabel()
{
	BtTime t = times[time_index];
	setCentralText(formatTime(t));
}

void TempLight::activate()
{
	dev->fixedTiming(time_index);
}

void TempLight::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case LightingDevice::DIM_DEVICE_ON:
			setState(it.value().toBool() ? ON : OFF);
			break;
		}
		++it;
	}
}


TempLightVariable::TempLightVariable(QWidget *parent, const QDomNode &config_node) :
	TempLight(parent, config_node)
{
	readTimes(config_node);
	updateTimeLabel();
}

void TempLightVariable::readTimes(const QDomNode &node)
{
	// here, times has still the times of the base class. Remove them
	times.clear();
	foreach (const QDomNode &time, getChildren(node, "time"))
	{
		QString s = time.toElement().text();
		QStringList sl = s.split("*");
		times << BtTime(sl[0].toInt(), sl[1].toInt(), sl[2].toInt());
	}
}

void TempLightVariable::inizializza(bool forza)
{
	dev->requestVariableTiming();
}

void TempLightVariable::activate()
{
	BtTime t = times[time_index];
	dev->variableTiming(t.hour(), t.minute(), t.second());
}



enum {
	TLF_TIME_STATES = 8,
};

TempLightFixed::TempLightFixed(QWidget *parent, const QDomNode &config_node) :
	BannOn2Labels(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

#ifdef CONFIG_BTOUCH
	// I think conf.xml will have only one node for time in this banner, however
	// such node is indicated as "timeX", so I'm using the following overkill code
	// to be safe
	QList<QDomNode> children = getChildren(config_node, "time");
	QStringList sl;
	foreach (const QDomNode &tmp, children)
		sl << tmp.toElement().text().split("*");

	Q_ASSERT_X(sl.size() == 3, "TempLightFixed::TempLightFixed", "Time must have 3 fields");
	lighting_time = BtTime(sl[0].toInt(), sl[1].toInt(), sl[2].toInt());
	total_time = lighting_time.hour() * 3600 + lighting_time.minute() * 60 + lighting_time.second();
#else
	total_time = getTextChild(config_node, "time").toInt();
	lighting_time = BtTime(total_time / 3600, (total_time / 60) % 60, total_time % 60);
#endif

	QString where = getTextChild(config_node, "where");
	dev = bt_global::add_device_to_cache(new LightingDevice(where));

	QString descr = getTextChild(config_node, "descr");
	initBanner(bt_global::skin->getImage("on"), bt_global::skin->getImage("lamp_status"),
		bt_global::skin->getImage("lamp_time"), descr, formatTime(lighting_time));

	request_timer.setInterval((total_time / TLF_TIME_STATES) * 1000);
	connect(&request_timer, SIGNAL(timeout()), SLOT(updateTimerLabel()));

	connect(right_button, SIGNAL(clicked()), SLOT(setOn()));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
}

void TempLightFixed::inizializza(bool forza)
{
	dev->requestStatus();
	dev->requestVariableTiming();
}

void TempLightFixed::requestStatus()
{
	dev->requestVariableTiming();
}

void TempLightFixed::setOn()
{
	dev->variableTiming(lighting_time.hour(), lighting_time.minute(), lighting_time.second());
	// TODO: is this ok? does it update correctly the timer slice the first time?
	request_timer.start();
	valid_update = false;
	update_retries = 0;
}

void TempLightFixed::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case LightingDevice::DIM_DEVICE_ON:
		{
			bool is_on = it.value().toBool();
			setElapsedTime(0);
			if (is_on)
			{
				setState(ON);
			}
			else
			{
				setState(OFF);
				stopTimer();
			}
		}
			break;
		case LightingDevice::DIM_VARIABLE_TIMING:
		{
			BtTime t = it.value().value<BtTime>();
			// all 0's means either the light is on or timer is stopped anyway, so stop request timer
			if ((t.hour() == 0) && (t.minute() == 0) && (t.second() == 0))
			{
				stopTimer();
				break;
			}
			// ignore strange frames (taken from old code)
			if ((t.hour() == 255) && (t.minute() == 255) && (t.second() == 255))
				break;
			// convert t to seconds, then compute the number of slices
			int time = qRound((t.hour() * 3600 + t.minute() * 60 + t.second()) * TLF_TIME_STATES / total_time);
			setElapsedTime(time);
			setState(ON);
			valid_update = true;
		}
			break;
		}
		++it;
	}
}

#define MAX_RETRY 2

void TempLightFixed::updateTimerLabel()
{
	if (!valid_update)
	{
		++update_retries;
		if (update_retries > MAX_RETRY)
			stopTimer();
	}
	requestStatus();
	valid_update = false;
}

void TempLightFixed::stopTimer()
{
	request_timer.stop();
	valid_update = false;
	update_retries = 0;
}


#if 0

dimmer::dimmer(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString icon, QString inactiveIcon, QString breakIcon,
	bool to_be_connect) : bannRegolaz(parent)
{

	setRange(20, 100);
	setStep(10);
	SetIcons(IconaSx, IconaDx, icon, inactiveIcon, breakIcon, false);
	setAddress(where);
	connect(this,SIGNAL(sxClick()),this,SLOT(Accendi()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Spegni()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
	connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
	if (to_be_connect)
	{
		// Crea o preleva il dispositivo dalla cache
		dev = bt_global::devices_cache.get_dimmer(getAddress());
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
		BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		BannerText->setFont(bt_global::font->get(FontManager::TEXT));
		BannerText->setText(qtesto);
	}
	if (SecondaryText)
	{
		SecondaryText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		SecondaryText->setFont(bt_global::font->get(FontManager::TEXT));
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
	dev->sendFrame(createMsgOpen("1", "1", getAddress()));
}

void dimmer::Spegni()
{
	dev->sendFrame(createMsgOpen("1", "0", getAddress()));
}

void dimmer::Aumenta()
{
	dev->sendFrame(createMsgOpen("1", "30", getAddress()));
}

void dimmer::Diminuisci()
{
	dev->sendFrame(createMsgOpen("1", "31", getAddress()));
}

void dimmer::inizializza(bool forza)
{
	qDebug("dimmer::inizializza");
	QString f = "*#1*" + getAddress() + "##";
	if (!forza)
		emit richStato(f);
	else
		dev->sendInit(f);
}


dimmer100::dimmer100(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString icon,
	QString inactiveIcon, QString breakIcon, int sstart, int sstop)
	: dimmer(parent, where, IconaSx, IconaDx, icon,inactiveIcon, breakIcon, false)
{
	qDebug("costruttore dimmer100");
	softstart = sstart;
	qDebug("softstart = %d", softstart);
	softstop = sstop;
	qDebug("softstop = %d", softstop);
	setRange(5,100);
	setStep(5);
	setValue(0);
	// this is necessary otherwise icons are loaded in the base class,
	// which has incorrect values for range() and step().
	SetIcons(IconaSx, IconaDx, icon, inactiveIcon, breakIcon, false);

	dev = bt_global::devices_cache.get_dimmer100(getAddress());
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
	//*1*0#velocita*dove##
	dev->sendFrame(QString("*1*1#%1*%2##").arg(softstart).arg(getAddress()));
}

void dimmer100::Spegni()
{
	qDebug("dimmer100::Spegni()");
	if (!isActive())
		return;
	last_on_lev = getValue();
	//*1*0#velocita*dove##
	dev->sendFrame(QString("*1*0#%1*%2##").arg(softstop).arg(getAddress()));
}

void dimmer100::Aumenta()
{
	qDebug("dimmer100::Aumenta()");

	// Simone agresta il 4/4/2006
	// per l'incremento e il decremento prova ad usare il valore di velocit? di
	// default 255.
	dev->sendFrame(createMsgOpen("1", "30#5#255", getAddress()));
}

void dimmer100::Diminuisci()
{
	qDebug("dimmer100::Diminuisci()");
	dev->sendFrame(createMsgOpen("1", "31#5#255", getAddress()));
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
			break;
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
				else
					impostaAttivo(1);
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
	qDebug("dimmer100::inizializza");
	QString f = "*#1*" + getAddress() + "*1##";
	
	if (!forza)
		emit richStato(f);
	else
		dev->sendInit(f);
}


grDimmer::grDimmer(QWidget *parent, QList<QString> addresses, QString IconaSx, QString IconaDx, QString iconsx,
	QString icondx) : bannRegolaz(parent)
{
	SetIcons(IconaSx, IconaDx, icondx, iconsx);
	elencoDisp = addresses;
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
	connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
}

void grDimmer::sendMsg(QString msg)
{
	for (int i = 0; i < elencoDisp.size(); ++i)
		sendFrame(createMsgOpen("1", msg, elencoDisp.at(i)));
}

void grDimmer::Attiva()
{
	sendMsg("1");
}

void grDimmer::Disattiva()
{
	sendMsg("0");
}

void grDimmer::Aumenta()
{
	sendMsg("30");
}

void grDimmer::Diminuisci()
{
	sendMsg("31");
}


grDimmer100::grDimmer100(QWidget *parent, QList<QString> addresses, QString IconaSx, QString IconaDx, QString iconsx,
	QString icondx, QList<int>sstart, QList<int>sstop) : grDimmer(parent, addresses, IconaSx, IconaDx, iconsx, icondx)
{
	qDebug("grDimmer100::grDimmer100()");
	qDebug("sstart[0] = %d", sstart[0]);
	soft_start = sstart;
	soft_stop = sstop;
}

void grDimmer100::Attiva()
{
	for (int idx = 0; idx < elencoDisp.size(); idx++)
		sendFrame(QString("*1*1#%1*%2##").arg(soft_start.at(idx)).arg(elencoDisp.at(idx)));
}

void grDimmer100::Disattiva()
{
	for (int idx = 0; idx < elencoDisp.size(); idx++)
		sendFrame(QString("*1*0#%1*%2##").arg(soft_stop.at(idx)).arg(elencoDisp.at(idx)));
}

void grDimmer100::Aumenta()
{
	for (int idx = 0; idx < elencoDisp.size(); idx++)
		sendFrame(createMsgOpen("1", "30#5#255", elencoDisp.at(idx)));
}

void grDimmer100::Diminuisci()
{
	for (int idx = 0; idx < elencoDisp.size(); idx++)
		sendFrame(createMsgOpen("1", "31#5#255", elencoDisp.at(idx)));
}




attuatAutomTemp::attuatAutomTemp(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString icon,
	QString pressedIcon, QList<QString> lt) : bannOnOff2scr(parent)
{
	SetIcons(IconaDx, IconaSx, icon, pressedIcon);
	setAddress(where);
	cntTempi = 0;
	// TODO: riscrivere con un QVector<QString>!!
	static const char *t[] =  { "1'", "2'", "3'", "4'", "5'", "15'", "30''" };
	int nt = lt.size() ? lt.size() : sizeof(t) / sizeof(char *);
	for (int i = 0; i < nt; i++)
	{
		QString s;
		if (i < lt.size())
			s = lt.at(i);
		else
			s = t[i];
		tempi.append(s);
	}

	assegna_tempo_display();
	setSecondaryText(tempo_display);
	dev = bt_global::devices_cache.get_light(getAddress());
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

	return tempi.at(cntTempi);
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
		{
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
		}
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
	dev->sendFrame(createMsgOpen("1", QString::number(11+cntTempi), getAddress()));
}

void attuatAutomTemp::CiclaTempo()
{
	cntTempi = (cntTempi+1) % ntempi();
	qDebug("ntempi = %d", ntempi());
	qDebug("cntTempi = %d", cntTempi);
	assegna_tempo_display();
	setSecondaryText(tempo_display);
	Draw();
}

void attuatAutomTemp::inizializza(bool forza)
{
	dev->sendInit("*#1*" + getAddress() + "##");
}

void attuatAutomTemp::assegna_tempo_display()
{
	tempo_display = leggi_tempo();
}


attuatAutomTempNuovoN::attuatAutomTempNuovoN(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString icon,
	QString pressedIcon, QList<QString> lt)
	: attuatAutomTemp(parent, where, IconaSx, IconaDx, icon, pressedIcon, lt)
{
	assegna_tempo_display();
	stato_noto = false;
	setSecondaryText(tempo_display);
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
		{
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
		}
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
	dev->sendFrame(QString("*#1*%1*#2*%2##").arg(getAddress()).arg(leggi_tempo()));
}

// *#1*dove*2## 
void attuatAutomTempNuovoN::inizializza(bool forza)
{
	dev->sendInit("*#1*" + getAddress() + "*2##");
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


#define NTIMEICONS 9

attuatAutomTempNuovoF::attuatAutomTempNuovoF(QWidget *parent, QString where, QString IconaCentroSx, QString IconaCentroDx,
	QString IconaDx, QString t) : bannOn2scr(parent)
{
	SetIcons(IconaCentroSx, IconaCentroDx, IconaDx);
	setAddress(where);
	setSecondaryText("????");

	// TODO: putting all 0's here we have a division by zero in Draw(). It must be configured
	// in conf.xml, but anyway '0*0*1' seems a reasonable default (at least BTouch doesn't segfault...)
	tempo = !t.isEmpty() ? t : "0*0*1";

	QStringList split_list = t.split("*");
	switch (split_list.size())
	{
	case 3:
		h = split_list.at(0).toInt();
		m = split_list.at(1).toInt();
		s = split_list.at(2).toInt();
		break;
	case 2:
		h = split_list.at(0).toInt();
		m = split_list.at(1).toInt();
		s = 0;
	case 1:
		h = split_list.at(0).toInt();
		m = 0;
		s = 0;
	}

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
	setSecondaryText(tmp);
	connect(this,SIGNAL(dxClick()),this,SLOT(Attiva())); 
	// Crea o preleva il dispositivo dalla cache
	dev = bt_global::devices_cache.get_newtimed(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
	dev = bt_global::devices_cache.get_dimmer(getAddress());
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
		{
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
		}
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
	dev->sendInit(QString("*#1*%1*2##").arg(getAddress()));
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
	dev->sendFrame(QString("*#1*%1*#2*%2##").arg(getAddress()).arg(tempo));
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
	dev->sendInit("*#1*" + getAddress() + "*2##");
}

// Chiede lo stato dell'attuatore con una frame vecchia
void attuatAutomTempNuovoF::chiedi_stato()
{
	emit richStato("*#1*" + getAddress() + "##");
}

void attuatAutomTempNuovoF::SetIcons(QString i1, QString i2, QString i3)
{
	qDebug("attuatAutomTempNuovoF::SetIcons");

	int pos = i2.indexOf(".");
	if (pos != -1)
		Icon[0] = bt_global::icons_cache.getIcon(i2.left(pos) + "off.png");
	else
	{
		qWarning() << "Cannot find dot on image " << i2;
		Icon[0] = bt_global::icons_cache.getIcon(ICON_VUOTO);
	}

	if (pos != -1)
		Icon[1] = bt_global::icons_cache.getIcon(i2.left(pos) + "on.png");
	else
	{
		qWarning() << "Cannot find dot on image " << i2;
		Icon[1] = bt_global::icons_cache.getIcon(ICON_VUOTO);
	}

	Icon[2] = bt_global::icons_cache.getIcon(i3);

	QString pressIconName = getPressName(i3);
	pressIcon[2] = bt_global::icons_cache.getIcon(pressIconName);

	pos = i1.indexOf(".");
	for (int i = 0; i < NTIMEICONS; i++)
	{
		if (pos != -1)
			Icon[3 + i] = bt_global::icons_cache.getIcon(i1.left(pos) + QString::number(i) + ".png");
		else
		{
			qWarning() << "Cannot find dot on image " << i2;
			Icon[3 + i] = bt_global::icons_cache.getIcon(ICON_VUOTO);
		}
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
		BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		BannerText->setFont(bt_global::font->get(FontManager::TEXT));
		BannerText->setText(qtesto);
	}
	if (SecondaryText)
	{
		SecondaryText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		SecondaryText->setFont(bt_global::font->get(FontManager::TEXT));
		SecondaryText->setText(qtestoSecondario);
	}
}
#endif
