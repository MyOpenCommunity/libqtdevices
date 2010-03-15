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
#include "bannercontent.h"
#include "main.h"
#include "skinmanager.h" // SkinContext

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>


Automation::Automation(const QDomNode &config_node)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
}

int Automation::sectionId()
{
	return AUTOMAZIONE;
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
	case ATTUAT_AUTOM_INT_SIC:
		b = new SecureInterblockedActuator(descr, where, oid);
		break;
	case ATTUAT_AUTOM_INT:
		b = new InterblockedActuator(descr, where, oid);
		break;
	case ATTUAT_AUTOM:
		b = new SingleActuator(descr, where, oid);
		break;
	case ATTUAT_VCT_SERR:
		b = new ButtonActuator(descr, where, VCT_SERR);
		break;
	case GR_ATTUAT_INT:
	{
		QStringList addresses;
#ifdef CONFIG_BTOUCH
		foreach (const QDomNode &el, getChildren(item_node, "element"))
			addresses << getTextChild(el, "where");
#else
		foreach (const QDomNode &el, getChildren(getElement(item_node, "addresses"), "where"))
			addresses << el.toElement().text();
#endif
		b = new InterblockedActuatorGroup(addresses, descr, oid);
	}
		break;
	case AUTOM_CANC_ATTUAT_ILL:
	{
		QStringList sl = getTextChild(item_node, "time").split("*");
		Q_ASSERT_X(sl.size() == 3, "Automation::getBanner", "time leaf must have 3 fields");
		BtTime t(sl[0].toInt(), sl[1].toInt(), sl[2].toInt());
		b = new GateLightingActuator(t, descr, where, oid);
	}
		break;
	case AUTOM_CANC_ATTUAT_VC:
		b = new GateEntryphoneActuator(descr, where, oid);
		break;
	case ATTUAT_AUTOM_PULS:
		b = new ButtonActuator(descr, where, AUTOMAZ);
		break;
	case PPT_STAT:
		b = new PPTStat(where, oid);
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
		if (banner *b = getBanner(item))
			page_content->appendBanner(b);
		else
		{
			int id = getTextChild(item, "id").toInt();
			qFatal("Type of item %d not handled on automation page!", id);
		}
	}
}

