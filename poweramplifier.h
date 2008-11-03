#ifndef POWER_AMPLIFIER_H
#define POWER_AMPLIFIER_H

#include "bannregolaz.h"
#include "bannonoff.h"
#include "bannonoff2scr.h"
#include "bannonoffcombo.h"
#include "poweramplifier_device.h"

#include <QDomNode>
#include <QWidget>
#include <QVector>

class sottoMenu;


/*****************************************************************
 **Power amplifier
 ****************************************************************/
/*!
 * \class PowerAmplifier
 * \brief This class is made to manage a power audio amplifier.
 *
 */
class PowerAmplifier : public bannRegolaz
{
Q_OBJECT
public:
	PowerAmplifier(QWidget *parent, const char *name, char* indirizzo, char* onIcon, char* offIcon, char *onAmpl, char *offAmpl, char* settingIcon);

private slots:
	void showSettings();
	void toggleStatus();
	void turnUp();
	void turnDown();
	void status_changed(QMap<poweramplifier_device::status_key_t, stat_var> st);

private:
	QString off_icon, on_icon;
	bool status;
	sottoMenu *settings_page;
	poweramplifier_device *dev;
};


class PowerAmplifierPreset : public bannOnOff
{
Q_OBJECT
public:
	PowerAmplifierPreset(QWidget *parent=0);

protected:
	void showEvent(QShowEvent *event);

private slots:
	void nextPreset();
	void prevPreset();

private:
	unsigned preset, num_preset;
	QVector<QString> preset_desc;
	void fillPresetDesc();
	QDomNode getPowerAmplifierNode();
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
