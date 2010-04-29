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


#include "scenario.h"
#include "bann_scenario.h"
#include "scenevocond.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "btmain.h" // bt_global::btmain
#include "bannercontent.h"
#include "main.h"
#include "skinmanager.h" //SkinContext

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>

#ifdef CONFIG_BTOUCH
enum BannerType
{
	BANN_SCENARIO = 4,
	SCENARIO_MODULE = 29,
	PPT_SCENARIO = 43,
	SCENARIO_EVOLUTO = 38,
	SCENARIO_SCHEDULATO = 39,
};
#else
enum BannerType
{
	BANN_SCENARIO = 1001,
	SCENARIO_MODULE = 1002,
	PPT_SCENARIO = 1003,
	SCENARIO_EVOLUTO = 9001,
	SCENARIO_SCHEDULATO = 9002,
};
#endif

Scenario::Scenario(const QDomNode &config_node)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
	section_id = getTextChild(config_node, "id").toInt();
}

int Scenario::sectionId() const
{
	return section_id;
}

banner *Scenario::getBanner(const QDomNode &item_node)
{
	SkinContext context(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	QString descr = getTextChild(item_node, "descr");
	QString where = getTextChild(item_node, "where");
	int oid = getTextChild(item_node, "openserver_id").toInt();

	banner *b = 0;
	switch (id)
	{
	case BANN_SCENARIO:
		b = new BannSimpleScenario(getTextChild(item_node, "what").toInt(), descr, where, oid);
		break;
	case SCENARIO_MODULE:
		b = new ScenarioModule(getTextChild(item_node, "what").toInt(), descr, where, oid);
		break;
	case SCENARIO_EVOLUTO:
	{
		ScenEvoTimeCondition *time_cond = 0;
		ScenEvoDeviceCondition *device_cond = 0;
		QString action;
		bool enabled;

#ifdef CONFIG_BTOUCH
		int item_id = 0;
		QDomNode time_node = getChildWithName(item_node, "condH");

		if (!time_node.isNull() && getTextChild(time_node, "value").toInt())
			time_cond = new ScenEvoTimeCondition(item_id, time_node);

		QDomNode device_node = getChildWithName(item_node, "condDevice");
		if (!device_node.isNull() && getTextChild(device_node, "value").toInt())
			device_cond = new ScenEvoDeviceCondition(0, device_node);

		action = getElement(item_node, "action/open").text();
		enabled = getTextChild(item_node, "enable").toInt();
#else
		int item_id = getTextChild(item_node, "itemID").toInt();
		QDomNode time_node = getElement(item_node, "scen/time");
		if (!time_node.isNull() && getTextChild(time_node, "status").toInt())
			time_cond = new ScenEvoTimeCondition(item_id, time_node);

		QDomNode device_node = getElement(item_node, "scen/device");
		if (!device_node.isNull() && getTextChild(device_node, "status").toInt())
			device_cond = new ScenEvoDeviceCondition(item_id, device_node);

		action = getElement(item_node, "scen/action/open").text();
		enabled = getElement(item_node, "scen/status").text().toInt();
#endif
		if (time_cond)
			QObject::connect(bt_global::btmain, SIGNAL(resettimer()), time_cond, SLOT(setupTimer()));

		b = new ScenarioEvolved(item_id, descr, action, enabled, time_cond, device_cond);
	}
		break;
	case SCENARIO_SCHEDULATO:
	{
		// prepare a vector of 4 empty actions, which will be used to init ScheduledScenario
		// 4 actions are: enable, start, stop, disable. Order is important!
		QStringList actions;
		for (int i = 0; i < 4; ++i)
			actions << QString();

		QStringList names;
#ifdef CONFIG_BTOUCH
		// these must be in the order: unable, start, stop, disable (the same given by actions above)
		names << "unable" << "start" << "stop" << "disable";
		for (int i = 0; i < names.size(); ++i)
		{
			QDomNode node = getChildWithName(item_node, names[i]);
			if (!node.isNull() && getTextChild(node, "value").toInt())
				actions[i] = getTextChild(node, "open");
		}
#else
		// these must be in the order: attiva, start, stop, disattiva (the same given by actions above)
		names << "attiva" << "start" << "stop" << "disattiva";
		for (int i = 0; i < names.size(); ++i)
		{
			// look for a node called where{attiva,disattiva,start,stop} to decide if the action is enabled
			QDomElement where = getElement(item_node, QString("schedscen/where") + names[i]);
			if (!where.isNull())
			{
				QDomElement what = getElement(item_node, QString("schedscen/what") + names[i]);
				actions[i] = QString("*15*%1*%2##").arg(what.text()).arg(where.text());
			}
		}
#endif
		b = new ScheduledScenario(actions[0], actions[1], actions[2], actions[3], descr);
		break;
	}
	case PPT_SCENARIO:
	{
		QString what = getTextChild(item_node, "what");
		if (!what.isEmpty())
			where = what + "*" + where;
		PPTSce *bann = new PPTSce(descr, where, oid);
		b = bann;
	}
		break;
	}

	if (b)
		b->setId(id);
	return b;
}

void Scenario::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode& item, getChildren(config_node, "item"))
	{
		if (banner *b = getBanner(item))
		{
			page_content->appendBanner(b);
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}
		else
		{
			int id = getTextChild(item, "id").toInt();
			qFatal("Type of item %d not handled on scenario page!", id);
		}
	}
}

