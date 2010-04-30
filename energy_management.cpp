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
#include "btbutton.h"
#include "navigation_bar.h"

#include <QVBoxLayout>

bool EnergyManagement::rate_edit_displayed = false;
bool EnergyManagement::is_built = false;


EnergyManagement::EnergyManagement(const QDomNode &conf_node)
	: SectionPage()
{
	SkinContext cxt(getTextChild(conf_node, "cid").toInt());

	QWidget *main_widget = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(main_widget);
	main_layout->setContentsMargins(5, 5, 25, 50);
	IconContent *content = new IconContent;
	main_layout->addWidget(content, 1);

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	Page::buildPage(main_widget, content, nav_bar);

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

	// call loadItems here so EnergyView/LoadManagement will see the correct value for isRateEditDisplayed
	loadItems(conf_node);

	if (rate_edit_displayed)
	{
#ifdef LAYOUT_BTOUCH
		QPoint pos = rect().bottomRight();
		pos -= QPoint(69, 79);

		EnergyRates::energy_rates.loadRates();
		if (EnergyRates::energy_rates.hasRates())
			addPage(new EnergyCost, getTextChild(conf_node, "descr"),
				bt_global::skin->getImage("currency_exchange"), pos.x(), pos.y());
#else
		Page *page = new EnergyCost;
		BtButton *b = new BtButton(bt_global::skin->getImage("currency_exchange"));
		connect(b, SIGNAL(clicked()), page, SLOT(showPage()));
		connect(page, SIGNAL(Closed()), this, SLOT(showPage()));
		main_layout->addWidget(b, 0, Qt::AlignRight);
#endif
	}
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
