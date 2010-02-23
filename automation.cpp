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

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>

#include <assert.h>


Automation::Automation(const QDomNode &config_node)
{
	loadItems(config_node);
}

void Automation::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		int cid = getTextChild(item, "cid").toInt();
		QString where = getTextChild(item, "where");
		QString img1 = IMG_PATH + getTextChild(item, "cimg1");
		QString img2 = IMG_PATH + getTextChild(item, "cimg2");
		QString img3 = IMG_PATH + getTextChild(item, "cimg3");
		QString img4 = IMG_PATH + getTextChild(item, "cimg4");
		QString time = getTextChild(item, "time");

		banner *b;
		switch (id)
		{
		case ATTUAT_AUTOM_INT_SIC:
//			b = new attuatAutomIntSic(this, where, img1, img2, img3, img4);
			b = new SecureInterblockedActuator(this, item);
			break;
		case ATTUAT_AUTOM_INT:
//			b = new attuatAutomInt(this, where, img1, img2, img3, img4);
			b = new InterblockedActuator(this, item);
			break;
		case ATTUAT_AUTOM:
//			b = new attuatAutom(this, where, img1, img2, img3, img4);
			b = new SingleActuator(this, item, where);
			break;
		case ATTUAT_VCT_SERR:
//			b = new attuatPuls(this, where, img1, img2, VCT_SERR);
			b = new ButtonActuator(this, item, VCT_SERR);
			break;
		case GR_ATTUAT_INT:
		{
//			QList<QString> addresses;
//			foreach (const QDomNode &el, getChildren(item, "element"))
//				addresses.append(getTextChild(el, "where"));
//
//			b = new grAttuatInt(this, addresses, img1, img2, img3);
			b = new InterblockedActuatorGroup(this, item);
			break;
		}
		case AUTOM_CANC_ATTUAT_ILL:
//			b = new automCancAttuatIll(this, where, img1, img2, time);
			b = new GateLightingActuator(this, item);
			break;
		case AUTOM_CANC_ATTUAT_VC:
//			b = new automCancAttuatVC(this, where, img1, img2);
			b = new GateEntryphoneActuator(this, item);
			break;
		case ATTUAT_AUTOM_PULS:
//			b = new attuatPuls(this, where, img1, img2, AUTOMAZ);
			b = new ButtonActuator(this, item, AUTOMAZ);
			break;
		case PPT_STAT:
			b = new PPTStat(this, where, cid);
			break;
		default:
			assert(!"Type of item not handled on automation page!");
		}

		b->setText(getTextChild(item, "descr"));
		b->setId(id);
		appendBanner(b); // TODO: deve gestire tutte le connect??
	}
}
