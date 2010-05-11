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
#include "stopandgo_device.h"

#include "openmsg.h"

#define STATUS_BITS 13

namespace StopAndGoCommands
{
	enum
	{
		CLOSE = 21,
		OPEN = 22,
		DIFF_SELFTEST_ACTIVATION = 23,
		DIFF_SELFTEST_DISACTIVATION = 24,
		AUTO_RESET_ACTIVATION = 26,
		AUTO_RESET_DISACTIVATION = 27,
		TRACKING_SYSTEM_ACTIVATION = 28,
		TRACKING_SYSTEM_DISACTIVATION = 29,
		SELFTEST_FREQ = 212
	};
}

namespace StopAndGoRequests
{
	enum
	{
		ICM_STATE = 250,
		SELFTEST_FREQ = 212
	};
}

int masc2int(const QString &masc)
{
	int length = masc.length();

	Q_ASSERT_X(length != (STATUS_BITS + 1), "masc2int", "masc must be STATUS_BITS + 1 long");

	int result = 0;

	for (int i = 0; i < length; i++)
	{
		int value = masc[i].digitValue();
		result = (value << (STATUS_BITS - i - 1)) | result;
	}

	return result;
}

bool getStatusValue(int status, int fields)
{
	int result = status & fields;

	return result == fields;
}

// Stop and Go device
StopAndGoDevice::StopAndGoDevice(const QString &where, int openserver_id) :
	device("18", where, openserver_id)
{
}

void StopAndGoDevice::init()
{
	requestICMState();
}

void StopAndGoDevice::sendAutoResetActivation()
{
	sendCommand(StopAndGoCommands::AUTO_RESET_ACTIVATION);
	requestICMState();
}

void StopAndGoDevice::sendAutoResetDisactivation()
{
	sendCommand(StopAndGoCommands::AUTO_RESET_DISACTIVATION);
	requestICMState();
}

void StopAndGoDevice::requestICMState()
{
	sendRequest(StopAndGoRequests::ICM_STATE);
}

bool StopAndGoDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (where.toInt() != msg.where() || msg.whatArgCnt() != 1)
		return false;

	if (static_cast<int>(msg.what()) == StopAndGoRequests::ICM_STATE)
	{
		const QString status = QString::fromStdString(msg.whatArg(0));
		int i = 0;

		foreach (const QChar &c, status)
			values_list[i++] = static_cast<bool>(c.digitValue());

		return true;
	}

	return device::parseFrame(msg, values_list);
}


// Stop and Go plus device
StopAndGoPlusDevice::StopAndGoPlusDevice(const QString &where, int openserver_id) :
	StopAndGoDevice(where, openserver_id)
{
}

void StopAndGoPlusDevice::sendTrackingSystemActivation()
{
	sendCommand(StopAndGoCommands::TRACKING_SYSTEM_ACTIVATION);
	requestICMState();
}

void StopAndGoPlusDevice::sendTrackingSystemDisactivation()
{
	sendCommand(StopAndGoCommands::TRACKING_SYSTEM_DISACTIVATION);
	requestICMState();
}

void StopAndGoPlusDevice::sendClose()
{
	sendCommand(StopAndGoCommands::CLOSE);
	requestICMState();
}

void StopAndGoPlusDevice::sendOpen()
{
	sendCommand(StopAndGoCommands::OPEN);
	requestICMState();
}


// Stop and Go BTest device
StopAndGoBTestDevice::StopAndGoBTestDevice(const QString &where, int openserver_id) :
	StopAndGoDevice(where, openserver_id)
{
}

void StopAndGoBTestDevice::sendDiffSelftestActivation()
{
	sendCommand(StopAndGoCommands::DIFF_SELFTEST_ACTIVATION);
	requestICMState();
}

void StopAndGoBTestDevice::sendDiffSelftestDisactivation()
{
	sendCommand(StopAndGoCommands::DIFF_SELFTEST_DISACTIVATION);
	requestICMState();
}

void StopAndGoBTestDevice::sendSelftestFreq(int days)
{
	Q_ASSERT_X(days >= 1 && days <= 180, "StopAndGoBTestDevice::sendSelftestFreq", "days must be between 1 and 180.");

	sendFrame(QString("*#18*%1*#212*%2##").arg(where).arg(days));
	requestSelftestFreq();
}

void StopAndGoBTestDevice::requestSelftestFreq()
{
	sendRequest(StopAndGoRequests::SELFTEST_FREQ);
}

bool StopAndGoBTestDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (where.toInt() != msg.where() || msg.whatArgCnt() != 1)
		return false;

	if (static_cast<int>(msg.what()) == StopAndGoRequests::SELFTEST_FREQ)
	{
		values_list[DIM_AUTOTEST_FREQ] = QString::fromStdString(msg.whatArg(0)).toInt();
		return true;
	}

	return StopAndGoDevice::parseFrame(msg, values_list);
}
