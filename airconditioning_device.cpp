/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "airconditioning_device.h"
#include "generic_functions.h" // createWriteRequestOpen

#include <QStringList>
#include <openmsg.h>


#define ADVANCED_SPLIT_DIM 22


AirConditioningDevice::AirConditioningDevice(QString where) : device("0", where)
{
}

void AirConditioningDevice::sendCommand(const QString &cmd) const
{
	sendFrame(QString("*%1*%2*%3##").arg(who).arg(cmd).arg(where));
}

void AirConditioningDevice::setOffCommand(QString off_cmd)
{
	off = off_cmd;
}

void AirConditioningDevice::turnOff() const
{
	Q_ASSERT_X(!off.isNull(), "AirConditioningDevice::turnOff", "Off command not set!");
	sendCommand(off);
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

	QString speed = st.vel == VEL_INVALID ? QString() : QString::number(st.vel);
	QString swing = st.swing == SWING_INVALID ? QString() : QString::number(st.swing);
	switch (st.mode)
	{
	case MODE_OFF:
		what = QString("%1*%2***").arg(ADVANCED_SPLIT_DIM).arg(st.mode);
		break;

	case MODE_FAN:
	case MODE_DEHUM:
		what = QString("%1*%2**%3*%4").arg(ADVANCED_SPLIT_DIM).arg(st.mode).arg(speed).arg(swing);
		break;

	default:
		what = QString("%1*%2*%3*%4*%5").arg(ADVANCED_SPLIT_DIM).arg(st.mode).arg(st.temp).arg(speed).arg(swing);
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

	// do not perform status check for OFF requests
	if (st.mode != MODE_OFF)
		last_status_set = what;
	else
		last_status_set.clear();

	sendFrame(createWriteRequestOpen(who, what, where));
}

void AdvancedAirConditioningDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));

	if (who.toInt() != msg.who() || msg.whereFull() != where.toStdString())
		return;

	int what = msg.what();
	StatusList status_list;

	// if we sent a command to set the status, check that the device set the
	// status correctly; the values that have not been explicitly set are
	// ignored
	if (what == ADVANCED_SPLIT_DIM && !last_status_set.isEmpty())
	{
		QStringList last_what = last_status_set.split("*");
		for (size_t i = 0; i < msg.whatArgCnt(); ++i)
		{
			if (last_what[i + 1].isEmpty())
				continue;
			QString value = QString::fromStdString(msg.whatArg(i));

			if (last_what[i + 1] != value)
				status_list[DIM_SETSTATUS_ERROR] = true;
		}

		last_status_set.clear();
	}

	if (status_list.count() != 0)
		emit status_changed(status_list);
}

void AdvancedAirConditioningDevice::turnOff() const
{
	setStatus(MODE_OFF, 0, VEL_AUTO, SWING_OFF);
}

void AdvancedAirConditioningDevice::activateScenario(const QString &what) const
{
	sendFrame(createWriteRequestOpen(who, what, where));
}
