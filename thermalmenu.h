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


#ifndef BTOUCH_THERMALMENU_H
#define BTOUCH_THERMALMENU_H

#include "bannerpage.h"
#include "bann1_button.h"
#include "bttime.h"

#include <QDomNode>
#include <QWidget>
#include <QString>


/*!
	\defgroup ThermalRegulation Temperature Control

	The temperature control section contains up to 4 subsections:
	- thermal regulator (implemented by PlantMenu)
	- air conditioning (implemented by AirConditioning, see also the \ref AirConditioning section)
	- external probes (implemented by ProbesPage)
	- non-controlled zones (implemented by ProbesPage)

	External probes and non-controlled zones only display a list of probes with the measured temperature.

	The \ref AirConditioning subsection is documented separately.

	The thermal regulator subsection is used to program the thermal regulator and check the temperature
	for all controlled zones.
*/

/*!
	\ingroup ThermalRegulation
	\brief Section page containing links to all thermal regulation pages.

	Depending on the configuration, this page can contain links to the list of
	external probes and non-controlled zones, to the air conditioning subsection
	and to the thermal regulator page.
 */
class ThermalMenu : public BannerPage
{
Q_OBJECT
public:
	ThermalMenu(const QDomNode &config_node);

	virtual int sectionId() const;

public slots:
	/*!
		Show ThermalMenu if there are two or more banners, show the only submenu below us
		otherwise.
	 */
	virtual void showPage();

private:
	BannSinglePuls *addMenuItem(QDomElement, QString);

	/*!
		Create a banner list to show external and non-controlled probes
		\param config    The node in the Dom tree that holds the `probe' section
		\param bann      A pointer to the banner that gives access to the subpage
		\param external  True if the probe is external, false otherwise
	 */
	void createProbeMenu(QDomNode config, BannSinglePuls *bann, bool external);

	void createPlantMenu(QDomNode config, BannSinglePuls *bann);
	void loadBanners(const QDomNode &config_node);

	unsigned bann_number;

	/// A reference to the only submenu below us
	Page *single_page;
};


/*!
	\ingroup ThermalRegulation
	\brief Show a list of external probes or non-controlled zones.
 */
class ProbesPage : public BannerPage
{
Q_OBJECT
public:
	ProbesPage(const QDomNode &config_node, bool are_probes_external);

	virtual int sectionId() const;

private:
	void loadItems(const QDomNode &config_node, bool are_probes_external);
};

#endif
