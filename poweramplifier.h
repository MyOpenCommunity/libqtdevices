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


#ifndef POWER_AMPLIFIER_H
#define POWER_AMPLIFIER_H

#include "bann4_buttons.h" // Bannlevel
#include "bann2_buttons.h" // BannOnOffState, BannOnOff2Labels
#include "bannonoffcombo.h"
#include "bannerpage.h"

#include <QVector>
#include <QString>
#include <QMap>

class PowerAmplifierPage;
class PowerAmplifierDevice;
class QDomNode;
class QWidget;

/**
 * Handle graphical changes for amplifiers, On/Off states and volume changes.
 */
class AdjustVolume : public BannLevel
{
Q_OBJECT
protected:
	enum States
	{
		ON,
		OFF,
	};
	AdjustVolume(QWidget *parent=0);
	void initBanner(const QString &left, const QString &_center_on, const QString &_center_off,
		const QString &right, States init_state, int init_level, const QString &banner_text);
	void setLevel(int level);
	void setState(States new_state);

private:
	void updateIcons();
	void setOnIcons();
	void setOffIcons();
	int current_level;
	States current_state;
	QString center_on, center_off;
};


/**
 * \class BannPowerAmplifier
 *
 * The main banner of the power amplifier. It instantiate the device and manage
 * the link with the page of settings.
 */
class BannPowerAmplifier : public AdjustVolume
{
Q_OBJECT
public:
	BannPowerAmplifier(const QString &descr, PowerAmplifierDevice *d, PowerAmplifierPage* page);

private slots:
	void toggleStatus();
	void volumeUp();
	void volumeDown();
	void valueReceived(const DeviceValues &values_list);

private:
	QString off_icon, on_icon;
	bool status;
	PowerAmplifierDevice *dev;
};


/**
 * \class PowerAmplifierPage
 *
 * The page of the settings of the power amplifier. It simply delegate the
 * functionality to the correct banner.
 */
class PowerAmplifierPage : public BannerPage
{
Q_OBJECT
public:
	PowerAmplifierPage(PowerAmplifierDevice *dev, const QDomNode &config_node);

private:
	void loadBanners(PowerAmplifierDevice *dev, const QDomNode &config_node);
};


class PowerAmplifierPreset : public Bann2Buttons
{
Q_OBJECT
public:
	PowerAmplifierPreset(PowerAmplifierDevice *d, QWidget *parent, const QMap<int, QString>& preset_list);

private slots:
	void next();
	void prev();
	void valueReceived(const DeviceValues &values_list);

private:
	int num_preset;
	QVector<QString> preset_desc;
	PowerAmplifierDevice *dev;
	void fillPresetDesc(const QMap<int, QString>& preset_list);
};


class PowerAmplifierTreble : public BannOnOff2Labels
{
Q_OBJECT
public:
	PowerAmplifierTreble(PowerAmplifierDevice *d, const QString &banner_text, QWidget *parent=0);

private slots:
	void up();
	void down();
	void valueReceived(const DeviceValues &values_list);

private:
	PowerAmplifierDevice *dev;
	void showLevel(int level);
};


class PowerAmplifierBass : public BannOnOff2Labels
{
Q_OBJECT
public:
	PowerAmplifierBass(PowerAmplifierDevice *d, const QString &banner_text, QWidget *parent=0);

private slots:
	void up();
	void down();
	void valueReceived(const DeviceValues &values_list);

private:
	PowerAmplifierDevice *dev;
	void showLevel(int level);
};


class PowerAmplifierBalance : public BannOnOffCombo
{
Q_OBJECT
public:
	PowerAmplifierBalance(PowerAmplifierDevice *d, const QString &descr);

private slots:
	void dx();
	void sx();
	void valueReceived(const DeviceValues &values_list);

private:
	PowerAmplifierDevice *dev;
	void showBalance(int balance);
};


class PowerAmplifierLoud : public BannOnOffState
{
Q_OBJECT
public:
	PowerAmplifierLoud(PowerAmplifierDevice *d, const QString &banner_text, QWidget *parent=0);

private slots:
	void on();
	void off();
	void valueReceived(const DeviceValues &values_list);

private:
	PowerAmplifierDevice *dev;
};

#endif
