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

#include "media_device.h"
#include "generic_functions.h"

#include <openmsg.h>


enum RequestDimension
{
	// Sources
	REQ_FREQUENCE_UP = 5,
	REQ_FREQUENCE_DOWN = 6,
	REQ_SOURCE_ON = 35,
	REQ_SAVE_STATION = 33,
	START_RDS = 31,
	STOP_RDS = 32,
	// Amplifiers
	AMPL_STATUS_ON_FOLLOW_ME = 34,
	AMPL_STATUS_OFF = 0,
	AMPL_VOLUME_UP = 3,
	AMPL_VOLUME_DOWN = 4,
	REQ_TREBLE_UP = 40,
	REQ_TREBLE_DOWN = 41,
	REQ_BASS_UP = 36,
	REQ_BASS_DOWN = 37,
	REQ_BALANCE_UP = 42,
	REQ_BALANCE_DOWN = 43,
	REQ_NEXT_PRESET = 55,
	REQ_PREV_PRESET = 56,
	REQ_LOUD = 20,
	SOURCE_TURNED_ON = 2,
	DIM_ACTIVE_AREAS = 13,
	DIM_MEMORIZED_STATION = 11,
	// the dimension in the frame is 1, but we need a separate value for the notification
	_REQ_AMPLI_ON = 1,
	_REQ_SET_VOLUME = 1,
};


SourceDevice::SourceDevice(QString source, int openserver_id) :
	device(QString("22"), "2#" + source, openserver_id)
{
	mmtype = 4;
	source_id = source;
}

void SourceDevice::init()
{
	requestActiveAreas();
}

void SourceDevice::nextTrack()
{
	sendCommand(REQ_NEXT_TRACK);
}

void SourceDevice::prevTrack()
{
	sendCommand(REQ_PREV_TRACK);
}

bool SourceDevice::isActive(QString area) const
{
	return active_areas.contains(area);
}

void SourceDevice::turnOn(QString area) const
{
	QString what = QString("%1#%2#%3#%4").arg(REQ_SOURCE_ON).arg(mmtype).arg(area).arg(source_id);
	QString where = QString("3#%1#0").arg(area);
	sendCommand(what, where);
}

void SourceDevice::requestTrack() const
{
	sendRequest(DIM_TRACK);
}

void SourceDevice::requestActiveAreas() const
{
	sendRequest(DIM_ACTIVE_AREAS);
}

bool SourceDevice::parseFrameOtherDevices(OpenMsg &msg, DeviceValues &values_list)
{
	if (isStatusRequestFrame(msg))
		return false;
	// skip frames destined to amplifiers
	if (msg.where() == 3)
		return false;
	int what = msg.what();

	// we need to handle some frames destined to other devices to correctly
	// update our status, because we do not receive a status update when
	// the device is turned off on an area but is still on on others
	if (what == SOURCE_TURNED_ON)
	{
		QString area = QString::fromStdString(msg.whatArg(1));

		if (active_areas.contains(area))
		{
			active_areas.remove(area);
			values_list[DIM_AREAS_UPDATED] = QVariant();

			return true;
		}
	}
	else if (what == DIM_ACTIVE_AREAS && isDimensionFrame(msg))
	{
		for (unsigned int i = 0; i < msg.whatArgCnt(); ++i)
		{
			QString area = QString::number(i);

			if (msg.whatArgN(i) == 1 && active_areas.contains(area))
			{
				active_areas.remove(area);
				values_list[DIM_AREAS_UPDATED] = QVariant();

				return true;
			}
		}
	}

	return false;
}

bool SourceDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (isStatusRequestFrame(msg))
		return false;
	QString msg_where = QString::fromStdString(msg.whereFull());

	if (msg_where != where && msg_where != QString("5#%1").arg(where))
		return parseFrameOtherDevices(msg, values_list);

	int what = msg.what();

	if (what == SOURCE_TURNED_ON)
	{
		active_areas.insert(QString::fromStdString(msg.whatArg(1)));
		values_list[DIM_AREAS_UPDATED] = QVariant();

		return true; // the frame is managed even if we aren't interested at the values list.
	}

	if (!isDimensionFrame(msg) || msg.whatArgCnt() == 0)
		return false;

	QVariant v;

	switch (what)
	{
	case DIM_STATUS:
		v.setValue(msg.whatArgN(0) == 1);
		if (msg.whatArgN(0) == 0)
		{
			active_areas.clear();
			values_list[DIM_AREAS_UPDATED] = QVariant();
		}
		break;
	case DIM_TRACK:
		v.setValue(msg.whatArgN(0));
		break;
	case DIM_ACTIVE_AREAS:
		active_areas.clear();
		for (unsigned int i = 0; i < msg.whatArgCnt(); ++i)
			if (msg.whatArgN(i) == 1)
				active_areas.insert(QString::number(i));
		values_list[DIM_AREAS_UPDATED] = QVariant();
		return true;
	default:
		return false;
	}

	values_list[what] = v;
	return true;
}


RadioSourceDevice::RadioSourceDevice(QString source_id, int openserver_id) :
	SourceDevice(source_id, openserver_id)
{
}

void RadioSourceDevice::init()
{
	SourceDevice::init();

	requestTrack();
	requestFrequency();
	requestRDS();
}

void RadioSourceDevice::frequenceUp(QString value) const
{
	sendCommand(QString("%1#%2").arg(REQ_FREQUENCE_UP).arg(value));
}

void RadioSourceDevice::frequenceDown(QString value) const
{
	sendCommand(QString("%1#%2").arg(REQ_FREQUENCE_DOWN).arg(value));
}

void RadioSourceDevice::saveStation(QString station) const
{
	sendCommand(QString("%1#%2").arg(REQ_SAVE_STATION).arg(station));
}

void RadioSourceDevice::setStation(QString station) const
{
	sendFrame(createWriteDimensionFrame(who, QString("%1*%2").arg(DIM_TRACK).arg(station), where));
}

void RadioSourceDevice::requestFrequency() const
{
	sendRequest(DIM_FREQUENCY);
}

void RadioSourceDevice::requestRDS() const
{
	sendCommand(START_RDS);
}

bool RadioSourceDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (SourceDevice::parseFrame(msg, values_list))
		return true;

	QString msg_where = QString::fromStdString(msg.whereFull());
	if (msg_where != where && msg_where != QString("5#%1").arg(where))
		return false;

	if (isCommandFrame(msg) && static_cast<int>(msg.what()) == STOP_RDS)
	{
		requestRDS();
		return true;
	}

	// check for status requests for a single dimension
	if (!isDimensionFrame(msg) || msg.whatArgCnt() == 0)
		return false;

	QVariant v;

	int what = msg.what();
	switch (what)
	{
	case DIM_FREQUENCY:
		v.setValue(msg.whatArgN(1));
		break;
	case DIM_MEMORIZED_STATION:
		values_list[DIM_FREQUENCY] = msg.whatArgN(1);
		values_list[DIM_TRACK] = msg.whatArgN(2);
		return true;
	case DIM_RDS:
	{
		QString rds_message;
		for (unsigned int i = 0; i < msg.whatArgCnt(); ++i)
			rds_message.append(QChar(msg.whatArgN(i)));
		v.setValue(rds_message);
		break;
	}
	default:
		return false;
	}

	values_list[what] = v;
	return true;
}


VirtualSourceDevice::VirtualSourceDevice(QString address, int openserver_id) :
	SourceDevice(address, openserver_id)
{

}

void VirtualSourceDevice::nextTrack()
{
	DeviceValues values_list;

	values_list[REQ_NEXT_TRACK] = true;
	emit valueReceived(values_list);
}

void VirtualSourceDevice::prevTrack()
{
	DeviceValues values_list;

	values_list[REQ_PREV_TRACK] = true;
	emit valueReceived(values_list);
}

bool VirtualSourceDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (isStatusRequestFrame(msg))
		return false;
	int what = msg.what();

	if (SourceDevice::parseFrame(msg, values_list) && what != DIM_STATUS && what != SOURCE_TURNED_ON)
		return true;

	QString msg_where = QString::fromStdString(msg.whereFull());
	if (msg_where != where && msg_where != QString("5#%1").arg(where))
		return false;

	if (isDimensionFrame(msg) && what == DIM_STATUS && msg.whatArgN(0) == 0)
	{
		values_list[REQ_SOURCE_OFF] = true;
		return true;
	}

	if (!isCommandFrame(msg))
		return false;

	switch (what)
	{
	case REQ_NEXT_TRACK:
	case REQ_PREV_TRACK:
		values_list[what] = true;
		break;
	case SOURCE_TURNED_ON:
		values_list[REQ_SOURCE_ON] = QString::fromStdString(msg.whatArg(1));
		break;
	default:
		return false;
	}

	return true;
}

QString VirtualSourceDevice::createMediaInitFrame(bool is_multichannel, const QString &source_addr, const QString &ampli_addr)
{
	QString matrix_input = is_multichannel ? source_addr : "";
	int is_source = !source_addr.isEmpty(), is_ampli = !ampli_addr.isEmpty(), is_gateway = 1;
	int reads_scs = is_source || is_ampli;

	QString ampli_area, ampli_point;
	if (is_ampli)
	{
		ampli_area = ampli_addr.at(0);
		ampli_point = ampli_addr.at(1);
	}

	QString frame = QString("*#22*7*#15*%1*%2*%3*9*9**%4*%5*%6*%7*%8##")
			.arg(source_addr).arg(ampli_area).arg(ampli_point).arg(matrix_input).arg(is_source).arg(is_gateway).arg(is_ampli).arg(reads_scs);

	return frame;
}



AmplifierDevice::AmplifierDevice(QString where, int openserver_id) :
	device(QString("22"), where == "0" ? "5#3#0#0" : QString("3#") + where.at(0) + "#" + where.at(1), openserver_id)
{
	if (where != "0")
	{
		area = where.at(0);
		point = where.at(1);
	}
	else
		area = point = '0';
}

void AmplifierDevice::init()
{
	requestStatus();
	requestVolume();
}

void AmplifierDevice::requestStatus() const
{
	sendRequest(DIM_STATUS);
}

void AmplifierDevice::requestVolume() const
{
	sendRequest(DIM_VOLUME);
}

void AmplifierDevice::turnOn()
{
	sendCommand(QString("%1#4#%2").arg(AMPL_STATUS_ON_FOLLOW_ME).arg(area));
}

void AmplifierDevice::turnOff()
{
	sendCommand(QString("%1#4#%2").arg(AMPL_STATUS_OFF).arg(area));
}

void AmplifierDevice::volumeUp()
{
	sendCommand(QString("%1#1").arg(AMPL_VOLUME_UP));
}

void AmplifierDevice::volumeDown()
{
	sendCommand(QString("%1#1").arg(AMPL_VOLUME_DOWN));
}

bool AmplifierDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (where != QString::fromStdString(msg.whereFull()))
		return false;

	if (!msg.whatArgCnt() || !isDimensionFrame(msg))
		return false;

	int what = msg.what();

	switch (what)
	{
	case DIM_STATUS:
		values_list[what] = msg.whatArgN(0) == 1;
		break;
	case DIM_VOLUME:
		values_list[what] = msg.whatArgN(0);
		break;
	default:
		return false;
	}
	return true;
}



VirtualAmplifierDevice::VirtualAmplifierDevice(const QString &where, int openserver_id) :
	AmplifierDevice(where, openserver_id)
{
	status = false;
	volume = 0;
}

void VirtualAmplifierDevice::init()
{
	updateStatus(status);
	updateVolume(volume);
}

void VirtualAmplifierDevice::updateStatus(bool _status)
{
	status = _status;
	sendFrame(createDimensionFrame(who, QString("12*%1*3").arg(status), "5#" + where));

	DeviceValues values_list;

	values_list[DIM_STATUS] = status;
	emit valueReceived(values_list);
}

void VirtualAmplifierDevice::updateVolume(int _volume)
{
	volume = _volume;
	sendFrame(createDimensionFrame(who, QString("1*%1").arg(volume), "5#" + where));

	DeviceValues values_list;

	values_list[DIM_VOLUME] = volume;
	emit valueReceived(values_list);
}

void VirtualAmplifierDevice::turnOn()
{
	DeviceValues values_list;

	values_list[REQ_AMPLI_ON] = true;
	emit valueReceived(values_list);
}

void VirtualAmplifierDevice::turnOff()
{
	DeviceValues values_list;

	values_list[REQ_AMPLI_ON] = false;
	emit valueReceived(values_list);
}

void VirtualAmplifierDevice::volumeUp()
{
	DeviceValues values_list;

	values_list[REQ_VOLUME_UP] = 1;
	emit valueReceived(values_list);
}

void VirtualAmplifierDevice::volumeDown()
{
	DeviceValues values_list;

	values_list[REQ_VOLUME_DOWN] = 1;
	emit valueReceived(values_list);
}

bool VirtualAmplifierDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (AmplifierDevice::parseFrame(msg, values_list))
		return true;

	if (isDimensionFrame(msg) || isStatusRequestFrame(msg))
		return false;

	QString msg_where = QString::fromStdString(msg.whereFull());
	if (msg_where != where)
		return false;

	int what = msg.what();

	if (isWriteDimensionFrame(msg) && what == _REQ_SET_VOLUME)
	{
		values_list[REQ_SET_VOLUME] = msg.whatArgN(0);
		return true;
	}

	switch (what)
	{
	case _REQ_AMPLI_ON:
	case AMPL_STATUS_OFF:
		values_list[REQ_AMPLI_ON] = static_cast<bool>(what);
		break;
	case REQ_VOLUME_DOWN:
	case REQ_VOLUME_UP:
		// value is missing, step is 1
		if (msg.whatArgCnt() == 0)
			values_list[what] = 1;
		else
			values_list[what] = msg.whatArgN(0);
		break;
	default:
		return false;
	}

	return true;
}


PowerAmplifierDevice::PowerAmplifierDevice(QString address, int openserver_id) :
	AmplifierDevice(address, openserver_id)
{
}

void PowerAmplifierDevice::init()
{
	requestStatus();
	requestVolume();
	requestPreset();
	requestTreble();
	requestBass();
	requestBalance();
	requestLoud();
}

bool PowerAmplifierDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (where != QString::fromStdString(msg.whereFull()))
		return false;

	// In some cases (when more than a power amplifier is present in the system)
	// a request frame can arrive from the monitor socket. We have to manage this
	// situation.
	if (!msg.whatArgCnt() || !isDimensionFrame(msg))
		return false;

	if (AmplifierDevice::parseFrame(msg, values_list))
		return true;

	int what = msg.what();

	QVariant v;

	switch (what)
	{
	case DIM_LOUD:
		v.setValue(msg.whatArgN(0) == 1);
		break;

	case DIM_TREBLE:
	case DIM_BASS:
		v.setValue(msg.whatArgN(0) / 3 - 10);
		break;
	case DIM_BALANCE:
	{
		QString raw_value = QString::fromStdString(msg.whatArg(0));
		if (raw_value.length() >= 2)
		{
			bool balance_left = raw_value[0] == '0';
			int value = raw_value.mid(1).toInt() / 3;
			v.setValue((balance_left ? -1 : 1) * value);
		}
		break;
	}
	case DIM_PRESET:
	{
		int value = -1;
		int raw_value = msg.whatArgN(0);
		if (raw_value >= 2 && raw_value <= 11) // fixed presets
			value = raw_value -2;
		else if (raw_value >= 16 && raw_value <= 25) // custom presets
			value = raw_value -6;

		if (value == -1)
			return false;
		else
			v.setValue(value);
		break;
	}
	default:
		return false;
	}

	values_list[what] = v;
	return true;
}

void PowerAmplifierDevice::requestTreble() const
{
	sendRequest(DIM_TREBLE);
}

void PowerAmplifierDevice::requestBass() const
{
	sendRequest(DIM_BASS);
}

void PowerAmplifierDevice::requestBalance() const
{
	sendRequest(DIM_BALANCE);
}

void PowerAmplifierDevice::requestPreset() const
{
	sendRequest(DIM_PRESET);
}

void PowerAmplifierDevice::requestLoud() const
{
	sendRequest(DIM_LOUD);
}

void PowerAmplifierDevice::trebleUp() const
{
	sendCommand(QString("%1#1").arg(REQ_TREBLE_UP));
}

void PowerAmplifierDevice::trebleDown() const
{
	sendCommand(QString("%1#1").arg(REQ_TREBLE_DOWN));
}

void PowerAmplifierDevice::bassUp() const
{
	sendCommand(QString("%1#1").arg(REQ_BASS_UP));
}

void PowerAmplifierDevice::bassDown() const
{
	sendCommand(QString("%1#1").arg(REQ_BASS_DOWN));
}

void PowerAmplifierDevice::balanceUp() const
{
	sendCommand(QString("%1#1").arg(REQ_BALANCE_UP));
}

void PowerAmplifierDevice::balanceDown() const
{
	sendCommand(QString("%1#1").arg(REQ_BALANCE_DOWN));
}

void PowerAmplifierDevice::nextPreset() const
{
	sendCommand(REQ_NEXT_PRESET);
}

void PowerAmplifierDevice::prevPreset() const
{
	sendCommand(REQ_PREV_PRESET);
}

void PowerAmplifierDevice::loudOn() const
{
	sendFrame(createWriteDimensionFrame(who, QString("%1*1").arg(REQ_LOUD), where));
}

void PowerAmplifierDevice::loudOff() const
{
	sendFrame(createWriteDimensionFrame(who, QString("%1*0").arg(REQ_LOUD), where));
}

