#ifndef BANN_AIRCONDITIONING_H
#define BANN_AIRCONDITIONING_H

#include "bann2_buttons.h" // BannOnOffNew, bann2But
#include "bann1_button.h" // BannStates

#include <QHash>
#include <QString>
#include <QtContainerFwd> // QList

class AirConditioningDevice;


class SingleSplit : public BannOnOffNew
{
Q_OBJECT
public:
	SingleSplit(QString descr, AirConditioningDevice *d);

private:
	AirConditioningDevice *dev;
};


class GeneralSplit : public BannOnOffNew
{
Q_OBJECT
public:
	GeneralSplit(QString descr);

signals:
	void sendGeneralOff();
};


class AdvancedSplit : public bann2But
{
Q_OBJECT
public:
	AdvancedSplit(QWidget *parent, QString descr);
};


class SplitTemperature : public bann2But
{
Q_OBJECT
public:
	SplitTemperature(QWidget *parent); // TODO: altri parametri, tipo temperature/format, range, step, ecc..
};


class SplitMode : public BannStates
{
Q_OBJECT
public:
	SplitMode(QList<int> modes, int current_mode);
private:
	QHash<int, QString> modes_descr;
};


class SplitSpeed : public BannStates
{
Q_OBJECT
public:
	SplitSpeed(QList<int> speeds, int current_speed);
private:
	QHash<int, QString> speeds_descr;
};


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

#endif // BANN_AIRCONDITIONING_H
