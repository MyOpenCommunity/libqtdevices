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


#include "message_device.h"
#include "openmsg.h"


enum
{
	MESSAGE_PARAM_OR_END = 9001,
	MESSAGE_CONTINUE = 9002,
	MESSAGE_BEGIN = 9012,
	MESSAGE_CHECKSUM = 9017
};

enum
{
	COMMAND_READY = 9013,
	COMMAND_BUSY = 9014,
	COMMAND_WRONG_CHECKSUM = 9015,
	COMMAND_TIMEOUT = 9016
};


MessageDevice::MessageDevice(QString where, int openserver_id) :
	device("8", where, openserver_id)
{
	timer.setInterval(3000);
	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), SLOT(timeout()));
}

void MessageDevice::manageFrame(OpenMsg &msg)
{

	QString dest_where = QString::number(msg.where());
	if (dest_where != where)
		return;

	if (msg.whereArgCnt() != 4)
	{
		qWarning("Malformed message where.");
		return;
	}

	int what = msg.what();
	switch (what)
	{
	case MESSAGE_BEGIN:
		if (cdp_where.isEmpty())
		{
			cdp_where = QString::fromStdString(msg.whereArg(2));
			sendReady();
			timer.start();
		}
		else
			sendBusy(msg.where());
		break;
	case MESSAGE_PARAM_OR_END:
		if (msg.IsMeasureFrame()) // message param
			resetTimer();
		else // end message
		{
			StatusList statusList;
			statusList[DIM_MESSAGE] = message;
			emit status_changed(statusList);
			cleanup();
		}
		break;
	case MESSAGE_CONTINUE:
		for (unsigned int i = 0; i < msg.whatArgCnt(); ++i) {
			char c = msg.whatArgN(i);
			message.append(c);
		}
		resetTimer();
		break;
	case MESSAGE_CHECKSUM:
		// TODO; verify the checksum and send wrong checksum if wrong.
		break;
	}
}

void MessageDevice::timeout()
{
	sendTimeout();
	cleanup();
}

void MessageDevice::sendReady()
{
	sendCommand(QString("%1").arg(COMMAND_READY), QString("%1#8#00#%2#8").arg(cdp_where).arg(where));
}

void MessageDevice::sendBusy(int caller_where)
{
	sendCommand(QString("%1").arg(COMMAND_BUSY), QString("%1#8#00#%2#8").arg(caller_where).arg(where));
}

void MessageDevice::sendWrongChecksum(QString message_id)
{
	sendCommand(QString("%1#%2").arg(COMMAND_WRONG_CHECKSUM).arg(message_id), QString("%1#8#00#%2#8").arg(cdp_where).arg(where));
}

void MessageDevice::sendTimeout()
{
	sendCommand(QString("%1#%2").arg(COMMAND_TIMEOUT).arg(message.size()), QString("%1#8#00#%2#8").arg(cdp_where).arg(where));
}

void MessageDevice::resetTimer()
{
	timer.stop();
	timer.start();
}

void MessageDevice::cleanup()
{
	timer.stop();
	cdp_where.clear();
	message.clear();
}
