/*!
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef PLANTMENU_H
#define PLANTMENU_H

#include "sottomenu.h"
#include "bannpuls.h"
#include "bannfullscreen.h"

#include <qdom.h>
#include <qptrvector.h>
#include <qsignalmapper.h>

class device;
class thermal_regulator_99z;
class thermal_regulator_4z;
class thermal_regulator;

class PlantMenu : public sottoMenu
{
Q_OBJECT
public:
	PlantMenu(QWidget *parent, char *name, QDomNode conf, QColor bg, QColor fg, QColor fg2);
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

	/// The second foreground color
	QColor second_fg;


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
