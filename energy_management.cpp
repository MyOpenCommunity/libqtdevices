#include "energy_management.h"
#include "energy_rates.h"
#include "energy_data.h" // EnergyCost
#include "skinmanager.h"
#include "xml_functions.h"
#include "main.h" // SUPERVISIONE, LOAD_MANAGEMENT


bool EnergyManagement::rate_edit_displayed = false;

EnergyManagement::EnergyManagement(const QDomNode &conf_node)
	: PageContainer(conf_node, false)
{
	SkinContext cxt(getTextChild(conf_node, "cid").toInt());

	QList<int> ids;
	foreach (const QDomNode &item, getChildren(conf_node, "item"))
		ids.append(getTextChild(item, "id").toInt());

	// if the energy management page only contains
	// load management and diagnostic, display the rate edit icon
	// inside load management
	rate_edit_displayed = true;

	if (ids.count() == 2 && ids.contains(SUPERVISIONE) && ids.contains(LOAD_MANAGEMENT))
		rate_edit_displayed = false;

	if (rate_edit_displayed)
	{
		QPoint pos = rect().bottomRight();
		pos -= QPoint(69, 79);

		EnergyRates::energy_rates.loadRates();
		if (EnergyRates::energy_rates.hasRates())
			addPage(new EnergyCost, -1, bt_global::skin->getImage("currency_exchange"), pos.x(), pos.y());
	}

	// call loadItems here so EnergyView/LoadManagement will see the correct value for isRateEditDisplayed
	loadItems(conf_node);
	addBackButton();
}

bool EnergyManagement::isRateEditDisplayed()
{
	return rate_edit_displayed;
}
