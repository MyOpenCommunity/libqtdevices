#include "bann_scenario.h"
#include "device.h"
#include "scenevocond.h"
#include "btbutton.h"
#include "generic_functions.h" // setCfgValue
#include "fontmanager.h" // bt_global::font
#include "devices_cache.h" // bt_global::devices_cache
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
	BannLeft(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	initBanner(bt_global::skin->getImage("on"), getTextChild(config_node, "descr"));

	QString where = getTextChild(config_node, "where");
	dev = bt_global::add_device_to_cache(new ScenarioDevice(where));

	scenario_number = getTextChild(config_node, "what").toInt();

	connect(left_button, SIGNAL(clicked()), SLOT(activate()));
}

void BannSimpleScenario::activate()
{
	dev->activateScenario(scenario_number);
}


ModifyScenario::ModifyScenario(QWidget *parent, const QDomNode &config_node) :
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

void ModifyScenario::inizializza(bool forza)
{
	dev->requestStatus();
}

void ModifyScenario::activate()
{
	dev->activateScenario(scenario_number);
}

void ModifyScenario::editScenario()
{
	is_editing = !is_editing;
	setState(is_editing ? EDIT_VIEW : UNLOCKED);
}

void ModifyScenario::startEditing()
{
	dev->startProgramming(scenario_number);
}

void ModifyScenario::deleteScenario()
{
	dev->deleteScenario(scenario_number);
}

void ModifyScenario::changeLeftFunction(const char *slot)
{
	left_button->disconnect(SIGNAL(clicked()));
	connect(left_button, SIGNAL(clicked()), slot);
}

void ModifyScenario::stopEditing()
{
	dev->stopProgramming(scenario_number);
}

void ModifyScenario::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case ScenarioDevice::DIM_LOCK:
			setState(it.value().toBool() ? LOCKED : UNLOCKED);
			// TODO: it seems reasonable that when LOCKED the left function returns
			// to activate(), since the device won't care about our frames.
			// Ask Agresta.
			// When UNLOCKED, however, the left function mustn't be touched
			//changeLeftFunction(SLOT(activate()));
			break;
		case ScenarioDevice::DIM_START:
		{
			Q_ASSERT_X(it.value().canConvert<ScenarioProgrammingStatus>(), "ModifyScenario::status_changed",
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
					setEditingState(EDIT_INACTIVE);
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


int scenEvo::next_serial_number = 1;

scenEvo::scenEvo(QWidget *parent, const QDomNode &conf_node, QList<scenEvo_cond*> c) :
	Bann3Buttons(parent), condList(c)
{
	current_condition = 0;

	serial_number = next_serial_number++;
	for (int i = 0; i < condList.size(); ++i)
	{
		scenEvo_cond *co = condList.at(i);
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
	enabled = getTextChild(conf_node, "scen/status").toInt();
#endif
	enable_icon = bt_global::skin->getImage("enable_scen");
	disable_icon = bt_global::skin->getImage("disable_scen");
	initBanner(enabled ? enable_icon : disable_icon, bt_global::skin->getImage("start"),
		bt_global::skin->getImage("program"), getTextChild(conf_node, "descr"));

	connect(left_button, SIGNAL(clicked()), SLOT(toggleAttivaScev()));
	connect(right_button, SIGNAL(clicked()), SLOT(configScev()));
	connect(center_button, SIGNAL(clicked()), SLOT(forzaScev()));
}

void scenEvo::toggleAttivaScev()
{
	qDebug("scenEvo::toggleAttivaScev");
	enabled = !enabled;
	left_button->setImage(enabled ? enable_icon : disable_icon);
	const char *s = enabled ? "1" : "0";
#ifdef CONFIG_BTOUCH
	// TODO: how to save into config file?
	setCfgValue("enable", s, SCENARIO_EVOLUTO, serial_number);
#endif
}

void scenEvo::configScev()
{
	qDebug("scenEvo::configScev");
	scenEvo_cond *co = condList.at(current_condition);
	connect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	connect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	connect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
	co->showPage();
}

void scenEvo::forzaScev()
{
	// Forced trigger
	trig(true);
}

void scenEvo::nextCond()
{
	qDebug("scenEvo::nextCond()");
	scenEvo_cond *old_cond = condList.at(current_condition);
	disconnect(old_cond, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	disconnect(old_cond, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	disconnect(old_cond, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));

	if (current_condition + 1 < static_cast<unsigned>(condList.size()))
	{
		++current_condition;
		scenEvo_cond *cond = condList.at(current_condition);
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

void scenEvo::prevCond()
{
	qDebug("scenEvo::prevCond()");
	scenEvo_cond *old_cond = condList.at(current_condition);
	disconnect(old_cond, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	disconnect(old_cond, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	disconnect(old_cond, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));

	if (current_condition > 0)
	{
		--current_condition;
		scenEvo_cond *cond = condList.at(current_condition);
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

void scenEvo::firstCond()
{
	qDebug("scenEvo::firstCond()");
	scenEvo_cond *co = condList.at(current_condition);
	disconnect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
	current_condition = 0;
	emit pageClosed();
}

void scenEvo::saveAndApplyAll()
{
	qDebug("scenEvo::saveAndApplyAll()");
	for (int i = 0; i < condList.size(); ++i)
	{
		scenEvo_cond *co = condList.at(i);
		co->Apply();
		co->save();
	}
}

void scenEvo::resetAll()
{
	qDebug("scenEvo::resetAll()");
	for (int i = 0; i < condList.size(); ++i)
	{
		scenEvo_cond *co = condList.at(i);
		co->reset();
	}
	emit pageClosed();
}

void scenEvo::trigOnStatusChanged()
{
	qDebug("scenEvo::trigOnStatusChanged()");
	for (int i = 0; i < condList.size(); ++i)
	{
		scenEvo_cond *co = condList.at(i);
		if (co->hasTimeCondition)
			return;
	}
	trig();
}

void scenEvo::trig(bool forced)
{
	if (action.isEmpty())
	{
		qDebug("scenEvo::trig(), act = NULL, non faccio niente");
		return;
	}

	if (!forced)
	{
		if (!enabled)
		{
			qDebug("scenEvo::trig(), non abilitato, non faccio niente");
			return;
		}
		for (int i = 0; i < condList.size(); ++i)
		{
			scenEvo_cond *co = condList.at(i);
			if (!co->isTrue())
			{
				qDebug("Condizione %p (%s), non verificata, non faccio niente",
						co, co->getDescription());
				return;
			}
		}
	}

	qDebug() << "scenEvo::trig(), act = " << action;
	sendFrame(action);
}

void scenEvo::inizializza(bool forza)
{
	qDebug("scenEvo::inizializza()");
	for (int i = 0; i < condList.size(); ++i)
	{
		scenEvo_cond *co = condList.at(i);
		qDebug() << "Ciclo n. " << i << co->getDescription();
		co->inizializza();
	}
}

scenEvo::~scenEvo()
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

