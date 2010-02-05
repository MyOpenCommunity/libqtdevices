#include "scenevodevicescond.h"
#include "device_status.h"
#include "probe_device.h" // NonControlledProbeDevice
#include "devices_cache.h" // add_device_to_cache
#include "lighting_device.h"
#include "scaleconversion.h"
#include "fontmanager.h" // bt_global::font

#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QDebug>

// The language used for the floating point number
static QLocale loc(QLocale::Italian);

#define CONDITION_MIN_TEMP 1050
#define CONDITION_MAX_TEMP  500


DeviceConditionDisplay::DeviceConditionDisplay(QWidget *parent) : QWidget(parent)
{
	label = new QLabel;
	label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	label->setFont(bt_global::font->get(FontManager::TEXT));
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addWidget(label);
}


void DeviceConditionDisplayOnOff::updateText(int min_condition_value, int max_condition_value)
{
	Q_UNUSED(max_condition_value)
	label->setText(min_condition_value ? tr("ON") : tr("OFF"));
}


void DeviceConditionDisplayDimming::updateText(int min_condition_value, int max_condition_value)
{
	if (min_condition_value == 0)
		label->setText(tr("OFF"));
	else
		label->setText(QString("%1% - %3%").arg(min_condition_value).arg(max_condition_value));
}


void DeviceConditionDisplayVolume::updateText(int min_condition_value, int max_condition_value)
{
	if (min_condition_value == -1)
		label->setText(tr("OFF"));
	else if (min_condition_value == 0 && max_condition_value == 31)
		label->setText(tr("ON"));
	else
	{
		int val_min = min_condition_value;
		int val_max = max_condition_value;
		int vmin = (val_min == 0 ? 0 : (10 * (val_min <= 15 ? val_min/3 : (val_min-1)/3) + 1));
		int vmax = 10 * (val_max <= 15 ? val_max/3 : (val_max-1)/3);
		label->setText(QString("%1% - %3%").arg(vmin).arg(vmax));
	}
}


void DeviceConditionDisplayTemperature::updateText(int min_condition_value, int max_condition_value)
{
	Q_UNUSED(max_condition_value)
	QString tmp = loc.toString(min_condition_value / 10.0, 'f', 1);
	TemperatureScale temp_scale = static_cast<TemperatureScale>(bt_global::config[TEMPERATURE_SCALE].toInt());

	switch (temp_scale)
	{
	case CELSIUS:
		tmp += TEMP_DEGREES"C \2611"TEMP_DEGREES"C";
		break;
	case FAHRENHEIT:
		tmp += TEMP_DEGREES"F \2611"TEMP_DEGREES"F";
		break;
	default:
		qWarning("Wrong temperature scale, defaulting to celsius");
		tmp += TEMP_DEGREES"C \2611"TEMP_DEGREES"C";
	}
	label->setText(tmp);
}


DeviceCondition::DeviceCondition()
{
	satisfied = false;
}

int DeviceCondition::get_min()
{
	return 0;
}

int DeviceCondition::get_max()
{
	return 0;
}

int DeviceCondition::get_step()
{
	return 1;
}

int DeviceCondition::get_divisor()
{
	return 1;
}

int DeviceCondition::get_condition_value()
{
	return cond_value;
}

void DeviceCondition::set_condition_value(int v)
{
	if (v > get_max())
		v = get_max();
	if (v < get_min())
		v = get_min();
	cond_value = v;
}

void DeviceCondition::set_condition_value(QString s)
{
	set_condition_value(s.toInt());
}

void DeviceCondition::get_condition_value(QString& out)
{
	char tmp[100];
	sprintf(tmp, "%d", get_condition_value());
	out = tmp;
}

void DeviceCondition::setGeometry(int x, int y, int sx, int sy)
{
	frame->setGeometry(x, y, sx, sy);
}

void DeviceCondition::Up()
{
	int val = get_current_value();
	val += get_step();
	set_current_value(val);
	qDebug("val = %d", get_current_value());
	Draw();
}

void DeviceCondition::Down()
{
	int val = get_current_value();
	val -= get_step();
	set_current_value(val);
	Draw();
}

void DeviceCondition::OK()
{
	set_condition_value(get_current_value());
}

int DeviceCondition::get_current_value()
{
	return current_value;
}

int DeviceCondition::set_current_value(int v)
{
	if (v > get_max())
		v = get_max();
	if (v < get_min())
		v = get_min();
	current_value = v;
	return current_value;
}

QString DeviceCondition::get_unit()
{
	return "";
}

void DeviceCondition::inizializza()
{
}

void DeviceCondition::reset()
{
	set_current_value(get_condition_value());
	Draw();
}

bool DeviceCondition::isTrue()
{
	return satisfied;
}


DeviceConditionLight::DeviceConditionLight(QWidget *parent, QString trigger, QString where)
{
	condition_display = new DeviceConditionDisplayOnOff(parent);
	set_condition_value(trigger);
	set_current_value(DeviceCondition::get_condition_value());
	dev = bt_global::add_device_to_cache(new LightingDevice(where, PULL));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
	Draw();
}

void DeviceConditionLight::inizializza()
{
	dev->requestStatus();
}

void DeviceConditionLight::Draw()
{
	condition_display->updateText(get_current_value(), get_current_value());
}

void DeviceConditionLight::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case LightingDevice::DIM_DEVICE_ON:
			if (DeviceCondition::get_condition_value() == static_cast<int>(it.value().toBool()))
			{
				if (!satisfied)
				{
					satisfied = true;
					emit condSatisfied();
				}
			}
			else
				satisfied = false;
			break;
		}
		++it;
	}
}

int DeviceConditionLight::get_max()
{
	return 1;
}

void DeviceConditionLight::set_condition_value(QString s)
{
	int v = 0;
	if (s == "1")
		v = 1;
	else if (s == "0")
		v = 0;
	else
		qDebug() << "Unknown condition value " << s << " for device_condition_light_status";
	DeviceCondition::set_condition_value(v);
}

void DeviceConditionLight::get_condition_value(QString& out)
{
	out = DeviceCondition::get_condition_value() ? "1" : "0";
}

void DeviceConditionLight::setGeometry(int x, int y, int sx, int sy)
{
	condition_display->setGeometry(x, y, sx, sy);
}


DeviceConditionDimming::DeviceConditionDimming(QWidget *parent, QString trigger, QString where)
{
	condition_display = new DeviceConditionDisplayDimming(parent);

	if (trigger == "0")
	{
		set_condition_value_min(0);
		set_condition_value_max(0);
	}
	else
	{
		QStringList parts = trigger.split('-');
		Q_ASSERT_X(parts.size() == 2, "DeviceConditionDimming::DeviceConditionDimming",
			"the trigger value must have 0 or 2 digit with - as separator");
		set_condition_value_min(parts[0].toInt());
		set_condition_value_max(parts[1].toInt());
	}

	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());

	// TODO: to PULL or not to PULL? That is the question...
	dev = bt_global::add_device_to_cache(new DimmerDevice(where, PULL));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
	Draw();
}

void DeviceConditionDimming::inizializza()
{
	dev->requestStatus();
}

QString DeviceConditionDimming::get_current_value()
{
	return QString::number(get_condition_value_min());
}

int DeviceConditionDimming::get_min()
{
	return 0;
}

int DeviceConditionDimming::get_max()
{
	return 100;
}

int DeviceConditionDimming::get_step()
{
	return 10;
}

void DeviceConditionDimming::Up()
{
	int val = get_current_value_min();
	switch (val)
	{
	case 0:
		set_current_value_min(2);
		set_current_value_max(4);
		break;
	case 2:
		set_current_value_min(5);
		set_current_value_max(7);
		break;
	case 5:
		set_current_value_min(8);
		set_current_value_max(10);
		break;
	default:
		break;
	}
	Draw();
}

void DeviceConditionDimming::Down()
{
	int val = get_current_value_min();
	switch (val)
	{
	case 2:
		set_current_value_min(0);
		set_current_value_max(0);
		break;
	case 5:
		set_current_value_min(2);
		set_current_value_max(4);
		break;
	case 8:
		set_current_value_min(5);
		set_current_value_max(7);
		break;
	default:
		break;
	}
	Draw();
}

void DeviceConditionDimming::Draw()
{
	condition_display->updateText(get_current_value_min()*10, get_current_value_max()*10);
}

void DeviceConditionDimming::OK()
{
	set_condition_value_min(get_current_value_min());
	set_condition_value_max(get_current_value_max());
}

void DeviceConditionDimming::reset()
{
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	Draw();
}

void DeviceConditionDimming::set_condition_value_min(int s)
{
	min_val = s;
}

int DeviceConditionDimming::get_condition_value_min()
{
	return min_val;
}

void DeviceConditionDimming::set_condition_value_max(int s)
{
	max_val = s;
}

int DeviceConditionDimming::get_condition_value_max()
{
	return max_val;
}

int DeviceConditionDimming::get_current_value_min()
{
	return current_value_min;
}

void DeviceConditionDimming::set_current_value_min(int min)
{
	current_value_min = min;
}

int DeviceConditionDimming::get_current_value_max()
{
	return current_value_max;
}

void DeviceConditionDimming::set_current_value_max(int max)
{
	current_value_max = max;
}

void DeviceConditionDimming::set_condition_value(QString s)
{
	DeviceCondition::set_condition_value(s.toInt() * 10);
}

void DeviceConditionDimming::get_condition_value(QString& out)
{
	char tmp[100];
	if (get_condition_value_min() == 0)
		sprintf(tmp, "0");
	else
		sprintf(tmp, "%d-%d", get_condition_value_min(), get_condition_value_max());
	out =  tmp;
}

void DeviceConditionDimming::setGeometry(int x, int y, int sx, int sy)
{
	condition_display->setGeometry(x, y, sx, sy);
}

void DeviceConditionDimming::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	int trig_min = get_condition_value_min();
	int trig_max = get_condition_value_max();

	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		// TODO: what about on level? is this code ok?
		case LightingDevice::DIM_DEVICE_ON:
		case LightingDevice::DIM_DIMMER_LEVEL:
		{
			int level = it.value().toInt() / 10;
			if (level >= trig_min && level <= trig_max)
			{
				if (!satisfied)
				{
					satisfied = true;
					emit condSatisfied();
				}
			}
			else
				satisfied = false;
		}
			break;
		}
		++it;
	}
}


DeviceConditionDimming100::DeviceConditionDimming100(QWidget *parent, QString trigger, QString where)
{
	condition_display = new DeviceConditionDisplayDimming(parent);
	if (trigger == "0")
	{
		set_condition_value_min(0);
		set_condition_value_max(0);
	}
	else
	{
		QStringList parts = trigger.split('-');
		Q_ASSERT_X(parts.size() == 2, "DeviceConditionDimming100::DeviceConditionDimming100",
			"the trigger value must have 0 or 2 digit with - as separator");
		set_condition_value_min(parts[0].toInt());
		set_condition_value_max(parts[1].toInt());
	}
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	dev = bt_global::add_device_to_cache(new Dimmer100Device(where, PULL));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
	Draw();
}

void DeviceConditionDimming100::setGeometry(int x, int y, int sx, int sy)
{
	condition_display->setGeometry(x, y, sx, sy);
}

void DeviceConditionDimming100::inizializza()
{
	dev->requestStatus();
}

QString DeviceConditionDimming100::get_current_value()
{
	return QString::number(get_condition_value_min());
}

int DeviceConditionDimming100::get_min()
{
	return 0;
}

int DeviceConditionDimming100::get_max()
{
	return 100;
}

int DeviceConditionDimming100::get_step()
{
	return 10;
}

void DeviceConditionDimming100::Up()
{
	int val = get_current_value_min();
	switch (val)
	{
	case 0:
		set_current_value_min(1);
		set_current_value_max(20);
		break;
	case 1:
		set_current_value_min(21);
		set_current_value_max(40);
		break;
	case 21:
		set_current_value_min(41);
		set_current_value_max(70);
		break;
	case 41:
		set_current_value_min(71);
		set_current_value_max(100);
		break;
	default:
		break;
	}
	Draw();
}

void DeviceConditionDimming100::Down()
{
	int val = get_current_value_min();
	switch (val)
	{
	case 1:
		set_current_value_min(0);
		set_current_value_max(0);
		break;
	case 21:
		set_current_value_min(1);
		set_current_value_max(20);
		break;
	case 41:
		set_current_value_min(21);
		set_current_value_max(40);
		break;
	case 71:
		set_current_value_min(41);
		set_current_value_max(70);
		break;
	default:
		break;
	}
	Draw();
}

void DeviceConditionDimming100::Draw()
{
	condition_display->updateText(get_current_value_min(), get_current_value_max());
}

void DeviceConditionDimming100::OK()
{
	set_condition_value_min(get_current_value_min());
	set_condition_value_max(get_current_value_max());
}

void DeviceConditionDimming100::reset()
{
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	Draw();
}


void DeviceConditionDimming100::set_condition_value_min(int s)
{
	min_val = s;
}

int DeviceConditionDimming100::get_condition_value_min()
{
	return min_val;
}

void DeviceConditionDimming100::set_condition_value_max(int s)
{
	max_val = s;
}

int DeviceConditionDimming100::get_condition_value_max()
{
	return max_val;
}

int DeviceConditionDimming100::get_current_value_min()
{
	return current_value_min;
}

void DeviceConditionDimming100::set_current_value_min(int min)
{
	current_value_min = min;
}

int DeviceConditionDimming100::get_current_value_max()
{
	return current_value_max;
}

void DeviceConditionDimming100::set_current_value_max(int max)
{
	current_value_max = max;
}

void DeviceConditionDimming100::set_condition_value(QString s)
{
	DeviceCondition::set_condition_value(s.toInt() * 10);
}

void DeviceConditionDimming100::get_condition_value(QString& out)
{
	char tmp[100];
	if (get_condition_value_min() == 0)
		sprintf(tmp, "0");
	else
		sprintf(tmp, "%d-%d", get_condition_value_min(), get_condition_value_max());
	out =  tmp;
}

void DeviceConditionDimming100::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	int trig_min = get_condition_value_min();
	int trig_max = get_condition_value_max();

	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		// TODO: previous code ignored dim_dimmer_level. What should I do?
		case LightingDevice::DIM_DIMMER100_LEVEL:
		{
			int level = it.value().toInt();
			if (level >= trig_min && level <= trig_max)
			{
				if (!satisfied)
				{
					satisfied = true;
					emit condSatisfied();
				}
			}
			else
				satisfied = false;
		}
			break;
		}
		++it;
	}
}


DeviceConditionVolume::DeviceConditionVolume(QWidget *parent, QString trigger, QString where)
{
	condition_display = new DeviceConditionDisplayVolume(parent);
	if (trigger == "-1")
	{
		set_condition_value_min(-1);
		set_condition_value_max(-1);
	}
	else
	{
		QStringList parts = trigger.split('-');
		Q_ASSERT_X(parts.size() == 2, "DeviceConditionVolume::DeviceConditionVolume",
			"the trigger value must have 0 or 2 digit with - as separator");
		set_condition_value_min(parts[0].toInt());
		set_condition_value_max(parts[1].toInt());
	}

	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	dev = bt_global::add_device_to_cache(new sound_device(QString(where)));
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
		this, SLOT(status_changed(QList<device_status*>)));
	Draw();
}

void DeviceConditionVolume::setGeometry(int x, int y, int sx, int sy)
{
	condition_display->setGeometry(x, y, sx, sy);
}

void DeviceConditionVolume::inizializza()
{
	dev->init();
}

void DeviceConditionVolume::set_condition_value_min(int s)
{
	min_val = s;
}

int DeviceConditionVolume::get_condition_value_min()
{
	return min_val;
}

void DeviceConditionVolume::set_condition_value_max(int s)
{
	max_val = s;
}

int DeviceConditionVolume::get_condition_value_max()
{
	return max_val;
}

int DeviceConditionVolume::get_current_value_min()
{
	return current_value_min;
}

void DeviceConditionVolume::set_current_value_min(int min)
{
	current_value_min = min;
}

int DeviceConditionVolume::get_current_value_max()
{
	return current_value_max;
}

void DeviceConditionVolume::set_current_value_max(int max)
{
	current_value_max = max;
}

int DeviceConditionVolume::get_min()
{
	return 0;
}

int DeviceConditionVolume::get_max()
{
	return 31;
}

void DeviceConditionVolume::set_condition_value(QString s)
{
	int v = s.toInt();
	DeviceCondition::set_condition_value(v);
}

void DeviceConditionVolume::get_condition_value(QString& out)
{
	char tmp[100];
	if (get_condition_value_min() == -1)
		sprintf(tmp, "-1");
	else
		sprintf(tmp, "%d-%d", get_condition_value_min(), get_condition_value_max());
	out =  tmp;
}

void DeviceConditionVolume::Up()
{
	int v_m = get_current_value_min();
	int v_M = get_current_value_max();
	qDebug("v_m = %d - v_M = %d", v_m, v_M);
	if (v_m == -1)
	{
		v_m = 0;
		v_M = 31;
	}
	else if (v_m == 0)
	{
		if (v_M == 31)
			v_M = 6;
		else
		{
			v_m = 7;
			v_M = 12;
		}
	}
	else if (v_m == 7)
	{
		v_m = 13;
		v_M = 22;
	}
	else
	{
		v_m = 23;
		v_M = 31;
	}
	qDebug("new value m = %d - M = %d", v_m, v_M);
	set_current_value_min(v_m);
	set_current_value_max(v_M);
	Draw();
}

void DeviceConditionVolume::Down()
{
	int v_m = get_current_value_min();
	int v_M = get_current_value_max();
	qDebug("v_m = %d - v_M = %d", v_m, v_M);
	if (v_m == 23)
	{
		v_m = 13;
		v_M = 22;
	}
	else if (v_m == 13)
	{
		v_m = 7;
		v_M = 12;
	}
	else if (v_m == 7)
	{
		v_m = 0;
		v_M = 6;
	}
	else if (v_m == 0)
	{
		if (v_M == 6)
			v_M = 31;
		else
		{
			v_m = -1;
			v_M = -1;
		}
	}
	qDebug("new value m = %d - M = %d", v_m, v_M);
	set_current_value_min(v_m);
	set_current_value_max(v_M);
	Draw();
}

void DeviceConditionVolume::OK()
{
	set_condition_value_min(get_current_value_min());
	set_condition_value_max(get_current_value_max());
}

void DeviceConditionVolume::Draw()
{
	condition_display->updateText(get_current_value_min(), get_current_value_max());
}

void DeviceConditionVolume::status_changed(QList<device_status*> sl)
{
	int trig_v_min = get_condition_value_min();
	int trig_v_max = get_condition_value_max();
	stat_var curr_volume(stat_var::AUDIO_LEVEL);
	stat_var curr_stato(stat_var::ON_OFF);

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::AMPLIFIER:
			qDebug("Amplifier status change");
			qDebug("Confition value_min = %d - value_max = %d", trig_v_min, trig_v_max);
			ds->read(device_status_amplifier::AUDIO_LEVEL_INDEX, curr_volume);
			ds->read(device_status_amplifier::ON_OFF_INDEX, curr_stato);
			qDebug("volume = %d - stato = %d", curr_volume.get_val(), curr_stato.get_val());
			if ((trig_v_min == -1) && (curr_stato.get_val() == 0))
			{
				qDebug("Condition triggered");
				if (!satisfied)
				{
					satisfied = true;
					emit condSatisfied();
				}
			}
			else if ((curr_stato.get_val() == 1) && (curr_volume.get_val() >= trig_v_min) && (curr_volume.get_val() <= trig_v_max))
			{
				qDebug("Condition triggered");
				if (!satisfied)
				{
					satisfied = true;
					emit condSatisfied();
				}
			}
			else
			{
			qDebug("Condition not triggered");
			satisfied = false;
			}
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}
}

void DeviceConditionVolume::reset()
{
	qDebug("device_condition_volume::reset()");
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	Draw();
}


DeviceConditionTemperature::DeviceConditionTemperature(QWidget *parent, QString trigger, QString where, bool external)
{
	condition_display = new DeviceConditionDisplayTemperature(parent);

	// Temp condition is expressed in bticino format
	int temp_condition = trigger.toInt();

	temp_scale = static_cast<TemperatureScale>(bt_global::config[TEMPERATURE_SCALE].toInt());
	switch (temp_scale)
	{
	case CELSIUS:
		max_temp = bt2Celsius(CONDITION_MAX_TEMP);
		min_temp = bt2Celsius(CONDITION_MIN_TEMP);
		set_condition_value(bt2Celsius(temp_condition));
		break;
	case FAHRENHEIT:
		max_temp = bt2Fahrenheit(CONDITION_MAX_TEMP);
		min_temp = bt2Fahrenheit(CONDITION_MIN_TEMP);
		set_condition_value(bt2Fahrenheit(temp_condition));
		break;
	default:
		qWarning("Wrong temperature scale, defaulting to celsius");
		temp_scale = CELSIUS;
		max_temp = bt2Celsius(CONDITION_MAX_TEMP);
		min_temp = bt2Celsius(CONDITION_MIN_TEMP);
		set_condition_value(bt2Celsius(temp_condition));
	}
	step = 1;

	// The condition value and the current value are stored in Celsius or Fahrenheit
	set_current_value(DeviceCondition::get_condition_value());
	dev = bt_global::add_device_to_cache(new NonControlledProbeDevice(where,
		external ? NonControlledProbeDevice::EXTERNAL : NonControlledProbeDevice::INTERNAL));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
	Draw();
}

int DeviceConditionTemperature::get_min()
{
	return min_temp;
}

int DeviceConditionTemperature::get_max()
{
	return max_temp;
}

int DeviceConditionTemperature::get_step()
{
	return step;
}

int DeviceConditionTemperature::get_divisor()
{
	return 10;
}

void DeviceConditionTemperature::Draw()
{
	condition_display->updateText(get_current_value(), get_current_value());
}

void DeviceConditionTemperature::get_condition_value(QString& out)
{
	// transform an int value to a string in bticino 4-digit form
	int val = DeviceCondition::get_condition_value();
	int temp;
	switch (temp_scale)
	{
	case CELSIUS:
		temp = celsius2Bt(val);
		break;
	case FAHRENHEIT:
		temp = fahrenheit2Bt(val);
		break;
	default:
		qWarning("Wrong temperature scale, defaulting to celsius");
		temp = celsius2Bt(val);
	}
	// bticino_temp is stored in 4 digit format.
	out = QString("%1").arg(temp, 4, 10, QChar('0'));
}

void DeviceConditionTemperature::inizializza()
{
	dev->requestStatus();
}

void DeviceConditionTemperature::status_changed(const StatusList &sl)
{
	if (!sl.contains(NonControlledProbeDevice::DIM_TEMPERATURE))
		return;

	// get_condition_value() returns an int, which is Celsius or Fahrenheit
	int trig_v = DeviceCondition::get_condition_value();
	int temp = sl[NonControlledProbeDevice::DIM_TEMPERATURE].toInt();

	qDebug("Temperature changed");
	qDebug("Current temperature %d", temp);
	int measured_temp;
	switch (temp_scale)
	{
	case CELSIUS:
		measured_temp = bt2Celsius(temp);
		break;
	case FAHRENHEIT:
		measured_temp = bt2Fahrenheit(temp);
		break;
	default:
		qWarning("Wrong temperature scale, defaulting to celsius");
		measured_temp = bt2Celsius(temp);
	}

	if (measured_temp >= (trig_v - 10) && measured_temp <= (trig_v + 10))
	{
		qDebug("Condition triggered");
		if (!satisfied)
		{
			satisfied = true;
			emit condSatisfied();
		}
	}
	else
	{
		qDebug("Condition not triggered");
		satisfied = false;
	}
}

void DeviceConditionTemperature::setGeometry(int x, int y, int sx, int sy)
{
	condition_display->setGeometry(x, y, sx, sy);
}


DeviceConditionAux::DeviceConditionAux(QWidget *parent, QString trigger, QString where) :
	device_initialized(false), device_value(-1)
{
	condition_display = new DeviceConditionDisplayOnOff(parent);

	set_condition_value(trigger);
	set_current_value(DeviceCondition::get_condition_value());
	dev = bt_global::add_device_to_cache(new aux_device(where));
	connect(dev, SIGNAL(status_changed(stat_var)), SLOT(status_changed(stat_var)));
	Draw();
}

void DeviceConditionAux::inizializza()
{
	dev->init();
}

void DeviceConditionAux::Draw()
{
	condition_display->updateText(get_current_value(), get_current_value());
}

void DeviceConditionAux::check_condition(bool emit_signal)
{
	int trig_v = DeviceCondition::get_condition_value();
	if (trig_v == device_value)
	{
		qDebug("aux condition (%d) satisfied", trig_v);
		if (!satisfied)
		{
			satisfied = true;
			if (emit_signal)
				emit condSatisfied();
		}
	}
	else
	{
		qDebug("aux condition (%d) NOT satisfied", trig_v);
		satisfied = false;
	}
}

void DeviceConditionAux::status_changed(stat_var status)
{
	device_value = status.get_val();
	// We emit signal condSatisfied only if the device is initialized.
	check_condition(device_initialized);
	device_initialized = true;
}

int DeviceConditionAux::get_max()
{
	return 1;
}

void DeviceConditionAux::set_condition_value(QString s)
{
	int v = 0;
	if (s == "1")
		v = 1;
	else if (s == "0")
		v = 0;
	else
		qDebug() << "Unknown condition value " << s << " for device_condition_aux";
	DeviceCondition::set_condition_value(v);
	check_condition(false);
}

void DeviceConditionAux::OK()
{
	DeviceCondition::OK();
	check_condition(false);
}

void DeviceConditionAux::setGeometry(int x, int y, int sx, int sy)
{
	condition_display->setGeometry(x, y, sx, sy);
}

