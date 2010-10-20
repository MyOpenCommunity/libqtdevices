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


#ifndef LIGHTING_H
#define LIGHTING_H

#include "bannerpage.h"

#include <QWidget>

class QDomNode;
class banner;


/*!
	\defgroup Lighting Lighting

	SimpleActuator, Dimmer and Dimmer100 can ba used to control a single actuator;
	LightGroup, DimmerGroup and Dimmer100Group control the corresponding actuator group.

	Note that it's possible to use a SimpleActuator to control a 10-level or 100-level dimmer,
	and that it's possible to insert a 10-level or 100-level dimmer in a light group (or
	a 100-level dimmer in a 10-level dimmer group).

	TempLight and TempLightVariable allow switching on a light (or dimmer) for a specified time.
 */

/*!
	\ingroup Lighting
	\brief Main lighting page
 */
class Lighting : public BannerPage
{
Q_OBJECT
public:
	Lighting(const QDomNode &config_node);

	virtual int sectionId() const;

	static banner *getBanner(const QDomNode &item_node);

private:
	void initDimmer();
	void loadItems(const QDomNode &config_node);
};

#endif
