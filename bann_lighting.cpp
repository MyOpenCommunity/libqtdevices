#include "bann_lighting.h"
#include "btbutton.h"
#include "fontmanager.h" // bt_global::font
#include "devices_cache.h" // bt_global::devices_cache
#include "generic_functions.h" //getBostikName
#include "icondispatcher.h" //bt_global::icons_cache
#include "skinmanager.h" // skincontext
#include "xml_functions.h" // getTextChild
#include "lighting_device.h"


#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QDomNode>

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
	connect(this, SIGNAL(center_left_clicked()), SLOT(increaseLevel()));
	connect(this, SIGNAL(center_right_clicked()), SLOT(decreaseLevel()));
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
	request_timer.setSingleShot(true);
	connect(&request_timer, SIGNAL(timeout()), SLOT(requestStatus()));

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
				request_timer.stop();
			}
		}
			break;
		case LightingDevice::DIM_VARIABLE_TIMING:
		{
			BtTime t = it.value().value<BtTime>();
			// all 0's means either the light is on or timer is stopped anyway, so stop request timer
			if ((t.hour() == 0) && (t.minute() == 0) && (t.second() == 0))
			{
				request_timer.stop();
				break;
			}
			// ignore strange frames (taken from old code)
			if ((t.hour() == 255) && (t.minute() == 255) && (t.second() == 255))
				break;
			// convert t to seconds, then compute the number of slices
			int time = qRound((t.hour() * 3600 + t.minute() * 60 + t.second()) * TLF_TIME_STATES / total_time);
			setElapsedTime(time);
			setState(ON);
			request_timer.start();
		}
			break;
		}
		++it;
	}
}
