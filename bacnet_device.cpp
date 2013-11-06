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

#include <QStringList>

#include <openmsg.h>
#include "bacnet_device.h"
#include "frame_functions.h"

const int Bacnet::NOTSET_NUM_VALUE = -1;

using namespace Bacnet;


ACSplitWithFanCoilDevice::ACSplitWithFanCoilDevice(QString where, int openserver_id) :
	device("4", where, openserver_id)
{
}

void ACSplitWithFanCoilDevice::requestStatus() const
{
	sendRequest(DIM_AC_FANCOIL);
}

void ACSplitWithFanCoilDevice::setStatus(
		int temp_set_point,
		Status active,
		Mode mode,
		FanSpeed speed,
		AirDirection direction,
		bool reset_filter_fault) const
{
	State st(temp_set_point, active, mode, speed, direction, reset_filter_fault);
	setStatus(st);
}

void ACSplitWithFanCoilDevice::setStatus(const State &st) const
{
	sendCompressedFrame(createWriteDimensionFrame(who, statusToWhat(st), where));
}

void ACSplitWithFanCoilDevice::init()
{
	requestStatus();
}

void ACSplitWithFanCoilDevice::turnOff() const
{
	setStatus(
				NOTSET_NUM_VALUE,
				STATUS_INACTIVE,
				MODE_NOTSET,
				SPEED_NOTSET,
				DIRECTION_NOTSET,
				false);
}

void ACSplitWithFanCoilDevice::turnOn() const
{
	setStatus(
				NOTSET_NUM_VALUE,
				STATUS_ACTIVE,
				MODE_NOTSET,
				SPEED_NOTSET,
				DIRECTION_NOTSET,
				false);
}

void ACSplitWithFanCoilDevice::resetFilterFault() const
{
	setStatus(
				NOTSET_NUM_VALUE,
				STATUS_NOTSET,
				MODE_NOTSET,
				SPEED_NOTSET,
				DIRECTION_NOTSET,
				true);
}

QString ACSplitWithFanCoilDevice::statusToWhat(const State &st) const
{
	QString what;

	QString temp_set_point = st.temperature_set_point == NOTSET_NUM_VALUE ? QString() : QString::number(st.temperature_set_point);
	QString status = st.status == STATUS_NOTSET ? QString() : QString::number((int)st.status);
	QString mode = st.mode == MODE_NOTSET ? QString() : QString::number((int)st.mode);
	QString speed = st.fan_speed == SPEED_NOTSET ? QString() : QString::number((int)st.fan_speed);
	QString direction = st.air_direction == DIRECTION_NOTSET ? QString() : QString::number((int)st.air_direction);
	QString filter_fault = st.reset_filter_fault ? "0" : QString();

	what = QString("%1*%2*%3*%4*%5*%6*%7").arg(DIM_AC_FANCOIL).arg(temp_set_point).arg(status).arg(mode).arg(speed).arg(direction).arg(filter_fault);

	return what;
}

bool ACSplitWithFanCoilDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (msg.whereFull() != where.toStdString())
	{
		return false;
	}

	int what = msg.what();
	if (what != DIM_AC_FANCOIL)
	{
		return false;
	}

	unsigned count = msg.whatArgCnt();

	State state;
	state.temperature_set_point = count > 0 ? QString::fromStdString(msg.whatArg(0)).toInt() : NOTSET_NUM_VALUE;
	state.status = count > 1 ? static_cast<Status>(QString::fromStdString(msg.whatArg(1)).toInt()) : STATUS_NOTSET;
	state.mode = count > 2 ? static_cast<Mode>(QString::fromStdString(msg.whatArg(2)).toInt()) : MODE_NOTSET;
	state.fan_speed = count > 3 ? static_cast<FanSpeed>(QString::fromStdString(msg.whatArg(3)).toInt()) : SPEED_NOTSET;
	state.air_direction = count > 4 ? static_cast<AirDirection>(QString::fromStdString(msg.whatArg(4)).toInt()) : DIRECTION_NOTSET;
	state.reset_filter_fault= count > 5 ? (bool)QString::fromStdString(msg.whatArg(5)).toInt() : false;
	state.temperature = count > 6 ? QString::fromStdString(msg.whatArg(6)).toInt() : NOTSET_NUM_VALUE;
	state.hasFault = count > 7 ? (bool)QString::fromStdString(msg.whatArg(7)).toInt() : false;
	state.faultDescription = count > 8 ? QString::fromStdString(msg.whatArg(8)).toInt() : NOTSET_NUM_VALUE;

	QVariant var;
	var.setValue(state);
	values_list[DIM_AC_FANCOIL] = var;

	return true;
}


ACSplitWithFanCoilDevice::State::State() :
	temperature_set_point(NOTSET_NUM_VALUE),
	status(STATUS_NOTSET),
	mode(MODE_NOTSET),
	fan_speed(SPEED_NOTSET),
	air_direction(DIRECTION_NOTSET),
	reset_filter_fault(false),
	temperature(NOTSET_NUM_VALUE),
	hasFault(false),
	faultDescription(NOTSET_NUM_VALUE)
{
}

ACSplitWithFanCoilDevice::State::State( int temp_set_point,
	   Status status,
	   Mode mode,
	   FanSpeed speed,
	   AirDirection direction,
	   bool filter_fault) :
	temperature_set_point(temp_set_point),
	status(status),
	mode(mode),
	fan_speed(speed),
	air_direction(direction),
	reset_filter_fault(filter_fault),
	temperature(NOTSET_NUM_VALUE),
	hasFault(false),
	faultDescription(NOTSET_NUM_VALUE)
{
}

bool ACSplitWithFanCoilDevice::State::operator==(const State &other) const
{
	if (temperature_set_point != other.temperature_set_point ||
		status != other.status ||
		mode != other.mode ||
		fan_speed != other.fan_speed ||
		air_direction != other.air_direction ||
		reset_filter_fault != other.reset_filter_fault ||
		temperature != other.temperature ||
		hasFault != other.hasFault ||
		faultDescription != other.faultDescription)
	{
		return false;
	}

	return true;
}

bool ACSplitWithFanCoilDevice::State::operator!=(const State &other) const
{
	return !(*this == other);
}

/***********************************************************************/
AirHandlingDevice::AirHandlingDevice(QString where, int openserver_id) :
	device("4", where, openserver_id)
{
}

void AirHandlingDevice::init()
{
	requestStatus();
}

void AirHandlingDevice::turnOff() const
{
	setStatus(
				NOTSET_NUM_VALUE,
				STATUS_INACTIVE,
				MODE_NOTSET,
				NOTSET_NUM_VALUE,
				NOTSET_NUM_VALUE,
				NOTSET_NUM_VALUE,
				NOTSET_NUM_VALUE,
				NOTSET_NUM_VALUE,
				SPEED_NOTSET);
}

void AirHandlingDevice::turnOn() const
{
	setStatus(
				NOTSET_NUM_VALUE,
				STATUS_ACTIVE,
				MODE_NOTSET,
				NOTSET_NUM_VALUE,
				NOTSET_NUM_VALUE,
				NOTSET_NUM_VALUE,
				NOTSET_NUM_VALUE,
				NOTSET_NUM_VALUE,
				SPEED_NOTSET);
}

void AirHandlingDevice::requestStatus() const
{
	sendRequest(DIM_AHU);
}

QString AirHandlingDevice::statusToWhat(const State &state) const
{
	QString what;

	QString temp_set_point = state.temperature_set_point == NOTSET_NUM_VALUE ? QString() : QString::number(state.temperature_set_point);
	QString status = state.status == STATUS_NOTSET ? QString() : QString::number((int)state.status);
	QString mode = state.mode == MODE_NOTSET ? QString() : QString::number((int)state.mode);
	QString hum_set_point = state.humidity_set_point == NOTSET_NUM_VALUE ? QString() : QString::number(state.humidity_set_point);
	QString flow_set_point = state.air_flow_set_point == NOTSET_NUM_VALUE ? QString() : QString::number(state.air_flow_set_point);
	QString min_flow = state.min_air_flow == NOTSET_NUM_VALUE ? QString() : QString::number(state.min_air_flow);
	QString max_flow = state.max_air_flow == NOTSET_NUM_VALUE ? QString() : QString::number(state.max_air_flow);
	QString recirc = state.recirculation == NOTSET_NUM_VALUE ? QString() : QString::number(state.recirculation);
	QString speed = state.fan_speed == SPEED_NOTSET ? QString() : QString::number((int)state.fan_speed);

	what = QString("%1*%2*%3*%4*%5*%6*%7*%8*%9*%10")
		   .arg(DIM_AHU)
		   .arg(temp_set_point)
		   .arg(status)
		   .arg(mode)
		   .arg(hum_set_point)
		   .arg(flow_set_point)
		   .arg(min_flow)
		   .arg(max_flow)
		   .arg(recirc)
		   .arg(speed);

	return what;
}

void AirHandlingDevice::setStatus(
		int temperature_set_point,
		Status status,
		Mode mode,
		int humidity_set_point,
		int air_flow_set_point,
		int min_air_flow,
		int max_air_flow,
		int recirculation,
		FanSpeed fan_speed) const
{
	State st(
				temperature_set_point,
				status,
				mode,
				humidity_set_point,
				air_flow_set_point,
				min_air_flow,
				max_air_flow,
				recirculation,
				fan_speed);

	setStatus(st);
}

void AirHandlingDevice::setStatus(const State &st) const
{
	sendCompressedFrame(createWriteDimensionFrame(who, statusToWhat(st), where));
}

bool AirHandlingDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (msg.whereFull() != where.toStdString())
	{
		return false;
	}

	int what = msg.what();
	if (what != DIM_AHU)
	{
		return false;
	}

	unsigned count = msg.whatArgCnt();

	State state;
	state.temperature_set_point = count > 0 ? QString::fromStdString(msg.whatArg(0)).toInt() : NOTSET_NUM_VALUE;
	state.status = count > 1 ? static_cast<Status>(QString::fromStdString(msg.whatArg(1)).toInt()) : STATUS_NOTSET;
	state.mode = count > 2 ? static_cast<Mode>(QString::fromStdString(msg.whatArg(2)).toInt()) : MODE_NOTSET;
	state.humidity_set_point = count > 3 ? QString::fromStdString(msg.whatArg(3)).toInt() : NOTSET_NUM_VALUE;
	state.air_flow_set_point = count > 4 ? QString::fromStdString(msg.whatArg(4)).toInt() : NOTSET_NUM_VALUE;
	state.min_air_flow = count > 5 ? QString::fromStdString(msg.whatArg(5)).toInt() : NOTSET_NUM_VALUE;
	state.max_air_flow = count > 6 ? QString::fromStdString(msg.whatArg(6)).toInt() : NOTSET_NUM_VALUE;
	state.recirculation = count > 7 ? QString::fromStdString(msg.whatArg(7)).toInt() : NOTSET_NUM_VALUE;
	state.fan_speed = count > 8 ? static_cast<FanSpeed>(QString::fromStdString(msg.whatArg(8)).toInt()) : SPEED_NOTSET;
	state.temperature = count > 9 ? QString::fromStdString(msg.whatArg(9)).toInt() : NOTSET_NUM_VALUE;
	state.hasFault = count > 10 ? (bool)QString::fromStdString(msg.whatArg(10)).toInt() : false;
	state.faultDescription = count > 11 ? QString::fromStdString(msg.whatArg(11)).toInt() : NOTSET_NUM_VALUE;
	state.humidity = count > 12 ? QString::fromStdString(msg.whatArg(12)).toInt() : NOTSET_NUM_VALUE;
	state.airFlow = count > 13 ? QString::fromStdString(msg.whatArg(13)).toInt() : NOTSET_NUM_VALUE;
	state.airQuality = count > 14 ? QString::fromStdString(msg.whatArg(14)).toInt() : NOTSET_NUM_VALUE;
	state.isAlarmActive = count > 15 ? (bool)QString::fromStdString(msg.whatArg(15)).toInt() : false;

	QVariant var;
	var.setValue(state);
	values_list[DIM_AHU] = var;

	return true;
}


AirHandlingDevice::State::State() :
	temperature_set_point(NOTSET_NUM_VALUE),
	status(STATUS_NOTSET),
	mode(MODE_NOTSET),
	humidity_set_point(NOTSET_NUM_VALUE),
	air_flow_set_point(NOTSET_NUM_VALUE),
	min_air_flow(NOTSET_NUM_VALUE),
	max_air_flow(NOTSET_NUM_VALUE),
	recirculation(NOTSET_NUM_VALUE),
	fan_speed(SPEED_NOTSET),
	temperature(NOTSET_NUM_VALUE),
	hasFault(false),
	faultDescription(NOTSET_NUM_VALUE),
	humidity(NOTSET_NUM_VALUE),
	airFlow(NOTSET_NUM_VALUE),
	airQuality(NOTSET_NUM_VALUE),
	isAlarmActive(false)
{
}

AirHandlingDevice::State::State(
		int temperature_set_point,
		Status status,
		Mode mode,
		int humidity_set_point,
		int air_flow_set_point,
		int min_air_flow,
		int max_air_flow,
		int recirculation,
		FanSpeed fan_speed)	:
	temperature_set_point(temperature_set_point),
	status(status),
	mode(mode),
	humidity_set_point(humidity_set_point),
	air_flow_set_point(air_flow_set_point),
	min_air_flow(min_air_flow),
	max_air_flow(max_air_flow),
	recirculation(recirculation),
	fan_speed(fan_speed),
	temperature(NOTSET_NUM_VALUE),
	hasFault(false),
	faultDescription(NOTSET_NUM_VALUE),
	humidity(NOTSET_NUM_VALUE),
	airFlow(NOTSET_NUM_VALUE),
	airQuality(NOTSET_NUM_VALUE),
	isAlarmActive(false)
{
}

bool AirHandlingDevice::State::operator==(const State &other) const
{
	if (temperature_set_point != other.temperature_set_point ||
		status != other.status ||
		mode != other.mode ||
		humidity_set_point != other.humidity_set_point ||
		air_flow_set_point != other.air_flow_set_point ||
		min_air_flow != other.min_air_flow ||
		max_air_flow != other.max_air_flow ||
		recirculation != other.recirculation ||
		fan_speed != other.fan_speed ||
		temperature != other.temperature ||
		hasFault != other.hasFault ||
		faultDescription != other.faultDescription ||
		humidity  != other.humidity ||
		airFlow  != other.airFlow ||
		airQuality  != other.airQuality ||
		isAlarmActive  != other.isAlarmActive)
	{
		return false;
	}

	return true;
}

bool AirHandlingDevice::State::operator!=(const State &other) const
{
	return !(*this == other);
}

/***********************************************************************/
VariableAirVolumeDevice::VariableAirVolumeDevice(QString where, int openserver_id) :
	device("4", where, openserver_id)
{
}

void VariableAirVolumeDevice::init()
{
	requestStatus();
}

void VariableAirVolumeDevice::turnOff() const
{
	setStatus(NOTSET_NUM_VALUE, STATUS_INACTIVE, MODE_NOTSET, NOTSET_NUM_VALUE);
}

void VariableAirVolumeDevice::turnOn() const
{
	setStatus(NOTSET_NUM_VALUE, STATUS_ACTIVE, MODE_NOTSET, NOTSET_NUM_VALUE);
}

void VariableAirVolumeDevice::requestStatus() const
{
	sendRequest(DIM_VAV);
}

QString VariableAirVolumeDevice::statusToWhat(const State &state) const
{
	QString what;

	QString temp_set_point = state.temperature_set_point == NOTSET_NUM_VALUE ? QString() : QString::number(state.temperature_set_point);
	QString status = state.status == STATUS_NOTSET ? QString() : QString::number((int)state.status);
	QString mode = state.mode == MODE_NOTSET ? QString() : QString::number((int)state.mode);
	QString flow_set_point = state.air_flow_set_point == NOTSET_NUM_VALUE ? QString() : QString::number(state.air_flow_set_point);

	what = QString("%1*%2*%3*%4*%5")
		   .arg(DIM_VAV)
		   .arg(temp_set_point)
		   .arg(status)
		   .arg(mode)
		   .arg(flow_set_point);

	return what;
}

void VariableAirVolumeDevice::setStatus(
		int temperature_set_point,
		Status status,
		Mode mode,
		int air_flow_set_point) const
{
	State state(temperature_set_point, status, mode, air_flow_set_point);
	setStatus(state);
}

void VariableAirVolumeDevice::setStatus(const State &state) const
{
	sendCompressedFrame(createWriteDimensionFrame(who, statusToWhat(state), where));
}

bool VariableAirVolumeDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (msg.whereFull() != where.toStdString())
	{
		return false;
	}

	int what = msg.what();
	if (what != DIM_VAV)
	{
		return false;
	}

	unsigned count = msg.whatArgCnt();

	State state;
	state.temperature_set_point = count > 0 ? QString::fromStdString(msg.whatArg(0)).toInt() : NOTSET_NUM_VALUE;
	state.status = count > 1 ? static_cast<Status>(QString::fromStdString(msg.whatArg(1)).toInt()) : STATUS_NOTSET;
	state.mode = count > 2 ? static_cast<Mode>(QString::fromStdString(msg.whatArg(2)).toInt()) : MODE_NOTSET;
	state.air_flow_set_point = count > 3 ? QString::fromStdString(msg.whatArg(3)).toInt() : NOTSET_NUM_VALUE;
	state.temperature = count > 4 ? QString::fromStdString(msg.whatArg(4)).toInt() : NOTSET_NUM_VALUE;
	state.hasFault = count > 5 ? (bool)QString::fromStdString(msg.whatArg(5)).toInt() : false;
	state.faultDescription = count > 6 ? QString::fromStdString(msg.whatArg(6)).toInt() : NOTSET_NUM_VALUE;

	QVariant var;
	var.setValue(state);
	values_list[DIM_VAV] = var;

	return true;
}


VariableAirVolumeDevice::State::State() :
	temperature_set_point(NOTSET_NUM_VALUE),
	status(STATUS_NOTSET),
	mode(MODE_NOTSET),
	air_flow_set_point(NOTSET_NUM_VALUE),
	temperature(NOTSET_NUM_VALUE),
	hasFault(false),
	faultDescription(NOTSET_NUM_VALUE)
{
}

VariableAirVolumeDevice::State::State(
		int temperature_set_point,
		Status status,
		Mode mode,
		int air_flow_set_point) :
	temperature_set_point(temperature_set_point),
	status(status),
	mode(mode),
	air_flow_set_point(air_flow_set_point),
	temperature(NOTSET_NUM_VALUE),
	hasFault(false),
	faultDescription(NOTSET_NUM_VALUE)
{
}

bool VariableAirVolumeDevice::State::operator==(const State &other) const
{
	if (temperature_set_point != other.temperature_set_point ||
		status != other.status ||
		mode != other.mode ||
		air_flow_set_point != other.air_flow_set_point ||
		temperature != other.temperature ||
		hasFault != other.hasFault ||
		faultDescription != other.faultDescription)
	{
		return false;
	}

	return true;
}

bool VariableAirVolumeDevice::State::operator!=(const State &other) const
{
	return !(*this == other);
}

/***********************************************************************/
UnderfloorHeatingDevice::UnderfloorHeatingDevice(QString where, int openserver_id) :
	device("4", where, openserver_id)
{
}

void UnderfloorHeatingDevice::init()
{
	requestStatus();
}

void UnderfloorHeatingDevice::turnOff() const
{
	setStatus(NOTSET_NUM_VALUE, STATUS_INACTIVE, MODE_NOTSET);
}

void UnderfloorHeatingDevice::turnOn() const
{
	setStatus(NOTSET_NUM_VALUE, STATUS_ACTIVE, MODE_NOTSET);
}

void UnderfloorHeatingDevice::requestStatus() const
{
	sendRequest(DIM_UH);
}

QString UnderfloorHeatingDevice::statusToWhat(const State &state) const
{
	QString what;

	QString temp_set_point = state.temperature_set_point == NOTSET_NUM_VALUE ? QString() : QString::number(state.temperature_set_point);
	QString status = state.status == STATUS_NOTSET ? QString() : QString::number((int)state.status);
	QString mode = state.mode == MODE_NOTSET ? QString() : QString::number((int)state.mode);

	what = QString("%1*%2*%3*%4")
		   .arg(DIM_UH)
		   .arg(temp_set_point)
		   .arg(status)
		   .arg(mode);

	return what;
}

void UnderfloorHeatingDevice::setStatus(int temperature_set_point, Status status, Mode mode) const
{
	State state(temperature_set_point, status, mode);
	setStatus(state);
}

void UnderfloorHeatingDevice::setStatus(const State &state) const
{
	sendCompressedFrame(createWriteDimensionFrame(who, statusToWhat(state), where));
}

bool UnderfloorHeatingDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (msg.whereFull() != where.toStdString())
	{
		return false;
	}

	int what = msg.what();
	if (what != DIM_UH)
	{
		return false;
	}

	unsigned count = msg.whatArgCnt();

	State state;
	state.temperature_set_point = count > 0 ? QString::fromStdString(msg.whatArg(0)).toInt() : NOTSET_NUM_VALUE;
	state.status = count > 1 ? static_cast<Status>(QString::fromStdString(msg.whatArg(1)).toInt()) : STATUS_NOTSET;
	state.mode = count > 2 ? static_cast<Mode>(QString::fromStdString(msg.whatArg(2)).toInt()) : MODE_NOTSET;
	state.temperature = count > 3 ? QString::fromStdString(msg.whatArg(3)).toInt() : NOTSET_NUM_VALUE;
	state.hasFault = count > 4 ? (bool)QString::fromStdString(msg.whatArg(4)).toInt() : false;
	state.faultDescription = count > 5 ? QString::fromStdString(msg.whatArg(5)).toInt() : NOTSET_NUM_VALUE;

	QVariant var;
	var.setValue(state);
	values_list[DIM_UH] = var;

	return true;
}


UnderfloorHeatingDevice::State::State() :
	temperature_set_point(NOTSET_NUM_VALUE),
	status(STATUS_NOTSET),
	mode(MODE_NOTSET),
	temperature(NOTSET_NUM_VALUE),
	hasFault(false),
	faultDescription(NOTSET_NUM_VALUE)
{
}

UnderfloorHeatingDevice::State::State(
		int temperature_set_point,
		Status status,
		Mode mode) :
	temperature_set_point(temperature_set_point),
	status(status),
	mode(mode),
	temperature(NOTSET_NUM_VALUE),
	hasFault(false),
	faultDescription(NOTSET_NUM_VALUE)
{
}

bool UnderfloorHeatingDevice::State::operator==(const State &other) const
{
	if (temperature_set_point != other.temperature_set_point ||
		status != other.status ||
		mode != other.mode ||
		temperature != other.temperature ||
		hasFault != other.hasFault ||
		faultDescription != other.faultDescription)
	{
		return false;
	}

	return true;
}

bool UnderfloorHeatingDevice::State::operator!=(const State &other) const
{
	return !(*this == other);
}
