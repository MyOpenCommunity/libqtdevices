#include "scenevocond.h"
#include "generic_functions.h" // setCfgValue
#include "btbutton.h"
#include "skinmanager.h" // bt_global::skin
#include "xml_functions.h" // getTextChild
#include "scenevodevicescond.h"

#include <QLabel>



int ScenEvoCondition::get_serial_number()
{
	return serial_number;
}

void ScenEvoCondition::set_serial_number(int n)
{
	serial_number = n;
}


ScenEvoTimeCondition::ScenEvoTimeCondition(int _item_id, const QDomNode &config_node) :
	time_edit(this)
{
	item_id = _item_id;

	QString h = getTextChild(config_node, "hour");
	QString m = getTextChild(config_node, "minute");
	time_edit.setMaxHours(24);
	time_edit.setMaxMinutes(60);

	time_edit.setTime(QTime(h.toInt(), m.toInt(), 0));

	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), SLOT(scaduta()));

	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 5, 0, 10);
	main_layout->setSpacing(0);

	QLabel *top_image = new QLabel;
	top_image->setPixmap(bt_global::skin->getImage("watch"));
	main_layout->addWidget(top_image, 0, Qt::AlignHCenter);

	main_layout->addWidget(&time_edit, 0, Qt::AlignHCenter);

	cond_time.setHMS(h.toInt(), m.toInt(), 0);
	setupTimer();
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

void ScenEvoTimeCondition::scaduta()
{
	emit condSatisfied();
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



ScenEvoDeviceCondition::ScenEvoDeviceCondition(int _item_id, const QDomNode &config_node)
{
	item_id = _item_id;

	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 0, 0, 0);
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
}

ScenEvoDeviceCondition::~ScenEvoDeviceCondition()
{
	delete device_cond;
}

void ScenEvoDeviceCondition::Apply()
{
	device_cond->OK();
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
	device_cond->reset();
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

