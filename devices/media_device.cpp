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
};


SourceDevice::SourceDevice(QString source, int openserver_id) :
	device(QString("22"), "2#" + source, openserver_id)
{
	mmtype = 4;
	source_id = source;
}

void SourceDevice::nextTrack() const
{
	sendCommand(REQ_NEXT_TRACK);
}

void SourceDevice::prevTrack() const
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

bool SourceDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	QString msg_where = QString::fromStdString(msg.whereFull());
	if (msg_where != where && msg_where != QString("5#%1").arg(where))
		return false;

	int what = msg.what();

	if (what == SOURCE_TURNED_ON)
	{
		active_areas.insert(QString::fromStdString(msg.whatArg(1)));
		values_list[DIM_AREAS_UPDATED] = QVariant();

		return true; // the frame is managed even if we aren't interested at the values list.
	}

	if (!isDimensionFrame(msg))
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
	QString msg_where = QString::fromStdString(msg.whereFull());
	if (msg_where != where && msg_where != QString("5#%1").arg(where))
		return false;

	if (SourceDevice::parseFrame(msg, values_list))
		return true;

	if (isCommandFrame(msg) && static_cast<int>(msg.what()) == STOP_RDS)
	{
		requestRDS();
		return true;
	}

	if (!isDimensionFrame(msg))
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

bool VirtualSourceDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	QString msg_where = QString::fromStdString(msg.whereFull());
	if (msg_where != where && msg_where != QString("5#%1").arg(where))
		return false;

	int what = msg.what();


	if (SourceDevice::parseFrame(msg, values_list) && what != DIM_STATUS && what != SOURCE_TURNED_ON)
		return true;

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

AmplifierDevice::AmplifierDevice(QString _area, QString _point, int openserver_id) :
	device(QString("22"), "3#" + _area + "#" + _point, openserver_id)
{
	area = _area;
	point = _point;
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

void AmplifierDevice::turnOn() const
{
	sendCommand(QString("%1#4#%2").arg(AMPL_STATUS_ON_FOLLOW_ME).arg(area));
}

void AmplifierDevice::turnOff() const
{
	sendCommand(QString("%1#4#%2").arg(AMPL_STATUS_OFF).arg(area));
}

void AmplifierDevice::volumeUp() const
{
	sendCommand(QString("%1#1").arg(AMPL_VOLUME_UP));
}

void AmplifierDevice::volumeDown() const
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
}

void VirtualAmplifierDevice::updateVolume(int vol)
{
	sendFrame(createDimensionFrame(who, QString("1*%1").arg(vol), where));
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

	if (isWriteDimensionFrame(msg) && what == REQ_SET_VOLUME)
	{
		values_list[REQ_SET_VOLUME] = msg.whatArgN(0);
		return true;
	}

	switch (what)
	{
	case REQ_AMPLI_ON:
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
	AmplifierDevice(address.at(0), address.at(1), openserver_id)
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

