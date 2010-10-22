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


#ifndef ENERGY_MANAGEMENT_H
#define ENERGY_MANAGEMENT_H

#include "sectionpage.h"

/*!
	\defgroup EnergyManagement Energy Management

	The Energy Management section is only a container to group together energy
	related sections, like \ref Supervision, \ref EnergyData and \ref LoadManagement.
*/


/*!
	\ingroup EnergyManagement
	\brief A container for other energy related sections.
*/
class EnergyManagement : public SectionPage
{
public:
	EnergyManagement(const QDomNode &conf_node);

	// returns true if the icon to edit rates is displayed in this page,
	// and must not be displayed in energy data/load management
	static bool isRateEditDisplayed();

	virtual int sectionId() const;

	// We have to know if the energy management is built, in order to update
	// correctly the sectionId of the sections that the EnergyManagement can contains.
	static bool isBuilt();

private:
	static bool rate_edit_displayed;
	static bool is_built;
};

#endif // ENERGY_MANAGEMENT_H
