#include "energy_management.h"
#include "energy_rates.h"
#include "energy_data.h" // EnergyCost
#include "skinmanager.h"
#include "xml_functions.h"


bool EnergyManagement::rate_edit_displayed = false;

EnergyManagement::EnergyManagement(const QDomNode &conf_node)
	: SectionPage(conf_node)
{
	SkinContext cxt(getTextChild(conf_node, "cid").toInt());

	QList<int> ids;
	foreach (const QDomNode &item, getChildren(conf_node, "item"))
		ids.append(getTextChild(item, "id").toInt());

	// TODO energy if the energy management page only contains
	//      load management and diagnostic, display the rate edit icon
	//      inside load management
	rate_edit_displayed = true;

	if (rate_edit_displayed)
	{
		QPoint pos = rect().bottomRight();
		pos -= QPoint(80, 80);

		EnergyRates::energy_rates.loadRates();
		addPage(new EnergyCost, getTextChild(conf_node, "descr"),
			bt_global::skin->getImage("currency_exchange"), pos.x(), pos.y());
	}

	addBackButton();
}

bool EnergyManagement::isRateEditDisplayed()
{
	return rate_edit_displayed;
}
