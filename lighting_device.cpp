#include "lighting_device.h"
#include "openmsg.h"

#include <QDebug>

enum
{
	LIGHT_ON = 1,
	LIGHT_OFF = 0,
};

LightingDevice::LightingDevice(QString where, PullMode pull) :
	device(QString("1"), where)
{
	mode = pull;
}

void LightingDevice::turnOn()
{
	sendCommand(QString::number(LIGHT_ON));
}

void LightingDevice::turnOn(int speed)
{
	// TODO: is this method ok also for Dimmers?
	sendCommand(QString::number(LIGHT_ON) + "#" + QString::number(speed));
}

void LightingDevice::turnOff()
{
	sendCommand(QString::number(LIGHT_OFF));
}

void LightingDevice::turnOff(int speed)
{
	// TODO: is this method ok also for Dimmers?
	sendCommand(QString::number(LIGHT_OFF) + "#" + QString::number(speed));
}

void LightingDevice::requestStatus()
{
	sendRequest(QString());
}

QPair<QString, QString> splitWhere(const QString &w)
{
	return qMakePair(w.left(w.indexOf("#")), w.mid(w.indexOf("#")));
}

QString getEnvironment(const QString &w)
{
	if (w.length() == 2)
		return w.mid(0, 1);
	else if (w.length() == 4)
		return w.mid(0, 2);

	return QString();
}

bool checkAddressIsForMe(const QString &msg_where, const QString &dev_where)
{
	// frame where (input)
	QPair<QString, QString> in = splitWhere(msg_where);
	// device where (our)
	QPair<QString, QString> our = splitWhere(dev_where);

	if (in.second != our.second && in.second != "#3")
		return false;

	// here we don't need to care about extension anymore
	// general address
	if (in.first == "0")
		return true;
	// environment address
	if (getEnvironment(our.first) == getEnvironment(in.first))
		return true;

	return false;
}

bool LightingDevice::isFrameInteresting(OpenMsg &msg)
{
	bool is_our = (msg.Extract_dove() == where);
	if (!is_our && mode != PULL)
		is_our = checkAddressIsForMe(msg.Extract_dove(), where);
	return is_our;
}

void LightingDevice::manageFrame(OpenMsg &msg)
{
	if (!isFrameInteresting(msg))
		return;

	StatusList sl;
	QVariant v;

	int what = msg.what();
	switch (what)
	{
	case DIM_DEVICE_ON:
		v.setValue(true);
		sl[what] = v;
		break;
	case DIM_DEVICE_OFF:
		v.setValue(true);
		sl[what] = v;
		break;
	// TODO: add "Temporizzazioni variabili" status change
	default:
		qDebug() << "LightingDevice::manageFrame(): Unknown what";
	}

	emit status_changed(sl);
}
