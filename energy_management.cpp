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


#include "energy_management.h"
#include "energy_rates.h"
#include "energy_data.h" // EnergyCost
#include "skinmanager.h"
#include "xml_functions.h"
#include "main.h" // SUPERVISIONE, LOAD_MANAGEMENT


bool EnergyManagement::rate_edit_displayed = false;
bool EnergyManagement::is_built = false;


EnergyManagement::EnergyManagement(const QDomNode &conf_node)
	: SectionPage(conf_node, false)
{
	SkinContext cxt(getTextChild(conf_node, "cid").toInt());

	is_built = true;
	QList<int> ids;
	foreach (const QDomNode &item, getChildren(conf_node, "item"))
		ids.append(getTextChild(item, "id").toInt());

	// if the energy management page only contains
	// supervision and another item, display the rate edit icon
	// inside the other item (either energy view or load management)
	rate_edit_displayed = true;

	if (ids.count() == 2 && ids.contains(SUPERVISIONE))
		rate_edit_displayed = false;

	if (rate_edit_displayed)
	{
		QPoint pos = rect().bottomRight();
		pos -= QPoint(69, 79);

		EnergyRates::energy_rates.loadRates();
		if (EnergyRates::energy_rates.hasRates())
			addPage(new EnergyCost, getTextChild(conf_node, "descr"),
				bt_global::skin->getImage("currency_exchange"), pos.x(), pos.y());
	}

	// call loadItems here so EnergyView/LoadManagement will see the correct value for isRateEditDisplayed
	loadItems(conf_node);
}

bool EnergyManagement::isBuilt()
{
	return is_built;
}

bool EnergyManagement::isRateEditDisplayed()
{
	return rate_edit_displayed;
}

int EnergyManagement::sectionId() const
{
	return ENERGY_MANAGEMENT;
}
