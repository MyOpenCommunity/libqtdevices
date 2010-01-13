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

#include <QDir>
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
			is_editing = false;
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


#if 1

bannScenario::bannScenario(QWidget *parent, QString where, QString IconaSx) : bannOnSx(parent)
{
	SetIcons(IconaSx, 1);
	setAddress(where);
	connect(this,SIGNAL(click()),this,SLOT(Attiva()));
}

void bannScenario::Attiva()
{
	char cosa[15];
	QByteArray buf_addr = getAddress().toAscii();
	strncpy(cosa, buf_addr.constData(),sizeof(cosa));
	if (strstr(cosa,"*"))
	{
		memset(index(cosa,'*'),'\000',sizeof(cosa)-(index(cosa,'*')-cosa));
		sendFrame(createMsgOpen("0", cosa, strstr(buf_addr.constData(),"*")+1));
	}
}


gesModScen::gesModScen(QWidget *parent, QString where, QString IcoSx, QString IcoDx, QString IcoCsx,
	QString IcoCdx, QString IcoDes, QString IcoSx2, QString IcoDx2) :  bann4tasLab(parent)
{
	icon_on = IcoSx;
	icon_stop = IcoSx2;
	icon_info = IcoDx;
	icon_no_info = IcoDx2;

	SetIcons(IcoSx, IcoDx, IcoCsx, IcoCdx, IcoDes);
	nascondi(BUT3);
	nascondi(BUT4);

	qDebug() << "gesModScen::gesModScen(): indirizzo =" << where;

	int pos = where.indexOf('*');
	if (pos != -1)
	{
		cosa = where.left(pos);
		dove = where.mid(pos + 1);
	}
	sendInProgr = 0;
	in_progr = 0;

	setAddress(dove);
	impostaAttivo(2);
	connect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
	connect(this,SIGNAL(dxClick()),this,SLOT(enterInfo()));
	connect(this,SIGNAL(csxClick()),this,SLOT(startProgScen()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(cancScen()));

	// Crea o preleva il dispositivo dalla cache
	dev = bt_global::devices_cache.get_modscen_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void gesModScen::attivaScenario()
{
	dev->sendFrame("*0*" + cosa + "*" + dove + "##");
}

void gesModScen::enterInfo()
{
	nascondi(ICON);
	mostra(BUT4);
	mostra(BUT3);
	SetIcons(1, icon_no_info);
	disconnect(this,SIGNAL(dxClick()),this,SLOT(enterInfo()));
	connect(this,SIGNAL(dxClick()),this,SLOT(exitInfo()));
	Draw();
}

void gesModScen::exitInfo()
{
	mostra(ICON);
	nascondi(BUT4);
	nascondi(BUT3);
	SetIcons(1, icon_info);
	connect(this,SIGNAL(dxClick()),this,SLOT(enterInfo()));
	disconnect(this,SIGNAL(dxClick()),this,SLOT(exitInfo()));
	Draw();
}

void gesModScen::startProgScen()
{
	dev->sendFrame("*0*40#" + cosa + "*" + dove + "##");
	sendInProgr = 1;
}

void gesModScen::stopProgScen()
{
	dev->sendFrame("*0*41#" + cosa + "*" + dove + "##");
	sendInProgr = 0;
}

void gesModScen::cancScen()
{
	dev->sendFrame("*0*42#" + cosa + "*" + dove + "##");
}

void gesModScen::status_changed(QList<device_status*> sl)
{
	stat_var curr_status(stat_var::STAT);
	qDebug("gesModScen::status_changed");
	bool aggiorna = false;

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::MODSCEN:
			qDebug("Modscen status change");
			ds->read((int)device_status_modscen::STAT_INDEX, curr_status);
			switch(curr_status.get_val())
			{
			case device_status_modscen::PROGRAMMING_START:
				qDebug("Programming start");
				if (sendInProgr)
				{
					SetIcons(0, icon_stop);
					disconnect(this,SIGNAL(sxClick()), this,SLOT(attivaScenario()));
					connect(this,SIGNAL(sxClick()), this,SLOT(stopProgScen()));
					in_progr = 0;
				}
				else
				{
					in_progr = 1;
					exitInfo();
				}
				aggiorna = 1;
				break;
			case device_status_modscen::LOCKED:
				qDebug("Locked");
				bloccato = 1;
				exitInfo();
				aggiorna = 1;
				break;
			case device_status_modscen::BUSY:
				qDebug("Busy");
				in_progr = 1;
				exitInfo();
				aggiorna = 1;
				break;
			case device_status_modscen::PROGRAMMING_STOP:
				qDebug("Programming stop");
				SetIcons(0, icon_on);
				disconnect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
				connect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
				disconnect(this,SIGNAL(sxClick()),this,SLOT(stopProgScen()));
				aggiorna = 1;
				in_progr = 0;
				break;
			case device_status_modscen::UNLOCKED:
				qDebug("Unlocked");
				SetIcons(0, icon_on);
				disconnect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
				connect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
				disconnect(this,SIGNAL(sxClick()),this,SLOT(stopProgScen()));
				aggiorna = 1;
				bloccato = 0;
				break;
			default:
				qDebug("Unknown status %d", curr_status.get_val());
			}
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}

	if (aggiorna)
	{
		if (bloccato || in_progr)
			nascondi(BUT2);
		else
			mostra(BUT2);
		Draw();
	}
}

void gesModScen::inizializza(bool forza)
{
	nascondi(BUT2);
}

#endif


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


#if 0
// DELETE
scenSched::scenSched(QWidget *parent, QString Icona1, QString Icona2, QString Icona3, QString Icona4,
	QString act_enable, QString act_disable, QString act_start, QString act_stop) : bann4But(parent)
{
	qDebug("scenSched::scenSched()");
	action_enable = act_enable;
	action_disable = act_disable;
	action_start = act_start;
	action_stop = act_stop;

	SetIcons(Icona1, Icona3, Icona4, Icona2);
	if (!act_enable.isNull())
	{
		// sx
		qDebug("BUT1 attivo");
		connect(this, SIGNAL(sxClick()), this, SLOT(enable()));
	}
	else
		nascondi(BUT1);
	if (!act_start.isNull())
	{
		// csx
		qDebug("BUT3 attivo");
		connect(this, SIGNAL(csxClick()), this, SLOT(start()));
	}
	else
		nascondi(BUT3);
	if (!act_stop.isNull())
	{
		// cdx
		qDebug("BUT4 attivo");
		connect(this, SIGNAL(cdxClick()), this, SLOT(stop()));
	}
	else
		nascondi(BUT4);
	if (!act_disable.isNull())
	{
		// dx
		qDebug("BUT2 attivo");
		connect(this, SIGNAL(dxClick()), this, SLOT(disable()));
	}
	else
		nascondi(BUT2);
	Draw();
}

void scenSched::enable()
{
	qDebug("scenSched::enable()");
	sendFrame(action_enable);
	Draw();
}

void scenSched::start()
{
	qDebug("scenSched::start()");
	sendFrame(action_start);
	Draw();
}

void scenSched::stop()
{
	qDebug("scenSched::stop()");
	sendFrame(action_stop);
	Draw();
}

void scenSched::disable()
{
	qDebug("scenSched::disable()");
	sendFrame(action_disable);
	Draw();
}

void scenSched::Draw()
{
	// Icon[0] => left button
	// pressIcon[0] => pressed left button
	// Icon[1] => center left button 
	// pressIcon[1] => pressed center left button
	// Icon[3] => center right button
	// pressIcon[3] => pressed center right button
	// Icon[2] => right button
	// pressIcon[2] => pressed right button
	qDebug("scenSched::Draw()");
	if (sxButton && Icon[0])
	{
		sxButton->setPixmap(*Icon[0]);
		if (pressIcon[0])
			sxButton->setPressedPixmap(*pressIcon[0]);
	}
	if (csxButton && Icon[1])
	{
		csxButton->setPixmap(*Icon[1]);
		if (pressIcon[1])
			csxButton->setPressedPixmap(*pressIcon[1]);
	}
	if (cdxButton && Icon[3])
	{
		cdxButton->setPixmap(*Icon[3]);
		if (pressIcon[3])
			cdxButton->setPressedPixmap(*pressIcon[3]);
	}
	if (dxButton && Icon[2])
	{
		dxButton->setPixmap(*Icon[2]);
		if (pressIcon[2])
			dxButton->setPressedPixmap(*pressIcon[2]);
	}
	if (BannerText)
	{
		BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		BannerText->setFont(bt_global::font->get(FontManager::BANNERTEXT));
		BannerText->setText(qtesto);
	}
}
#endif


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

