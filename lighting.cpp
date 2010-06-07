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
#include "main.h" // LIGHTING
#include "skinmanager.h" // SkinContext

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>

#ifdef CONFIG_BTOUCH
// configuration values for old configuration files
enum BannerType
{
	DIMMER10 = 1,
	SINGLE_LIGHT = 0,
	DIMMER_GROUP = 6,
	LIGHT_GROUP = 5,
	TEMP_LIGHT = 9,
	STAIR_LIGHT = 12,
	DIMMER100 = 35,
	TEMP_LIGHT_VARIABLE = 36,
	DIMMER100_GROUP = 44,
	TEMP_LIGHT_FIXED = 37,
};
#else
enum BannerType
{
	DIMMER10 = 2002,
	SINGLE_LIGHT = 2003,
	DIMMER_GROUP = 2006,
	LIGHT_GROUP = 2004,
	TEMP_LIGHT = 2007,
	STAIR_LIGHT = 2011,
	DIMMER100 = 2001,
	TEMP_LIGHT_VARIABLE = 2008,
	DIMMER100_GROUP = 2005,
	TEMP_LIGHT_FIXED = 2010,
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


namespace
{
	QList<QString> getAddresses100(QDomNode item, QList<int> *start_values, QList<int> *stop_values)
	{
		QList<QString> l;
		foreach (const QDomNode &item_node, getChildren(getElement(item, "addresses"), "item"))
		{
			l.append(getTextChild(item_node, "where"));
			start_values->append(getTextChild(item_node, "softstart").toInt());
			stop_values->append(getTextChild(item_node, "softstop").toInt());
		}

		Q_ASSERT_X(!l.isEmpty(), "getAddresses", "No device found!");

		return l;
	}

	QList<QString> getAddresses(QDomNode item, QList<int> *start_values = 0, QList<int> *stop_values = 0)
	{
		QList<QString> l;
#ifdef CONFIG_BTOUCH
		foreach (const QDomNode &el, getChildren(item, "element"))
		{
			l.append(getTextChild(el, "where"));
#else
		foreach (const QDomNode &el, getChildren(getElement(item, "addresses"), "where"))
		{
			l.append(el.toElement().text());
#endif

#ifdef CONFIG_BTOUCH
			if (start_values)
				start_values->append(getTextChild(el, "softstart").toInt());
			if (stop_values)
				stop_values->append(getTextChild(el, "softstop").toInt());
#else
			Q_ASSERT_X(start_values == NULL && stop_values == NULL, "getAddresses", "Use getAddresses100 for dimmer100 group on TouchX");
#endif
		}

		Q_ASSERT_X(!l.isEmpty(), "getAddresses", "No device found!");

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
		Q_ASSERT_X(!times.isEmpty(), "getTimes()", "Time node missing");
		return times;
	}
}


Lighting::Lighting(const QDomNode &config_node)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
}

int Lighting::sectionId() const
{
	return LIGHTING;
}

banner *Lighting::getBanner(const QDomNode &item_node)
{
	SkinContext ctx(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	QString where = getTextChild(item_node, "where");
	QString descr = getTextChild(item_node, "descr");
	int oid = getTextChild(item_node, "openserver_id").toInt();

	banner *b = 0;
	switch (id)
	{
	case DIMMER10:
		b = new Dimmer(descr, where, oid, getPullMode(item_node));
		break;
	case SINGLE_LIGHT:
		b = new SingleActuator(descr, where, oid, getPullMode(item_node));
		break;
	case DIMMER_GROUP:
		b = new DimmerGroup(getAddresses(item_node), descr);
		break;
	case LIGHT_GROUP:
		b = new LightGroup(getAddresses(item_node), descr);
		break;
	case TEMP_LIGHT:
		b = new TempLight(descr, where, oid, getPullMode(item_node));
		break;
	case STAIR_LIGHT:
		b = new ButtonActuator(descr, where, VCT_STAIRLIGHT, oid, getPullMode(item_node));
		break;
	case DIMMER100:
	{
		int start = getTextChild(item_node, "softstart").toInt();
		int stop = getTextChild(item_node, "softstop").toInt();
		b = new Dimmer100(descr, where, oid, getPullMode(item_node), start, stop);
	}
		break;
	case TEMP_LIGHT_VARIABLE:
		b = new TempLightVariable(getTimes(item_node), descr, where, oid, getPullMode(item_node));
		break;
	case DIMMER100_GROUP:
	{
		QList<int> start, stop;
#ifdef CONFIG_BTOUCH
		QList<QString> addresses = getAddresses(item_node, &start, &stop);
#else
		QList<QString> addresses = getAddresses100(item_node, &start, &stop);
#endif
		b = new Dimmer100Group(addresses, start, stop, descr);
	}
		break;
	case TEMP_LIGHT_FIXED:
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
		b = new TempLightFixed(t, descr, where, oid, getPullMode(item_node));
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

