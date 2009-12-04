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
	SingleSplit(QString descr, QString off_cmd, AirConditioningDevice *d);

private:
	AirConditioningDevice *dev;
	QString off;

public slots:
	void sendOff();
};


class GeneralSplit : public BannOnOffNew
{
Q_OBJECT
public:
	GeneralSplit(QString descr);
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


#endif // BANN_AIRCONDITIONING_H
