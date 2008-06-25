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
class thermal_regulator_99z;
class thermal_regulator_4z;

class PlantMenu : public sottoMenu
{
Q_OBJECT
public:
	PlantMenu(QWidget *parent, char *name, QDomNode conf, QColor bg, QColor fg, QColor fg2);
private:
	/**
	 * Utility function to create the settings menu for a 99z thermal regulator.
	 */
	void create99zSettings(QDomNode conf, thermal_regulator_99z *dev);

	/**
	 * Utility function to create the settings menu for a 4z thermal regulator.
	 */
	void create4zSettings(QDomNode conf, thermal_regulator_4z *dev);

	/**
	 * Utility function to create the submenu to set the weekly program in thermal
	 * regulator device.
	 */
	void weekSettings(sottoMenu *settings, QDomNode conf);

	/**
	 * Utility function to create the submenu to set the scenario program in thermal
	 * regulator device.
	 */
	void scenarioSettings(sottoMenu *settings, QDomNode conf);

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
	 * Sub-sub menu used to show the full screen banners corresponding to the
	 * same small banners present in the plant menu.
	 */
	sottoMenu items_submenu;

	/*
	 * Used to call show(int) on SottoMenu based on which banner is clicked.
	 */
	QSignalMapper signal_mapper;
};

class OpenFrameSender : public QObject
{
Q_OBJECT
public:
	OpenFrameSender(DateEditMenu *_date_edit, TimeEditMenu *_time_edit, WeeklyMenu *_program_menu, QObject *parent = 0);
	OpenFrameSender(WeeklyMenu *_program_menu, QObject *parent = 0);
	void setAddress(QString where);
public slots:
	void holidaySettingsEnd(int program);
	void weekSettingsEnd(int program);
private:
	QString th_regulator_where;
	DateEditMenu *date_edit;
	TimeEditMenu *time_edit;
	WeeklyMenu   *program_menu;
};
#endif // PLANTMENU_H
