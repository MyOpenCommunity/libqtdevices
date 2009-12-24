#include "airconditioning_device.h"
#include "generic_functions.h" // createWriteRequestOpen

#include <openmsg.h>


#define ADVANCED_SPLIT_DIM 22


AirConditioningDevice::AirConditioningDevice(QString where) : device("0", where)
{
}

void AirConditioningDevice::sendCommand(const QString &cmd) const
{
	sendFrame(QString("*%1*%2*%3##").arg(who).arg(cmd).arg(where));
}

void AirConditioningDevice::sendOff() const
{
	Q_ASSERT_X(!off.isNull(), "AirConditioningDevice::sendOff", "Off command not set!");
	sendCommand(off);
}

void AirConditioningDevice::setOffCommand(QString off_cmd)
{
	off = off_cmd;
}

void AirConditioningDevice::turnOff() const
{
	sendOff();
}

void AirConditioningDevice::activateScenario(const QString &what) const
{
	sendCommand(what);
}



AdvancedAirConditioningDevice::AdvancedAirConditioningDevice(QString where) : device("4", where)
{
}

void AdvancedAirConditioningDevice::requestStatus() const
{
	sendRequest(QString::number(ADVANCED_SPLIT_DIM));
}

QString AdvancedAirConditioningDevice::statusToString(const AirConditionerStatus &st) const
{
	QString what;

	switch (st.mode)
	{
	case MODE_OFF:
		what = QString("%1*%2***").arg(ADVANCED_SPLIT_DIM).arg(st.mode);
		break;

	case MODE_FAN:
	case MODE_DEHUM:
		what = QString("%1*%2**%3*%4").arg(ADVANCED_SPLIT_DIM).arg(st.mode).arg(st.vel).arg(st.swing);
		break;

	default:
		what = QString("%1*%2*%3*%4*%5").arg(ADVANCED_SPLIT_DIM).arg(st.mode).arg(st.temp).arg(st.vel).arg(st.swing);
		break;
	}

	return what;
}

void AdvancedAirConditioningDevice::setStatus(Mode mode, int temp, Velocity vel, Swing swing) const
{
	AirConditionerStatus st(mode, temp, vel, swing);
	setStatus(st);
}

// overload for the above function, useful to pass all the parameters around packed together
void AdvancedAirConditioningDevice::setStatus(AirConditionerStatus st) const
{
	QString what = statusToString(st);
	sendFrame(createWriteRequestOpen(who, what, where));
}

void AdvancedAirConditioningDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));

	if (who.toInt() != msg.who() || msg.where() != where.toInt())
		return;
}

void AdvancedAirConditioningDevice::turnOff() const
{
	setStatus(MODE_OFF, 0, VEL_AUTO, SWING_OFF);
}

void AdvancedAirConditioningDevice::activateScenario(const QString &what) const
{
	sendFrame(createWriteRequestOpen(who, what, where));
}
