#ifndef POWER_AMPLIFIER_H
#define POWER_AMPLIFIER_H

#include "bannregolaz.h"
#include "bann2_buttons.h" // bannOnOff, bannOnOff2scr
#include "bannonoffcombo.h"
#include "poweramplifier_device.h"
#include "sottomenu.h"

#include <QDomNode>
#include <QWidget>
#include <QVector>
#include <QMap>

class QString;


class BannPowerAmplifier : public bannRegolaz
{
Q_OBJECT
public:
	BannPowerAmplifier(QWidget *parent, const QDomNode& config_node, QString indirizzo, QString onIcon, QString offIcon, QString onAmpl, QString offAmpl, QString settingIcon);
	virtual void inizializza(bool forza=false);

private slots:
	void toggleStatus();
	void volumeUp();
	void volumeDown();
	void status_changed(const StatusList &status_list);

private:
	QString off_icon, on_icon;
	bool status;
	sottoMenu *settings_page;
	PowerAmplifierDevice *dev;
};


class PowerAmplifier : public sottoMenu
{
Q_OBJECT
public:
	PowerAmplifier(const QDomNode &config_node);

private slots:
	void status_changed(const StatusList &status_list);

private:
	void loadBanners(const QDomNode &config_node);
};


class PowerAmplifierPreset : public bannOnOff
{
Q_OBJECT
public:
	PowerAmplifierPreset(QWidget *parent, const QMap<int, QString>& preset_list);

private slots:
	void nextPreset();
	void prevPreset();

private:
	int curr_preset, num_preset;
	QVector<QString> preset_desc;
	void fillPresetDesc(const QMap<int, QString>& preset_list);
};


class PowerAmplifierTreble : public bannOnOff2scr
{
Q_OBJECT
public:
	PowerAmplifierTreble(QWidget *parent=0);

private slots:
	void up();
	void down();

private:
	int level;
	void showLevel();
};


class PowerAmplifierBass : public bannOnOff2scr
{
Q_OBJECT
public:
	PowerAmplifierBass(QWidget *parent=0);

private slots:
	void up();
	void down();

private:
	int level;
	void showLevel();
};


class PowerAmplifierBalance : public BannOnOffCombo
{
Q_OBJECT
public:
	PowerAmplifierBalance(QWidget *parent=0);

private slots:
	void dx();
	void sx();

private:
	int balance;
	void showBalance();
};


class PowerAmplifierLoud : public bannOnOff
{
Q_OBJECT
public:
	PowerAmplifierLoud(QWidget *parent=0);

private slots:
	void loudOn();
	void loudOff();
};

#endif
