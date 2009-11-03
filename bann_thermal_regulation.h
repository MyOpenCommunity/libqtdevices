/*!
 * \bann_thermal_regulation.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief A set of full screen banners
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef BANN_THERMAL_REGULATION_H
#define BANN_THERMAL_REGULATION_H

#include "banner.h"
#include "bann3_buttons.h" // bann3But
#include "bann4_buttons.h" // bann4But
#include "bann1_button.h" // bannPuls
#include "device_status.h"
#include "bttime.h"
#include "main.h"
#include "page.h"

#include <QLayout>
#include <QButtonGroup>
#include <QDomNode>
#include <QLCDNumber>
#include <QMap>

class device;
class ThermalDevice;
class ThermalDevice4Zones;
class ThermalDevice99Zones;
class temperature_probe_controlled;
class TimeEditMenu;
class DateEditMenu;
class ProgramMenu;
class WeeklyMenu;
class ScenarioMenu;
class BtTimeEdit;
class BtDateEdit;
class sottoMenu;
class NavigationPage;

class QLabel;

enum Season
{
	SUMMER = 0,
	WINTER = 1,
};


/**
 * An interface class for all full screen banners.
 */
class BannFullScreen : public banner
{
Q_OBJECT
protected:
	BannFullScreen(QWidget *parent);
	BtButton *getButton(QString img);
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

/// Factory function to get banners
NavigationPage *getPage(BannID id, QWidget *parent, QDomNode n, QString ind_centrale,
			TemperatureScale scale = CELSIUS);


class NavigationPage : public Page
{
Q_OBJECT
public:
	NavigationPage(QWidget *parent);

protected:
	NavigationBar *createNavigationBar(const QString &forwardicon = QString());

signals:
	void backClick();
	void upClick();
	void downClick();

protected:
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
	PageSimpleProbe(QWidget *parent, QDomNode n, TemperatureScale scale = CELSIUS);
	virtual void Draw();
public slots:
	virtual void status_changed(QList<device_status*> sl);

protected:
	/// Measured temperature label and string
	QLabel *temp_label;
	unsigned temp;
	/// Zone description label and string
	QLabel *descr_label;
	QString descr;
	/// Temperature scale
	TemperatureScale temp_scale;
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
	PageProbe(QDomNode n, temperature_probe_controlled *_dev, ThermalDevice *thermo_reg, QWidget *parent,
		  TemperatureScale scale = CELSIUS);
	virtual void Draw();
	BtButton *customButton();
public slots:
	virtual void status_changed(QList<device_status*> sl);
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

	QDomNode conf_root;
	temperature_probe_controlled *dev;
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

	/// absolute position of local temp label
	static const unsigned LOCAL_TEMP_X = (MAX_WIDTH - 50) / 2;
	static const unsigned LOCAL_TEMP_Y = (MAX_HEIGHT - 50) / 2;
	static const unsigned LOCAL_TEMP_WIDTH = 50;
	static const unsigned LOCAL_TEMP_HEIGHT = 50;
	// absolute position of setpoint temperature
	static const unsigned SETPOINT_X = 70;
	static const unsigned SETPOINT_Y = 90;
	static const unsigned SETPOINT_WIDTH = 100;
	static const unsigned SETPOINT_HEIGHT = 50;

	bool isOff, isAntigelo;
	probe_status status;
	thermo_type_t probe_type;

	/// Time to wait (in msec) before sending a frame when the user presses on plus or minus buttons to modify setpoint temperature
	/// \sa setpoint
	const unsigned setpoint_delay;
	/// The delta of temperature (in 1/10 of degrees) when the user presses on plus or minus
	const unsigned setpoint_delta;
	/// The minimum temperature that can be set with manual operation
	int minimum_manual_temp;
	/// The maximum temperature that can be set with manual operation
	int maximum_manual_temp;
	BtButton *navbar_button;

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
	PageTermoReg(QDomNode n, QWidget *parent = 0);
	virtual void Draw();
	virtual ThermalDevice *dev() = 0;
public slots:
	virtual void status_changed(const StatusList &sl);
protected:
	/**
	 * Utility function to create settings menu for the thermal regulator device.
	 */
	virtual void createSettingsMenu(QWidget *back) = 0;

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
	void weekSettings(sottoMenu *settings, QDomNode conf, ThermalDevice *dev);

	/**
	 * Utility function to create the submenu to set the scenario program in thermal
	 * regulator device.
	 */
	void scenarioSettings(sottoMenu *settings, QDomNode conf, ThermalDevice99Zones *dev);

	/**
	 * Utility function to create the submenu to set manually the temperature
	 * for the thermal regulator device.
	 */
	void manualSettings(sottoMenu *settings, ThermalDevice *dev);

	/**
	 * Utility function to create the submenu for holiday settings.
	 */
	void holidaySettings(sottoMenu *settings, QDomNode conf, ThermalDevice *dev);

	/**
	 * Utility function to create the submenu for weekend settings.
	 */
	void weekendSettings(sottoMenu *settings, QDomNode conf, ThermalDevice *dev);

	/**
	 * Utility function to create off, antifreeze and summer/winter banners.
	 */
	void createButtonsBanners(ThermalDevice *dev);

	/// The settings menu of the thermal regulator
	sottoMenu *settings;
	/// A reference to the configuration of the thermal regulator
	QDomNode conf_root;

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
private:
	enum weekend_t
	{
		WEEKEND = 0,
		HOLIDAY,
	};
	/// A flag to determine if the user started a weekend or holiday selection
	weekend_t weekendHolidayStatus;
	/**
	 * Utility function to create the submenu structure needed for holiday and weekend mode.
	 * \param icon The icon to be visualized on the banner
	 * \return The banner that will open the date edit menu
	 */
	banner *createHolidayWeekendBanner(sottoMenu *settings, QString icon);

	DateEditMenu *createDateEdit(sottoMenu *settings);
	TimeEditMenu *createTimeEdit(sottoMenu *settings);
	WeeklyMenu *createProgramChoice(sottoMenu *settings, QDomNode conf, device *dev);

	/// Label and string that may be visualized
	QLabel *description_label;
	QString description;
	/// Set visibility status for description (in off, antifreeze, holiday and weekend description should not be visible)
	bool description_visible;
	/// Status icon (summer/winter)
	QLabel *season_icon;
	/// Mode icon (off, protection, manual, week program, holiday, weekend)
	QLabel *mode_icon;


	QDate date_end;
	BtTime time_end;
	TimeEditMenu *time_edit;
	DateEditMenu *date_edit;
	ProgramMenu *program_choice;
	sottoMenu *manual_menu;
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
	PageTermoReg4z(QDomNode n, ThermalDevice4Zones *device, QWidget *parent, QWidget *back);
	virtual ThermalDevice *dev();
protected:
	virtual void createSettingsMenu(QWidget *back);
private:
	/**
	 * Utility function to create the submenu for timed manual operation mode.
	 * This is used only with 4 zones thermal regulators
	 */
	void timedManualSettings(sottoMenu *settings, ThermalDevice4Zones *dev);

	ThermalDevice4Zones *_dev;
	sottoMenu *timed_manual_menu;
private slots:
	void manualTimedSelected(BtTime time, int temp);
	void showSettingsMenu();
};


/**
 * The difference with PageTermoReg4z is in settings menu. 99 zones thermal regulators allow the user
 * to set the scenario and do not have a manual timed mode
 */
class PageTermoReg99z : public PageTermoReg
{
Q_OBJECT
public:
	PageTermoReg99z(QDomNode n, ThermalDevice99Zones *device, QWidget *parent, QWidget *back);
	virtual ThermalDevice *dev();
protected:
	virtual void createSettingsMenu(QWidget *back);
	virtual void setSeason(Season new_season);
private:
	void scenarioSettings(sottoMenu *settings, QDomNode conf, ThermalDevice99Zones *dev);

	ThermalDevice99Zones *_dev;
	ScenarioMenu *scenario_menu;
private slots:
	void scenarioSelected(int scenario);
	void showSettingsMenu();
};


/**
 * Displays information about a controlled probe with fancoil. In addition to PageProbe, it displays
 * at the bottom of the page 4 buttons to set the speed of fancoil.
 */
class PageFancoil : public PageProbe
{
Q_OBJECT
public:
	PageFancoil(QDomNode n, temperature_probe_controlled *_dev, ThermalDevice *thermo_reg, QWidget *parent,
		    TemperatureScale scale = CELSIUS);
	virtual void Draw();
	virtual void status_changed(QList<device_status*> sl);
private:
	/**
	 * Creates fancoil buttons and loads icons
	 */
	void createFancoilButtons();
	QButtonGroup fancoil_buttons;
	/// A mapping between speed values and fancoil buttons
	QMap<int, int> speed_to_btn_tbl;
	QMap<int, int> btn_to_speed_tbl;
	int fancoil_status;
private slots:
	void handleFancoilButtons(int pressedButton);
};


class FSBannManual : public BannFullScreen
{
Q_OBJECT
public:
	FSBannManual(QWidget *parent, ThermalDevice *_dev, TemperatureScale scale = CELSIUS);
	virtual void Draw();
	virtual BtButton *customButton();
public slots:
	void status_changed(const StatusList &sl);
protected:
	QVBoxLayout main_layout;
	/// The button to be set on the navbar
	BtButton *navbar_button;
	/// The setpoint temperature set on the interface. The scale is given by temp_scale
	int temp;
	TemperatureScale temp_scale;
private:
	QString descr;
	QLabel *descr_label;
	QLabel *temp_label;
	ThermalDevice *dev;
	int maximum_manual_temp;
	int minimum_manual_temp;
	unsigned setpoint_delta;
private slots:
	void incSetpoint();
	void decSetpoint();
	void performAction();
signals:
	void temperatureSelected(unsigned);
};


/**
 * A fullscreen banner to edit setpoint temperature and the duration of manual settings
 */
class FSBannManualTimed : public FSBannManual
{
Q_OBJECT
public:
	FSBannManualTimed(QWidget *parent, ThermalDevice4Zones *_dev, TemperatureScale scale = CELSIUS);
	void setMaxHours(int max);
	void setMaxMinutes(int max);
private:
	ThermalDevice4Zones *dev;
	/// TimeEdit widget
	BtTimeEdit *time_edit;
private slots:
	void performAction();
signals:
	void timeAndTempSelected(BtTime, int);
};


class FSBannDate : public BannFullScreen
{
Q_OBJECT
public:
	FSBannDate(QWidget *parent);
	QDate date();
private:
	QVBoxLayout main_layout;
	BtDateEdit *date_edit;
};


class FSBannTime : public BannFullScreen
{
Q_OBJECT
public:
	FSBannTime(QWidget *parent);
	BtTime time();
private:
	QVBoxLayout main_layout;
	BtTimeEdit *time_edit;
	int hours, minutes;
};


/**
 * This banner shuts the thermal regulator off when clicked. Before using it, be sure to
 * set the address of the thermal regulator.
 * It displays a button in the center with icon "OFF".
 */
class BannOff : public bann3But
{
Q_OBJECT
public:
	BannOff(QWidget *parent, ThermalDevice *_dev);
public slots:
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
class BannAntifreeze : public bann3But
{
Q_OBJECT
public:
	BannAntifreeze(QWidget *parent, ThermalDevice *_dev);
public slots:
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
 * This banner sets the thermal regulator in summer or winter status, depending on the
 * button pressed.
 * It displays two buttons at the center, one with the summer icon and one with the winter icon.
 */
class BannSummerWinter : public bann4But
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
class BannWeekly : public bannPuls
{
Q_OBJECT
public:
	BannWeekly(QWidget *parent);
	void setProgram(int prog);
public slots:
	void performAction();
private:
	int program;
signals:
	void programNumber(int);
};

#endif // BANN_THERMAL_REGULATION_H
