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


DeviceConditionDisplayOnOff::DeviceConditionDisplayOnOff(QWidget *parent) : QWidget(parent)
{
	label = new QLabel;
	label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	label->setFont(bt_global::font->get(FontManager::TEXT));
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addWidget(label);
}

void DeviceConditionDisplayOnOff::updateText(int new_value)
{
	label->setText(new_value ? tr("ON") : tr("OFF"));
}


/*****************************************************************
 ** Actual generic device condition
****************************************************************/
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

bool DeviceCondition::show_OFF_on_zero()
{
	return false;
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
	qDebug() << "DeviceCondition::set_condition_value (" << s << ")";
	set_condition_value(s.toInt());
}

void DeviceCondition::get_condition_value(QString& out)
{
	qDebug("DeviceCondition::get_condition_value(QString&)");
	char tmp[100];
	sprintf(tmp, "%d", get_condition_value());
	out = tmp;
}

void DeviceCondition::Draw()
{
	QString tmp;
	int v = get_current_value();
	if (show_OFF_on_zero() && !v)
		tmp = tr("OFF");
	else
		tmp = QString("%1%2").arg(v).arg(get_unit());
	((QLabel *)frame)->setText(tmp);
}

void DeviceCondition::setGeometry(int x, int y, int sx, int sy)
{
	frame->setGeometry(x, y, sx, sy);
}

void DeviceCondition::Up()
{
	qDebug("DeviceCondition::Up()");
	int val = get_current_value();
	val += get_step();
	set_current_value(val);
	qDebug("val = %d", get_current_value());
	Draw();
}

void DeviceCondition::Down()
{
	qDebug("DeviceCondition::Down()");
	int val = get_current_value();
	val -= get_step();
	set_current_value(val);
	Draw();
}

void DeviceCondition::OK()
{
	qDebug("DeviceCondition::OK()");
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
	qDebug("DeviceCondition::reset()");
	set_current_value(get_condition_value());
	Draw();
}

bool DeviceCondition::isTrue()
{
	return satisfied;
}


/*****************************************************************
** Actual light status device condition
****************************************************************/
device_condition_light_status::device_condition_light_status(QWidget *parent, QString trigger, QString where)
{
	condition_display = new DeviceConditionDisplayOnOff(parent);
	set_condition_value(trigger);
	set_current_value(DeviceCondition::get_condition_value());
	dev = bt_global::add_device_to_cache(new LightingDevice(where, PULL));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
	Draw();
}

void device_condition_light_status::inizializza()
{
	dev->requestStatus();
}

void device_condition_light_status::Draw()
{
	condition_display->updateText(get_current_value());
}

void device_condition_light_status::status_changed(const StatusList &sl)
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

int device_condition_light_status::get_max()
{
	return 1;
}

void device_condition_light_status::set_condition_value(QString s)
{
	qDebug("device_condition_light_status::set_condition_value");
	int v = 0;
	if (s == "1")
		v = 1;
	else if (s == "0")
		v = 0;
	else
		qDebug() << "Unknown condition value " << s << " for device_condition_light_status";
	DeviceCondition::set_condition_value(v);
}

void device_condition_light_status::get_condition_value(QString& out)
{
	out = DeviceCondition::get_condition_value() ? "1" : "0";
}

void device_condition_light_status::setGeometry(int x, int y, int sx, int sy)
{
	condition_display->setGeometry(x, y, sx, sy);
}

/*****************************************************************
** Actual dimming value device condition
****************************************************************/
device_condition_dimming::device_condition_dimming(QWidget *parent, QString trigger, QString where)
{
	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	l->setFont(bt_global::font->get(FontManager::TEXT));

	frame = l;
	if (trigger == "0")
	{
		set_condition_value_min(0);
		set_condition_value_max(0);
	}
	else
	{
		set_condition_value_min((QString) trigger.at(0));
		QByteArray buf = trigger.toAscii();
		set_condition_value_max(buf.constData()+2);
	}
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	// TODO: to PULL or not to PULL? That is the question...
	dev = bt_global::add_device_to_cache(new DimmerDevice(where, PULL));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
	Draw();
}

void device_condition_dimming::inizializza()
{
	dev->requestStatus();
}

QString device_condition_dimming::get_current_value()
{
	char val[50];
	int val_m = get_condition_value_min();
	sprintf(val, "%d", val_m);
	return val;
}

int device_condition_dimming::get_min()
{
	return 0;
}

int device_condition_dimming::get_max()
{
	return 100;
}

int device_condition_dimming::get_step()
{
	return 10;
}

void device_condition_dimming::Up()
{
	qDebug("device_condition_dimming::Up()");

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

void device_condition_dimming::Down()
{
	qDebug("device_condition_dimming::Down()");
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

void device_condition_dimming::Draw()
{
	QString tmp;
	qDebug("device_condition_dimming::Draw()");
	int v = get_current_value_min();
	if (!v)
		tmp = tr("OFF");
	else
	{
		QString u = get_unit();
		int M = get_current_value_max();
		tmp = QString("%1%2 - %3%4").arg(v*10).arg(u).arg(M*10).arg(u);
	}
	((QLabel *)frame)->setText(tmp);
}

void device_condition_dimming::OK()
{
	qDebug("device_condition_dimming::OK()");
	set_condition_value_min(get_current_value_min());
	set_condition_value_max(get_current_value_max());
}

void device_condition_dimming::reset()
{
	qDebug("device_condition_dimming::reset()");
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	Draw();
}

QString device_condition_dimming::get_unit()
{
	return "%";
}

bool device_condition_dimming::show_OFF_on_zero()
{
	return true;
}

void device_condition_dimming::set_condition_value_min(int s)
{
	qDebug("device_condition_dimming::set_condition_value_min(%d)", s);
	min_val = s;
}

void device_condition_dimming::set_condition_value_min(QString s)
{
	qDebug() << "device_condition_dimming::set_condition_value_min(" << s << ")";
	min_val = s.toInt();
}

int device_condition_dimming::get_condition_value_min()
{
	return min_val;
}

void device_condition_dimming::set_condition_value_max(int s)
{
	qDebug("device_condition_dimming::set_condition_value_max(%d)", s);
	max_val = s;
}

void device_condition_dimming::set_condition_value_max(QString s)
{
	qDebug() << "device_condition_dimming::set_condition_value_max(" << s << ")";
	max_val = s.toInt();
}

int device_condition_dimming::get_condition_value_max()
{
	return max_val;
}

int device_condition_dimming::get_current_value_min()
{
	return current_value_min;
}

void device_condition_dimming::set_current_value_min(int min)
{
	current_value_min = min;
}

int device_condition_dimming::get_current_value_max()
{
	return current_value_max;
}

void device_condition_dimming::set_current_value_max(int max)
{
	current_value_max = max;
}

void device_condition_dimming::set_condition_value(QString s)
{
	DeviceCondition::set_condition_value(s.toInt() * 10);
}

void device_condition_dimming::get_condition_value(QString& out)
{
	char tmp[100];
	qDebug("device_condition_dimming::get_condition_value()");
	if (get_condition_value_min() == 0)
		sprintf(tmp, "0");
	else
		sprintf(tmp, "%d-%d", get_condition_value_min(), get_condition_value_max());
	out =  tmp;
}

void device_condition_dimming::status_changed(const StatusList &sl)
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


/*****************************************************************
 ** Actual dimming 100 value device condition
****************************************************************/
device_condition_dimming_100::device_condition_dimming_100(QWidget *parent, QString trigger, QString where)
{
	char sup[10];
	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	l->setFont(bt_global::font->get(FontManager::TEXT));
	frame = l;
	if (trigger == "0")
	{
		set_condition_value_min(0);
		set_condition_value_max(0);
	}
	else
	{
		QByteArray buf = trigger.toAscii();
		sprintf(sup, "%s", buf.constData());
		strtok(sup, "-");
		set_condition_value_min(sup);
		sprintf(sup, "%s", strchr(buf.constData(), '-')+1);
		set_condition_value_max(sup);
	}
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	dev = bt_global::add_device_to_cache(new Dimmer100Device(where, PULL));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
	Draw();
}

void device_condition_dimming_100::inizializza()
{
	dev->requestStatus();
}

QString device_condition_dimming_100::get_current_value()
{
	char val[50];
	int val_m = get_condition_value_min();
	sprintf(val, "%d", val_m);
	return val;
}

int device_condition_dimming_100::get_min()
{
	return 0;
}

int device_condition_dimming_100::get_max()
{
	return 100;
}

int device_condition_dimming_100::get_step()
{
	return 10;
}

void device_condition_dimming_100::Up()
{
	qDebug("device_condition_dimming_100::Up()");

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

void device_condition_dimming_100::Down()
{
	qDebug("device_condition_dimming_100::Down()");
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

void device_condition_dimming_100::Draw()
{
	QString tmp;
	qDebug("device_condition_dimming_100::Draw()");
	QString u = get_unit();
	int v = get_current_value_min();
	if (!v)
		tmp = tr("OFF");
	else
	{
		int M = get_current_value_max();
		tmp = QString("%1%2 - %3%4").arg(v).arg(u).arg(M).arg(u);
	}
	((QLabel *)frame)->setText(tmp);
}

void device_condition_dimming_100::OK()
{
	qDebug("device_condition_dimming_100::OK()");
	set_condition_value_min(get_current_value_min());
	set_condition_value_max(get_current_value_max());
}

void device_condition_dimming_100::reset()
{
	qDebug("device_condition_dimming_100::reset()");
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	Draw();
}

QString device_condition_dimming_100::get_unit()
{
	return "%";
}

bool device_condition_dimming_100::show_OFF_on_zero()
{
	return true;
}

void device_condition_dimming_100::set_condition_value_min(int s)
{
	qDebug("device_condition_dimming_100::set_condition_value_min(%d)", s);
	min_val = s;
}

void device_condition_dimming_100::set_condition_value_min(QString s)
{
	qDebug() << "device_condition_dimming_100::set_condition_value_min(" << s << ")";
	min_val = s.toInt();
}

int device_condition_dimming_100::get_condition_value_min()
{
	return min_val;
}

void device_condition_dimming_100::set_condition_value_max(int s)
{
	qDebug("device_condition_dimming_100::set_condition_value_max(%d)", s);
	max_val = s;
}

void device_condition_dimming_100::set_condition_value_max(QString s)
{
	qDebug() << "device_condition_dimming_100::set_condition_value_max(" << s << ")";
	max_val = s.toInt();
}

int device_condition_dimming_100::get_condition_value_max()
{
	return max_val;
}

int device_condition_dimming_100::get_current_value_min()
{
	return current_value_min;
}

void device_condition_dimming_100::set_current_value_min(int min)
{
	current_value_min = min;
}

int device_condition_dimming_100::get_current_value_max()
{
	return current_value_max;
}

void device_condition_dimming_100::set_current_value_max(int max)
{
	current_value_max = max;
}

void device_condition_dimming_100::set_condition_value(QString s)
{
	DeviceCondition::set_condition_value(s.toInt() * 10);
}

void device_condition_dimming_100::get_condition_value(QString& out)
{
	char tmp[100];
	qDebug("device_condition_dimming_100::get_condition_value()");
	if (get_condition_value_min() == 0)
		sprintf(tmp, "0");
	else
		sprintf(tmp, "%d-%d", get_condition_value_min(), get_condition_value_max());
	out =  tmp;
}

void device_condition_dimming_100::status_changed(const StatusList &sl)
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


/*****************************************************************
** Actual volume device condition
****************************************************************/
device_condition_volume::device_condition_volume(QWidget *parent, QString trigger, QString where)
{
	char sup[10];
	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	l->setFont(bt_global::font->get(FontManager::TEXT));
	frame = l;
	if (trigger == "-1")
	{
		set_condition_value_min(-1);
		set_condition_value_max(-1);
	}
	else
	{
		QByteArray buf = trigger.toAscii();
		sprintf(sup, "%s", buf.constData());
		strtok(sup, "-");
		set_condition_value_min(sup);
		sprintf(sup, "%s", strchr(buf.constData(), '-')+1);
		set_condition_value_max(sup);
	}
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	dev = bt_global::add_device_to_cache(new sound_device(QString(where)));
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
		this, SLOT(status_changed(QList<device_status*>)));
	Draw();
}

void device_condition_volume::inizializza()
{
	dev->init();
}

void device_condition_volume::set_condition_value_min(int s)
{
	qDebug("device_condition_volume::set_condition_value_min(%d)", s);
	min_val = s;
}

void device_condition_volume::set_condition_value_min(QString s)
{
	qDebug() << "device_condition_volume::set_condition_value_min(" << s << ")";
	min_val = s.toInt();
}

int device_condition_volume::get_condition_value_min()
{
	return min_val;
}

void device_condition_volume::set_condition_value_max(int s)
{
	qDebug("device_condition_volume::set_condition_value_max(%d)", s);
	max_val = s;
}

void device_condition_volume::set_condition_value_max(QString s)
{
	qDebug() << "device_condition_volume::set_condition_value_max(" << s << ")";
	max_val = s.toInt();
}

int device_condition_volume::get_condition_value_max()
{
	return max_val;
}

int device_condition_volume::get_current_value_min()
{
	return current_value_min;
}

void device_condition_volume::set_current_value_min(int min)
{
	current_value_min = min;
}

int device_condition_volume::get_current_value_max()
{
	return current_value_max;
}

void device_condition_volume::set_current_value_max(int max)
{
	current_value_max = max;
}

int device_condition_volume::get_min()
{
	return 0;
}

int device_condition_volume::get_max()
{
	return 31;
}

void device_condition_volume::set_condition_value(QString s)
{
	int v = s.toInt();
	qDebug("setting condition value to %d", v);
	DeviceCondition::set_condition_value(v);
}

void device_condition_volume::get_condition_value(QString& out)
{
	char tmp[100];
	qDebug("device_condition_volume::get_condition_value()");
	if (get_condition_value_min() == -1)
		sprintf(tmp, "-1");
	else
		sprintf(tmp, "%d-%d", get_condition_value_min(), get_condition_value_max());
	out =  tmp;
}

void device_condition_volume::Up()
{
	qDebug("device_condition_volume::Up()");
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

void device_condition_volume::Down()
{
	qDebug("device_condition_volume::Down()");
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

void device_condition_volume::OK()
{
	qDebug("device_condition_volume::OK()");
	set_condition_value_min(get_current_value_min());
	set_condition_value_max(get_current_value_max());
}

void device_condition_volume::Draw()
{
	QString tmp;
	QString u = get_unit();
	int val_min = get_current_value_min();
	int val_max = get_current_value_max();
	qDebug("device_condition_volume::Draw(), val_min = %d - val_max = %d", val_min, val_max);
	if (val_min == -1)
		tmp = tr("OFF");
	else if (val_min == 0 && val_max == 31)
		tmp = tr("ON");
	else
	{
		int vmin = (val_min == 0 ? 0 : (10 * (val_min <= 15 ? val_min/3 : (val_min-1)/3) + 1));
		int vmax = 10 * (val_max <= 15 ? val_max/3 : (val_max-1)/3);

		tmp = QString("%1%2 - %3%4").arg(vmin).arg(u).arg(vmax).arg(u);
	}
	((QLabel *)frame)->setText(tmp);
}

void device_condition_volume::status_changed(QList<device_status*> sl)
{
	int trig_v_min = get_condition_value_min();
	int trig_v_max = get_condition_value_max();
	stat_var curr_volume(stat_var::AUDIO_LEVEL);
	stat_var curr_stato(stat_var::ON_OFF);
	qDebug("device_condition_volume::status_changed()");

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

QString device_condition_volume::get_unit()
{
	return "%";
}

void device_condition_volume::reset()
{
	qDebug("device_condition_volume::reset()");
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	Draw();
}

/*****************************************************************
** Actual temperature device condition
****************************************************************/
device_condition_temp::device_condition_temp(QWidget *parent, QString trigger, QString where, bool external)
{
	// Temp condition is expressed in bticino format
	int temp_condition = trigger.toInt();

	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	l->setFont(bt_global::font->get(FontManager::TEXT));
	frame = l;

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

int device_condition_temp::get_min()
{
	return min_temp;
}

int device_condition_temp::get_max()
{
	return max_temp;
}

int device_condition_temp::get_step()
{
	return step;
}

int device_condition_temp::get_divisor()
{
	return 10;
}

QString device_condition_temp::get_unit()
{
	switch (temp_scale)
	{
	case CELSIUS:
		return TEMP_DEGREES"C \2611"TEMP_DEGREES"C";
	case FAHRENHEIT:
		return TEMP_DEGREES"F \2611"TEMP_DEGREES"F";
	default:
		qWarning("Wrong temperature scale, defaulting to celsius");
		return TEMP_DEGREES"C \2611"TEMP_DEGREES"C";
	}
}

void device_condition_temp::Draw()
{
	QString u = get_unit();
	// val is an integer either in Celsius or in Fahrenheit degrees
	int val = get_current_value();
	qDebug("device_condition_temp::Draw(), val = %d", val);

	QString tmp = loc.toString(val / 10.0, 'f', 1);
	tmp += u;

	((QLabel *)frame)->setText(tmp);
}

void device_condition_temp::get_condition_value(QString& out)
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

void device_condition_temp::inizializza()
{
	dev->requestStatus();
}

void device_condition_temp::status_changed(const StatusList &sl)
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


/*****************************************************************
** Aux device condition
****************************************************************/


device_condition_aux::device_condition_aux(QWidget *parent, QString trigger, QString where) :
	device_initialized(false), device_value(-1)
{
	condition_display = new DeviceConditionDisplayOnOff(parent);

	set_condition_value(trigger);
	set_current_value(DeviceCondition::get_condition_value());
	dev = bt_global::add_device_to_cache(new aux_device(where));
	connect(dev, SIGNAL(status_changed(stat_var)), SLOT(status_changed(stat_var)));
	Draw();
}

void device_condition_aux::inizializza()
{
	dev->init();
}

void device_condition_aux::Draw()
{
	condition_display->updateText(get_current_value());
}

void device_condition_aux::check_condition(bool emit_signal)
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

void device_condition_aux::status_changed(stat_var status)
{
	qDebug("device_condition_aux::status_changed");
	device_value = status.get_val();
	// We emit signal condSatisfied only if the device is initialized.
	check_condition(device_initialized);
	device_initialized = true;
}

int device_condition_aux::get_max()
{
	return 1;
}

void device_condition_aux::set_condition_value(QString s)
{
	qDebug("device_condition_aux::set_condition_value");
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

void device_condition_aux::OK()
{
	qDebug("device_condition_aux::OK()");
	DeviceCondition::OK();
	check_condition(false);
}

void device_condition_aux::setGeometry(int x, int y, int sx, int sy)
{
	condition_display->setGeometry(x, y, sx, sy);
}

