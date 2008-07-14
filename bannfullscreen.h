/*!
 * \bannfullscreen.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief A set of full screen banners
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef BANNFULLSCREEN_H
#define BANNFULLSCREEN_H

#include "banner.h"
#include "device_status.h"
#include "btwidgets.h"

#include <qlayout.h>
#include <qbuttongroup.h>
#include <qdom.h>
#include <qlcdnumber.h>
#include <qdatetime.h>
#include <qtimer.h>

class device;
class thermal_regulator;
class thermal_regulator_4z;
class thermal_regulator_99z;
class temperature_probe_controlled;
class TimeEditMenu;
class DateEditMenu;
class ProgramMenu;
class WeeklyMenu;
class ScenarioMenu;
class BtLabelEvo;


/**
 * An interface class for all full screen banners.
 */
class BannFullScreen : public banner
{
Q_OBJECT
public:
	BannFullScreen(QWidget *parent, const char *name);
	virtual void Draw();
	void setSecondForeground(QColor fg2);

	/**
	 * This function is reimplemented from banner because fullscreen banners don't use
	 * the same items as banners (BUT1, BUT2, TEXT and so on).
	 */
	virtual void setBGColor(QColor bg);
	virtual void setFGColor(QColor bg);
public slots:
	/**
	 * Called whenever the status of the device associated with the banner changes, so that
	 * the banner can display the variations.
	 */
	virtual void status_changed(QPtrList<device_status> list) = 0;
protected:
	QColor second_fg;
	BtButton *getButton(const char *img);
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
	fs_date_edit,                         // settings: date edit
	fs_time_edit,                         // settings: time edit
};

/// Factory function to get banners
BannFullScreen *getBanner(BannID id, QWidget *parent, QDomNode n, QString ind_centrale);

/**
 * A base class for banners that represent a probe. It displays a label with zone name on top
 * and the measured temperature.
 */
class FSBannSimpleProbe : public BannFullScreen
{
Q_OBJECT
public:
	FSBannSimpleProbe(QWidget *parent, QDomNode n, const char *name = 0);
	virtual void Draw();
	void setSecondForeground(QColor fg2);
public slots:
	virtual void status_changed(QPtrList<device_status> list);

protected:
	/// Global layout for the banner
	QVBoxLayout main_layout;
	/// Measured temperature label and string
	BtLabelEvo *temp_label;
	QString temp;
	/// Zone description label and string
	BtLabelEvo *descr_label;
	QString descr;
};

/**
 * Displays information about a probe controlled by a thermal regulator.
 * It has a label with setpoint temperature and local status. In case the status is
 * protection or off, it displays the relative icon.
 */
class FSBannProbe : public FSBannSimpleProbe
{
Q_OBJECT
public:
	FSBannProbe(QDomNode n, temperature_probe_controlled *_dev, bool change_status, QWidget *parent, const char *name = 0);
	virtual void Draw();
	BtButton *customButton();
public slots:
	virtual void status_changed(QPtrList<device_status> list);
protected:
	/// Setpoint temperature. All temperatures are expressed in 1/10 of degrees, temperatures > 1000 are negative.
	/// Example: 1235 is -23.5 (Celsius degrees), 395 is 39.5. Precision is generally 5.
	unsigned setpoint;
	BtLabelEvo  *setpoint_label;
	/// This flag is used to syncrhonize with other devices in the home. True when setpoint temperature is modified
	/// by this BTouch, false otherwise
	bool delta_setpoint;

	BtButton *btn_minus, *btn_plus;
	BtLabelEvo *icon_off, *icon_antifreeze;
	// FIXME: setpoint e' la temperatura impostata mentre la rotellina e' `locale'
	// le impostazioni per il locale (rotellina) sono nella specifica del protocollo,
	// ie. 0 = (rotella su) 0, 1 = 1, ... , 11 = -1, 12 = -2, 13 = -3, 4 = Off, 5 = Antigelo
	QString local_temp;
	BtLabelEvo *local_temp_label;

	QDomNode conf_root;
	temperature_probe_controlled *dev;
private:
	enum probe_status
	{
		AUTOMATIC,
		MANUAL
	};

	bool isOff, isAntigelo;
	probe_status status;
	bool status_change_enabled;

	/// Send a setpoint frame only if 2 seconds are elapsed
	QTimer setpoint_timer;
	/// Time to wait (in msec) before sending a frame when the user presses on plus or minus buttons to modify setpoint temperature
	/// \sa setpoint
	const unsigned setpoint_delay;
	/// The delta of temperature (in 1/10 of degrees) when the user presses on plus or minus
	const unsigned setpoint_delta;
	/// The minimum temperature that can be set with manual operation
	const unsigned minimum_manual_temp;
	/// The maximum temperature that can be set with manual operation
	const unsigned maximum_manual_temp;
	BtButton *navbar_button;

private slots:
	void changeStatus();
	void setSetpoint();

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
class FSBannTermoReg : public BannFullScreen
{
Q_OBJECT
public:
	FSBannTermoReg(QDomNode n, QWidget *parent = 0, const char *name = 0);
	virtual void Draw();
	BtButton *customButton();
	virtual thermal_regulator *dev() = 0;
public slots:
	virtual void status_changed(QPtrList<device_status> list);
protected:
	/**
	 * Utility function to create settings menu for the thermal regulator device.
	 */
	virtual void createSettingsMenu() = 0;

	/**
	 * Utility function to find in the DOM the program description to be displayed on screen.
	 * \param season The season we are interested into. It must be either "summer" or "winter".
	 * \param program_number The number of the program we are looking the description of.
	 * \return The description of the program as written in DOM.
	 */
	QString lookupProgramDescription(QString season, int program_number);

	/**
	 * Utility function to find in the DOM the program description to be displayed on screen.
	 * \param season The season we are interested into. It must be either "summer" or "winter".
	 * \param scenario_number The number of the scenario we are looking the description of.
	 * \return The description of the program as written in DOM.
	 */
	QString lookupScenarioDescription(QString season, int scenario_number);

	/**
	 * Utility function to create the submenu to set the weekly program in thermal
	 * regulator device.
	 */
	void weekSettings(sottoMenu *settings, QDomNode conf, thermal_regulator *dev);

	/**
	 * Utility function to create the submenu to set the scenario program in thermal
	 * regulator device.
	 */
	void scenarioSettings(sottoMenu *settings, QDomNode conf, thermal_regulator_99z *dev);

	/**
	 * Utility function to create the submenu to set manually the temperature
	 * for the thermal regulator device.
	 */
	void manualSettings(sottoMenu *settings, thermal_regulator *dev);

	/**
	 * Utility function to create the submenu for holiday settings.
	 */
	void holidaySettings(sottoMenu *settings, QDomNode conf, thermal_regulator *dev);

	/// The settings menu of the thermal regulator
	sottoMenu *settings;
	/// A reference to the configuration of the thermal regulator
	QDomNode conf_root;

	BtButton *navbar_button;
private slots:
	/**
	 * Show the first submenu of holiday settings (date_edit)
	 */
	void holidaySettingsStart();

	/**
	 * User cancelled date editing, go back to main settings menu.
	 */
	void dateCancelled();

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
	void timeSelected(QTime t);

	/**
	 * User cancelled program selection, go back to time editing.
	 */
	void programCancelled();

	/**
	 * User confirmed program. Send the relevant frames through the device.
	 */
	void holidaySettingsEnd(int program);

	void manualCancelled();
	void manualSelected(unsigned temp);

	void weekProgramCancelled();
	void weekProgramSelected(int program);
private:
	QVBoxLayout main_layout;
	/// Label and string that may be visualized
	BtLabelEvo *description_label;
	QString description;
	/// Status icon (summer/winter)
	BtLabelEvo *season_icon;
	/// Mode icon (off, protection, manual, week program, holiday, weekend)
	BtLabelEvo *mode_icon;
	QDate holiday_date_end;
	QTime holiday_time_end;
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
class FSBannTermoReg4z : public FSBannTermoReg
{
Q_OBJECT
public:
	FSBannTermoReg4z(QDomNode n, thermal_regulator_4z *device, QWidget *parent, const char *name = 0);
	virtual thermal_regulator *dev();
protected:
	virtual void createSettingsMenu();
private:
	/**
	 * Utility function to create the submenu for timed manual operation mode.
	 * This is used only with 4 zones thermal regulators
	 */
	void timedManualSettings(sottoMenu *settings, thermal_regulator_4z *dev);

	thermal_regulator_4z *_dev;
	sottoMenu *timed_manual_menu;
private slots:
	void manualTimedSelected(QTime time, int temp);
	void manualTimedCancelled();
};

class FSBannTermoReg99z : public FSBannTermoReg
{
Q_OBJECT
public:
	FSBannTermoReg99z(QDomNode n, thermal_regulator_99z *device, QWidget *parent, const char *name = 0);
	virtual thermal_regulator *dev();
protected:
	virtual void createSettingsMenu();
private:
	void scenarioSettings(sottoMenu *settings, QDomNode conf, thermal_regulator_99z *dev);

	thermal_regulator_99z *_dev;
	ScenarioMenu *scenario_menu;
private slots:
	void scenarioCancelled();
	void scenarioSelected(int scenario);
};

/**
 * Displays information about a controlled probe with fancoil. In addition to FSBannProbe, it displays
 * at the bottom of the page 4 buttons to set the speed of fancoil.
 */
class FSBannFancoil : public FSBannProbe
{
Q_OBJECT
public:
	FSBannFancoil(QDomNode n, temperature_probe_controlled *_dev, bool change_status, QWidget *parent, const char *name = 0);
	virtual void Draw();
	virtual void status_changed(QPtrList<device_status> list);
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
	FSBannManual(QWidget *parent, const char *name, thermal_regulator *_dev);
	virtual void Draw();
	virtual BtButton *customButton();
public slots:
	void status_changed(QPtrList<device_status> list);
protected:
	QVBoxLayout main_layout;
	/// The button to be set on the navbar
	BtButton *navbar_button;
	/// The setpoint temperature set on the interface
	unsigned temp;
private:
	QString descr;
	BtLabelEvo *descr_label;
	BtLabelEvo *temp_label;
	thermal_regulator *dev;
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
	FSBannManualTimed(QWidget *parent, const char *name, thermal_regulator_4z *_dev);
	virtual void Draw();
public slots:
	void status_changed(QPtrList<device_status> list);
private:
	thermal_regulator_4z *dev;
	/// TimeEdit widget
	BtTimeEdit *time_edit;
private slots:
	void performAction();
signals:
	void timeAndTempSelected(QTime, int);
};

class FSBannDate : public BannFullScreen
{
Q_OBJECT
public:
	FSBannDate(QWidget *parent, const char *name = 0);
	virtual void Draw();
	QDate date();
public slots:
	void status_changed(QPtrList<device_status> list);
private:
	QVBoxLayout main_layout;
	BtDateEdit *date_edit;
};

class FSBannTime : public BannFullScreen
{
Q_OBJECT
public:
	FSBannTime(QWidget *parent, const char *name = 0);
	virtual void Draw();
	QTime time();
public slots:
	void status_changed(QPtrList<device_status> list);
private:
	QVBoxLayout main_layout;
	BtTimeEdit *time_edit;
	int hours, minutes;
};

#endif // BANNFULLSCREEN_H
