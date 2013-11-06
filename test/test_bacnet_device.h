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

#ifndef TESTBACNETDEVICE_H
#define TESTBACNETDEVICE_H

#include "test_device.h"

namespace Bacnet
{

	class AirHandlingDevice;
	class UnderfloorHeatingDevice;
	class VariableAirVolumeDevice;
	class ACSplitWithFanCoilDevice;

	class TestACSplitWithFanCoilDevice : public TestDevice
	{
		Q_OBJECT
	private slots:
		void initTestCase();
		void cleanupTestCase();

		void sendRequestStatus();

		void sendSetStatus();
		void sendResetFilterStatus();
		void sendSetFanStatus();
		void sendSetModeStatus();
		void sendSetActiveStatus();
		void sendSetTemperatureStatus();
		void sendSetAirDirectionStatus();

		void sendTurnOff();
		void sendTurnOn();

		void receiveStatus();
		void receiveFilterStatus();
		void receiveFanStatus();
		void receiveModeStatus();
		void receiveActiveStatus();
		void receiveTemperatureStatus();
		void receiveAirDirectionStatus();
		void receiveGeneralFaultStatus();

	private:
		ACSplitWithFanCoilDevice *dev;
	};


	class TestAirHandlingDevice : public TestDevice
	{
		Q_OBJECT
	private slots:
		void initTestCase();
		void cleanupTestCase();

		void sendRequestStatus();

		void sendSetStatus();
		void sendSetHumidityStatus();
		void sendSetFanStatus();
		void sendSetModeStatus();
		void sendSetActiveStatus();
		void sendSetTemperatureStatus();
		void sendSetAirFlowStatus();
		void sendSetMinAirFlowStatus();
		void sendSetMaxAirFlowStatus();
		void sendSetRecirculationStatus();

		void sendTurnOff();
		void sendTurnOn();

		void receiveStatus();
		void receiveHumidityStatus();
		void receiveFanStatus();
		void receiveModeStatus();
		void receiveActiveStatus();
		void receiveTemperatureStatus();
		void receiveAirFlowStatus();
		void receiveMinAirFlowStatus();
		void receiveMaxAirFlowStatus();
		void receiveRecirculationStatus();
		void receiveGeneralFaultStatus();
		void receiveActualTemperatureStatus();
		void receiveActualHumidityStatus();
		void receiveActualAirFlowStatus();
		void receiveActualAirQualityStatus();
		void receiveAntiFreezeAlarmStatus();

	private:
		AirHandlingDevice *dev;
	};


	class TestVariableAirVolumeDevice : public TestDevice
	{
		Q_OBJECT
	private slots:
		void initTestCase();
		void cleanupTestCase();

		void sendRequestStatus();

		void sendSetStatus();
		void sendSetModeStatus();
		void sendSetActiveStatus();
		void sendSetTemperatureStatus();
		void sendSetAirFlowStatus();

		void sendTurnOff();
		void sendTurnOn();

		void receiveStatus();
		void receiveModeStatus();
		void receiveActiveStatus();
		void receiveTemperatureStatus();
		void receiveAirFLowStatus();
		void receiveActualTemperatureStatus();
		void receiveGeneralFaultStatus();

	private:
		VariableAirVolumeDevice *dev;
	};


	class TestUnderfloorHeatingDevice : public TestDevice
	{
		Q_OBJECT
	private slots:
		void initTestCase();
		void cleanupTestCase();

		void sendRequestStatus();

		void sendSetStatus();
		void sendSetModeStatus();
		void sendSetActiveStatus();
		void sendSetTemperatureStatus();

		void sendTurnOff();
		void sendTurnOn();

		void receiveStatus();
		void receiveModeStatus();
		void receiveActiveStatus();
		void receiveTemperatureStatus();
		void receiveActualTemperatureStatus();
		void receiveGeneralFaultStatus();

	private:
		UnderfloorHeatingDevice *dev;
	};

}

#endif // TESTBACNETDEVICE_H
