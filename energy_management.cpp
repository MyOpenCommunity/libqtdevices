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
#include "main.h" // SUPERVISION, LOAD_MANAGEMENT
#include "btbutton.h"
#include "navigation_bar.h"
#include "icondispatcher.h"

#include <QVBoxLayout>

bool EnergyManagement::rate_edit_displayed = false;
bool EnergyManagement::is_built = false;


EnergyManagement::EnergyManagement(const QDomNode &conf_node)
{
	SkinContext cxt(getTextChild(conf_node, "cid").toInt());

	QWidget *main_widget = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(main_widget);
	main_layout->setContentsMargins(5, 5, 25, 18);
	IconContent *content = new IconContent;
	main_layout->addWidget(content, 1);

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);

#ifdef LAYOUT_TS_3_5
	PageTitleWidget *title_widget = 0;
#else
	PageTitleWidget *title_widget = new PageTitleWidget(getTextChild(conf_node, "descr"), SMALL_TITLE_HEIGHT);
#endif
	Page::buildPage(main_widget, content, nav_bar, title_widget);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	is_built = true;
	QList<int> ids;
	foreach (const QDomNode &item, getChildren(conf_node, "item"))
		ids.append(getTextChild(item, "id").toInt());

	// if the energy management page only contains
	// supervision and another item, display the rate edit icon
	// inside the other item (either energy view or load management)
	rate_edit_displayed = true;

	if (ids.count() == 2 && ids.contains(SUPERVISION))
		rate_edit_displayed = false;

	// call loadItems here so EnergyView/LoadManagement will see the correct value for isRateEditDisplayed
	loadItems(conf_node);

	EnergyRates::energy_rates.loadRates();
	if (rate_edit_displayed && EnergyRates::energy_rates.hasRates())
	{
#ifdef LAYOUT_TS_3_5
		QPoint pos = rect().bottomRight();
		// Align with the back button (thanks to the icons cache, we don't care about
		// calculating the positioning using the size of the button's image)
		QPixmap *pixmap = bt_global::icons_cache.getIcon(bt_global::skin->getImage("currency_exchange"));
		QSize size = pixmap->size();
		pos -= QPoint(size.width() + 5, size.height() + 14);

		addPage(new EnergyCost, getTextChild(conf_node, "descr"),
			bt_global::skin->getImage("currency_exchange"), QString(), pos.x(), pos.y());
#else
		Page *page = new EnergyCost;
		BtButton *b = new BtButton(bt_global::skin->getImage("currency_exchange"));
		connect(b, SIGNAL(clicked()), page, SLOT(showPage()));
		connect(page, SIGNAL(Closed()), this, SLOT(showPage()));
		main_layout->addWidget(b, 0, Qt::AlignRight | Qt::AlignBottom);
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
