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

#include "sottomenu.h"
#include "bann1_button.h"
#include "bttime.h"
#include "bann_thermal_regulation.h"

#include <QDomNode>
#include <QWidget>
#include <QString>

class FSBannTime;
class FSBannDate;

class ThermalMenu : public sottoMenu
{
Q_OBJECT
public:
	/**
	 * 
	 */
	ThermalMenu(const QDomNode &config_node);

public slots:
	/**
	 * Show ThermalMenu if there are two or more banners, show the only submenu below us
	 * otherwise.
	 */
	virtual void showPage();

private:
	bannPuls *addMenuItem(QDomElement, QString);
	/**
	 * Create a sottoMenu to show external and not controlled probes
	 *
	 * \param config    The node in the Dom tree that holds the `probe' section
	 * \param bann      A pointer to the banner that gives access to the
	 * sottoMenu
	 * \param external  True if the probe is external, false otherwise
	 */
	void createProbeMenu(QDomNode config, bannPuls *bann, bool external);

	void createPlantMenu(QDomNode config, bannPuls *bann);
	void loadBanners(const QDomNode &config_node);

	/// do NOT setAutoDelete(true), since banners are children of
	/// ThermalMenu and will be deleted by Qt

	unsigned bann_number;
	/// A reference to the only submenu below us
	Page *single_page;
};

/**
 * A base class for submenus that allow to choose one program in a list. The list changes
 * when season changes (summer/winter).
 * This class emits a signal when a program is clicked. This signal should be used to close
 * the submenu and to take further action, for example sending a frame to the thermal regulator.
 */
class ProgramMenu : public sottoMenu
{
Q_OBJECT
public:
	ProgramMenu(QWidget *parent, QDomNode conf);
	virtual void createSummerBanners() = 0;
	virtual void createWinterBanners() = 0;
	void setSeason(Season new_season);
protected:
	int season;
	QDomNode conf_root;
	/**
	 * \param season Either "summer" or "winter"
	 * \param what Either "prog" or "scen"
	 */
	void createSeasonBanner(QString season, QString what, QString icon);
signals:
	void programClicked(int);
};

/**
 * This is a specialized version of ProgramMenu to select week programs. The list
 * of programs is read from DOM.
 */
class WeeklyMenu : public ProgramMenu
{
Q_OBJECT
public:
	WeeklyMenu(QWidget *parent, QDomNode conf);
	virtual void createSummerBanners();
	virtual void createWinterBanners();
};

/**
 * This is a specialized version of ProgramMenu to select scenarios. The list
 * of scenarios is read from DOM and updated when season changes
 */
class ScenarioMenu : public ProgramMenu
{
Q_OBJECT
public:
	ScenarioMenu(QWidget *parent, QDomNode conf);
	virtual void createSummerBanners();
	virtual void createWinterBanners();
};

/**
 * A submenu that let the user choose the time.
 */
class TimeEditMenu : public sottoMenu
{
Q_OBJECT
public:
	TimeEditMenu(QWidget *parent=0);
	BtTime time();
private:
	FSBannTime *time_edit;
private slots:
	void performAction();
signals:
	void timeSelected(BtTime);
};

/**
 * A submenu that let the users choose a date and emits a signal with the selected date when the user
 * confirms the choice.
 */
class DateEditMenu : public sottoMenu
{
Q_OBJECT
public:
	DateEditMenu(QWidget *parent=0);
	QDate date();
private:
	FSBannDate *date_edit;
private slots:
	void performAction();
signals:
	void dateSelected(QDate);
};

#endif
