#include "scenevocond.h"
#include "generic_functions.h" // setCfgValue
#include "btbutton.h"
#include "fontmanager.h" // bt_global::font
#include "main.h" // bt_global::config
#include "skinmanager.h"
#include "icondispatcher.h" // icons_cache
#include "xml_functions.h" //getTextChild
#include "scenevodevicescond.h"

#include <QLabel>
#include <QLocale>
#include <QDebug>

#define BOTTOM_BORDER 10


ScenEvoCondition::ScenEvoCondition()
{
	hasTimeCondition = false;
}

const char *ScenEvoCondition::getDescription()
{
	return "Generic scenEvo condition";
}

void ScenEvoCondition::Next()
{
	qDebug("ScenEvoCondition::Next()");
	emit SwitchToNext();
}

void ScenEvoCondition::Prev()
{
	qDebug("ScenEvoCondition::Prev()");
	reset();
	emit resetAll();
	emit SwitchToFirst();
}

void ScenEvoCondition::OK()
{
	qDebug("ScenEvoCondition::OK()");
	save();
	emit SwitchToFirst();
}

void ScenEvoCondition::Apply()
{
	qDebug("ScenEvoCondition::Apply()");
}

void ScenEvoCondition::save()
{
	qDebug("ScenEvoCondition::save()");
}

void ScenEvoCondition::reset()
{
	qDebug("ScenEvoCondition::reset()");
}

int ScenEvoCondition::get_serial_number()
{
	return serial_number;
}

void ScenEvoCondition::set_serial_number(int n)
{
	serial_number = n;
}

void ScenEvoCondition::inizializza()
{
}

bool ScenEvoCondition::isTrue()
{
	return false;
}


ScenEvoTimeCondition::ScenEvoTimeCondition(int _item_id, const QDomNode &config_node, bool has_next) :
	time_edit(this)
{
	item_id = _item_id;

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

const char *ScenEvoTimeCondition::getDescription()
{
	return "scenEvo hour condition";
}

void ScenEvoTimeCondition::setupTimer()
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

void ScenEvoTimeCondition::Apply()
{
	BtTime tmp = time_edit.time();
	cond_time.setHMS(tmp.hour(), tmp.minute(), 0);
	setupTimer();
}

void ScenEvoTimeCondition::OK()
{
	qDebug("ScenEvoTimeCondition::OK()");
	Apply();
	ScenEvoCondition::OK();
}

void ScenEvoTimeCondition::scaduta()
{
	qDebug("ScenEvoTimeCondition::scaduta()");
	emit verificata();
	setupTimer();
}

void ScenEvoTimeCondition::save()
{
	qDebug("ScenEvoTimeCondition::save()");

	QMap<QString, QString> data;
#ifdef CONFIG_BTOUCH
	data["condH/hour"] = cond_time.toString("hh");
	data["condH/minute"] = cond_time.toString("mm");
	setCfgValue(data, SCENARIO_EVOLUTO, get_serial_number());
#else
	data["scen/time/hour"] = cond_time.toString("hh");
	data["scen/time/minute"] = cond_time.toString("mm");
	setCfgValue(data, item_id);
#endif
}

void ScenEvoTimeCondition::reset()
{
	time_edit.setTime(cond_time);
}

bool ScenEvoTimeCondition::isTrue()
{
	QTime cur = QDateTime::currentDateTime().time();
	return ((cond_time.hour() == cur.hour()) &&
			(cond_time.minute() == cur.minute()));
}


ScenEvoDeviceCondition::ScenEvoDeviceCondition(int _item_id, const QDomNode &config_node)
{
	item_id = _item_id;

	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 5, 0, 10);
	main_layout->setSpacing(0);

	QString trigger = getTextChild(config_node, "trigger");

#ifdef CONFIG_BTOUCH
	int condition_type = getTextChild(config_node, "value").toInt();
#else
	int condition_type = getTextChild(config_node, "objectID").toInt();
#endif

	QString descr = getTextChild(config_node, "descr");
	QString w = getTextChild(config_node, "where");
	bool external = false;
	DeviceConditionDisplay *condition_display;

	switch (condition_type)
	{
	case 1:
		condition_display = new DeviceConditionDisplayOnOff(this, descr, bt_global::skin->getImage("light"));
		device_cond = new DeviceConditionLight(condition_display, trigger, w);
		break;
	case 2:
		condition_display = new DeviceConditionDisplayDimming(this, descr, bt_global::skin->getImage("dimmer"));
		device_cond = new DeviceConditionDimming(condition_display, trigger, w);
		break;
	case 7:
		external = true;
		w += "00";
	case 3:
	case 8:
		condition_display = new DeviceConditionDisplayTemperature(this, descr, bt_global::skin->getImage("probe"));
		device_cond = new DeviceConditionTemperature(condition_display, trigger, w, external);
		break;
	case 9:
		condition_display = new DeviceConditionDisplayOnOff(this, descr, bt_global::skin->getImage("aux"));
		device_cond = new DeviceConditionAux(condition_display, trigger, w);
		break;
	case 4:
		condition_display = new DeviceConditionDisplayVolume(this, descr, bt_global::skin->getImage("amplifier"));
		device_cond = new DeviceConditionVolume(condition_display, trigger, w);
		break;
	case 6:
		condition_display = new DeviceConditionDisplayDimming(this, descr, bt_global::skin->getImage("dimmer"));
		device_cond = new DeviceConditionDimming100(condition_display, trigger, w);
		break;
	default:
		qFatal("Unknown device condition: %d", condition_type);
	}

	connect(condition_display, SIGNAL(upClicked()), device_cond, SLOT(Up()));
	connect(condition_display, SIGNAL(downClicked()), device_cond, SLOT(Down()));
	connect(device_cond, SIGNAL(condSatisfied()), SIGNAL(condSatisfied()));

	main_layout->addWidget(condition_display);

	QHBoxLayout *bottom_layout = new QHBoxLayout;

	BtButton *back = new BtButton;
	back->setImage(bt_global::skin->getImage("back"));
	connect(back, SIGNAL(clicked()), SLOT(Prev()));
	bottom_layout->addWidget(back);
	bottom_layout->addStretch(1);

	BtButton *ok = new BtButton;
	ok->setImage(bt_global::skin->getImage("ok"));
	bottom_layout->addWidget(ok);
	connect(ok, SIGNAL(clicked()), SLOT(OK()));

	main_layout->addLayout(bottom_layout);
}

ScenEvoDeviceCondition::~ScenEvoDeviceCondition()
{
	delete device_cond;
}

const char *ScenEvoDeviceCondition::getDescription()
{
	return "scenEvo device condition";
}

void ScenEvoDeviceCondition::Apply()
{
	device_cond->OK();
}

void ScenEvoDeviceCondition::OK()
{
	qDebug("ScenEvoDeviceCondition::OK()");
	// Save ALL conditions here (not just this one)
	emit okAll();
	emit SwitchToFirst();
}

void ScenEvoDeviceCondition::save()
{
	qDebug("ScenEvoDeviceCondition::save()");
	QString s;
	device_cond->get_condition_value(s);
#ifdef CONFIG_BTOUCH
	setCfgValue("condDevice/trigger", s, SCENARIO_EVOLUTO, get_serial_number());
#else
	setCfgValue("scen/device/trigger", s, item_id);
#endif
	reset();
	inizializza();
}

void ScenEvoDeviceCondition::reset()
{
	qDebug("ScenEvoDeviceCondition::reset()");
	device_cond->reset();
}

void ScenEvoDeviceCondition::inizializza()
{
	if (device_cond)
		device_cond->inizializza();
}

bool ScenEvoDeviceCondition::isTrue()
{
	return device_cond ? device_cond->isTrue() : false;
}

