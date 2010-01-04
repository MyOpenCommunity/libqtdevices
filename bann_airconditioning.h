#ifndef BANN_AIRCONDITIONING_H
#define BANN_AIRCONDITIONING_H

#include "bann2_buttons.h" // BannOnOffNew, Bann2Buttons
#include "bann1_button.h" // BannStates
#include "airconditioning_device.h"

#include <QList>
#include <QPair>
#include <QHash>
#include <QButtonGroup>

typedef AdvancedAirConditioningDevice::AirConditionerStatus AirConditionerStatus;
class NonControlledProbeDevice;


/**
 * The banner for a split
 */
class SingleSplit : public BannOnOffNew
{
Q_OBJECT
public:
	SingleSplit(QString descr, AirConditioningInterface *d, NonControlledProbeDevice *d_probe=0);
	virtual void inizializza(bool forza=false);

private:
	AirConditioningInterface *dev;
	NonControlledProbeDevice *dev_probe;

private slots:
	void setDeviceOff();
	void status_changed(const StatusList &status_list);
};


/**
 * The banner for managing the whole list of splits.
 */
class GeneralSplit : public BannOnOffNew
{
Q_OBJECT
public:
	GeneralSplit(QString descr);

signals:
	void sendGeneralOff();
};


/**
 * The banner for a scenario of a single split (configured as advanced)
 */
class AdvancedSplitScenario : public Bann2Buttons
{
Q_OBJECT
public:
	// TODO: add device to constructor!
	AdvancedSplitScenario(QWidget *parent, QString descr, AdvancedAirConditioningDevice *d);

public slots:
	void splitValuesChanged(const AirConditionerStatus &st);

private:
	AirConditionerStatus status;
	AdvancedAirConditioningDevice *dev;

private slots:
	void onButtonClicked();
};


/**
 * The banner that represent the temperature of the split
 */
class SplitTemperature : public Bann2Buttons
{
Q_OBJECT
public:
	SplitTemperature(int init_temp, int level_max, int level_min, int step);
	// TODO: what is the scale for this temp? The easiest thing is to assume
	// the same scale that was given when the object was created.
	void setTemperature(int new_temp);
	int temperature();

private slots:
	void increaseTemp();
	void decreaseTemp();

private:
	void updateText();
	int current_temp, max_temp, min_temp, temp_step;
};


/**
 * The banner that represent the mode of the split
 */
class SplitMode : public BannStates
{
Q_OBJECT
public:
	SplitMode(QList<int> modes, int current_mode);
private:
	QHash<int, QString> modes_descr;
};


/**
 * The banner that represent the speed of the split
 */
class SplitSpeed : public BannStates
{
Q_OBJECT
public:
	SplitSpeed(QList<int> speeds, int current_speed);
private:
	QHash<int, QString> speeds_descr;
};


/**
 * The banner that represent the swing of the split
 */
class SplitSwing : public Bann2Buttons
{
Q_OBJECT
public:
	SplitSwing(QString descr, bool init_swing);
	void setSwingOn(bool swing_on);
	bool swing();

private:
	QButtonGroup buttons;
};


/**
 * The banner for a scenario of a single split (configured as basic)
 */
class SplitScenario : public Bann2Buttons
{
Q_OBJECT
public:
	SplitScenario(QString descr, QString cmd, AirConditioningDevice *d);

private:
	AirConditioningDevice *dev;
	QString command;

private slots:
	void sendScenarioCommand();
};


/**
 * The banner for a scenario of a general split
 */
class GeneralSplitScenario : public Bann2Buttons
{
Q_OBJECT
public:
	GeneralSplitScenario(QString descr);
	void appendDevice(QString cmd, AirConditioningInterface *d);

private:
	QList<QPair<QString, AirConditioningInterface*> > devices_list;

private slots:
	void sendScenarioCommand();
};

#endif // BANN_AIRCONDITIONING_H
