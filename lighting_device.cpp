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

void LightingDevice::manageFrame(OpenMsg &msg)
{
	// TODO: this is not true! we must check also "environment" and general commands
	LightAddress addr(msg);
	if (!addr.isReceiver(where))
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

LightAddress::LightAddress(OpenMsg &msg)
{
	is_general = false;
	is_environment = false;

	// environment + light point
	QString elp = msg.whereStr().c_str();
	// FIXME: how do we handle groups???

	// check global address
	if (elp.length() == 1 && elp == "0")
	{
		is_general = true;
	}
	// check environment address
	else if ((elp.length() == 1) || (elp == "00") || (elp == "100"))
	{
		is_environment = true;
		// set the correct environment value
		environment = elp == "100" ? "10" : elp;
	}
	// check point to point address
	else
	{
		splitP2PAddress(elp, environment, light_point);
		Q_ASSERT_X(light_point.toInt() < 16, "LightAddress::LightAddress",
			"Light point address must be less than 16");
	}

	// check for extended frames. These are independent from the checks above
	for (unsigned i = 0; i < msg.whereArgCnt(); ++i)
		extension += QString("#") + msg.whereArg(i).c_str();
}

bool LightAddress::isReceiver(QString where)
{
	QString env, lp;
	splitP2PAddress(where.left(where.indexOf("#")), env, lp);
	QString dev_extension = where.mid(where.indexOf("#"));

	if ((is_general && extension.isEmpty()) || (is_general && extension == dev_extension))
		return true;
	return false;
}

void LightAddress::splitP2PAddress(const QString &elp, QString &env, QString &lp)
{
	if (elp.length() == 2)
	{
		env = elp.mid(0, 1);
		lp = elp.mid(1);
	}
	else if (elp.length() == 4)
	{
		env = elp.mid(0, 2);
		lp = elp.mid(2);
	}
	else
		Q_ASSERT_X(0, "LightAddress::splitP2PAddress",
			"Point to point address length is not 2 or 4");
}
