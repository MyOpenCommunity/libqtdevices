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


#ifndef AIR_CONDITIONING_H
#define AIR_CONDITIONING_H

#include "page.h"
#include "bannerpage.h"
#include "airconditioning_device.h"

class AirConditioningInterface;
class AirConditioningDevice;
class banner;
class QDomNode;
class SplitSwing;
class SplitMode;
class SplitTemperature;
class SplitSpeed;
class NonControlledProbeDevice;
class GeneralSplit;

typedef AdvancedAirConditioningDevice::Status AirConditionerStatus;


namespace AirConditioningAdvanced
{
	AirConditionerStatus parseSettings(const QDomNode &values_node);
}

namespace AirConditioningPrivate
{
	// A global object that contains the devices for single splits (required
	// because the general off send the off for each single split device).
	class DeviceContainer : public QObject
	{
	Q_OBJECT
	public:
		void append(AirConditioningInterface *d);

	public slots:
		void sendGeneralOff();

	private:
		QList<AirConditioningInterface*> devices_list;
	};
}

/*!
	\defgroup AirConditioning Air Conditioning

	This module is a part of the \ref ThermalRegulation section.

	It allows the user to control, through the AirConditioning page, a list of
	splits. The list is composed by basic splits (SingleSplit), advanced splits
	(AdvancedSingleSplit) plus a general (GeneralSplit) that can control a
	group of splits (even the entire group).

	In the basic interface, for each split there is a SplitPage that contains
	all the scenarios (SplitScenario) configured for the split plus the off
	command.

	The advanced interface is similar: for each split there is an AdvancedSplitPage
	that allows the user to set a scenario
	(AdvancedSplitScenario) or switch off the split. In addition, it contains
	a CustomScenario banner that permits to customize the scenario using the
	SplitSettings page.
*/

/*!
	\ingroup AirConditioning
	\brief The main page, that contains banners for all the splits configured
	and for the general, if present.
*/
class AirConditioning : public BannerPage
{
Q_OBJECT
public:
	AirConditioning(const QDomNode &config_node);
	static banner *getBanner(const QDomNode &item_node);

	virtual int sectionId() const;

private:
	static NonControlledProbeDevice *createProbeDevice(const QDomNode &item_node);
	static GeneralSplit *createGeneralBanner(Page *gen_split_page, const QString &descr);
	void loadItems(const QDomNode &config_node);
};


/*!
	\ingroup AirConditioning
	\brief Contains all the scenarios (SplitScenario) configured for the related
	split and an off button if configured.
*/
class SplitPage : public BannerPage
{
Q_OBJECT
public:
	SplitPage(const QDomNode &config_node, AirConditioningDevice *d);

private:
	void loadScenarios(const QDomNode &config_node);
	AirConditioningDevice *dev;

private slots:
	void setDeviceOff();
};


/*!
	\ingroup AirConditioning
	\brief Contains all the scenarios (AdvancedSplitScenario) configured for the
	related advanced split and an off button if configured.

	The page contains also a CustomScenario banner to view and modify the settings
	of the split through the SplitSettings page.
*/
class AdvancedSplitPage : public BannerPage
{
Q_OBJECT
public:
	AdvancedSplitPage(const QDomNode &config_node, AdvancedAirConditioningDevice *d);
	// Yuck! The only purpose of such ugliness is to support saving the new split settings into conf file
	// DELETE it when porting to the new conf file
	void setSerialNumber(int ser);
	virtual void showPage();

private:
	void loadScenarios(const QDomNode &config_node, AdvancedAirConditioningDevice *d);
	AdvancedAirConditioningDevice *dev;
	Page *single_page, *error_page;

private slots:
	void setDeviceOff();
	void valueReceived(const DeviceValues &values_list);
};


/*!
	\ingroup AirConditioning
	\brief Allows the user to set a custom AdvancedAirConditioningDevice::Status.
*/
#ifdef LAYOUT_TS_3_5
class SplitSettings : public BannerPage
#else
class SplitSettings : public Page
#endif
{
Q_OBJECT
public:
	SplitSettings(const QDomNode &values_node, const QDomNode &config_node, QString descr);
	AirConditionerStatus getCurrentStatus();

protected:
	virtual void showEvent(QShowEvent *);

private slots:
	void resetChanges();
	void acceptChanges();

private:
	void readBannerValues();
	void readModeConfig(const QDomNode &mode_node, int init_mode);
	void readTempConfig(const QDomNode &temp_node, int init_temp);
	void readSwingConfig(const QDomNode &swing_node);
	void readSpeedConfig(const QDomNode &speed_node);
	void sendUpdatedValues();
	SplitSwing *swing;
	SplitTemperature *temperature;
	SplitMode *mode;
	SplitSpeed *speed;
	int current_mode, current_temp, current_fan_speed, current_swing;

signals:
	void splitSettingsChanged(const AirConditionerStatus &);
};


/*!
	\ingroup AirConditioning
	\brief Contains all the scenarios for the general that controls basic splits.
*/
class GeneralSplitPage : public BannerPage
{
Q_OBJECT
public:
	GeneralSplitPage(const QDomNode &config_node);

private:
	void loadScenarios(const QDomNode &config_node);
};


/*!
	\ingroup AirConditioning
	\brief Contains all the scenarios for the general that controls advanced splits.
*/
class AdvancedGeneralSplitPage : public BannerPage
{
Q_OBJECT
public:
	AdvancedGeneralSplitPage(const QDomNode &config_node);

private:
	void loadScenarios(const QDomNode &config_node);
};


/*!
	\ingroup AirConditioning
	\brief A popup page that notifies a failure in a command sent to a split.
*/
class SplitErrorPage : public Page
{
Q_OBJECT
public:
	SplitErrorPage(const QString &image);

	virtual void showPage();

private slots:
	void handleClose();

private:
	QTimer timeout;
};

#endif // AIR_CONDITIONING_H
