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

#include "bannerpage.h"
#include "navigation_bar.h" // AbstractNavigationBar
#include "main.h" // TemperatureScale
#include "thermal_device.h" // thermo_type_t

#include <QVBoxLayout>
#include <QButtonGroup>

class NavigationPage;
class ControlledProbeDevice;
class ThermalDevice;
class QLabel;


enum ThermalPageID
{
	fs_4z_thermal_regulator = 1,          /*!< 4-zone thermal regulator device */
	fs_4z_probe,                          /*!< 4-zone controlled probe */
	fs_4z_fancoil,                        /*!< 4-zone controlled probe with fancoil */
	fs_99z_thermal_regulator,             /*!< 99-zone thermal regulator device */
	fs_99z_probe,                         /*!< 99-zone controlled probe */
	fs_99z_fancoil,                       /*!< 99-zone controlled probe with fancoil */
};


/*!
	\ingroup ThermalRegulation
	\brief Contains links to the thermal regulator and to the controlled zones.
 */
class PlantMenu : public BannerPage
{
Q_OBJECT
public:
	PlantMenu(const QDomNode &conf);
	static banner *getBanner(const QDomNode &item_node);

	virtual int sectionId() const;

private:
	void loadItems(const QDomNode &conf);

	/**
	 * Utility function to create a banner in the plant menu and the corresponding full
	 * screen banner.
	 * \param n             The node in the configuration DOM which is a `item' tag
	 * \param central_icon  The path of the icon to be shown in plant menu banner
	 * \param type          The type of full screen banner that is linked to the banner in plant menu
	 * \return A pointer to the small banner that will show the corresponding full screen banner.
	 */
	NavigationPage *addMenuItem(QDomNode n, QString central_icon, ThermalPageID type);

	/// Address of the thermal regulation device
	/// empty in case of 99z device
	QString ind_centrale;
	// The openserver id for the thermal regulation device
	int openserver_id;
};


/*!
	\ingroup ThermalRegulation
	\brief Navigation bar with an OK button on the right and a "back" button on the left (only for TS 3.5'')
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


/*!
	\ingroup ThermalRegulation
	\brief Base class for thermal regulator and zone pages.
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


/*!
	\ingroup ThermalRegulation
	\brief Base class for zone pages; displays zone description and temperature.
 */
// TODO merge with PageProbe
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


/*!
	\ingroup ThermalRegulation
	\brief Display information for a controlled zone.

	Displays the target temperature (or an icon for antifreeze/off) and the
	local offset.
 */
class PageProbe : public PageSimpleProbe
{
Q_OBJECT
public:
	PageProbe(QDomNode n, ControlledProbeDevice *_dev, ThermalDevice *thermo_reg,
		TemperatureScale scale = CELSIUS);

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


/*!
	\ingroup ThermalRegulation
	\brief Display information for a controlled zone with fancoil.
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

#endif // PLANTMENU_H
