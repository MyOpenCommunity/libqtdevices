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

#include <QtTest/QtTest>

#include <openclient.h>
#include <bacnet_device.h>
#include "device_tester.h"
#include "openserver_mock.h"
#include "test_bacnet_device.h"

using namespace Bacnet;

/**************************************************/
void TestACSplitWithFanCoilDevice::initTestCase()
{
	dev = new ACSplitWithFanCoilDevice("11");
}

void TestACSplitWithFanCoilDevice::cleanupTestCase()
{
	delete dev;
}

void TestACSplitWithFanCoilDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#4*%1*%2##").arg(dev->where).arg(QString::number(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL)));
}

void TestACSplitWithFanCoilDevice::sendSetStatus()
{
	ACSplitWithFanCoilDevice::State state;
	state.temperature_set_point = 205;
	state.status = STATUS_ACTIVE;
	state.mode = MODE_WINTER;
	state.fan_speed = SPEED_SLOW;
	state.air_direction = ACSplitWithFanCoilDevice::DIRECTION_CYCLING;
	state.reset_filter_fault = true;

	QString set_point = QString::number(state.temperature_set_point);
	QString active = QString::number(state.status);
	QString mode = QString::number(state.mode);
	QString fan = QString::number(state.fan_speed);
	QString air_direction = QString::number(state.air_direction);
	QString filter = state.reset_filter_fault ? "0" : "";

	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(
				server->frameCommand(),
				QString("*#4*%1*#%2*%3*%4*%5*%6*%7*%8##")
				.arg(dev->where)
				.arg(QString::number(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL))
				.arg(set_point)
				.arg(active)
				.arg(mode)
				.arg(fan)
				.arg(air_direction)
				.arg(filter));
}

void TestACSplitWithFanCoilDevice::sendResetFilterStatus()
{
	ACSplitWithFanCoilDevice::State state;
	state.reset_filter_fault = true;
	QString filter = state.reset_filter_fault ? "0" : QString();
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2******%3##").arg(dev->where).arg(QString::number(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL)).arg(filter));
}

void TestACSplitWithFanCoilDevice::sendSetFanStatus()
{
	ACSplitWithFanCoilDevice::State state;
	state.fan_speed = SPEED_SLOW;
	QString fan = QString::number(state.fan_speed);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2****%3**##").arg(dev->where).arg(QString::number(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL)).arg(fan));
}

void TestACSplitWithFanCoilDevice::sendSetModeStatus()
{
	ACSplitWithFanCoilDevice::State state;
	state.mode = MODE_WINTER;
	QString mode = QString::number(state.mode);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2***%3***##").arg(dev->where).arg(QString::number(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL)).arg(mode));
}

void TestACSplitWithFanCoilDevice::sendSetActiveStatus()
{
	ACSplitWithFanCoilDevice::State state;
	state.status = STATUS_ACTIVE;
	QString active = QString::number(state.status);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2**%3****##").arg(dev->where).arg(QString::number(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL)).arg(active));
}

void TestACSplitWithFanCoilDevice::sendSetTemperatureStatus()
{
	ACSplitWithFanCoilDevice::State state;
	state.temperature_set_point = 205;
	QString temperature = QString::number(state.temperature_set_point);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2*%3*****##").arg(dev->where).arg(QString::number(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL)).arg(temperature));
}

void TestACSplitWithFanCoilDevice::sendSetAirDirectionStatus()
{
	ACSplitWithFanCoilDevice::State state;
	state.air_direction = ACSplitWithFanCoilDevice::DIRECTION_CYCLING;
	QString air_direction = QString::number(ACSplitWithFanCoilDevice::DIRECTION_CYCLING);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2*****%3*##").arg(dev->where).arg(QString::number(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL)).arg(air_direction));
}

void TestACSplitWithFanCoilDevice::sendTurnOff()
{
	ACSplitWithFanCoilDevice::State state;
	state.status = STATUS_INACTIVE;
	QString status = QString::number(state.status);
	dev->turnOff();
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2**%3****##").arg(dev->where).arg(QString::number(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL)).arg(status));
}

void TestACSplitWithFanCoilDevice::sendTurnOn()
{
	ACSplitWithFanCoilDevice::State state;
	state.status = STATUS_ACTIVE;
	QString status = QString::number(state.status);
	dev->turnOn();
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2**%3****##").arg(dev->where).arg(QString::number(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL)).arg(status));
}

void TestACSplitWithFanCoilDevice::receiveStatus()
{
	DeviceTester tst(dev, ACSplitWithFanCoilDevice::DIM_AC_FANCOIL);
	ACSplitWithFanCoilDevice::State state;
	state.temperature_set_point = 1;
	state.status = STATUS_ACTIVE;
	state.mode = MODE_WINTER;
	state.air_direction = ACSplitWithFanCoilDevice::DIRECTION_FORWARD;
	state.fan_speed = SPEED_SLOW;
	state.faultDescription = 210;
	state.hasFault = true;
	state.reset_filter_fault = true;
	state.temperature = 1;
	tst.check(QString("*#4*%1*%2*1*1*1*1*1*1*1*1*210##").arg(dev->where).arg(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL), state);
}

void TestACSplitWithFanCoilDevice::receiveFanStatus()
{
	DeviceTester tst(dev, ACSplitWithFanCoilDevice::DIM_AC_FANCOIL);
	ACSplitWithFanCoilDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.fan_speed = SPEED_SLOW;
	tst.check(QString("*#4*%1*%2****1##").arg(dev->where).arg(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL), state);
}

void TestACSplitWithFanCoilDevice::receiveModeStatus()
{
	DeviceTester tst(dev, ACSplitWithFanCoilDevice::DIM_AC_FANCOIL);
	ACSplitWithFanCoilDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_WINTER;
	tst.check(QString("*#4*%1*%2***1##").arg(dev->where).arg(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL), state);
}

void TestACSplitWithFanCoilDevice::receiveActiveStatus()
{
	DeviceTester tst(dev, ACSplitWithFanCoilDevice::DIM_AC_FANCOIL);
	ACSplitWithFanCoilDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_ACTIVE;
	tst.check(QString("*#4*%1*%2**1##").arg(dev->where).arg(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL), state);
}

void TestACSplitWithFanCoilDevice::receiveTemperatureStatus()
{
	DeviceTester tst(dev, ACSplitWithFanCoilDevice::DIM_AC_FANCOIL);
	ACSplitWithFanCoilDevice::State state;
	state.temperature_set_point = 305;
	tst.check(QString("*#4*%1*%2*305##").arg(dev->where).arg(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL), state);
}

void TestACSplitWithFanCoilDevice::receiveAirDirectionStatus()
{
	DeviceTester tst(dev, ACSplitWithFanCoilDevice::DIM_AC_FANCOIL);
	ACSplitWithFanCoilDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.fan_speed = SPEED_AUTO;
	state.air_direction = ACSplitWithFanCoilDevice::DIRECTION_RANDOM;
	tst.check(QString("*#4*%1*%2*****3##").arg(dev->where).arg(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL), state);
}

void TestACSplitWithFanCoilDevice::receiveGeneralFaultStatus()
{
	DeviceTester tst(dev, ACSplitWithFanCoilDevice::DIM_AC_FANCOIL);
	ACSplitWithFanCoilDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.fan_speed = SPEED_AUTO;
	state.air_direction = ACSplitWithFanCoilDevice::DIRECTION_AUTO;
	state.reset_filter_fault = false;
	state.temperature = 0;
	state.hasFault = true;
	state.faultDescription = 210;
	tst.check(QString("*#4*%1*%2********1*210##").arg(dev->where).arg(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL), state);
}

void TestACSplitWithFanCoilDevice::receiveFilterStatus()
{
	DeviceTester tst(dev, ACSplitWithFanCoilDevice::DIM_AC_FANCOIL);
	ACSplitWithFanCoilDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.fan_speed = SPEED_AUTO;
	state.air_direction = ACSplitWithFanCoilDevice::DIRECTION_AUTO;
	state.reset_filter_fault = true;
	tst.check(QString("*#4*%1*%2******1##").arg(dev->where).arg(ACSplitWithFanCoilDevice::DIM_AC_FANCOIL), state);
}

/**************************************************/
void TestAirHandlingDevice::initTestCase()
{
	dev = new AirHandlingDevice("11");
}

void TestAirHandlingDevice::cleanupTestCase()
{
	delete dev;
}

void TestAirHandlingDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#4*%1*%2##").arg(dev->where).arg(QString::number(AirHandlingDevice::DIM_AHU)));
}

void TestAirHandlingDevice::sendSetStatus()
{
	AirHandlingDevice::State state;
	state.temperature_set_point = 205;
	state.status = STATUS_ACTIVE;
	state.mode = MODE_WINTER;
	state.fan_speed = SPEED_SLOW;
	state.humidity_set_point = 50;
	state.air_flow_set_point = 40;
	state.min_air_flow = 20;
	state.max_air_flow = 80;
	state.recirculation = 30;

	QString temp_set_point = QString::number(state.temperature_set_point);
	QString status = QString::number(state.status);
	QString mode = QString::number(state.mode);
	QString fan = QString::number(state.fan_speed);
	QString hum_set_point = QString::number(state.humidity_set_point);
	QString air_flow_set_point = QString::number(state.air_flow_set_point);
	QString min_air_flow = QString::number(state.min_air_flow);
	QString max_air_flow = QString::number(state.max_air_flow);
	QString recirculation = QString::number(state.recirculation);

	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(
				server->frameCommand(),
				QString("*#4*%1*#%2*%3*%4*%5*%6*%7*%8*%9*%10*%11##")
				.arg(dev->where)
				.arg(QString::number(AirHandlingDevice::DIM_AHU))
				.arg(temp_set_point)
				.arg(status)
				.arg(mode)
				.arg(hum_set_point)
				.arg(air_flow_set_point)
				.arg(min_air_flow)
				.arg(max_air_flow)
				.arg(recirculation)
				.arg(fan));
}

void TestAirHandlingDevice::sendSetHumidityStatus()
{
	AirHandlingDevice::State state;
	state.humidity_set_point = 84;
	QString humidity = QString::number(state.humidity_set_point);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2****%3*****##").arg(dev->where).arg(QString::number(AirHandlingDevice::DIM_AHU)).arg(humidity));
}

void TestAirHandlingDevice::sendSetFanStatus()
{
	AirHandlingDevice::State state;
	state.fan_speed = SPEED_SLOW;
	QString fan = QString::number(state.fan_speed);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2*********%3##").arg(dev->where).arg(QString::number(AirHandlingDevice::DIM_AHU)).arg(fan));
}

void TestAirHandlingDevice::sendSetModeStatus()
{
	AirHandlingDevice::State state;
	state.mode = MODE_WINTER;
	QString mode = QString::number(state.mode);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2***%3******##").arg(dev->where).arg(QString::number(AirHandlingDevice::DIM_AHU)).arg(mode));
}

void TestAirHandlingDevice::sendSetActiveStatus()
{
	AirHandlingDevice::State state;
	state.status = STATUS_ACTIVE;
	QString active = QString::number(state.status);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2**%3*******##").arg(dev->where).arg(QString::number(AirHandlingDevice::DIM_AHU)).arg(active));
}

void TestAirHandlingDevice::sendSetTemperatureStatus()
{
	AirHandlingDevice::State state;
	state.temperature_set_point = 205;
	QString temperature = QString::number(state.temperature_set_point);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2*%3********##").arg(dev->where).arg(QString::number(AirHandlingDevice::DIM_AHU)).arg(temperature));
}

void TestAirHandlingDevice::sendSetAirFlowStatus()
{
	AirHandlingDevice::State state;
	state.air_flow_set_point = 40;
	QString air_flow = QString::number(state.air_flow_set_point);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2*****%3****##").arg(dev->where).arg(QString::number(AirHandlingDevice::DIM_AHU)).arg(air_flow));
}

void TestAirHandlingDevice::sendSetMinAirFlowStatus()
{
	AirHandlingDevice::State state;
	state.min_air_flow = 10;
	QString min_air_flow = QString::number(state.min_air_flow);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2******%3***##").arg(dev->where).arg(QString::number(AirHandlingDevice::DIM_AHU)).arg(min_air_flow));
}

void TestAirHandlingDevice::sendSetMaxAirFlowStatus()
{
	AirHandlingDevice::State state;
	state.max_air_flow = 100;
	QString max_air_flow = QString::number(state.max_air_flow);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2*******%3**##").arg(dev->where).arg(QString::number(AirHandlingDevice::DIM_AHU)).arg(max_air_flow));
}

void TestAirHandlingDevice::sendSetRecirculationStatus()
{
	AirHandlingDevice::State state;
	state.recirculation = 90;
	QString recirculation = QString::number(state.recirculation);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2********%3*##").arg(dev->where).arg(QString::number(AirHandlingDevice::DIM_AHU)).arg(recirculation));
}

void TestAirHandlingDevice::sendTurnOff()
{
	AirHandlingDevice::State state;
	state.status = STATUS_INACTIVE;
	QString status = QString::number(state.status);
	dev->turnOff();
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2**%3*******##").arg(dev->where).arg(QString::number(AirHandlingDevice::DIM_AHU)).arg(status));
}

void TestAirHandlingDevice::sendTurnOn()
{
	AirHandlingDevice::State state;
	state.status = STATUS_ACTIVE;
	QString status = QString::number(state.status);
	dev->turnOn();
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2**%3*******##").arg(dev->where).arg(QString::number(AirHandlingDevice::DIM_AHU)).arg(status));
}

void TestAirHandlingDevice::receiveStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 205;
	state.status = STATUS_ACTIVE;
	state.mode = MODE_DRY;
	state.humidity_set_point = 50;
	state.air_flow_set_point = 60;
	state.min_air_flow = 20;
	state.max_air_flow = 80;
	state.recirculation = 50;
	state.fan_speed = SPEED_NORMAL;
	state.temperature = 105;
	state.hasFault = true;
	state.faultDescription = 221;
	state.humidity = 50;
	state.airFlow = 40;
	state.airQuality = 50;
	state.isAlarmActive = true;
	tst.check(QString("*#4*%1*%2*205*1*4*50*60*20*80*50*2*105*1*221*50*40*50*1##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

void TestAirHandlingDevice::receiveFanStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.humidity_set_point = 0;
	state.air_flow_set_point = 0;
	state.min_air_flow = 0;
	state.max_air_flow = 0;
	state.recirculation = 0;
	state.fan_speed = SPEED_SLOW;
	tst.check(QString("*#4*%1*%2*********1##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

void TestAirHandlingDevice::receiveModeStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_WINTER;
	tst.check(QString("*#4*%1*%2***1##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

void TestAirHandlingDevice::receiveActiveStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_ACTIVE;
	tst.check(QString("*#4*%1*%2**1##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

void TestAirHandlingDevice::receiveTemperatureStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 305;
	tst.check(QString("*#4*%1*%2*305##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

void TestAirHandlingDevice::receiveAirFlowStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.humidity_set_point = 0;
	state.air_flow_set_point = 40;
	tst.check(QString("*#4*%1*%2*****40##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

void TestAirHandlingDevice::receiveGeneralFaultStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.humidity_set_point = 0;
	state.air_flow_set_point = 0;
	state.min_air_flow = 0;
	state.max_air_flow = 0;
	state.recirculation = 0;
	state.fan_speed = SPEED_AUTO;
	state.temperature = 0;
	state.hasFault = true;
	state.faultDescription = 210;
	tst.check(QString("*#4*%1*%2***********1*210##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

void TestAirHandlingDevice::receiveMinAirFlowStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.humidity_set_point = 0;
	state.air_flow_set_point = 0;
	state.min_air_flow = 10;
	tst.check(QString("*#4*%1*%2******10##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

void TestAirHandlingDevice::receiveMaxAirFlowStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.humidity_set_point = 0;
	state.air_flow_set_point = 0;
	state.min_air_flow = 0;
	state.max_air_flow = 80;
	tst.check(QString("*#4*%1*%2*******80##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

void TestAirHandlingDevice::receiveRecirculationStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.humidity_set_point = 0;
	state.air_flow_set_point = 0;
	state.min_air_flow = 0;
	state.max_air_flow = 0;
	state.recirculation = 60;
	tst.check(QString("*#4*%1*%2********60##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

void TestAirHandlingDevice::receiveHumidityStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.humidity_set_point = 90;
	tst.check(QString("*#4*%1*%2****90##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

void TestAirHandlingDevice::receiveActualTemperatureStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.humidity_set_point = 0;
	state.air_flow_set_point = 0;
	state.min_air_flow = 0;
	state.max_air_flow = 0;
	state.recirculation = 0;
	state.fan_speed = SPEED_AUTO;
	state.temperature = 105;
	tst.check(QString("*#4*%1*%2**********105##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

void TestAirHandlingDevice::receiveActualHumidityStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.humidity_set_point = 0;
	state.air_flow_set_point = 0;
	state.min_air_flow = 0;
	state.max_air_flow = 0;
	state.recirculation = 0;
	state.fan_speed = SPEED_AUTO;
	state.temperature = 0;
	state.hasFault = false;
	state.faultDescription = 0;
	state.humidity = 90;
	tst.check(QString("*#4*%1*%2*************90##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

void TestAirHandlingDevice::receiveActualAirFlowStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.humidity_set_point = 0;
	state.air_flow_set_point = 0;
	state.min_air_flow = 0;
	state.max_air_flow = 0;
	state.recirculation = 0;
	state.fan_speed = SPEED_AUTO;
	state.temperature = 0;
	state.hasFault = false;
	state.faultDescription = 0;
	state.humidity = 0;
	state.airFlow = 50;
	tst.check(QString("*#4*%1*%2**************50##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

void TestAirHandlingDevice::receiveActualAirQualityStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.humidity_set_point = 0;
	state.air_flow_set_point = 0;
	state.min_air_flow = 0;
	state.max_air_flow = 0;
	state.recirculation = 0;
	state.fan_speed = SPEED_AUTO;
	state.temperature = 0;
	state.hasFault = false;
	state.faultDescription = 0;
	state.humidity = 0;
	state.airFlow = 0;
	state.airQuality = 35;
	tst.check(QString("*#4*%1*%2***************35##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

void TestAirHandlingDevice::receiveAntiFreezeAlarmStatus()
{
	DeviceTester tst(dev, AirHandlingDevice::DIM_AHU);
	AirHandlingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.humidity_set_point = 0;
	state.air_flow_set_point = 0;
	state.min_air_flow = 0;
	state.max_air_flow = 0;
	state.recirculation = 0;
	state.fan_speed = SPEED_AUTO;
	state.temperature = 0;
	state.hasFault = false;
	state.faultDescription = 0;
	state.humidity = 0;
	state.airFlow = 0;
	state.airQuality = 0;
	state.isAlarmActive = true;
	tst.check(QString("*#4*%1*%2****************1##").arg(dev->where).arg(AirHandlingDevice::DIM_AHU), state);
}

/**************************************************/
void TestVariableAirVolumeDevice::initTestCase()
{
	dev = new VariableAirVolumeDevice("11");
}

void TestVariableAirVolumeDevice::cleanupTestCase()
{
	delete dev;
}

void TestVariableAirVolumeDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#4*%1*%2##").arg(dev->where).arg(QString::number(VariableAirVolumeDevice::DIM_VAV)));
}

void TestVariableAirVolumeDevice::sendSetStatus()
{
	VariableAirVolumeDevice::State state;
	state.temperature_set_point = 205;
	state.status = STATUS_ACTIVE;
	state.mode = MODE_WINTER;
	state.air_flow_set_point = 40;

	QString temp_set_point = QString::number(state.temperature_set_point);
	QString status = QString::number(state.status);
	QString mode = QString::number(state.mode);
	QString air_flow_set_point = QString::number(state.air_flow_set_point);

	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(
				server->frameCommand(),
				QString("*#4*%1*#%2*%3*%4*%5*%6##")
				.arg(dev->where)
				.arg(QString::number(VariableAirVolumeDevice::DIM_VAV))
				.arg(temp_set_point)
				.arg(status)
				.arg(mode)
				.arg(air_flow_set_point));
}

void TestVariableAirVolumeDevice::sendSetModeStatus()
{
	VariableAirVolumeDevice::State state;
	state.mode = MODE_WINTER;
	QString mode = QString::number(state.mode);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2***%3*##").arg(dev->where).arg(QString::number(VariableAirVolumeDevice::DIM_VAV)).arg(mode));
}

void TestVariableAirVolumeDevice::sendSetActiveStatus()
{
	VariableAirVolumeDevice::State state;
	state.status = STATUS_ACTIVE;
	QString active = QString::number(state.status);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2**%3**##").arg(dev->where).arg(QString::number(VariableAirVolumeDevice::DIM_VAV)).arg(active));
}

void TestVariableAirVolumeDevice::sendSetTemperatureStatus()
{
	VariableAirVolumeDevice::State state;
	state.temperature_set_point = 405;
	QString temperature_set_point = QString::number(state.temperature_set_point);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2*%3***##").arg(dev->where).arg(QString::number(VariableAirVolumeDevice::DIM_VAV)).arg(temperature_set_point));
}

void TestVariableAirVolumeDevice::sendSetAirFlowStatus()
{
	VariableAirVolumeDevice::State state;
	state.air_flow_set_point = 40;
	QString air_flow = QString::number(state.air_flow_set_point);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2****%3##").arg(dev->where).arg(QString::number(VariableAirVolumeDevice::DIM_VAV)).arg(air_flow));
}

void TestVariableAirVolumeDevice::sendTurnOff()
{
	VariableAirVolumeDevice::State state;
	state.status = STATUS_INACTIVE;
	QString status = QString::number(state.status);
	dev->turnOff();
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2**%3**##").arg(dev->where).arg(QString::number(VariableAirVolumeDevice::DIM_VAV)).arg(status));
}

void TestVariableAirVolumeDevice::sendTurnOn()
{
	VariableAirVolumeDevice::State state;
	state.status = STATUS_ACTIVE;
	QString status = QString::number(state.status);
	dev->turnOn();
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2**%3**##").arg(dev->where).arg(QString::number(VariableAirVolumeDevice::DIM_VAV)).arg(status));
}

void TestVariableAirVolumeDevice::receiveStatus()
{
	DeviceTester tst(dev, VariableAirVolumeDevice::DIM_VAV);
	VariableAirVolumeDevice::State state;
	state.temperature_set_point = 205;
	state.status = STATUS_ACTIVE;
	state.mode = MODE_DRY;
	state.air_flow_set_point = 50;
	tst.check(QString("*#4*%1*%2*205*1*4*50##").arg(dev->where).arg(VariableAirVolumeDevice::DIM_VAV), state);
}

void TestVariableAirVolumeDevice::receiveModeStatus()
{
	DeviceTester tst(dev, VariableAirVolumeDevice::DIM_VAV);
	VariableAirVolumeDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_WINTER;
	tst.check(QString("*#4*%1*%2***1##").arg(dev->where).arg(VariableAirVolumeDevice::DIM_VAV), state);
}

void TestVariableAirVolumeDevice::receiveActiveStatus()
{
	DeviceTester tst(dev, VariableAirVolumeDevice::DIM_VAV);
	VariableAirVolumeDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_ACTIVE;
	tst.check(QString("*#4*%1*%2**1##").arg(dev->where).arg(VariableAirVolumeDevice::DIM_VAV), state);
}

void TestVariableAirVolumeDevice::receiveTemperatureStatus()
{
	DeviceTester tst(dev, VariableAirVolumeDevice::DIM_VAV);
	VariableAirVolumeDevice::State state;
	state.temperature_set_point = 205;
	tst.check(QString("*#4*%1*%2*205##").arg(dev->where).arg(VariableAirVolumeDevice::DIM_VAV), state);
}

void TestVariableAirVolumeDevice::receiveAirFLowStatus()
{
	DeviceTester tst(dev, VariableAirVolumeDevice::DIM_VAV);
	VariableAirVolumeDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.air_flow_set_point = 55;
	tst.check(QString("*#4*%1*%2****55##").arg(dev->where).arg(VariableAirVolumeDevice::DIM_VAV), state);
}

void TestVariableAirVolumeDevice::receiveActualTemperatureStatus()
{
	DeviceTester tst(dev, VariableAirVolumeDevice::DIM_VAV);
	VariableAirVolumeDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.air_flow_set_point = 0;
	state.temperature = 105;
	tst.check(QString("*#4*%1*%2*****105##").arg(dev->where).arg(VariableAirVolumeDevice::DIM_VAV), state);
}

void TestVariableAirVolumeDevice::receiveGeneralFaultStatus()
{
	DeviceTester tst(dev, VariableAirVolumeDevice::DIM_VAV);
	VariableAirVolumeDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.air_flow_set_point = 0;
	state.temperature = 0;
	state.hasFault = true;
	state.faultDescription = 200;
	tst.check(QString("*#4*%1*%2******1*200##").arg(dev->where).arg(VariableAirVolumeDevice::DIM_VAV), state);
}

/**************************************************/
void TestUnderfloorHeatingDevice::initTestCase()
{
	dev = new UnderfloorHeatingDevice("11");
}

void TestUnderfloorHeatingDevice::cleanupTestCase()
{
	delete dev;
}

void TestUnderfloorHeatingDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#4*%1*%2##").arg(dev->where).arg(QString::number(UnderfloorHeatingDevice::DIM_UH)));
}

void TestUnderfloorHeatingDevice::sendSetStatus()
{
	UnderfloorHeatingDevice::State state;
	state.temperature_set_point = 205;
	state.status = STATUS_ACTIVE;
	state.mode = MODE_WINTER;

	QString temp_set_point = QString::number(state.temperature_set_point);
	QString status = QString::number(state.status);
	QString mode = QString::number(state.mode);

	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(
				server->frameCommand(),
				QString("*#4*%1*#%2*%3*%4*%5##")
				.arg(dev->where)
				.arg(QString::number(UnderfloorHeatingDevice::DIM_UH))
				.arg(temp_set_point)
				.arg(status)
				.arg(mode));
}

void TestUnderfloorHeatingDevice::sendSetModeStatus()
{
	UnderfloorHeatingDevice::State state;
	state.mode = MODE_WINTER;
	QString mode = QString::number(state.mode);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2***%3##").arg(dev->where).arg(QString::number(UnderfloorHeatingDevice::DIM_UH)).arg(mode));
}

void TestUnderfloorHeatingDevice::sendSetActiveStatus()
{
	UnderfloorHeatingDevice::State state;
	state.status = STATUS_ACTIVE;
	QString status = QString::number(state.status);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2**%3*##").arg(dev->where).arg(QString::number(UnderfloorHeatingDevice::DIM_UH)).arg(status));
}

void TestUnderfloorHeatingDevice::sendSetTemperatureStatus()
{
	UnderfloorHeatingDevice::State state;
	state.temperature_set_point = 205;
	QString temperature = QString::number(state.temperature_set_point);
	dev->setStatus(state);
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2*%3**##").arg(dev->where).arg(QString::number(UnderfloorHeatingDevice::DIM_UH)).arg(temperature));
}

void TestUnderfloorHeatingDevice::sendTurnOff()
{
	UnderfloorHeatingDevice::State state;
	state.status = STATUS_INACTIVE;
	QString status = QString::number(state.status);
	dev->turnOff();
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2**%3*##").arg(dev->where).arg(QString::number(UnderfloorHeatingDevice::DIM_UH)).arg(status));
}

void TestUnderfloorHeatingDevice::sendTurnOn()
{
	UnderfloorHeatingDevice::State state;
	state.status = STATUS_ACTIVE;
	QString status = QString::number(state.status);
	dev->turnOn();
	flushCompressedFrames(dev);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*%1*#%2**%3*##").arg(dev->where).arg(QString::number(UnderfloorHeatingDevice::DIM_UH)).arg(status));
}

void TestUnderfloorHeatingDevice::receiveStatus()
{
	DeviceTester tst(dev, UnderfloorHeatingDevice::DIM_UH);
	UnderfloorHeatingDevice::State state;
	state.temperature_set_point = 205;
	state.status = STATUS_ACTIVE;
	state.mode = MODE_DRY;
	tst.check(QString("*#4*%1*%2*205*1*4##").arg(dev->where).arg(UnderfloorHeatingDevice::DIM_UH), state);
}

void TestUnderfloorHeatingDevice::receiveModeStatus()
{
	DeviceTester tst(dev, UnderfloorHeatingDevice::DIM_UH);
	UnderfloorHeatingDevice::State state;
	state.mode = MODE_WINTER;
	state.status = STATUS_INACTIVE;
	state.temperature_set_point = 0;
	tst.check(QString("*#4*%1*%2***1##").arg(dev->where).arg(UnderfloorHeatingDevice::DIM_UH), state);
}

void TestUnderfloorHeatingDevice::receiveActiveStatus()
{
	DeviceTester tst(dev, UnderfloorHeatingDevice::DIM_UH);
	UnderfloorHeatingDevice::State state;
	state.status = STATUS_ACTIVE;
	state.temperature_set_point = 0;
	tst.check(QString("*#4*%1*%2**1##").arg(dev->where).arg(UnderfloorHeatingDevice::DIM_UH), state);
}

void TestUnderfloorHeatingDevice::receiveTemperatureStatus()
{
	DeviceTester tst(dev, UnderfloorHeatingDevice::DIM_UH);
	UnderfloorHeatingDevice::State state;
	state.temperature_set_point = 205;
	tst.check(QString("*#4*%1*%2*205##").arg(dev->where).arg(UnderfloorHeatingDevice::DIM_UH), state);
}

void TestUnderfloorHeatingDevice::receiveActualTemperatureStatus()
{
	DeviceTester tst(dev, UnderfloorHeatingDevice::DIM_UH);
	UnderfloorHeatingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.temperature = 205;
	tst.check(QString("*#4*%1*%2****205##").arg(dev->where).arg(UnderfloorHeatingDevice::DIM_UH), state);
}

void TestUnderfloorHeatingDevice::receiveGeneralFaultStatus()
{
	DeviceTester tst(dev, UnderfloorHeatingDevice::DIM_UH);
	UnderfloorHeatingDevice::State state;
	state.temperature_set_point = 0;
	state.status = STATUS_INACTIVE;
	state.mode = MODE_INACTIVE;
	state.temperature = 0;
	state.hasFault = true;
	state.faultDescription = 150;
	tst.check(QString("*#4*%1*%2*****1*150##").arg(dev->where).arg(UnderfloorHeatingDevice::DIM_UH), state);
}
