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


#ifndef BANN_THERMAL_REGULATION_H
#define BANN_THERMAL_REGULATION_H

#include "bann1_button.h" // BannSinglePuls
#include "bann2_buttons.h" // Bann2CentralButtons
#include "bttime.h"
#include "main.h"
#include "page.h"
#include "thermal_device.h" // thermo_type_t, ThermalDevice*
#include "navigation_bar.h" // AbstractNavigationBar

#include <QLayout>
#include <QButtonGroup>
#include <QDomNode>
#include <QLCDNumber>
#include <QMap>
#include <QDate>

class device;
class ControlledProbeDevice;
class TimeEditMenu;
class DateEditMenu;
class ProgramMenu;
class WeeklyMenu;
class ScenarioMenu;
class BtTimeEdit;
class BtDateEdit;
class NavigationPage;
class PageSetDate;
class PageSetTime;
class PageSetDateTime;
class SettingsPage;

class QLabel;

enum Season
{
	SUMMER = 0,
	WINTER = 1,
};


enum BannID
{
	fs_nc_probe = 1,                      // not controlled probe
	fs_4z_thermal_regulator,              // 4 zones thermal regulator device
	fs_4z_probe,                          // 4 zones controlled probe
	fs_4z_fancoil,                        // 4 zones controlled probe with fancoil
	fs_99z_thermal_regulator,             // 99 zones thermal regulator device
	fs_99z_probe,                         // 99 zones controlled probe
	fs_99z_fancoil,                       // 99 zones controlled probe with fancoil
};

/// Factory function to get pages
NavigationPage *getPage(BannID id, QDomNode n, QString ind_centrale, int openserver_id, TemperatureScale scale, banner* bann);


/**
 * Simple widget with an OK button on the right
 * and a "back" button on the left
 */
class ThermalNavigation : public AbstractNavigationBar
{
Q_OBJECT
public:
	ThermalNavigation(QWidget *parent = 0);

signals:
	void forwardClick();
	void backClick();
};


/**
 * Base class for thermal menu subpages with a navigation bar at the bottom
 */
class NavigationPage : public Page
{
Q_OBJECT
public:
	NavigationPage();

signals:
	void backClick();
	void upClick();
	void downClick();

protected:
	NavigationBar *createNavigationBar(const QString &forwardicon = QString(), int title_height = TITLE_HEIGHT);

protected:
	/// Content widget
	QWidget content;
	/// Global layout for the content
	QVBoxLayout main_layout;
	/// Navigation bar
	NavigationBar *nav_bar;
};


/**
 * A base class for banners that represent a probe. It displays a label with zone name on top
 * and the measured temperature.
 */
class PageSimpleProbe : public NavigationPage
{
Q_OBJECT
public:
	PageSimpleProbe(QDomNode n, TemperatureScale scale = CELSIUS);

public slots:
	virtual void valueReceived(const DeviceValues &values_list);

protected:
	void setTemperature(unsigned temp);

protected:
	/// Measured temperature label and string
	QLabel *temp_label;
	/// Temperature scale
	TemperatureScale temp_scale;
	// button to toggle manual/automatic mode
	BtButton *toggle_mode;
};


/**
 * Displays information about a probe controlled by a thermal regulator.
 * It has a label with setpoint temperature and local status. In case the status is
 * protection or off, it displays the relative icon.
 */
class PageProbe : public PageSimpleProbe
{
Q_OBJECT
public:
	PageProbe(QDomNode n, ControlledProbeDevice *_dev, ThermalDevice *thermo_reg,
		TemperatureScale scale = CELSIUS);

	virtual void inizializza();

public slots:
	virtual void valueReceived(const DeviceValues &values_list);
protected:
	void updatePointLabel();
	void updateControlState();
protected:
	/// Setpoint temperature. The scale of the temperature may be Celsius or Fahrenheit, depending on the value
	/// of PageSimpleProbe::temp_scale. Units represent 1/10 of degree, for example -23.5 Celsius degrees
	/// are represented as -235 if temp_scale == CELSIUS, or -103 if temp_scale == FAHRENHEIT
	int setpoint;
	QLabel  *setpoint_label;
	/// This flag is used to syncrhonize with other devices in the home. True when setpoint temperature is modified
	/// by this BTouch, false otherwise
	bool delta_setpoint;

	BtButton *btn_minus, *btn_plus;
	QLabel *icon_off, *icon_antifreeze;
	// setpoint e' la temperatura impostata mentre la rotellina e' `locale'
	// le impostazioni per il locale (rotellina) sono nella specifica del protocollo,
	// ie. 0 = (rotella su) 0, 1 = 1, ... , 11 = -1, 12 = -2, 13 = -3, 4 = Off, 5 = Antigelo
	QString local_temp;
	QLabel *local_temp_label;
	QHBoxLayout bottom_icons;

	ControlledProbeDevice *dev;
private:
	/**
	 * Called when it's needed to set the device to manual operation. A conversion to Celsius degrees is done if needed.
	 */
	void setDeviceToManual();

	enum probe_status
	{
		AUTOMATIC,
		MANUAL
	};

	bool isOff, isAntigelo;
	probe_status status;
	thermo_type_t probe_type;
	QString probe_icon_auto, probe_icon_manual;

	/// The delta of temperature (in 1/10 of degrees) when the user presses on plus or minus
	const unsigned setpoint_delta;
	/// The minimum temperature that can be set with manual operation
	int minimum_manual_temp;
	/// The maximum temperature that can be set with manual operation
	int maximum_manual_temp;

private slots:
	void changeStatus();

	/**
	 * Called when the user press on the plus button to increase setpoint temperature.
	 */
	void incSetpoint();

	/**
	 * Called when the user press on the minus button to decrease setpoint temperature.
	 */
	void decSetpoint();
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


/**
 * Displays information about a controlled probe with fancoil. In addition to PageProbe, it displays
 * at the bottom of the page 4 buttons to set the speed of fancoil.
 */
class PageFancoil : public PageProbe
{
Q_OBJECT
public:
	PageFancoil(QDomNode n, ControlledProbeDevice *_dev, ThermalDevice *thermo_reg,
		TemperatureScale scale = CELSIUS);
	virtual void valueReceived(const DeviceValues &values_list);
protected:
	void setFancoilStatus(int status);
private:
	/**
	 * Creates fancoil buttons and loads icons
	 */
	void createFancoilButtons();
	QButtonGroup fancoil_buttons;
	/// A mapping between speed values and fancoil buttons
	QMap<int, int> speed_to_btn_tbl;
	QMap<int, int> btn_to_speed_tbl;
private slots:
	void handleFancoilButtons(int pressedButton);
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
 * This banner shuts the thermal regulator off when clicked. Before using it, be sure to
 * set the address of the thermal regulator.
 * It displays a button in the center with icon "OFF".
 */
class BannOff : public BannCenteredButton
{
Q_OBJECT
public:
	BannOff(QWidget *parent, ThermalDevice *_dev);
private slots:
	/**
	 * Shut down the thermal regulator
	 */
	void performAction();
private:
	/// The device that this banner sends commands to
	ThermalDevice *dev;
signals:
	void clicked();
};


/**
 * This banner sets the thermal regulator in antifreeze protection. Be sure to set the
 * address of the device.
 * It displays one button at the center with icon antifreeze on it.
 */
class BannAntifreeze : public BannCenteredButton
{
Q_OBJECT
public:
	BannAntifreeze(QWidget *parent, ThermalDevice *_dev);
private slots:
	/**
	 * Set thermal regulator in antifreeze protection
	 */
	void performAction();
private:
	/// The device that this banner sends commands to
	ThermalDevice *dev;
signals:
	void clicked();
};


/**
 * Banner with two buttons, one to set the thermal regulator to "off" and one
 * to set antifreeze mode.
 */
class BannOffAntifreeze : public Bann2CentralButtons
{
Q_OBJECT
public:
	BannOffAntifreeze(QWidget *parent, ThermalDevice *_dev);

signals:
	void clicked();

private slots:
	void setOff();
	void setAntifreeze();

private:
	/// The device that this banner sends commands to
	ThermalDevice *dev;
};


/**
 * This banner sets the thermal regulator in summer or winter status, depending on the
 * button pressed.
 * It displays two buttons at the center, one with the summer icon and one with the winter icon.
 */
class BannSummerWinter : public Bann2CentralButtons
{
Q_OBJECT
public:
	BannSummerWinter(QWidget *parent, ThermalDevice *_dev);
	enum seasons {WINTER, SUMMER};
public slots:
	void setSummer();
	void setWinter();
private:
	seasons status;
	/// The device that this banner sends commands to
	ThermalDevice *dev;
signals:
	void clicked();
};


/**
 * This banner emits a signal with an int, corresponding to the program set with setProgram(). Default
 * program is 1, so be sure to set the program you want to be set before using it.
 * It displays a not clickable image on the center, an ok button on the right and program description
 * below.
 */
class BannWeekly : public BannSinglePuls
{
Q_OBJECT
public:
	BannWeekly(QWidget *parent, int index);

private slots:
	void performAction();

signals:
	void programNumber(int);

private:
	int index;
};

#endif // BANN_THERMAL_REGULATION_H
