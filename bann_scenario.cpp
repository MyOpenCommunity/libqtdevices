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

#include <QDebug>
#include <QLabel>
#include <QTimerEvent>
#include <QDomNode>

#define PPTSCE_INTERVAL 1000


BannSimpleScenario::BannSimpleScenario(QWidget *parent, const QDomNode &config_node) :
	Bann2Buttons(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	initBanner(bt_global::skin->getImage("on"), QString(), getTextChild(config_node, "descr"));

	QString where = getTextChild(config_node, "where");
	dev = bt_global::add_device_to_cache(new ScenarioDevice(where));

	scenario_number = getTextChild(config_node, "what").toInt();

	connect(left_button, SIGNAL(clicked()), SLOT(activate()));
}

void BannSimpleScenario::activate()
{
	dev->activateScenario(scenario_number);
}


ScenarioModule::ScenarioModule(QWidget *parent, const QDomNode &config_node) :
	Bann4ButtonsIcon(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	initBanner(bt_global::skin->getImage("edit"), bt_global::skin->getImage("forward"),
		bt_global::skin->getImage("label"), bt_global::skin->getImage("start_prog"),
		bt_global::skin->getImage("del_scen"), bt_global::skin->getImage("on"),
		bt_global::skin->getImage("stop"), LOCKED, getTextChild(config_node, "descr"));

	QString where = getTextChild(config_node, "where");
	dev = bt_global::add_device_to_cache(new ScenarioDevice(where));

	scenario_number = getTextChild(config_node, "what").toInt();
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

ScenarioEvolved::ScenarioEvolved(QWidget *parent, const QDomNode &conf_node, QList<ScenEvoCondition*> c) :
	Bann3Buttons(parent), condList(c)
{
	item_id = getTextChild(conf_node, "itemID").toInt();

	current_condition = 0;

	serial_number = next_serial_number++;
	for (int i = 0; i < condList.size(); ++i)
	{
		ScenEvoCondition *co = condList.at(i);
		qDebug() << "connecting condition: " << co->getDescription();
		co->set_serial_number(serial_number);
		connect(co, SIGNAL(verificata()), this, SLOT(trig()));
		connect(co, SIGNAL(condSatisfied()), this, SLOT(trigOnStatusChanged()));
		connect(co, SIGNAL(okAll()), this, SLOT(saveAndApplyAll()));
		connect(co, SIGNAL(resetAll()), this, SLOT(resetAll()));
	}

#ifdef CONFIG_BTOUCH
	action = getElement(conf_node, "action/open").text();
	enabled = getTextChild(conf_node, "enable").toInt();
#else
	action = getElement(conf_node, "scen/action/open").text();
	enabled = getElement(conf_node, "scen/status").text().toInt();
#endif
	enable_icon = bt_global::skin->getImage("enable_scen");
	disable_icon = bt_global::skin->getImage("disable_scen");
	initBanner(enabled ? enable_icon : disable_icon, bt_global::skin->getImage("start"),
		bt_global::skin->getImage("program"), getTextChild(conf_node, "descr"));

	connect(left_button, SIGNAL(clicked()), SLOT(toggleAttivaScev()));
	connect(right_button, SIGNAL(clicked()), SLOT(configScev()));
	connect(center_button, SIGNAL(clicked()), SLOT(forzaScev()));
}

void ScenarioEvolved::toggleAttivaScev()
{
	qDebug("ScenarioEvolved::toggleAttivaScev");
	enabled = !enabled;
	left_button->setImage(enabled ? enable_icon : disable_icon);
#ifdef CONFIG_BTOUCH
	setCfgValue("enable", enabled, SCENARIO_EVOLUTO, serial_number);
#else
	setCfgValue("scen/status", enabled, item_id);
#endif
}

void ScenarioEvolved::configScev()
{
	qDebug("ScenarioEvolved::configScev");
	ScenEvoCondition *co = condList.at(current_condition);
	connect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	connect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	connect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
	co->showPage();
}

void ScenarioEvolved::forzaScev()
{
	// Forced trigger
	trig(true);
}

void ScenarioEvolved::nextCond()
{
	qDebug("ScenarioEvolved::nextCond()");
	ScenEvoCondition *old_cond = condList.at(current_condition);
	disconnect(old_cond, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	disconnect(old_cond, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	disconnect(old_cond, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));

	if (current_condition + 1 < static_cast<unsigned>(condList.size()))
	{
		++current_condition;
		ScenEvoCondition *cond = condList.at(current_condition);
		qDebug("cond = %p", cond);
		if (cond)
		{
			connect(cond, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
			connect(cond, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
			connect(cond, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
			cond->inizializza();
			cond->showPage();
		}
	}
	else
		current_condition = 0;
}

void ScenarioEvolved::prevCond()
{
	qDebug("ScenarioEvolved::prevCond()");
	ScenEvoCondition *old_cond = condList.at(current_condition);
	disconnect(old_cond, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	disconnect(old_cond, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	disconnect(old_cond, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));

	if (current_condition > 0)
	{
		--current_condition;
		ScenEvoCondition *cond = condList.at(current_condition);
		qDebug("cond = %p", cond);
		if (cond)
		{
			connect(cond, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
			connect(cond, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
			connect(cond, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
			cond->showPage();
		}
	}
}

void ScenarioEvolved::firstCond()
{
	qDebug("ScenarioEvolved::firstCond()");
	ScenEvoCondition *co = condList.at(current_condition);
	disconnect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
	current_condition = 0;
	emit pageClosed();
}

void ScenarioEvolved::saveAndApplyAll()
{
	qDebug("ScenarioEvolved::saveAndApplyAll()");
	for (int i = 0; i < condList.size(); ++i)
	{
		ScenEvoCondition *co = condList.at(i);
		co->Apply();
		co->save();
	}
}

void ScenarioEvolved::resetAll()
{
	qDebug("ScenarioEvolved::resetAll()");
	for (int i = 0; i < condList.size(); ++i)
	{
		ScenEvoCondition *co = condList.at(i);
		co->reset();
	}
	emit pageClosed();
}

void ScenarioEvolved::trigOnStatusChanged()
{
	qDebug("ScenarioEvolved::trigOnStatusChanged()");
	for (int i = 0; i < condList.size(); ++i)
		if (qobject_cast<ScenEvoTimeCondition*>(condList.at(i)))
			return;
	trig();
}

void ScenarioEvolved::trig(bool forced)
{
	if (action.isEmpty())
	{
		qDebug("ScenarioEvolved::trig(), act = NULL, non faccio niente");
		return;
	}

	if (!forced)
	{
		if (!enabled)
		{
			qDebug("ScenarioEvolved::trig(), non abilitato, non faccio niente");
			return;
		}
		for (int i = 0; i < condList.size(); ++i)
		{
			ScenEvoCondition *co = condList.at(i);
			if (!co->isTrue())
			{
				qDebug("Condizione %p (%s), non verificata, non faccio niente",
						co, co->getDescription());
				return;
			}
		}
	}

	qDebug() << "ScenarioEvolved::trig(), act = " << action;
	sendFrame(action);
}

void ScenarioEvolved::inizializza(bool forza)
{
	qDebug("ScenarioEvolved::inizializza()");
	for (int i = 0; i < condList.size(); ++i)
	{
		ScenEvoCondition *co = condList.at(i);
		qDebug() << "Ciclo n. " << i << co->getDescription();
		co->inizializza();
	}
}

ScenarioEvolved::~ScenarioEvolved()
{
	while (!condList.isEmpty())
		delete condList.takeFirst();
}


ScheduledScenario::ScheduledScenario(QWidget *parent, const QDomNode &config_node) :
	Bann4Buttons(parent)
{
	initBanner(bt_global::skin->getImage("disable_scen"), bt_global::skin->getImage("stop"),
		bt_global::skin->getImage("start"), bt_global::skin->getImage("enable_scen"),
		getTextChild(config_node, "descr"));
	connect(left_button, SIGNAL(clicked()), SLOT(enable()));
	connect(center_left_button, SIGNAL(clicked()), SLOT(start()));
	connect(center_right_button, SIGNAL(clicked()), SLOT(stop()));
	connect(right_button, SIGNAL(clicked()), SLOT(disable()));

	QList<QString *> actions;
	actions << &action_enable << &action_start << &action_stop << &action_disable;
	// these must be in the same position as the list above!
	QList<BtButton *> buttons;
	buttons << left_button << center_left_button << center_right_button << right_button;

#ifdef CONFIG_BTOUCH
	QList<QString> nodes;
	// these must be in the order: unable, start, stop, disable (the same given by actions above)
	nodes << "unable" << "start" << "stop" << "disable";
	for (int i = 0; i < nodes.size(); ++i)
	{
		QDomNode node = getChildWithName(config_node, nodes[i]);
		if (!node.isNull() && getTextChild(node, "value").toInt())
			*actions[i] = getTextChild(node, "open");
		else
			deleteButton(buttons[i]);
	}
#else
	QList<QString> names;
	// these must be in the order: attiva, start, stop, disattiva (the same given by actions above)
	names << "attiva" << "start" << "stop" << "disattiva";
	for (int i = 0; i < names.size(); ++i)
	{
		// look for a node called where{attiva,disattiva,start,stop} to decide if the action is enabled
		QDomElement where = getElement(config_node, QString("schedscen/where") + names[i]);
		if (!where.isNull())
		{
			QDomElement what = getElement(config_node, QString("schedscen/what") + names[i]);
			*actions[i] = QString("*15*%1*%2##").arg(what.text()).arg(where.text());
		}
		else
			deleteButton(buttons[i]);
	}
#endif
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

