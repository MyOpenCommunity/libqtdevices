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
	REQ_FREQUENCE_UP = 5,
	REQ_FREQUENCE_DOWN = 6,
	REQ_NEXT_TRACK = 9,
	REQ_PREV_TRACK = 10,
	REQ_SOURCE_ON = 35,
	REQ_SAVE_STATION = 33,
};


SourceDevice::SourceDevice(QString source, int openserver_id) :
	device(QString("22"), "2#" + source, openserver_id)
{
	mmtype = 4;
	source_id = source;
}

void SourceDevice::nextTrack() const
{
	sendCommand(QString::number(REQ_NEXT_TRACK));
}

void SourceDevice::prevTrack() const
{
	sendCommand(QString::number(REQ_PREV_TRACK));
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

bool SourceDevice::parseFrame(OpenMsg &msg, StatusList &status_list)
{
	QString msg_where = QString::fromStdString(msg.whereFull());
	if (msg_where != where && msg_where != QString("5#%1").arg(where))
		return false;

	if (!isDimensionFrame(msg))
		return false;

	QVariant v;

	int what = msg.what();
	switch (what)
	{
	case DIM_STATUS:
		v.setValue(msg.whatArgN(0) == 1);
		break;
	}

	status_list[what] = v;
	return !status_list.isEmpty();
}


RadioSourceDevice::RadioSourceDevice(QString source_id, int openserver_id) :
	SourceDevice(source_id, openserver_id)
{
	mmtype = 4;
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

bool RadioSourceDevice::parseFrame(OpenMsg &msg, StatusList &status_list)
{
	QString msg_where = QString::fromStdString(msg.whereFull());
	if (msg_where != where && msg_where != QString("5#%1").arg(where))
		return false;

	SourceDevice::parseFrame(msg, status_list);

	if (!isDimensionFrame(msg))
		return false;

	QVariant v;

	int what = msg.what();
	switch (what)
	{
	case DIM_FREQUENCY:
		v.setValue(msg.whatArgN(1));
		break;
	}

	status_list[what] = v;
	return !status_list.isEmpty();
}


VirtualSourceDevice::VirtualSourceDevice(QString address, int openserver_id) :
	SourceDevice(address, openserver_id)
{

}

