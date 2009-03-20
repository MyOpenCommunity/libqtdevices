/**
 * \file
 * <!--
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This file contain the banners for the power amplifier.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 */

#ifndef POWER_AMPLIFIER_H
#define POWER_AMPLIFIER_H

#include "bannregolaz.h"
#include "bann2_buttons.h" // bannOnOff, bannOnOff2scr
#include "bannonoffcombo.h"
#include "poweramplifier_device.h"
#include "sottomenu.h"

#include <QVector>
#include <QString>
#include <QMap>

class QDomNode;
class QWidget;


/**
 * \class BannPowerAmplifier
 *
 * The main banner of the power amplifier. It instantiate the device and manage
 * the link with the page of settings.
 */
class BannPowerAmplifier : public bannRegolaz
{
Q_OBJECT
public:
	BannPowerAmplifier(QWidget *parent, const QDomNode& config_node, QString indirizzo, QString onIcon,
		QString offIcon, QString onAmpl, QString offAmpl, QString settingIcon);
	virtual void inizializza(bool forza=false);

private slots:
	void toggleStatus();
	void volumeUp();
	void volumeDown();
	void status_changed(const StatusList &status_list);

private:
	QString off_icon, on_icon;
	bool status;
	PowerAmplifierDevice *dev;
};

/**
 * \class PowerAmplifier
 *
 * The page of the settings of the power amplifier. It simply delegate the
 * functionality to the correct banner.
 */
class PowerAmplifier : public sottoMenu
{
Q_OBJECT
public:
	PowerAmplifier(PowerAmplifierDevice *dev, const QDomNode &config_node);

private:
	void loadBanners(PowerAmplifierDevice *dev, const QDomNode &config_node);
};


class PowerAmplifierPreset : public bannOnOff
{
Q_OBJECT
public:
	PowerAmplifierPreset(PowerAmplifierDevice *d, QWidget *parent, const QMap<int, QString>& preset_list);
	virtual void inizializza(bool forza=false);

private slots:
	void next();
	void prev();
	void status_changed(const StatusList &status_list);

private:
	int num_preset;
	QVector<QString> preset_desc;
	PowerAmplifierDevice *dev;
	void fillPresetDesc(const QMap<int, QString>& preset_list);
};


class PowerAmplifierTreble : public bannOnOff2scr
{
Q_OBJECT
public:
	PowerAmplifierTreble(PowerAmplifierDevice *d, QWidget *parent=0);
	virtual void inizializza(bool forza=false);

private slots:
	void up();
	void down();
	void status_changed(const StatusList &status_list);

private:
	PowerAmplifierDevice *dev;
	void showLevel(int level);
};


class PowerAmplifierBass : public bannOnOff2scr
{
Q_OBJECT
public:
	PowerAmplifierBass(PowerAmplifierDevice *d, QWidget *parent=0);
	virtual void inizializza(bool forza=false);

private slots:
	void up();
	void down();
	void status_changed(const StatusList &status_list);

private:
	PowerAmplifierDevice *dev;
	void showLevel(int level);
};


class PowerAmplifierBalance : public BannOnOffCombo
{
Q_OBJECT
public:
	PowerAmplifierBalance(PowerAmplifierDevice *d, QWidget *parent=0);
	virtual void inizializza(bool forza=false);

private slots:
	void dx();
	void sx();
	void status_changed(const StatusList &status_list);

private:
	PowerAmplifierDevice *dev;
	void showBalance(int balance);
};


class PowerAmplifierLoud : public bannOnOff
{
Q_OBJECT
public:
	PowerAmplifierLoud(PowerAmplifierDevice *d, QWidget *parent=0);
	virtual void inizializza(bool forza=false);

private slots:
	void on();
	void off();
	void status_changed(const StatusList &status_list);

private:
	PowerAmplifierDevice *dev;
};

#endif
