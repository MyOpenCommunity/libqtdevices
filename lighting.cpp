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


#include "lighting.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "bann_lighting.h"
#include "actuators.h"
#include "bannercontent.h"
#include "main.h"
#include "skinmanager.h" // SkinContext

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>


namespace
{
	QList<QString> getAddresses(QDomNode item, QList<int> *start_values = 0, QList<int> *stop_values = 0)
	{
		QList<QString> l;
		foreach (const QDomNode &el, getChildren(item, "element"))
		{
			l.append(getTextChild(el, "where"));
			if (start_values)
				start_values->append(getTextChild(el, "softstart").toInt());
			if (stop_values)
				stop_values->append(getTextChild(el, "softstop").toInt());
		}
		return l;
	}

	QList<BtTime> getTimes(const QDomNode &item)
	{
		QList<BtTime> times;
		foreach (const QDomNode &time, getChildren(item, "time"))
		{
			QString s = time.toElement().text();
			QStringList sl = s.split("*");
			Q_ASSERT_X(sl.size() == 3, "getTimes()", "Time values must have exactly 3 fields");
			times << BtTime(sl[0].toInt(), sl[1].toInt(), sl[2].toInt());
		}
		return times;
	}
}


Lighting::Lighting(const QDomNode &config_node)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
}

int Lighting::sectionId()
{
	return ILLUMINAZIONE;
}

banner *Lighting::getBanner(const QDomNode &item_node)
{
	SkinContext ctx(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	QString where = getTextChild(item_node, "where");
	QString descr = getTextChild(item_node, "descr");

	banner *b = 0;
	switch (id)
	{
	case DIMMER:
		b = new Dimmer(descr, where);
		break;
	case ATTUAT_AUTOM:
		b = new SingleActuator(descr, where);
		break;
	case GR_DIMMER:
		b = new DimmerGroup(getAddresses(item_node), descr);
		break;
	case GR_ATTUAT_AUTOM:
		b = new LightGroup(getAddresses(item_node), descr);
		break;
	case ATTUAT_AUTOM_TEMP:
		b = new TempLight(descr, where);
		break;
	case ATTUAT_VCT_LS:
		b = new ButtonActuator(descr, where, VCT_LS);
		break;
	case DIMMER_100:
	{
		int start = getTextChild(item_node, "softstart").toInt();
		int stop = getTextChild(item_node, "softstop").toInt();
		b = new Dimmer100(descr, where, start, stop);
	}
		break;
	case ATTUAT_AUTOM_TEMP_NUOVO_N:
		b = new TempLightVariable(getTimes(item_node), descr, where);
		break;
	case GR_DIMMER100:
	{
		QList<int> start, stop;
		QList<QString> addresses = getAddresses(item_node, &start, &stop);
		b = new Dimmer100Group(addresses, start, stop, descr);
	}
		break;
	case ATTUAT_AUTOM_TEMP_NUOVO_F:
	{
		int t;
	#ifdef CONFIG_BTOUCH
		// I think conf.xml will have only one node for time in this banner, however
		// such node is indicated as "timeX", so I'm using the following overkill code
		// to be safe
		QList<QDomNode> children = getChildren(item_node, "time");
		QStringList sl;
		foreach (const QDomNode &tmp, children)
			sl << tmp.toElement().text().split("*");

		Q_ASSERT_X(sl.size() == 3, "Lighting::getBanner", "Fixed time must have exactly 3 fields");
		t = sl[0].toInt() * 3600 + sl[1].toInt() * 60 + sl[2].toInt();
	#else
		t = getTextChild(item_node, "time").toInt();
	#endif
		b = new TempLightFixed(t, descr, where);
		}
		break;
	}

	if (b)
		b->setId(id);
	return b;
}

void Lighting::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		if (banner *b = getBanner(item))
			page_content->appendBanner(b);
		else
		{
			int id = getTextChild(item, "id").toInt();
			qFatal("Type of item %d not handled on lighting page!", id);
		}
	}
}

void Lighting::inizializza()
{
	initDimmer();
}

void Lighting::initDimmer()
{
	qDebug("Lighting::initDimmer()");
	for (int i = 0; i < page_content->bannerCount(); ++i)
	{
		banner *b = page_content->getBanner(i);
		switch (b->getId())
		{
		case DIMMER:
		case DIMMER_100:
		case ATTUAT_AUTOM:
		case ATTUAT_AUTOM_TEMP:
		case ATTUAT_AUTOM_TEMP_NUOVO_N:
		case ATTUAT_AUTOM_TEMP_NUOVO_F:
			b->inizializza(true);
			break;
		default:
			break;
		}
	}
}

