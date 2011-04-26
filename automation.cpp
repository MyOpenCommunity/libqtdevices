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


#include "automation.h"
#include "bann_automation.h"
#include "actuators.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "main.h" // AUTOMATION
#include "skinmanager.h" // SkinContext
#include "bannerfactory.h"

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>

#ifdef CONFIG_TS_3_5
enum BannerType
{
	SECURE_AUTOMATIC_ACTUATOR = 8,
	INTERBLOCKED_ACTUATOR = 2,
	SIMPLE_ACTUATOR = 0,
	DOOR_LOCK_VCT = 13,
	PPT_STAT_AUTO = 42,
	ACTUATOR_GROUP = 10,
	DOOR_LOCK = 11,
	GATE_LIGHTING_ACT = 40,
	GATE_VCT_ACT =  41
};
#else
enum BannerType
{
	SECURE_AUTOMATIC_ACTUATOR = 3002,
	INTERBLOCKED_ACTUATOR = 3001,
	SIMPLE_ACTUATOR = 3007,
	DOOR_LOCK_VCT = 3011,
	PPT_STAT_AUTO = 3012,
	ACTUATOR_GROUP = 3014,
	DOOR_LOCK = 3010,
	GATE_LIGHTING_ACT = 3005,
	GATE_VCT_ACT = 3006
};
#endif


// TODO remove after debugging is complete
namespace
{
	inline PullMode getPullMode(const QDomNode &node)
	{
		QDomNode element = getChildWithName(node, "pul");
		Q_ASSERT_X(!element.isNull(), "getPullMode", qPrintable(QString("Pull device node %1 without <pul> child").arg(getTextChild(node, "id"))));
		bool ok;
		int value = element.toElement().text().toInt(&ok);
		Q_ASSERT_X(ok && (value == 0 || value == 1), "getPullMode", qPrintable(QString("Pull device %1 with invalid <pul> child").arg(getTextChild(node, "id"))));

		return value ? PULL : NOT_PULL;
	}
}


Automation::Automation(const QDomNode &config_node)
{
	buildPage(getTextChild(config_node, "descr"));
#ifdef LAYOUT_TS_3_5
	setSpacing(5);
#endif
	loadItems(config_node);
}

int Automation::sectionId() const
{
	return AUTOMATION;
}

banner *Automation::getBanner(const QDomNode &item_node)
{
	int id = getTextChild(item_node, "id").toInt();
	SkinContext ctx(getTextChild(item_node, "cid").toInt());
	QString where = getTextChild(item_node, "where");
	QString descr = getTextChild(item_node, "descr");
	int oid = getTextChild(item_node, "openserver_id").toInt();

	banner *b = 0;
	switch (id)
	{
	case SECURE_AUTOMATIC_ACTUATOR:
		b = new SecureInterblockedActuator(descr, where, oid, getPullMode(item_node));
		break;
	case INTERBLOCKED_ACTUATOR:
		b = new InterblockedActuator(descr, where, oid, getPullMode(item_node));
		break;
	case SIMPLE_ACTUATOR:
		b = new SingleActuator(descr, where, oid, getPullMode(item_node));
		break;
	case DOOR_LOCK_VCT:
	{
		QDomNode addresses = getElement(item_node, "addresses");
		where = getTextChild(addresses, "dev") + getTextChild(addresses, "where");
		b = new ButtonActuator(descr, where, VCT_LOCK, oid, NOT_PULL);
		break;
	}
	case ACTUATOR_GROUP:
	{
		QStringList addresses;
#ifdef CONFIG_TS_3_5
		foreach (const QDomNode &el, getChildren(item_node, "element"))
			addresses << getTextChild(el, "where");
#else
		foreach (const QDomNode &el, getChildren(getElement(item_node, "addresses"), "where"))
			addresses << el.toElement().text();
#endif
		b = new InterblockedActuatorGroup(addresses, descr, oid);
	}
		break;
	case GATE_LIGHTING_ACT:
	{
		QStringList sl = getTextChild(item_node, "time").split("*");
		Q_ASSERT_X(sl.size() == 3, "Automation::getBanner", "time leaf must have 3 fields");
		BtTime t(sl[0].toInt(), sl[1].toInt(), sl[2].toInt());
		b = new GateLightingActuator(t, descr, where, oid, NOT_PULL);
	}
		break;
	case GATE_VCT_ACT:
	{
#ifdef CONFIG_TS_3_5
		where = getTextChild(item_node, "dev") + getTextChild(item_node, "where");
#else
		QDomNode addresses = getElement(item_node, "addresses");
		where = getTextChild(addresses, "dev") + getTextChild(addresses, "where");
#endif
		b = new GateEntryphoneActuator(descr, where, oid);
		break;
	}
	case DOOR_LOCK:
		b = new ButtonActuator(descr, where, PULSE_ACT, oid, NOT_PULL);
		break;
	case PPT_STAT_AUTO:
		b = new PPTStat(descr, where, oid);
		break;
	}

	if (b)
		b->setId(id);

	return b;
}

void Automation::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		// here we use the getBanner from bannerfactory.h because there can be
		// a light group in the automation section
		if (banner *b = ::getBanner(item))
			page_content->appendBanner(b);
		else
		{
			int id = getTextChild(item, "id").toInt();
			qFatal("Type of item %d not handled on automation page!", id);
		}
	}
}

