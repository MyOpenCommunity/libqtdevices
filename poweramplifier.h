#ifndef POWER_AMPLIFIER_H
#define POWER_AMPLIFIER_H

#include "bannregolaz.h"
#include "bannonoff.h"
#include "bannonoff2scr.h"
#include "bannonoffcombo.h"

#include <qvaluevector.h>
#include <qdom.h>

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
	void setBGColor(QColor c);
	void setFGColor(QColor c);

private slots:
	void showSettings();
	void toggleStatus();
	void turnUp();
	void turnDown();
private:
	QString off_icon, on_icon;
	bool status;
	sottoMenu *settings_page;
};


class PowerAmplifierPreset : public bannOnOff
{
Q_OBJECT
public:
	PowerAmplifierPreset(QWidget *parent=0, const char *name=NULL);

protected:
	void showEvent(QShowEvent *event);

private slots:
	void nextPreset();
	void prevPreset();

private:
	unsigned preset, num_preset;
	QValueVector <QString> preset_desc;
	void fillPresetDesc();
	QDomNode getPowerAmplifierNode();
};


class PowerAmplifierTreble : public bannOnOff2scr
{
Q_OBJECT
public:
	PowerAmplifierTreble(QWidget *parent=0, const char *name=NULL, QColor SecondForeground=QColor(0,0,0));
	void setFGColor(QColor);

private slots:
	void up();
	void down();

private:
	int level;
	QColor secondary_fg;
	void showLevel();
};


class PowerAmplifierBass : public bannOnOff2scr
{
Q_OBJECT
public:
	PowerAmplifierBass(QWidget *parent=0, const char *name=NULL, QColor SecondForeground=QColor(0,0,0));
	void setFGColor(QColor);

private slots:
	void up();
	void down();

private:
	int level;
	QColor secondary_fg;
	void showLevel();
};


class PowerAmplifierBalance : public BannOnOffCombo
{
Q_OBJECT
public:
	PowerAmplifierBalance(QWidget *parent=0, const char *name=NULL, QColor SecondForeground=QColor(0,0,0));
	void setFGColor(QColor c);

private slots:
	void dx();
	void sx();

private:
	int balance;
	QColor secondary_fg;
	void showBalance();
};


class PowerAmplifierLoud : public bannOnOff
{
Q_OBJECT
public:
	PowerAmplifierLoud(QWidget *parent=0, const char *name=NULL);

private slots:
	void loudOn();
	void loudOff();
};

#endif
