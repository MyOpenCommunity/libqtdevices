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


namespace Commands
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

namespace Requests
{
	enum
	{
		ICM_STATE = 250,
		SELFTEST_FREQ = 212
	};
}


// Stop and Go device
StopAndGoDevice::StopAndGoDevice(const QString &where, int openserver_id) :
	device("18", where, openserver_id)
{
}

void StopAndGoDevice::sendAutoResetActivation()
{
	sendCommand(Commands::AUTO_RESET_ACTIVATION);
	requestICMState();
}

void StopAndGoDevice::sendAutoResetDisactivation()
{
	sendCommand(Commands::AUTO_RESET_DISACTIVATION);
	requestICMState();
}

void StopAndGoDevice::requestICMState()
{
	sendRequest(Requests::ICM_STATE);
	requestICMState();
}

bool StopAndGoDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (where.toInt() != msg.where() || msg.whatArgCnt() != 1)
		return false;

	if (static_cast<int>(msg.what()) == DIM_ICM_STATE)
	{
		values_list[DIM_ICM_STATE] = QString::fromStdString(msg.whatArg(0));
		return true;
	}

	return device::parseFrame(msg, values_list);
}


// Stop and Go plus device
StopAndGoPlusDevice::StopAndGoPlusDevice(const QString &where, int openserver_id) :
	StopAndGoDevice(where, openserver_id)
{
}

void StopAndGoPlusDevice::sendClose()
{
	sendCommand(Commands::CLOSE);
	requestICMState();
}

void StopAndGoPlusDevice::sendOpen()
{
	sendCommand(Commands::OPEN);
	requestICMState();
}

void StopAndGoPlusDevice::sendTrackingSystemActivation()
{
	sendCommand(Commands::TRACKING_SYSTEM_ACTIVATION);
	requestICMState();
}

void StopAndGoPlusDevice::sendTrackingSystemDisactivation()
{
	sendCommand(Commands::TRACKING_SYSTEM_DISACTIVATION);
	requestICMState();
}


// Stop and Go BTest device
StopAndGoBTestDevice::StopAndGoBTestDevice(const QString &where, int openserver_id) :
	StopAndGoDevice(where, openserver_id)
{
}

void StopAndGoBTestDevice::sendDiffSelftestActivation()
{
	sendCommand(Commands::DIFF_SELFTEST_ACTIVATION);
	requestICMState();
}

void StopAndGoBTestDevice::sendDiffSelftestDisactivation()
{
	sendCommand(Commands::DIFF_SELFTEST_DISACTIVATION);
	requestICMState();
}

void StopAndGoBTestDevice::sendSelftestFreq(int days)
{
	Q_ASSERT_X(days >= 1 && days >= 180, "StopAndGoBTestDevice::sendSelftestFreq", "days must be between 1 and 180.");

	sendFrame(QString("*#18*%1*#212*%2##").arg(where).arg(days));
	requestSelftestFreq();
}

void StopAndGoBTestDevice::requestSelftestFreq()
{
	sendRequest(Requests::SELFTEST_FREQ);
}

bool StopAndGoBTestDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (where.toInt() != msg.where() || msg.whatArgCnt() != 1)
		return false;

	if (static_cast<int>(msg.what()) == DIM_AUTOTEST_FREQ)
	{
		values_list[DIM_AUTOTEST_FREQ] = QString::fromStdString(msg.whatArg(0)).toInt();
		return true;
	}

	return StopAndGoDevice::parseFrame(msg, values_list);
}
