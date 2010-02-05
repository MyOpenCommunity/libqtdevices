#ifndef ENERGY_MANAGEMENT_H
#define ENERGY_MANAGEMENT_H

#include "sectionpage.h"

class EnergyManagement : public SectionPage
{
public:
	EnergyManagement(const QDomNode &conf_node);

	// returns true if the icon to edit rates is displayed in this page,
	// and must not be displayed in energy data/load management
	static bool isRateEditDisplayed();

private:
	static bool rate_edit_displayed;
};

#endif // ENERGY_MANAGEMENT_H
