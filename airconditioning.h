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

typedef AdvancedAirConditioningDevice::AirConditionerStatus AirConditionerStatus;


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


// The main page of the Air Conditioning
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

/**
 * The page for a split (basic) which contains all the scenarios configured for
 * that split.
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


/**
 * The page for a split (advanced) which contains all the scenarios configured
 * for that split.
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


/**
 * The page of the settings (also known as custom page) for an advanced scenario.
 */
#ifdef LAYOUT_BTOUCH
class SplitSettings : public BannerPage
#else
class SplitSettings : public Page
#endif
{
Q_OBJECT
public:
	SplitSettings(const QDomNode &values_node, const QDomNode &config_node);
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


/**
 * The page for a general split which contains all the scenarios for that.
 */
class GeneralSplitPage : public BannerPage
{
Q_OBJECT
public:
	GeneralSplitPage(const QDomNode &config_node);

private:
	void loadScenarios(const QDomNode &config_node);
};


class AdvancedGeneralSplitPage : public BannerPage
{
Q_OBJECT
public:
	AdvancedGeneralSplitPage(const QDomNode &config_node);

private:
	void loadScenarios(const QDomNode &config_node);
};


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
