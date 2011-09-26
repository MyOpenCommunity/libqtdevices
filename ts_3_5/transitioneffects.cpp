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


#include "transitioneffects.h"
#include "displaycontrol.h" // bt_global::display
#include "transitionwidget.h"
#include "generic_functions.h" // setCfgValue
#include "xml_functions.h" // getTextChild

#include <QMap>
#include <QDomNode>
#include <QString>
#include <QDebug>


TransitionEffects::TransitionEffects(const QDomNode &conf_node) :
	SingleChoicePage(getTextChild(conf_node, "descr"))
{
	addBanner(SingleChoice::createBanner(tr("None")), TransitionWidget::NONE);
	addBanner(SingleChoice::createBanner(tr("Blending")), TransitionWidget::BLENDING);
	addBanner(SingleChoice::createBanner(tr("Mosaic")), TransitionWidget::MOSAIC);
	addBanner(SingleChoice::createBanner(tr("Enlarge")), TransitionWidget::ENLARGE);

	connect(this, SIGNAL(Closed()), SLOT(cleanUp()));

	// this load the current transition effect into the global display object
	bannerSelected(getTextChild(conf_node, "type").toInt());

	// to save parameters
	item_id = getTextChild(conf_node, "itemID").toInt();
}

void TransitionEffects::cleanUp()
{
	QMap<QString, QString> data;
	data["type"] = QString::number(bt_global::display->currentTransitionEffects());
	setCfgValue(data, item_id);
}

void TransitionEffects::showPage()
{
	setCheckedId(getCurrentId());
	SingleChoicePage::showPage();
}

int TransitionEffects::getCurrentId()
{
	return bt_global::display->currentTransitionEffects();
}

void TransitionEffects::bannerSelected(int id)
{
	bt_global::display->installTransitionEffects(static_cast<TransitionWidget::Type>(id));
}

