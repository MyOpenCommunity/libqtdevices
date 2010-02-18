#include "bann_scenario.h"
#include "scenevocond.h"
#include "btbutton.h"
#include "generic_functions.h" // setCfgValue
#include "fontmanager.h" // bt_global::font
#include "devices_cache.h" // bt_global::add_device_to_cache
#include "automation_device.h"
#include "skinmanager.h" // SkinContext, bt_global::skin
#include "scenario_device.h"
#include "xml_functions.h" // getTextChild
#include "scenevomanager.h"

#include <QDebug>
#include <QLabel>
#include <QTimerEvent>
#include <QDomNode>

#define PPTSCE_INTERVAL 1000


BannSimpleScenario::BannSimpleScenario(int scenario, const QString &descr, const QString &where) :
	Bann2Buttons(0)
{
	initBanner(bt_global::skin->getImage("on"), QString(), descr);
	dev = bt_global::add_device_to_cache(new ScenarioDevice(where));
	scenario_number = scenario;
	connect(left_button, SIGNAL(clicked()), SLOT(activate()));
}

void BannSimpleScenario::activate()
{
	dev->activateScenario(scenario_number);
}


ScenarioModule::ScenarioModule(int scenario, const QString &descr, const QString &where) :
	Bann4ButtonsIcon(0)
{
	initBanner(bt_global::skin->getImage("edit"), bt_global::skin->getImage("forward"),
		bt_global::skin->getImage("label"), bt_global::skin->getImage("start_prog"),
		bt_global::skin->getImage("del_scen"), bt_global::skin->getImage("on"),
		bt_global::skin->getImage("stop"), LOCKED, descr);

	dev = bt_global::add_device_to_cache(new ScenarioDevice(where));

	scenario_number = scenario;
	is_editing = false;
	connect(left_button, SIGNAL(clicked()), SLOT(activate()));
	connect(right_button, SIGNAL(clicked()), SLOT(editScenario()));
	connect(center_left_button, SIGNAL(clicked()), SLOT(startEditing()));
	connect(center_right_button, SIGNAL(clicked()), SLOT(deleteScenario()));
	connect(dev, SIGNAL(status_changed(StatusList)), SLOT(status_changed(const StatusList &)));
}

void ScenarioModule::inizializza(bool forza)
{
	dev->requestStatus();
}

void ScenarioModule::activate()
{
	dev->activateScenario(scenario_number);
}

void ScenarioModule::editScenario()
{
	is_editing = !is_editing;
	setState(is_editing ? EDIT_VIEW : UNLOCKED);
}

void ScenarioModule::startEditing()
{
	dev->startProgramming(scenario_number);
}

void ScenarioModule::deleteScenario()
{
	dev->deleteScenario(scenario_number);
}

void ScenarioModule::changeLeftFunction(const char *slot)
{
	left_button->disconnect(SIGNAL(clicked()));
	connect(left_button, SIGNAL(clicked()), slot);
}

void ScenarioModule::stopEditing()
{
	dev->stopProgramming(scenario_number);
}

void ScenarioModule::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case ScenarioDevice::DIM_LOCK:
			setState(it.value().toBool() ? LOCKED : UNLOCKED);
			is_editing = false;
			break;
		case ScenarioDevice::DIM_START:
		{
			Q_ASSERT_X(it.value().canConvert<ScenarioProgrammingStatus>(), "ScenarioModule::status_changed",
				"Cannot convert values in DIM_START");
			ScenarioProgrammingStatus val = it.value().value<ScenarioProgrammingStatus>();
			if (val.first)
			{
				if (val.second == scenario_number)
				{
					setEditingState(EDIT_ACTIVE);
					changeLeftFunction(SLOT(stopEditing()));
				}
				else
				{
					setState(LOCKED);
					changeLeftFunction(SLOT(activate()));
				}
			}
			else
			{
				if (val.second == scenario_number)
				{
					setState(UNLOCKED);
					setEditingState(EDIT_INACTIVE);
					is_editing = false;
				}
				else
					setState(UNLOCKED);

				changeLeftFunction(SLOT(activate()));
			}
		}
			break;
		}
		++it;
	}
}


int ScenarioEvolved::next_serial_number = 1;

ScenarioEvolved::ScenarioEvolved(int _item_id, QString descr, QString _action, bool _enabled,
	ScenEvoTimeCondition *tcond,  ScenEvoDeviceCondition *dcond) : Bann3Buttons(0)
{
	time_cond = tcond;
	device_cond = dcond;
	item_id = _item_id;
	action = _action;
	enabled = _enabled;
	serial_number = next_serial_number++;
	if (time_cond)
	{
		time_cond->set_serial_number(serial_number);
		connect(time_cond, SIGNAL(condSatisfied()), SLOT(trig()));
	}

	if (device_cond)
	{
		device_cond->set_serial_number(serial_number);
		connect(device_cond, SIGNAL(condSatisfied()), SLOT(trigOnStatusChanged()));
	}

	ScenEvoManager *p = new ScenEvoManager(time_cond, device_cond);
	connectButtonToPage(right_button, p);
	connect(p, SIGNAL(reset()), SLOT(reset()));
	connect(p, SIGNAL(save()), SLOT(save()));

	enable_icon = bt_global::skin->getImage("enable_scen");
	disable_icon = bt_global::skin->getImage("disable_scen");
	initBanner(enabled ? enable_icon : disable_icon, bt_global::skin->getImage("start"),
		bt_global::skin->getImage("program"), descr);

	connect(left_button, SIGNAL(clicked()), SLOT(toggleActivation()));
	connect(center_button, SIGNAL(clicked()), SLOT(forceTrig()));
}

void ScenarioEvolved::reset()
{
	if (device_cond)
		device_cond->reset();
	if (time_cond)
		time_cond->reset();
}

void ScenarioEvolved::save()
{
	if (device_cond)
	{
		device_cond->Apply();
		device_cond->save();
	}

	if (time_cond)
	{
		time_cond->Apply();
		time_cond->save();
	}
}

void ScenarioEvolved::toggleActivation()
{
	enabled = !enabled;
	left_button->setImage(enabled ? enable_icon : disable_icon);
#ifdef CONFIG_BTOUCH
	setCfgValue("enable", enabled, SCENARIO_EVOLUTO, serial_number);
#else
	setCfgValue("scen/status", enabled, item_id);
#endif
}

void ScenarioEvolved::forceTrig()
{
	trig(true);
}

void ScenarioEvolved::trigOnStatusChanged()
{
	if (time_cond)
		return;

	trig();
}

void ScenarioEvolved::trig(bool forced)
{
	if (action.isEmpty())
	{
		qDebug("ScenarioEvolved::trig(), act = NULL, skip");
		return;
	}

	if (!forced)
	{
		if (!enabled)
		{
			qDebug("ScenarioEvolved::trig() not enabled");
			return;
		}

		if (device_cond && !device_cond->isTrue())
		{
			qDebug("Device Condition not verified");
			return;
		}
	}

	qDebug() << "ScenarioEvolved::trig(), act = " << action;
	sendFrame(action);
}

void ScenarioEvolved::inizializza(bool forza)
{
	Q_UNUSED(forza)
	if (device_cond)
		device_cond->inizializza();
}


ScheduledScenario::ScheduledScenario(const QString &enable, const QString &start, const QString &stop, const QString &disable, const QString &descr) :
	Bann4Buttons(0),
	action_enable(enable),
	action_disable(disable),
	action_start(start),
	action_stop(stop)
{
	QString en_icon;
	if (!action_enable.isEmpty())
	{
		en_icon = bt_global::skin->getImage("enable_scen");
		connect(left_button, SIGNAL(clicked()), SLOT(enable()));
	}
	QString dis_icon;
	if (!action_disable.isEmpty())
	{
		dis_icon = bt_global::skin->getImage("disable_scen");
		connect(right_button, SIGNAL(clicked()), SLOT(disable()));
	}
	QString start_icon;
	if (!action_start.isEmpty())
	{
		start_icon = bt_global::skin->getImage("start");
		connect(center_left_button, SIGNAL(clicked()), SLOT(start()));
	}
	QString stop_icon;
	if (!action_stop.isEmpty())
	{
		stop_icon =bt_global::skin->getImage("stop");
		connect(center_right_button, SIGNAL(clicked()), SLOT(stop()));
	}

	initBanner(dis_icon, stop_icon, start_icon, en_icon, descr);
}

void ScheduledScenario::enable()
{
	sendFrame(action_enable);
}

void ScheduledScenario::start()
{
	sendFrame(action_start);
}

void ScheduledScenario::stop()
{
	sendFrame(action_stop);
}

void ScheduledScenario::disable()
{
	sendFrame(action_disable);
}


PPTSce::PPTSce(QWidget *parent, QString where, int cid) : bann4But(parent)
{
	dev = bt_global::add_device_to_cache(new PPTSceDevice(where));
	connect(this, SIGNAL(sxClick()), dev, SLOT(turnOff()));
	connect(this, SIGNAL(dxClick()), dev, SLOT(turnOn()));

	// For csx e cdx buttons we have to send a frame every X mseconds when the
	// button is down and send another frame when the button is raised.
	increase_timer = 0;
	decrease_timer = 0;
	connect(this, SIGNAL(csxPressed()), SLOT(startIncrease()));
	connect(this, SIGNAL(cdxPressed()), SLOT(startDecrease()));
	connect(this, SIGNAL(csxReleased()), SLOT(stop()));
	connect(this, SIGNAL(cdxReleased()), SLOT(stop()));

	SkinContext context(cid);
	QString img_on = bt_global::skin->getImage("pptsce_on");
	QString img_off = bt_global::skin->getImage("pptsce_off");
	QString img_inc = bt_global::skin->getImage("pptsce_increase");
	QString img_decr = bt_global::skin->getImage("pptsce_decrease");
	SetIcons(img_off, img_on, img_inc, img_decr);
	Draw();
}

void PPTSce::startIncrease()
{
	if (!increase_timer)
	{
		dev->increase();
		increase_timer = startTimer(PPTSCE_INTERVAL);
	}
}

void PPTSce::startDecrease()
{
	if (!decrease_timer)
	{
		dev->decrease();
		decrease_timer = startTimer(PPTSCE_INTERVAL);
	}
}

void PPTSce::timerEvent(QTimerEvent *e)
{
	if (e->timerId() == increase_timer)
		dev->increase();
	else
		dev->decrease();
}

void PPTSce::stop()
{
	if (increase_timer)
	{
		killTimer(increase_timer);
		increase_timer = 0;
		dev->stop();
	}
	if (decrease_timer)
	{
		killTimer(decrease_timer);
		decrease_timer = 0;
		dev->stop();
	}
}

