#ifndef BANN_AIRCONDITIONING_H
#define BANN_AIRCONDITIONING_H

#include "bann2_buttons.h" // BannOnOffNew, bann2But
#include "bann1_button.h" // BannStates

#include <QList>
#include <QPair>
#include <QHash>
#include <QString>

class AirConditioningDevice;


/**
 * The banner for a split
 */
class SingleSplit : public BannOnOffNew
{
Q_OBJECT
public:
	SingleSplit(QString descr, AirConditioningDevice *d);

private:
	AirConditioningDevice *dev;
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
class AdvancedSplitScenario : public bann2But
{
Q_OBJECT
public:
	AdvancedSplitScenario(QWidget *parent, QString descr);
};


/**
 * The banner that represent the temperature of the split
 */
class SplitTemperature : public bann2But
{
Q_OBJECT
public:
	SplitTemperature(QWidget *parent); // TODO: altri parametri, tipo temperature/format, range, step, ecc..
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
class SplitSwing : public BannLeft
{
Q_OBJECT
public:
	SplitSwing(QString descr);

private slots:
	void toggleSwing();
private:
	bool status;
};


/**
 * The banner for a scenario of a single split (configured as basic)
 */
class SplitScenario : public BannLeft
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
class GeneralSplitScenario : public BannLeft
{
Q_OBJECT
public:
	GeneralSplitScenario(QString descr);
	void appendDevice(QString cmd, AirConditioningDevice *d);

private:
	QList<QPair<QString, AirConditioningDevice*> > devices_list;

private slots:
	void sendScenarioCommand();
};


#endif // BANN_AIRCONDITIONING_H
