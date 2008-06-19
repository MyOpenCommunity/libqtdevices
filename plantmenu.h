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
#include "weeklymenu.h"

#include <qdom.h>
#include <qptrvector.h>
#include <qsignalmapper.h>

class device;

class PlantMenu : public sottoMenu
{
Q_OBJECT
public:
	PlantMenu(QWidget *parent, char *name, QDomNode conf, QColor bg, QColor fg, QColor fg2);
private:
	/**
	 * Utility function to create the settings menu for a 99z thermal regulator.
	 */
	sottoMenu *create99zSettings(QDomNode conf);

	/**
	 * Utility function to create the settings menu for a 4z thermal regulator.
	 */
	sottoMenu *create4zSettings(QDomNode conf);

	/**
	 * Utility function to create the submenu to set the weekly program in thermal
	 * regulator device.
	 */
	void weekSettings(sottoMenu *settings, QDomNode conf);

	/**
	 * Utility function to create the submenu to set manually the temperature
	 * for the thermal regulator device.
	 */
	void manualSettings(sottoMenu *settings);

	/**
	 * Utility function to create the submenu for timed manual operation mode.
	 * This is used only with 4 zones thermal regulators
	 */
	void timedManualSettings(sottoMenu *settings);

	/**
	 * Utility function to create the submenu for holiday settings.
	 */
	void holidaySettings(sottoMenu *settings, QDomNode conf);

	/**
	 * Utility function to create a banner in the plant menu and the corresponding full
	 * screen banner.
	 * \param n             The node in the configuration DOM which is a `item' tag
	 * \param central_icon  The path of the icon to be shown in plant menu banner
	 * \param descr         The text to be shown in the banner in plant menu
	 * \param type          The type of full screen banner that is linked to the banner in plant menu
	 * \param addr          The address of the device observed by the banner
	 * \param dev           A pointer to the device observed by the banner
	 */
	bannPuls *addMenuItem(QDomNode n, QString central_icon, QString descr, BannID type,
			QString addr, device *dev);

	/// Node in the Dom tree that is a `plant' tag
	QDomNode conf_root;

	/// Address of the thermal regulation device
	/// empty in case of 99z device
	QString ind_centrale;

	/// The second foreground color
	QColor second_fg;

	/*
	 * Used to call show(int) on SottoMenu based on which banner is clicked.
	 */
	QSignalMapper signal_mapper;

	/*
	 * Sub-sub menu used to show the full screen banners corresponding to the
	 * same small banners present in the plant menu.
	 */
	sottoMenu items_submenu;
};
#endif // PLANTMENU_H
