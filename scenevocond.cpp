/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "scenevocond.h"
#include "device.h"
#include "devices_cache.h" // add_device_to_cache
#include "generic_functions.h" // setCfgValue
#include "btbutton.h"
#include "fontmanager.h" // bt_global::font
#include "scaleconversion.h"
#include "main.h" // (*bt_global::config)
#include "skinmanager.h"
#include "icondispatcher.h" // icons_cache
#include "xml_functions.h" //getTextChild

#include "lighting_device.h"

#include <QLocale>
#include <QDebug>
#include <QLabel>

#define BOTTOM_BORDER 10

// TODO: create a global locale object
namespace
{
// The language used for the floating point number
QLocale loc(QLocale::Italian);
}


/*****************************************************************
** Advanced scenario management generic condition
****************************************************************/

scenEvo_cond::scenEvo_cond()
{
	hasTimeCondition = false;
}

const char *scenEvo_cond::getDescription()
{
	return "Generic scenEvo condition";
}

void scenEvo_cond::Next()
{
	qDebug("scenEvo_cond::Next()");
	emit SwitchToNext();
}

void scenEvo_cond::Prev()
{
	qDebug("scenEvo_cond::Prev()");
	reset();
	emit resetAll();
	emit SwitchToFirst();
}

void scenEvo_cond::OK()
{
	qDebug("scenEvo_cond::OK()");
	save();
	emit SwitchToFirst();
}

void scenEvo_cond::Apply()
{
	qDebug("scenEvo_cond::Apply()");
}

void scenEvo_cond::save()
{
	qDebug("scenEvo_cond::save()");
}

void scenEvo_cond::reset()
{
	qDebug("scenEvo_cond::reset()");
}

// TODO remove after debugging is complete
namespace
{
	inline PullMode getPullMode(const QDomNode &node)
	{
		QDomNode element = getChildWithName(node, "pul");
		Q_ASSERT_X(!element.isNull(), "getPullMode", qPrintable(QString("Pull device node %1 without <pul> child").arg(getTextChild(node, "where"))));
		bool ok;
		int value = element.toElement().text().toInt(&ok);
		Q_ASSERT_X(ok && (value == 0 || value == 1), "getPullMode", qPrintable(QString("Pull device %1 with invalid <pul> child").arg(getTextChild(node, "where"))));

		return value ? PULL : NOT_PULL;
	}
}


int scenEvo_cond::get_serial_number()
{
	return serial_number;
}

void scenEvo_cond::set_serial_number(int n)
{
	serial_number = n;
}

void scenEvo_cond::inizializza()
{
}

bool scenEvo_cond::isTrue()
{
	return false;
}

int scenEvo_cond::height()
{
	return 320;
}

int scenEvo_cond::width()
{
	return 240;
}


/*****************************************************************
 ** Advanced scenario management, time condition
****************************************************************/

scenEvo_cond_h::scenEvo_cond_h(const QDomNode &config_node, bool has_next) :
	time_edit(this)
{
	qDebug("***** scenEvo_cond_h::scenEvo_cond_h");

	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), SLOT(scaduta()));

	const int BUT_DIM = 60;
	// Top icon
	QLabel *image = new QLabel(this);
	image->setPixmap(bt_global::skin->getImage("watch"));
	image->setGeometry(width()/2 - BUT_DIM/2, 0, BUT_DIM, BUT_DIM);

	QString h = getTextChild(config_node, "hour");
	QString m = getTextChild(config_node, "minute");
	time_edit.setMaxHours(24);
	time_edit.setMaxMinutes(60);
	const int TIME_EDIT_W = width() / 2;
	// +10 below is for compatibility reasons
	const int TIME_EDIT_H = height() / 2 + 10;
	time_edit.setGeometry(width()/2 - TIME_EDIT_W/2, height()/2 - TIME_EDIT_H/2, TIME_EDIT_W, TIME_EDIT_H);
	time_edit.setTime(QTime(h.toInt(), m.toInt(), 0));

	bottom_left = new BtButton(this);
	bottom_left->setGeometry(0, height() - (BUT_DIM + BOTTOM_BORDER), BUT_DIM, BUT_DIM);
	bottom_left->setImage(bt_global::skin->getImage("ok"));
	connect(bottom_left, SIGNAL(released()), SLOT(OK()));

	bottom_right = new BtButton(this);
	bottom_right->setGeometry(width() - BUT_DIM, height() - (BUT_DIM + BOTTOM_BORDER), BUT_DIM, BUT_DIM);

	if (has_next)
	{
		bottom_center = new BtButton(this);
		bottom_center->setGeometry(width()/2 - BUT_DIM/2, height() - (BUT_DIM + BOTTOM_BORDER), BUT_DIM, BUT_DIM);
		bottom_center->setImage(bt_global::skin->getImage("back"));
		connect(bottom_center, SIGNAL(released()),SLOT(Prev()));

		bottom_right->setImage(bt_global::skin->getImage("forward"));
		connect(bottom_right, SIGNAL(released()), SLOT(Next()));
	}
	else
	{
		bottom_right->setImage(bt_global::skin->getImage("back"));
		connect(bottom_right, SIGNAL(released()), SLOT(Prev()));
	}

	hasTimeCondition = true;
	cond_time.setHMS(h.toInt(), m.toInt(), 0);
	setupTimer();
}

const char *scenEvo_cond_h::getDescription()
{
	return "scenEvo hour condition";
}

void scenEvo_cond_h::setupTimer()
{
	QTime now = QTime::currentTime();
	int msecsto = now.msecsTo(cond_time);

	while (msecsto <= 0) // Do it tomorrow
		msecsto += 24 * 60 * 60 * 1000;

	while (msecsto >=  24 * 60 * 60 * 1000)
		msecsto -= 24 * 60 * 60 * 1000;

	qDebug("(re)starting timer with interval = %d", msecsto);
	timer.stop();
	timer.start(msecsto);
}

void scenEvo_cond_h::Apply()
{
	BtTime tmp = time_edit.time();
	cond_time.setHMS(tmp.hour(), tmp.minute(), 0);
	setupTimer();
}

void scenEvo_cond_h::OK()
{
	qDebug("scenEvo_cond_h::OK()");
	Apply();
	scenEvo_cond::OK();
}

void scenEvo_cond_h::scaduta()
{
	qDebug("scenEvo_cond_h::scaduta()");
	emit verificata();
	setupTimer();
}

void scenEvo_cond_h::save()
{
	qDebug("scenEvo_cond_h::save()");

	QMap<QString, QString> data;
	data["condH/hour"] = cond_time.toString("hh");
	data["condH/minute"] = cond_time.toString("mm");
	setCfgValue(data, SCENARIO_EVOLUTO, get_serial_number());
}

void scenEvo_cond_h::reset()
{
	time_edit.setTime(cond_time);
}

bool scenEvo_cond_h::isTrue()
{
	QTime cur = QDateTime::currentDateTime().time();
	return ((cond_time.hour() == cur.hour()) &&
			(cond_time.minute() == cur.minute()));
}

/*****************************************************************
** Advanced scenario management, device condition
****************************************************************/

scenEvo_cond_d::scenEvo_cond_d(const QDomNode &config_node)
{
	QLabel *area1_ptr = new QLabel(this);
	area1_ptr->setGeometry(0, 0, BUTTON_DIM, BUTTON_DIM);

	QLabel *area2_ptr = new QLabel(this);
	area2_ptr->setGeometry(BUTTON_DIM, BUTTON_DIM/2 - TEXT_Y_DIM/2, width() - BUTTON_DIM, TEXT_Y_DIM);
	area2_ptr->setFont(bt_global::font->get(FontManager::TEXT));
	area2_ptr->setAlignment(Qt::AlignCenter);
	area2_ptr->setText(getTextChild(config_node, "descr"));

	// create condition buttons
	BtButton *condition_up = new BtButton(this);
	condition_up->setGeometry(width()/2 - BUTTON_DIM/2, 80, BUTTON_DIM, BUTTON_DIM);
	condition_up->setImage(bt_global::skin->getImage("plus"));
	connect(condition_up, SIGNAL(clicked()), SLOT(Up()));

	BtButton *condition_down = new BtButton(this);
	condition_down->setGeometry(width()/2 - BUTTON_DIM/2, 190, BUTTON_DIM, BUTTON_DIM);
	condition_down->setImage(bt_global::skin->getImage("minus"));
	connect(condition_down, SIGNAL(clicked()), SLOT(Down()));

	// create bottom (navigation) buttons
	BtButton *b = new BtButton(this);
	b->setGeometry(0, height() - (BUTTON_DIM + BOTTOM_BORDER), BUTTON_DIM, BUTTON_DIM);
	b->setImage(bt_global::skin->getImage("ok"));
	connect(b, SIGNAL(clicked()), SLOT(OK()));

	b = new BtButton(this);
	b->setGeometry(width() - BUTTON_DIM, height() - (BUTTON_DIM + BOTTOM_BORDER), BUTTON_DIM, BUTTON_DIM);
	b->setImage(bt_global::skin->getImage("back"));
	connect(b, SIGNAL(clicked()), SLOT(Prev()));

	QString trigger = getTextChild(config_node, "trigger");
	// Create actual device condition
	device_condition *dc;
	int condition_type = getTextChild(config_node, "value").toInt();
	qDebug("#### Condition type = %d", condition_type);
	QString icon;
	QString w = getTextChild(config_node, "where");
	bool external = false;
	switch (condition_type)
	{
	case 1:
		dc = new device_condition_light_status(this, &trigger, getPullMode(config_node));
		icon = bt_global::skin->getImage("light");
		break;
	case 2:
		dc = new device_condition_dimming(this, &trigger, getPullMode(config_node));
		icon = bt_global::skin->getImage("dimmer");
		break;
	case 7:
		external = true;
		w += "00";
	case 3:
	case 8:
		dc = new device_condition_temp(this, &trigger, external);
		condition_up->setAutoRepeat(true);
		condition_down->setAutoRepeat(true);
		icon = bt_global::skin->getImage("probe");
		break;
	case 9:
		dc = new device_condition_aux(this, &trigger);
		icon = bt_global::skin->getImage("aux");
		break;
	case 4:
		dc = new device_condition_volume(this, &trigger);
		icon = bt_global::skin->getImage("amplifier");
		break;
	case 6:
		dc = new device_condition_dimming_100(this, &trigger, getPullMode(config_node));
		icon = bt_global::skin->getImage("dimmer");
		break;
	default:
		qDebug("Unknown device condition");
		dc = NULL;
	}
	area1_ptr->setPixmap(*bt_global::icons_cache.getIcon(icon));

	if (dc)
	{
		dc->setGeometry(40,140,160,50);
		connect(dc, SIGNAL(condSatisfied()), SIGNAL(condSatisfied()));
		dc->setup_device(w);
	}
	actual_condition = dc;
}

const char *scenEvo_cond_d::getDescription()
{
	return "scenEvo device condition";
}

void scenEvo_cond_d::Up()
{
	qDebug("scenEvo_cond_d::Up()");
	actual_condition->Up();
}

void scenEvo_cond_d::Down()
{
	qDebug("scenEvo_cond_d::Down()");
	actual_condition->Down();
}

void scenEvo_cond_d::Apply()
{
	actual_condition->OK();
}

void scenEvo_cond_d::OK()
{
	qDebug("scenEvo_cond_d::OK()");
	// Save ALL conditions here (not just this one)
	emit okAll();
	emit SwitchToFirst();
}

void scenEvo_cond_d::save()
{
	qDebug("scenEvo_cond_d::save()");
	QString s;
	actual_condition->get_condition_value(s);
	setCfgValue("condDevice/trigger", s, SCENARIO_EVOLUTO, get_serial_number());
	reset();
	inizializza();
}

void scenEvo_cond_d::reset()
{
	qDebug("scenEvo_cond_d::reset()");
	actual_condition->reset();
}

void scenEvo_cond_d::inizializza()
{
	if (actual_condition)
		actual_condition->inizializza();
}

bool scenEvo_cond_d::isTrue()
{
	return actual_condition ? actual_condition->isTrue() : false;
}

/*****************************************************************
 ** Actual generic device condition
****************************************************************/
device_condition::device_condition()
{
	satisfied = false;
	initialized = false;
}

int device_condition::get_min()
{
	return 0;
}

int device_condition::get_max()
{
	return 0;
}

int device_condition::get_step()
{
	return 1;
}

int device_condition::get_divisor()
{
	return 1;
}

bool device_condition::show_OFF_on_zero()
{
	return false;
}

int device_condition::get_condition_value()
{
	return cond_value;
}

void device_condition::set_condition_value(int v)
{
	if (v > get_max())
		v = get_max();
	if (v < get_min())
		v = get_min();
	cond_value = v;
}

void device_condition::set_condition_value(QString s)
{
	qDebug() << "device_condition::set_condition_value (" << s << ")";
	set_condition_value(s.toInt());
}

void device_condition::get_condition_value(QString& out)
{
	qDebug("device_condition::get_condition_value(QString&)");
	char tmp[100];
	sprintf(tmp, "%d", get_condition_value());
	out = tmp;
}

void device_condition::Draw()
{
	QString tmp;
	int v = get_current_value();
	if (show_OFF_on_zero() && !v)
		tmp = tr("OFF");
	else
		tmp = QString("%1%2").arg(v).arg(get_unit());
	((QLabel *)frame)->setText(tmp);
}

void device_condition::setGeometry(int x, int y, int sx, int sy)
{
	frame->setGeometry(x, y, sx, sy);
}

void device_condition::Up()
{
	qDebug("device_condition::Up()");
	int val = get_current_value();
	val += get_step();
	set_current_value(val);
	qDebug("val = %d", get_current_value());
	Draw();
}

void device_condition::Down()
{
	qDebug("device_condition::Down()");
	int val = get_current_value();
	val -= get_step();
	set_current_value(val);
	Draw();
}

void device_condition::OK()
{
	qDebug("device_condition::OK()");
	if (get_current_value() != get_condition_value())
	{
		set_condition_value(get_current_value());

		if (satisfied)
			satisfied = false;
		else
			initialized = false;
	}
}

int device_condition::get_current_value()
{
	return current_value;
}

int device_condition::set_current_value(int v)
{
	if (v > get_max())
		v = get_max();
	if (v < get_min())
		v = get_min();
	current_value = v;
	return current_value;
}

QString device_condition::get_unit()
{
	return "";
}

void device_condition::inizializza()
{
	qDebug("device_condition::inizializza()");
	dev->init(true);
}

void device_condition::reset()
{
	qDebug("device_condition::reset()");
	set_current_value(get_condition_value());
	Draw();
}

bool device_condition::isTrue()
{
	return satisfied;
}

void device_condition::setup_device(QString s)
{
	qDebug() << "device_condition::setup_device(" << s << ")";
	dev->set_where(s);
	// Add the device to cache, or replace it with the instance found in cache
	dev = bt_global::add_device_to_cache(dev);
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
		this, SLOT(status_changed(QList<device_status*>)));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
}

void device_condition::status_changed(const StatusList &sl)
{
}


/*****************************************************************
** Actual light status device condition
****************************************************************/
device_condition_light_status::device_condition_light_status(QWidget *parent, QString *c, PullMode pull_mode)
{
	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	l->setFont(bt_global::font->get(FontManager::TEXT));

	frame = l;
	set_condition_value(*c);
	set_current_value(device_condition::get_condition_value());
	// TODO: we just need dummy device here, address will be set later on by setup_device()
	dev = new LightingDevice("", pull_mode);
	Draw();
}

QString device_condition_light_status::get_string()
{
	return get_current_value() ? tr("ON") : tr("OFF");
}

void device_condition_light_status::Draw()
{
	((QLabel *)frame)->setText(get_string());
}

void device_condition_light_status::status_changed(const StatusList &sl)
{
	qDebug() << "device_condition_light_status::status_changed start";
	StatusList::const_iterator it = sl.constBegin();
	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case LightingDevice::DIM_DEVICE_ON:
			if (device_condition::get_condition_value() == static_cast<int>(it.value().toBool()))
			{
				if (!satisfied)
				{
					satisfied = true;
					if (initialized)
						emit condSatisfied();
				}
			}
			else
				satisfied = false;
			initialized = true;
			break;
		}
		++it;
	}
	qDebug() << "device_condition_light_status::status_changed end";
}

void device_condition_light_status::status_changed(QList<device_status*> sl)
{
	qFatal("Old status changed on device_condition_light_status not implemented!");
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
	device_condition::set_condition_value(v);
}

void device_condition_light_status::get_condition_value(QString& out)
{
	out = device_condition::get_condition_value() ? "1" : "0";
}


/*****************************************************************
** Actual dimming value device condition
****************************************************************/
device_condition_dimming::device_condition_dimming(QWidget *parent, QString *c, PullMode pull_mode)
{
	qDebug() << "device_condition_dimming::device_condition_dimming(" << c << ")";
	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	l->setFont(bt_global::font->get(FontManager::TEXT));

	frame = l;
	if (*c == "0")
	{
		set_condition_value_min(0);
		set_condition_value_max(0);
	}
	else
	{
		set_condition_value_min((QString) c->at(0));
		QByteArray buf = c->toAscii();
		set_condition_value_max(buf.constData()+2);
	}
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	dev = new DimmerDevice("", pull_mode);
	Draw();
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
	return 10;
}

int device_condition_dimming::get_step()
{
	return 1;
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
	if (get_current_value_min() != get_condition_value_min() ||
		get_current_value_max() != get_condition_value_max())
	{
		set_condition_value_min(get_current_value_min());
		set_condition_value_max(get_current_value_max());
		if (satisfied)
			satisfied = false;
		else
			initialized = false;
	}
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
	device_condition::set_condition_value(s.toInt() * 10);
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
		int level;
		if ((it.key() == LightingDevice::DIM_DEVICE_ON && it.value().toBool() == false) || it.key() == LightingDevice::DIM_DIMMER_LEVEL)
			level = it.value().toInt() / 10;
		else if (it.key() == LightingDevice::DIM_DEVICE_ON && it.value().toBool() == true && !sl.contains(LightingDevice::DIM_DIMMER_LEVEL))
			level = 1;
		else
		{
			++it;
			continue;
		}

		if (level >= trig_min && level <= trig_max)
		{
			if (!satisfied)
			{
					satisfied = true;
					if (initialized)
						emit condSatisfied();
			}
		}
		else
			satisfied = false;
		initialized = true;

		++it;
	}
}

void device_condition_dimming::status_changed(QList<device_status*> sl)
{
	qFatal("Old status changed on device_condition_dimming not implemented!");
}


/*****************************************************************
 ** Actual dimming 100 value device condition
****************************************************************/
device_condition_dimming_100::device_condition_dimming_100(QWidget *parent, QString *c, PullMode pull_mode)
{
	char sup[10];
	qDebug() << "device_condition_dimming_100::device_condition_dimming_100(" << c << ")";
	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	l->setFont(bt_global::font->get(FontManager::TEXT));
	frame = l;
	if (*c == "0")
	{
		set_condition_value_min(0);
		set_condition_value_max(0);
	}
	else
	{
		QByteArray buf = c->toAscii();
		sprintf(sup, "%s", buf.constData());
		strtok(sup, "-");
		set_condition_value_min(sup);
		sprintf(sup, "%s", strchr(buf.constData(), '-')+1);
		set_condition_value_max(sup);
	}
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	dev = new Dimmer100Device("", pull_mode);
	Draw();
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
	if (get_current_value_min() != get_condition_value_min() ||
		get_current_value_max() != get_condition_value_max())
	{
		set_condition_value_min(get_current_value_min());
		set_condition_value_max(get_current_value_max());
		if (satisfied)
			satisfied = false;
		else
			initialized = false;
	}
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
	device_condition::set_condition_value(s.toInt() * 10);
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
		int level;
		if ((it.key() == LightingDevice::DIM_DEVICE_ON  && it.value().toBool() == false) || it.key() == LightingDevice::DIM_DIMMER100_LEVEL)
			level = it.value().toInt();
		else if (it.key() == LightingDevice::DIM_DEVICE_ON && it.value().toBool() == true && !sl.contains(LightingDevice::DIM_DIMMER100_LEVEL))
			level = 1;
		else
		{
			++it;
			continue;
		}

		if (level >= trig_min && level <= trig_max)
		{
			if (!satisfied)
			{
				satisfied = true;
				if (initialized)
					emit condSatisfied();
			}
		}
		else
			satisfied = false;
		initialized = true;
		++it;
	}
}

void device_condition_dimming_100::status_changed(QList<device_status*> sl)
{
	qFatal("Old status changed on device_condition_dimming_100 not implemented!");
}

/*****************************************************************
** Actual volume device condition
****************************************************************/
device_condition_volume::device_condition_volume(QWidget *parent, QString *c)
{
	char sup[10];
	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	l->setFont(bt_global::font->get(FontManager::TEXT));
	frame = l;
	if (*c == "-1")
	{
		set_condition_value_min(-1);
		set_condition_value_max(-1);
	}
	else
	{
		QByteArray buf = c->toAscii();
		sprintf(sup, "%s", buf.constData());
		strtok(sup, "-");
		set_condition_value_min(sup);
		sprintf(sup, "%s", strchr(buf.constData(), '-')+1);
		set_condition_value_max(sup);
	}
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	Draw();
	dev = new AmplifierDevice(QString(""));
	amplifier_on = false;
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
	device_condition::set_condition_value(v);
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
	if (get_current_value_min() != get_condition_value_min() ||
		get_current_value_max() != get_condition_value_max())
	{
		set_condition_value_min(get_current_value_min());
		set_condition_value_max(get_current_value_max());
		if (satisfied)
			satisfied = false;
		else
			initialized = false;
	}
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

void device_condition_volume::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	int trig_v_min = get_condition_value_min();
	int trig_v_max = get_condition_value_max();

	while (it != sl.constEnd())
	{
		// When the Amplifier is turned on the device send both the DIM_STATUS
		// and the DIM_VOLUME. But we can't simply ignore the DIM_STATUS
		// because that dimension can arrive after a status request.
		// So we need to remember the status of the amplifier, and trigger
		// for the On/Off condition only after a DIM_STATUS followed by
		// a DIM_VOLUME.

		if (it.key() == AmplifierDevice::DIM_STATUS)
		{
			amplifier_on = (it.value().toInt() != 0);
			if (it.value().toInt() == 0)
			{
				if (trig_v_min == -1)
				{
					if (!satisfied)
					{
						satisfied = true;
						if (initialized)
							emit condSatisfied();
					}
				}
				else
					satisfied = false;
				initialized = true;
			}
		}
		else if (it.key() == AmplifierDevice::DIM_VOLUME && amplifier_on)
		{
			int volume = it.value().toInt();
			if (volume >= trig_v_min && volume <= trig_v_max)
			{
				if (!satisfied)
				{
					satisfied = true;
					if (initialized)
						emit condSatisfied();
				}
			}
			else
				satisfied = false;
			initialized = true;
		}
		++it;
	}
}

void device_condition_volume::status_changed(QList<device_status*> sl)
{
	qFatal("Old status changed on device_condition_volume not implemented!");
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
device_condition_temp::device_condition_temp(QWidget *parent, QString *c, bool external)
{
	// Temp condition is expressed in bticino format
	int temp_condition = c->toInt();

	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	l->setFont(bt_global::font->get(FontManager::TEXT));
	frame = l;

	temp_scale = static_cast<TemperatureScale>((*bt_global::config)[TEMPERATURE_SCALE].toInt());
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
	set_current_value(device_condition::get_condition_value());

	Draw();
	dev = new temperature_probe_notcontrolled(QString(""), external);
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
	int val = device_condition::get_condition_value();
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

void device_condition_temp::status_changed(QList<device_status*> sl)
{
	// get_condition_value() returns an int, which is Celsius or Fahrenheit
	int trig_v = device_condition::get_condition_value();
	stat_var curr_temp(stat_var::TEMPERATURE);
	qDebug("device_condition_temp::status_changed()");
	qDebug("trig_v = %d", trig_v);

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::TEMPERATURE_PROBE:
			qDebug("Temperature changed");
			ds->read(device_status_temperature_probe::TEMPERATURE_INDEX, curr_temp);
			qDebug("Current temperature %d", curr_temp.get_val());
			int measured_temp;
			switch (temp_scale)
			{
			case CELSIUS:
				measured_temp = bt2Celsius(curr_temp.get_val());
				break;
			case FAHRENHEIT:
				measured_temp = bt2Fahrenheit(curr_temp.get_val());
				break;
			default:
				qWarning("Wrong temperature scale, defaulting to celsius");
				measured_temp = bt2Celsius(curr_temp.get_val());
			}

			if (measured_temp >= (trig_v - 10) && measured_temp <= (trig_v + 10))
			{
				qDebug("Condition triggered");
				if (!satisfied)
				{
					satisfied = true;
					if (initialized)
						emit condSatisfied();
				}
			}
			else
			{
				qDebug("Condition not triggered");
				satisfied = false;
			}
			initialized = true;
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}
}


/*****************************************************************
** Aux device condition
****************************************************************/

device_condition_aux::device_condition_aux(QWidget *parent, QString *c)
{
	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	l->setFont(bt_global::font->get(FontManager::TEXT));

	frame = l;
	set_condition_value(*c);
	set_current_value(device_condition::get_condition_value());
	dev = new aux_device(QString(""));

	Draw();
}

void device_condition_aux::setup_device(QString s)
{
	device_condition::setup_device(s);
	// The device can be replaced by "add_device" method of device_cache, thus
	// the connection must be after that.
	connect(dev, SIGNAL(status_changed(stat_var)), SLOT(status_changed(stat_var)));
}

void device_condition_aux::Draw()
{
	((QLabel *)frame)->setText(get_current_value() ? tr("ON") : tr("OFF"));
}

void device_condition_aux::status_changed(stat_var status)
{
	qDebug("device_condition_aux::status_changed");
	int device_value = status.get_val();

	int trig_v = device_condition::get_condition_value();
	if (trig_v == device_value)
	{
		qDebug("aux condition (%d) satisfied", trig_v);
		if (!satisfied)
		{
			satisfied = true;
			if (initialized)
				emit condSatisfied();
		}
	}
	else
	{
		qDebug("aux condition (%d) NOT satisfied", trig_v);
		satisfied = false;
	}
	initialized = true;
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
	device_condition::set_condition_value(v);
}

void device_condition_aux::status_changed(QList<device_status*> sl)
{
	qFatal("Old status changed on device_condition_aux not implemented!");
}

