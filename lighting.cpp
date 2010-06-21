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

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>

#include <assert.h>


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

static QList<QString> getAddresses(QDomNode item)
{
	QList<QString> l;
	foreach (const QDomNode &el, getChildren(item, "element"))
		l.append(getTextChild(el, "where"));
	return l;
}


Lighting::Lighting(const QDomNode &config_node)
{
	loadItems(config_node);
}

void Lighting::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		QString where = getTextChild(item, "where");
		QString img1 = IMG_PATH + getTextChild(item, "cimg1");
		QString img2 = IMG_PATH + getTextChild(item, "cimg2");
		QString img3 = IMG_PATH + getTextChild(item, "cimg3");
		QString img4 = IMG_PATH + getTextChild(item, "cimg4");
		QString img5 = IMG_PATH + getTextChild(item, "cimg5");

		QList<QString> times;
		foreach (const QDomNode &el, getChildren(item, "time"))
			times.append(el.toElement().text());

		banner *b;
		switch (id)
		{
		case DIMMER:
//			b = new dimmer(this, where, img1, img2, img3, img4, img5);
			b = new DimmerNew(this, item, where, getPullMode(item));
			break;
		case ATTUAT_AUTOM:
//			b = new attuatAutom(this, where, img1, img2, img3, img4);
			b = new SingleActuator(this, item, where, getPullMode(item));
			break;
		case GR_DIMMER:
//			b = new grDimmer(this, getAddresses(item), img1, img2, img3, img4);
			b = new DimmerGroup(this, item, getAddresses(item));
			break;
		case GR_ATTUAT_AUTOM:
//			b = new grAttuatAutom(this, getAddresses(item), img1, img2, img3);
			b = new LightGroup(this, item, getAddresses(item));
			break;
		case ATTUAT_AUTOM_TEMP:
//			b = new attuatAutomTemp(this, where, img1, img2, img3, img4, times);
			b = new TempLight(this, item, getPullMode(item));
			break;
		case ATTUAT_VCT_LS:
//			b = new attuatPuls(this, where, img1, img2, VCT_LS);
			b = new ButtonActuator(this, item, VCT_LS, getPullMode(item));
			break;
		case DIMMER_100:
//			b = new dimmer100(this, where, img1, img2 ,img3, img4, img5, getTextChild(item,"softstart").toInt(),
//					getTextChild(item,"softstop").toInt());
			b = new Dimmer100New(this, item, getPullMode(item));
			break;
		case ATTUAT_AUTOM_TEMP_NUOVO_N:
//			b = new attuatAutomTempNuovoN(this, where, img1, img2, img3, img4, times);
			b = new TempLightVariable(this, item, getPullMode(item));
			break;
		case ATTUAT_AUTOM_TEMP_NUOVO_F:
//			if (!times.count())
//				times.append("");
//			b = new attuatAutomTempNuovoF(this, where, img1, img2, img3, times.at(0));
			b = new TempLightFixed(this, item, getPullMode(item));
			break;
		case GR_DIMMER100:
		{
//			QList<int> sstart, sstop;
//			QList<QString> addresses;
//
//			foreach (const QDomNode &el, getChildren(item, "element"))
//			{
//				sstart.append(getTextChild(el, "softstart").toInt());
//				sstop.append(getTextChild(el, "softstop").toInt());
//				addresses.append(getTextChild(el, "where"));
//			}
//
//			b = new grDimmer100(this, addresses, img1, img2, img3, img4, sstart, sstop);
			b = new Dimmer100Group(this, item);
			break;
		}
		default:
			assert(!"Type of item not handled on lighting page!");
		}
		b->setText(getTextChild(item, "descr"));
		b->setId(id);
		appendBanner(b); // TODO: deve gestire tutte le connect??
	}
}

void Lighting::inizializza()
{
	initDimmer();
}

void Lighting::initDimmer()
{
	qDebug("Lighting::initDimmer()");
	for (int i = 0; i < elencoBanner.size(); ++i)
	{
		switch(elencoBanner.at(i)->getId())
		{
		case DIMMER:
		case DIMMER_100:
		case ATTUAT_AUTOM:
		case ATTUAT_AUTOM_TEMP:
		case ATTUAT_AUTOM_TEMP_NUOVO_N:
		case ATTUAT_AUTOM_TEMP_NUOVO_F:
			elencoBanner.at(i)->inizializza(true);
			break;
		default:
			break;
		}
	}
}
