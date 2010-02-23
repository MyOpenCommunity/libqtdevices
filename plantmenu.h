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


#ifndef PLANTMENU_H
#define PLANTMENU_H

#include "sottomenu.h"
#include "bann1_button.h"
#include "bann_thermal_regulation.h"

#include <QSignalMapper>
#include <QString>
#include <QDomNode>

class device;
class thermal_regulator_99z;
class thermal_regulator_4z;
class thermal_regulator;


class PlantMenu : public sottoMenu
{
Q_OBJECT
public:
	PlantMenu(QWidget *parent, QDomNode conf);

private:
	/**
	 * Utility function to create a banner in the plant menu and the corresponding full
	 * screen banner.
	 * \param n             The node in the configuration DOM which is a `item' tag
	 * \param central_icon  The path of the icon to be shown in plant menu banner
	 * \param descr         The text to be shown in the banner in plant menu
	 * \param type          The type of full screen banner that is linked to the banner in plant menu
	 * \param addr          The address of the device observed by the banner
	 * \param dev           A pointer to the device observed by the banner
	 * \param bannfullscreen The full screen banner that is created with the call to \a addMenuItem
	 * \return A pointer to the small banner that will show the corresponding full screen banner.
	 */
	bannPuls *addMenuItem(QDomNode n, QString central_icon, QString descr, BannID type);

	/// Node in the Dom tree that is a `plant' tag
	QDomNode conf_root;

	/// Address of the thermal regulation device
	/// empty in case of 99z device
	QString ind_centrale;

	/*
	 * Sub-sub menu used to show the full screen banners corresponding to the
	 * same small banners present in the plant menu.
	 */
	sottoMenu items_submenu;

	/*
	 * Used to call show(int) on SottoMenu based on which banner is clicked.
	 */
	QSignalMapper signal_mapper;
};
#endif // PLANTMENU_H
