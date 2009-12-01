#ifndef BANN_AIRCONDITIONING_H
#define BANN_AIRCONDITIONING_H

#include "bann2_buttons.h"

class AirConditioningDevice;
class QString;


class SingleSplit : public BannOnOffNew
{
Q_OBJECT
public:
	SingleSplit(QString descr, QString off_cmd, AirConditioningDevice *d);

private:
	AirConditioningDevice *dev;
	QString off;

private slots:
	void sendOff();
};


class GeneralSplit : public BannOnOffNew
{
Q_OBJECT
public:
	GeneralSplit(QWidget *parent, QString descr);
};


class AdvancedSplit : public bann2But
{
Q_OBJECT
public:
	AdvancedSplit(QWidget *parent, QString descr);
};


class TemperatureSplit : public bann2But
{
Q_OBJECT
public:
	TemperatureSplit(QWidget *parent); // TODO: altri parametri, tipo temperature/format, range, step, ecc..
};

#endif // BANN_AIRCONDITIONING_H
