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


#ifndef THERMAL_REGULATOR_H
#define THERMAL_REGULATOR_H

#include "plantmenu.h" // NavigationPage
#include "bttime.h"

#include <QDate>

class SettingsPage;
class BannSinglePuls;
class PageSetDate;
class PageSetTime;
class PageSetDateTime;
class WeeklyMenu;
class ProgramMenu;
class ScenarioMenu;
class BtTimeEdit;
class BtDateEdit;


enum Season
{
	SUMMER = 0,
	WINTER = 1,
};


/**
 *
 */
class PageTermoReg : public NavigationPage
{
Q_OBJECT
public:
	PageTermoReg(QDomNode n);
	virtual ThermalDevice *dev() = 0;

public slots:
	virtual void valueReceived(const DeviceValues &values_list);

protected:
	void showDescription(const QString &desc);
	void hideDescription();

	/**
	 * Utility function to create settings menu for the thermal regulator device.
	 */
	virtual void createSettingsMenu(QDomNode regulator_node) = 0;
	void createSettingsItem(QDomNode item, SettingsPage *settings, ThermalDevice *dev);

	/**
	 * Set the icon on the main page of thermal regulator and calls setSeason() on
	 * his own ProgramMenu's.
	 */
	virtual void setSeason(int new_season);

	/**
	 * Utility function to find in the DOM the program description to be displayed on screen.
	 * \param season The season we are interested into. It must be either "summer" or "winter".
	 * \param what Either "prog" or "scen"
	 * \param program_number The number of the program we are looking the description of.
	 * \return The description of the program as written in DOM.
	 */
	QString lookupProgramDescription(QString season, QString what, int program_number);

	/**
	 * Utility function to create the submenu to set the weekly program in thermal
	 * regulator device.
	 */
	void weekSettings(QDomNode n, SettingsPage *settings, QMap<QString, QString> programs, ThermalDevice *dev);

	/**
	 * Utility function to create the submenu to set the scenario program in thermal
	 * regulator device.
	 */
	void scenarioSettings(QDomNode n, SettingsPage *settings, QMap<QString, QString> scenarios, ThermalDevice99Zones *dev);

	/**
	 * Utility function to create the submenu to set manually the temperature
	 * for the thermal regulator device.
	 */
	void manualSettings(QDomNode n, SettingsPage *settings, ThermalDevice *dev);

	/**
	 * Utility function to create the submenu for holiday settings.
	 */
	void holidaySettings(QDomNode n, SettingsPage *settings, QMap<QString, QString> programs, ThermalDevice *dev);

	/**
	 * Utility function to create the submenu for weekend settings.
	 */
	void weekendSettings(QDomNode n, SettingsPage *settings, QMap<QString, QString> programs, ThermalDevice *dev);

	/**
	 * Utility function to create off, antifreeze and summer/winter banners.
	 */
	void createButtonsBanners(SettingsPage *settings, ThermalDevice *dev);

	/// The settings menu of the thermal regulator
	SettingsPage *settings;

	/// list of programs/scenarios defined in the configuration
	QMap<QString, QString> programs, scenarios;

	TemperatureScale temp_scale;

private slots:
	/**
	 * The following slots are used to control the status of input for holiday/weekend mode.
	 * Three submenus are shown in sequence: date input, time input and program selection. The slots
	 * are used to go forward and backwards in this sequence. At the beginning a flag is set to determine,
	 * at the end, whenever the user started a holiday or weekend selection and to issue the correct frames.
	 * Show the first submenu of holiday settings (date_edit)
	 */
	void holidaySettingsStart();

	/**
	 * Show the first submenu of weekend settings (date_edit)
	 */
	void weekendSettingsStart();

	/**
	 * User confirmed date, go to time editing.
	 */
	void dateSelected(QDate d);

	/**
	 * User confirmed date and time, go to time editing.
	 */
	void dateTimeSelected(QDate d, BtTime t);

	/**
	 * User cancelled time editing, go back to date editing.
	 */
	void timeCancelled();

	/**
	 * User confirmed time editing, go on with program selection.
	 */
	void timeSelected(BtTime t);

	/**
	 * User cancelled program selection, go back to time editing.
	 */
	void programCancelled();

	/**
	 * User confirmed program. Send the relevant frames through the device.
	 */
	void weekendHolidaySettingsEnd(int program);
	// End of holiday/weekend related functions


	void manualSelected(unsigned temp);

	void weekProgramSelected(int program);

	// display the settings menu page
	void showSettingsMenu();

private:
	enum weekend_t
	{
		WEEKEND = 0,
		HOLIDAY,
	};
	/// A flag to determine if the user started a weekend or holiday selection
	weekend_t weekendHolidayStatus;
	QString holiday_title, weekend_title;

	/**
	 * Utility function to create the submenu structure needed for holiday and weekend mode.
	 * \param icon The icon to be visualized on the banner
	 * \return The banner that will open the date edit menu
	 */
	BannSinglePuls *createHolidayWeekendBanner(SettingsPage *settings, QString icon, QString description);

	PageSetDate *createDateEdit(SettingsPage *settings);
	PageSetTime *createTimeEdit(SettingsPage *settings);
	PageSetDateTime *createDateTimeEdit(SettingsPage *settings);
	WeeklyMenu *createProgramChoice(SettingsPage *settings, QMap<QString, QString> programs, device *dev);

	/// Label and string that may be visualized
	QLabel *description_label;
	/// Status icon (summer/winter)
	QLabel *season_icon;
	/// Mode icon (off, protection, manual, week program, holiday, weekend)
	QLabel *mode_icon;

	QString icon_summer, icon_winter;
	QStringList status_icons;
	QDate date_end;
	BtTime time_end;
	PageSetTime *time_edit;
	PageSetDate *date_edit;
	PageSetDateTime *date_time_edit;
	ProgramMenu *program_choice;
	WeeklyMenu *program_menu;
};


/**
 * Displays information about the thermal regulator device. Information may be: operation mode, label
 * describing more information about mode (for example, the program name), current season.
 * Operation mode and current season are icons and are always present. The label is optional and depends
 * on the active mode: it is present for manual (setpoint temperature) and week program (program name).
 */
class PageTermoReg4z : public PageTermoReg
{
Q_OBJECT
public:
	PageTermoReg4z(QDomNode n, ThermalDevice4Zones *device);
	virtual ThermalDevice *dev();
protected:
	virtual void createSettingsMenu(QDomNode regulator_node);
	void createSettingsItem(QDomNode item, SettingsPage *settings, ThermalDevice4Zones *dev);
private:
	/**
	 * Utility function to create the submenu for timed manual operation mode.
	 * This is used only with 4 zones thermal regulators
	 */
	void timedManualSettings(QDomNode n, SettingsPage *settings, ThermalDevice4Zones *dev);

	ThermalDevice4Zones *_dev;
private slots:
	void manualTimedSelected(BtTime time, int temp);
};


/**
 * The difference with PageTermoReg4z is in settings menu. 99 zones thermal regulators allow the user
 * to set the scenario and do not have a manual timed mode
 */
class PageTermoReg99z : public PageTermoReg
{
Q_OBJECT
public:
	PageTermoReg99z(QDomNode n, ThermalDevice99Zones *device);
	virtual ThermalDevice *dev();

protected:
	virtual void createSettingsMenu(QDomNode regulator_node);
	virtual void setSeason(Season new_season);
	void createSettingsItem(QDomNode item, SettingsPage *settings, ThermalDevice99Zones *dev);

private:
	void scenarioSettings(QDomNode n, SettingsPage *settings, QMap<QString, QString> scenarios, ThermalDevice99Zones *dev);

	ThermalDevice99Zones *_dev;
	ScenarioMenu *scenario_menu;

private slots:
	void scenarioSelected(int scenario);
};


class PageManual : public Page
{
Q_OBJECT
public:
	PageManual(ThermalDevice *_dev, TemperatureScale scale = CELSIUS);

public slots:
	void valueReceived(const DeviceValues &values_list);

protected:
	void updateTemperature();

protected slots:
	virtual void performAction();

protected:
	QWidget content;
	QVBoxLayout main_layout;
	/// The setpoint temperature set on the interface. The scale is given by temp_scale
	int temp;
	TemperatureScale temp_scale;

private:
	QLabel *temp_label;
	ThermalDevice *dev;
	int maximum_manual_temp;
	int minimum_manual_temp;
	unsigned setpoint_delta;

private slots:
	void incSetpoint();
	void decSetpoint();

signals:
	void temperatureSelected(unsigned);
};


/**
 * A fullscreen banner to edit setpoint temperature and the duration of manual settings
 */
class PageManualTimed : public PageManual
{
Q_OBJECT
public:
	PageManualTimed(ThermalDevice4Zones *_dev, TemperatureScale scale = CELSIUS);
	void setMaxHours(int max);
	void setMaxMinutes(int max);

protected slots:
	virtual void performAction();

private:
	ThermalDevice4Zones *dev;
	/// TimeEdit widget
	BtTimeEdit *time_edit;

signals:
	void timeAndTempSelected(BtTime, int);
};


class PageSetDate : public Page
{
Q_OBJECT
public:
	PageSetDate();
	QDate date();
signals:
	void dateSelected(QDate);
private slots:
	void performAction();
private:
	QWidget content;
	QVBoxLayout main_layout;
	BtDateEdit *date_edit;
};


class PageSetTime : public Page
{
Q_OBJECT
public:
	PageSetTime();
	BtTime time();
signals:
	void timeSelected(BtTime);
private slots:
	void performAction();
private:
	QWidget content;
	QVBoxLayout main_layout;
	BtTimeEdit *time_edit;
};


/**
 * A base class for submenus that allow to choose one program in a list. The list changes
 * when season changes (summer/winter).
 * This class emits a signal when a program is clicked. This signal should be used to close
 * the submenu and to take further action, for example sending a frame to the thermal regulator.
 */
class ProgramMenu : public BannerPage
{
Q_OBJECT
public:
	ProgramMenu(QWidget *parent, QMap<QString, QString> descriptions, QString title);
	virtual void createSummerBanners() = 0;
	virtual void createWinterBanners() = 0;
	void setSeason(Season new_season);
protected:
	QString summer_icon, winter_icon;
	int season;
	QMap<QString, QString> descriptions;
	/**
	 * \param season Either "summer" or "winter"
	 * \param what Either "prog" or "scen"
	 */
	void createSeasonBanner(QString season, QString icon);
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
	WeeklyMenu(QWidget *parent, QMap<QString, QString> programs, QString title = "");
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
	ScenarioMenu(QWidget *parent, QMap<QString, QString> scenarios, QString title = "");
	virtual void createSummerBanners();
	virtual void createWinterBanners();
};

#endif // THERMAL_REGULATOR_H
