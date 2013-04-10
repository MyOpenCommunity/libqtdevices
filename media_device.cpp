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
#include "frame_functions.h" // isStatusRequestFrame, isDimensionFrame, createWriteDimensionFrame
#include "devices_cache.h" // bt_global::add_device_to_cache

#include <openmsg.h>

#include <QHashIterator>
#include <QDebug>
#include <QTimer>


#define RADIO_MIN_FREQ  8750
#define RADIO_MAX_FREQ 10800

const int STOPPING_TIMEOUT = 100;

#define ARRAY_SIZE(x) int(sizeof(x)/sizeof(x[0]))


enum RequestDimension
{
	// Sources
	REQ_FREQUENCE_UP = 5,
	REQ_FREQUENCE_DOWN = 6,
	REQ_SLIDE = 22,
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
	// Aux
	CMD_TURN_ON = 1,
	CMD_TURN_OFF = 0
};


// Inizialization of static member
QHash<int, SourceDevice*> AlarmSoundDiffDevice::sources;
QHash<int, AmplifierDevice*> AlarmSoundDiffDevice::amplifiers;
AlarmSoundDiffDevice *AlarmSoundDiffDevice::alarm_device = 0;
QString AmplifierDevice::virtual_amplifier_where;
bool AmplifierDevice::is_multichannel = false;
bool SourceDevice::is_multichannel = false;

AlarmSoundDiffDevice::AlarmSoundDiffDevice()
	: device("22", "")
{
	receive_frames = false;

	// NOTE: this works only because the device_cache destroys all the AlarmSoundDiffDevice
	// except the first (because they have all the same key for the cache)
	if (!alarm_device)
		alarm_device = this;

	foreach (SourceDevice *source, sources)
		connect(source, SIGNAL(valueReceived(DeviceValues)), SLOT(sourceValueReceived(DeviceValues)));

	foreach (AmplifierDevice *amplifier, amplifiers)
		connect(amplifier, SIGNAL(valueReceived(DeviceValues)), SLOT(amplifierValueReceived(DeviceValues)));
}

void AlarmSoundDiffDevice::addSource(SourceDevice *dev, int source_id)
{
	// NOTE: required by the device_cache.
	// To mantain a clean code we want that sources or amplifiers add
	// themselves to the list in their constructor using the addSource/addAmplifier
	// methods. However, the device_cache can delete the newly created device
	// if another object with the same key is found on the cache. So, we
	// have to mimic the behavior of the device_cache adding a source or an
	// amplifier to the list only if it is not already present.
	// We use integers for address and source_id to avoid problems with leading zeros.
	if (!sources.contains(source_id))
		sources[source_id] = dev;

	if (alarm_device)
		QObject::connect(dev, SIGNAL(valueReceived(DeviceValues)), alarm_device, SLOT(sourceValueReceived(DeviceValues)));
}

void AlarmSoundDiffDevice::addAmplifier(AmplifierDevice *dev, int address)
{
	// NOTE: required by the device_cache (see above).
	if (!amplifiers.contains(address))
		amplifiers[address] = dev;

	if (alarm_device)
		QObject::connect(dev, SIGNAL(valueReceived(DeviceValues)), alarm_device, SLOT(amplifierValueReceived(DeviceValues)));
}

void AlarmSoundDiffDevice::setReceiveFrames(bool receive)
{
	receive_frames = receive;
}

void AlarmSoundDiffDevice::setActiveArea(QString active_area)
{
	area = active_area;

	DeviceValues alarm_values_list;
	updateActiveSource(alarm_values_list);

	if (alarm_values_list.count() > 0)
		emit valueReceived(alarm_values_list);
}

void AlarmSoundDiffDevice::requestStation(int source)
{
	// If the source is not a radio, we skip the frame
	if (RadioSourceDevice *dev = qobject_cast<RadioSourceDevice*>(sources[source]))
		dev->requestTrack();
}

bool AlarmSoundDiffDevice::isValid(int source, int radio_station, int *alarmVolumes)
{
	Q_UNUSED(radio_station);

	if (!sources[source])
	{
		qDebug() << "Invalid alarm clock source" << source;

		return false;
	}

	for (int amplifier = 0; amplifier < AMPLI_NUM; ++amplifier)
	{
		if (alarmVolumes[amplifier] < 0)
			continue;

		if (!amplifiers[amplifier])
		{
			qDebug() << "Invalid alarm clock amplifier" << amplifier;

			return false;
		}
	}

	return true;
}

void AlarmSoundDiffDevice::startAlarm(bool is_multichannel, int source, int radio_station, int *alarmVolumes)
{
	bool areas[AMPLI_NUM / 10 + 1];
	for (int i = 0; i < ARRAY_SIZE(areas); ++i)
		areas[i] = false;

	SourceDevice *source_device = sources[source];
	Q_ASSERT_X(source_device != 0, "AlarmSoundDiffDevice::startAlarm", "Invalid source");
	source_device->turnOn("0");
	if (radio_station)
	{
		RadioSourceDevice *radio_source = qobject_cast<RadioSourceDevice*>(source_device);
		if (radio_source)
			radio_source->setStation(QString::number(radio_station));
		else
			qWarning() << QString("AlarmSoundDiffDevice::startAlarm: the source with address %1 is not a radio source!").arg(source);
	}

	for (int amplifier = 0; amplifier < AMPLI_NUM; ++amplifier)
	{
		if (alarmVolumes[amplifier] < 0)
			continue;

		int area = amplifier / 10;
		if (is_multichannel && area > 0 && area < 9)
		{
			if (!areas[area])
			{
				source_device->turnOn(QString::number(area));
				areas[area] = true;
			}
		}
		AmplifierDevice *ampli_device = amplifiers[amplifier];
		Q_ASSERT_X(ampli_device != 0, "AlarmSoundDiffDevice::startAlarm", "Invalid amplifier");
		int vol = alarmVolumes[amplifier] < 10 ? alarmVolumes[amplifier] : 8;

		ampli_device->setVolume(vol);
		ampli_device->turnOn();
	}
}

void AlarmSoundDiffDevice::stopAlarm(int source, int *alarmVolumes)
{
	Q_UNUSED(source);
	for (int amplifier = 0; amplifier < AMPLI_NUM; ++amplifier)
	{
		if (alarmVolumes[amplifier] < 0)
			continue;

		AmplifierDevice *ampli_device = amplifiers[amplifier];
		Q_ASSERT_X(ampli_device != 0, "AlarmSoundDiffDevice::startAlarm", "Invalid amplifier");
		ampli_device->turnOff();
	}
}

void AlarmSoundDiffDevice::setVolume(int amplifier, int volume)
{
	AmplifierDevice *ampli_device = amplifiers[amplifier];
	Q_ASSERT_X(ampli_device != 0, "AlarmSoundDiffDevice::startAlarm", "Invalid amplifier");
	ampli_device->setVolume(volume);
}

void AlarmSoundDiffDevice::amplifierValueReceived(const DeviceValues &values_list)
{
	if (!receive_frames)
		return;

	DeviceValues alarm_values_list;

	if (values_list.contains(AmplifierDevice::DIM_STATUS))
	{
		// if we got an "on" state, wait for the volume to emit the
		// valueReceived notification
		if (!values_list[AmplifierDevice::DIM_STATUS].toBool())
		{
			AmplifierDevice *amplifier = qobject_cast<AmplifierDevice*>(sender());
			alarm_values_list[DIM_AMPLIFIER] = amplifier->getArea() + amplifier->getPoint();
			alarm_values_list[DIM_STATUS] = false;
		}
	}
	else if (values_list.contains(AmplifierDevice::DIM_VOLUME))
	{
		AmplifierDevice *amplifier = qobject_cast<AmplifierDevice*>(sender());
		int volume = values_list[AmplifierDevice::DIM_VOLUME].toInt();

		alarm_values_list[DIM_AMPLIFIER] = amplifier->getArea() + amplifier->getPoint();
		alarm_values_list[DIM_STATUS] = true;
		alarm_values_list[DIM_VOLUME] = volume;
	}

	if (alarm_values_list.count() > 0)
		emit valueReceived(alarm_values_list);
}

void AlarmSoundDiffDevice::sourceValueReceived(const DeviceValues &values_list)
{
	if (!receive_frames)
		return;

	DeviceValues alarm_values_list;
	if (values_list.contains(SourceDevice::DIM_TRACK))
		alarm_values_list[DIM_RADIO_STATION] = values_list[SourceDevice::DIM_TRACK];
	else if (values_list.contains(SourceDevice::DIM_STATUS))
	{
		if (values_list[SourceDevice::DIM_STATUS].toBool())
		{
			SourceDevice *source = qobject_cast<SourceDevice*>(sender());
			alarm_values_list[DIM_SOURCE] = source->getSourceId();
		}
	}
	else if (values_list.contains(SourceDevice::DIM_AREAS_UPDATED))
		updateActiveSource(alarm_values_list);

	if (alarm_values_list.count() > 0)
		emit valueReceived(alarm_values_list);
}

void AlarmSoundDiffDevice::updateActiveSource(DeviceValues &values_list)
{
	foreach (SourceDevice *source, sources)
	{
		if (source->isActive(area))
		{
			values_list[DIM_SOURCE] = source->getSourceId();
			break;
		}
	}
}


SourceDevice::SourceDevice(QString source, int openserver_id) :
	device(QString("22"), "2#" + source, openserver_id)
{
	mmtype = 4;
	source_id = source;
	AlarmSoundDiffDevice::addSource(this, source.toInt());
}

void SourceDevice::setIsMultichannel(bool multichannel)
{
	is_multichannel = multichannel;
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

bool SourceDevice::isActive() const
{
	return !active_areas.isEmpty();
}

void SourceDevice::sendTurnOn(QString area)
{
	QString what = QString("%1#%2#%3#%4").arg(REQ_SOURCE_ON).arg(mmtype).arg(area).arg(source_id);
	QString where = QString("3#%1#0").arg(area);
	sendCommand(what, where);
}

void SourceDevice::turnOn(QString area)
{
	if (area.isNull())
	{
		for (int i = 1; i <= 8; ++i)
			sendTurnOn(QString::number(i));
	}
	else
		sendTurnOn(area);
}

void SourceDevice::requestTrack() const
{
	sendRequest(DIM_TRACK);
}

void SourceDevice::requestActiveAreas() const
{
	sendRequest(DIM_ACTIVE_AREAS);
}

bool SourceDevice::parseFrameAmplifiers(OpenMsg &msg, DeviceValues &values_list)
{
	// only process general frames
	if (msg.where() != 5)
		return false;
	int what = msg.what();

	if (what == REQ_NEXT_TRACK && msg.whereArgCnt() == 3 && msg.whereArg(0) == "3")
	{
		// monochannel case, we always deliver the next track request
		if (active_areas.size() == 1 && active_areas.contains("0"))
			values_list[what] = true;
		// multichannel case, we only deliver the event if the areas match
		else if (active_areas.contains(QString::fromStdString(msg.whereArg(1))))
			values_list[what] = true;

		return true;
	}

	return false;
}

bool SourceDevice::parseFrameOtherSources(OpenMsg &msg, DeviceValues &values_list)
{
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
			values_list[DIM_AREAS_UPDATED] = true;

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
				values_list[DIM_AREAS_UPDATED] = true;

				return true;
			}
		}
	}

	return false;
}

bool SourceDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	QString msg_where = QString::fromStdString(msg.whereFull());

	if (msg_where != where && msg_where != QString("5#%1").arg(where))
		return parseFrameOtherSources(msg, values_list);

	int what = msg.what();

	if (what == SOURCE_TURNED_ON)
	{
		// As exception, if we are in monochannel mode (where the only area is "0"), we
		// turn the source on even if the area in the frame is not the right one.
		QString area = is_multichannel ? QString::fromStdString(msg.whatArg(1)) : "0";
		active_areas.insert(area);
		values_list[DIM_AREAS_UPDATED] = true;

		return true;
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
			values_list[DIM_AREAS_UPDATED] = true;
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
		values_list[DIM_AREAS_UPDATED] = true;
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
	frequency = -1;
	update_count = 0;
	rds_updates = false;
	stopping_timer = new QTimer(this);
	stopping_timer->setSingleShot(true);
	stopping_timer->setInterval(STOPPING_TIMEOUT);
	connect(stopping_timer, SIGNAL(timeout()), SLOT(stopRDS()));
}

void RadioSourceDevice::init()
{
	SourceDevice::init();

	requestTrack();
	requestFrequency();
}

void RadioSourceDevice::frequenceUp(QString value)
{
	// we do not receive status updates for the manual frequency up command: emulate it
	if (frequency != -1 && !value.isEmpty())
	{
		frequency += value.toInt() * 5;
		if (frequency > RADIO_MAX_FREQ)
			frequency = RADIO_MIN_FREQ;

		DeviceValues values_list;

		values_list[DIM_FREQUENCY] = frequency;
		emit valueReceived(values_list);

		sendCommand(QString("%1#%2").arg(REQ_FREQUENCE_UP).arg(value));
	}
	else if(value.isEmpty())
		sendCommand(QString("%1").arg(REQ_FREQUENCE_UP));
}

void RadioSourceDevice::frequenceDown(QString value)
{
	// we do not receive status updates for the manual frequency downb command: emulate it
	if (frequency != -1 && !value.isEmpty())
	{
		frequency -= value.toInt() * 5;
		if (frequency < RADIO_MIN_FREQ)
			frequency = RADIO_MAX_FREQ;

		DeviceValues values_list;

		values_list[DIM_FREQUENCY] = frequency;
		emit valueReceived(values_list);

		sendCommand(QString("%1#%2").arg(REQ_FREQUENCE_DOWN).arg(value));
	}
	else if(value.isEmpty())
		sendCommand(QString("%1").arg(REQ_FREQUENCE_DOWN));
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

void RadioSourceDevice::requestStartRDS()
{
	if (++update_count == 1)
	{
		rds_updates = true;
		if (stopping_timer->isActive())
			stopping_timer->stop();
		else
			sendCommand(START_RDS);
	}
}

void RadioSourceDevice::requestStopRDS()
{
	if (!rds_updates)
		return;

	if (--update_count == 0)
	{
		rds_updates = false;
		// We use a timer to avoid a couple stop-start frames when the user
		// change the page between two pages that both need to know the RDS updates.
		stopping_timer->start();
	}
}

void RadioSourceDevice::stopRDS() const
{
	sendCommand(STOP_RDS);
}

bool RadioSourceDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (SourceDevice::parseFrame(msg, values_list))
		return true;

	QString msg_where = QString::fromStdString(msg.whereFull());
	if (msg_where != where && msg_where != QString("5#%1").arg(where))
		return false;

	if (isCommandFrame(msg) && static_cast<int>(msg.what()) == STOP_RDS && rds_updates)
		sendCommand(START_RDS);

	// check for status requests for a single dimension
	if (!isDimensionFrame(msg) || msg.whatArgCnt() == 0)
		return false;

	int what = msg.what();
	switch (what)
	{
	case DIM_FREQUENCY:
		values_list[DIM_FREQUENCY] = frequency = msg.whatArgN(1);
		break;
	case DIM_MEMORIZED_STATION:
		values_list[DIM_FREQUENCY] = frequency = msg.whatArgN(1);
		values_list[DIM_TRACK] = msg.whatArgN(2);
		return true;
	case DIM_RDS:
	{
		QString rds_message;
		for (unsigned int i = 0; i < msg.whatArgCnt(); ++i)
			rds_message.append(QChar(msg.whatArgN(i)));
		values_list[DIM_RDS] = rds_message;
		break;
	}
	default:
		return false;
	}

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
	values_list[DIM_SELF_REQUEST] = true;
	emit valueReceived(values_list);
}

void VirtualSourceDevice::prevTrack()
{
	DeviceValues values_list;

	values_list[REQ_PREV_TRACK] = true;
	values_list[DIM_SELF_REQUEST] = true;
	emit valueReceived(values_list);
}

void VirtualSourceDevice::turnOn(QString area)
{
	SourceDevice::turnOn(area);

	DeviceValues values_list;

	active_areas.insert(area);
	values_list[DIM_AREAS_UPDATED] = true;
	values_list[DIM_SELF_REQUEST] = true;
	values_list[REQ_SOURCE_ON] = area;
	emit valueReceived(values_list);
}

bool VirtualSourceDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	int what = msg.what();

	if (SourceDevice::parseFrame(msg, values_list) && what != DIM_STATUS && what != SOURCE_TURNED_ON)
		return true;
	// parse general frames where the source is an amplifier
	if (msg.where() == 5 && parseFrameAmplifiers(msg, values_list))
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
	case REQ_SOURCE_ON:
		values_list[what] = QString::fromStdString(msg.whatArg(1));
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

	QString source = source_addr.isEmpty() ? "0" : source_addr;
	QString frame = QString("*#22*7*#15*%1*%2*%3*9*9**%4*%5*%6*%7*%8##")
		.arg(source).arg(ampli_area).arg(ampli_point).arg(matrix_input).arg(is_source).arg(is_gateway).arg(is_ampli).arg(reads_scs);

	return frame;
}



void AmplifierDevice::setVirtualAmplifierWhere(const QString &where)
{
	virtual_amplifier_where = where;
}

void AmplifierDevice::setIsMultichannel(bool multichannel)
{
	is_multichannel = multichannel;
}

bool AmplifierDevice::isGeneralAddress(const QString &where)
{
	return where == "0";
}

bool AmplifierDevice::isAreaAddress(const QString &where)
{
	return where.size() == 2 && where.at(0) == '#';
}

QString AmplifierDevice::getAmplifierArea(const QString &where)
{
	return where.at(0) == '#' ? where.at(1) : where.at(0);
}

namespace
{
	QString getAmplifierPoint(const QString &where)
	{
		return where.length() == 2 && where.at(0) != '#' ? where.at(1) : '0';
	}

	QString createAmplifierWhere(const QString &where_conf)
	{
		if (where_conf == "0")
			return "5#3#0#0";
		if (AmplifierDevice::isAreaAddress(where_conf))
			return QString("4#%1").arg(AmplifierDevice::getAmplifierArea(where_conf));
		return QString("3#") + where_conf.at(0) + "#" + where_conf.at(1);
	}
}

AmplifierDevice *AmplifierDevice::createVirtualDevice()
{
	return bt_global::add_device_to_cache(new VirtualAmplifierDevice(virtual_amplifier_where));
}

AmplifierDevice *AmplifierDevice::createDevice(const QString &where, int openserver_id)
{
	if (!virtual_amplifier_where.isEmpty())
	{
		if (virtual_amplifier_where == where)
			return createVirtualDevice();
		else if (AmplifierDevice::isGeneralAddress(where) ||
			 (AmplifierDevice::isAreaAddress(where) &&
			  AmplifierDevice::getAmplifierArea(where) == AmplifierDevice::getAmplifierArea(virtual_amplifier_where)))
		{
			AmplifierDevice *v = createVirtualDevice();
			AmplifierDevice *n = bt_global::add_device_to_cache(new AmplifierDevice(where, openserver_id));

			return bt_global::add_device_to_cache(new CompositeAmplifierDevice(QList<AmplifierDevice*>() << n << v));
		}

	}

	return bt_global::add_device_to_cache(new AmplifierDevice(where, openserver_id));
}

AmplifierDevice::AmplifierDevice(QString _where, int openserver_id) :
	device(QString("22"), createAmplifierWhere(_where), openserver_id)
{
	area = getAmplifierArea(_where);
	point = getAmplifierPoint(_where);

	AlarmSoundDiffDevice::addAmplifier(this, _where.toInt());
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

void AmplifierDevice::setVolume(int volume)
{
	sendFrame(createWriteDimensionFrame(who, QString("%1*%2").arg(DIM_VOLUME).arg(volume), where));
}

bool AmplifierDevice::checkAddressIsForMe(OpenMsg &msg)
{
	if (msg.where() == 3)
	{
		// point to point
		if (msg.whereArgCnt() == 2 &&
		    msg.whereArg(0) == area.toStdString() &&
		    msg.whereArg(1) == point.toStdString())
			return true;
	}
	else if (msg.where() == 4)
	{
		// area
		if (msg.whereArgCnt() == 1 &&
		    msg.whereArg(0) == area.toStdString())
			return true;
	}
	else if (msg.where() == 5)
	{
		// general
		if (msg.whereArgCnt() == 3 &&
		    msg.whereArg(0) == "3" &&
		    msg.whereArg(1) == "0" &&
		    msg.whereArg(2) == "0")
			return true;
	}

	return false;
}

bool AmplifierDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (!checkAddressIsForMe(msg))
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

	values_list[DIM_SELF_REQUEST] = true;
	values_list[DIM_STATUS] = status;
	emit valueReceived(values_list);
}

void VirtualAmplifierDevice::updateVolume(int _volume)
{
	volume = _volume;
	sendFrame(createDimensionFrame(who, QString("1*%1").arg(volume), "5#" + where));

	DeviceValues values_list;

	values_list[DIM_SELF_REQUEST] = true;
	values_list[DIM_VOLUME] = volume;
	emit valueReceived(values_list);
}

void VirtualAmplifierDevice::turnOn()
{
	DeviceValues values_list;

	AmplifierDevice::turnOn();

	values_list[DIM_SELF_REQUEST] = true;
	values_list[REQ_AMPLI_ON] = true;
	emit valueReceived(values_list);
}

void VirtualAmplifierDevice::turnOff()
{
	DeviceValues values_list;

	AmplifierDevice::turnOff();

	values_list[DIM_SELF_REQUEST] = true;
	values_list[REQ_AMPLI_ON] = false;
	emit valueReceived(values_list);
}

void VirtualAmplifierDevice::volumeUp()
{
	DeviceValues values_list;

	values_list[DIM_SELF_REQUEST] = true;
	values_list[REQ_VOLUME_UP] = 1;
	emit valueReceived(values_list);
}

void VirtualAmplifierDevice::volumeDown()
{
	DeviceValues values_list;

	values_list[DIM_SELF_REQUEST] = true;
	values_list[REQ_VOLUME_DOWN] = 1;
	emit valueReceived(values_list);
}

void VirtualAmplifierDevice::setVolume(int volume)
{
	DeviceValues values_list;

	values_list[DIM_SELF_REQUEST] = true;
	values_list[REQ_SET_VOLUME] = volume;
	emit valueReceived(values_list);
}

bool VirtualAmplifierDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (AmplifierDevice::parseFrame(msg, values_list))
		return true;

	if (isDimensionFrame(msg))
		return false;

	// here we interpret the "turn source off" with where 6 (all sources)
	// to mean "turn the amplifier off temporarily"; it is probably cleaner
	// to do it here rather than in the source device
	if (msg.where() == 6 &&
		msg.what() == static_cast<int>(AMPL_STATUS_OFF) &&
	    msg.whatArgCnt() == 2)
	{
		if (is_multichannel &&
		    msg.whatArg(1) != area.toStdString())
			return false;

		values_list[REQ_TEMPORARY_OFF] = true;
		return true;
	}

	// the amplifier slide frame must turn the local amplifier off if it comes
	// from the same area (for multichannel) or for any source amplifier (for monochannel)
	if (msg.what() == int(REQ_SLIDE) &&
	    msg.where() == 5 &&
	    msg.whereArgCnt() == 3)
	{
		if (is_multichannel &&
		    msg.whereArg(1) != area.toStdString())
			return false;

		values_list[REQ_TEMPORARY_OFF] = true;
		return true;
	}

	// handle the general off frame from the special command
	if (   msg.what() == int(AMPL_STATUS_OFF)
	    && msg.whatArgCnt() == 2
	    && msg.whatArgN(1) == 15
	    && msg.whereFull() == "5#1#1")
	{
		values_list[REQ_AMPLI_ON] = false;

		return true;
	}

	if (!checkAddressIsForMe(msg))
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
	case REQ_SLIDE:
		// in case we receive a slide request, we must lower the volume
		// to 0 for a second
		values_list[REQ_TEMPORARY_OFF] = true;
		break;
	default:
		return false;
	}

	return true;
}


CompositeAmplifierDevice::CompositeAmplifierDevice(const QList<AmplifierDevice *> &_devices) :
	AmplifierDevice("0") // the where is not really used by the device
{
	devices = _devices;

	// not computationally expensive, and simplifies get_key implementation
	qSort(devices);

	foreach (device *d, devices)
		key += "-" + d->get_key();
}

QString CompositeAmplifierDevice::get_key()
{
	return key;
}

void CompositeAmplifierDevice::turnOn()
{
	foreach (AmplifierDevice *dev, devices)
		dev->turnOn();
}

void CompositeAmplifierDevice::turnOff()
{
	foreach (AmplifierDevice *dev, devices)
		dev->turnOff();
}

void CompositeAmplifierDevice::volumeUp()
{
	foreach (AmplifierDevice *dev, devices)
		dev->volumeUp();
}

void CompositeAmplifierDevice::volumeDown()
{
	foreach (AmplifierDevice *dev, devices)
		dev->volumeDown();
}

bool CompositeAmplifierDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	Q_UNUSED(msg);
	Q_UNUSED(values_list);

	// avoid actually parsing the frames
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
	if (!checkAddressIsForMe(msg))
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

void PowerAmplifierDevice::balanceLeft() const
{
	sendCommand(QString("%1#1").arg(REQ_BALANCE_UP));
}

void PowerAmplifierDevice::balanceRight() const
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

void PowerAmplifierDevice::setPreset(int preset) const
{
	if (preset >= 0 && preset <= 9) // fixed preset
		preset += 2;
	else if (preset >= 10 && preset <= 19) // custom preset
		preset += 6;

	sendFrame(createWriteDimensionFrame(who, QString("%1*%2").arg(DIM_PRESET).arg(preset), where));
}

void PowerAmplifierDevice::loudOn() const
{
	sendFrame(createWriteDimensionFrame(who, QString("%1*1").arg(REQ_LOUD), where));
}

void PowerAmplifierDevice::loudOff() const
{
	sendFrame(createWriteDimensionFrame(who, QString("%1*0").arg(REQ_LOUD), where));
}


AuxDevice::AuxDevice(QString address, int openserver_id) : device("9", address, openserver_id)
{
}

void AuxDevice::requestStatus()
{
	sendRequest(QString());
}

void AuxDevice::init()
{
	requestStatus();
}

bool AuxDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (where.toStdString() != msg.whereFull())
		return false;

	if (!isCommandFrame(msg))
		return false;

	switch (msg.what())
	{
	case CMD_TURN_ON:
		values_list[DIM_STATUS] = true;
		break;
	case CMD_TURN_OFF:
		values_list[DIM_STATUS] = false;
		break;
	default:
		return false;
	}
	return true;
}

