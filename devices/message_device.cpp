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
#include "generic_functions.h"

#define TIMEOUT 3000

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

int MessageDevicePrivate::checksum(const QString &string)
{
	QByteArray data;
	foreach (const QChar &c, string)
	{
		ushort u = c.unicode();
		data.append(u >> 8);
		data.append((u & 0x00ff));
	}

	int data_size = data.size();

	int chk2 = 1;
	for (int i = 0; i < data_size; ++i)
		chk2 += data[i];
	chk2 = chk2 % 256;

	int chk1 = data_size;
	for (int i = data_size - 1; i > 0; --i)
		chk1 += i * data[data_size - i];
	chk1 = chk1 % 256;

	return (chk1 << 8) | chk2;
}

using namespace MessageDevicePrivate;

MessageDevice::MessageDevice(QString where, int openserver_id) :
	device("8", where, openserver_id)
{
	timer.setInterval(TIMEOUT);
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
	{
		QString caller_where = QString::fromStdString(msg.whereArg(2));
		if (cdp_where.isEmpty())
		{
			cdp_where = caller_where;
			sendReady();
			timer.start();
		}
		else
			sendBusy(caller_where);
		break;
	}
	case MESSAGE_PARAM_OR_END:
		if (isWriteDimensionFrame(msg)) // message param
			resetTimer();
		else // end message
		{
			StatusList sl;
			sl[DIM_MESSAGE] = message;
			emit status_changed(sl);
			cleanup();
		}
		break;
	case MESSAGE_CONTINUE:
		for (unsigned int i = 0; i < msg.whatArgCnt(); ++i)
			message.append(QChar(msg.whatArgN(i)));
		resetTimer();
		break;
	case MESSAGE_CHECKSUM:
		int check = checksum(message);
		// The checksum to verify is made by the 5 rightmost chars of the id
		int to_verify = QString::fromStdString(msg.whatArg(0)).right(5).toInt();

		if (check != to_verify) // checksums not corresponding
		{
			sendWrongChecksum(QString::number(to_verify));
			cleanup();
		}
		else
			resetTimer();
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
	sendCommand(QString::number(COMMAND_READY), QString("%1#8#00#%2#8").arg(cdp_where).arg(where));
}

void MessageDevice::sendBusy(const QString &caller_where)
{
	sendCommand(QString::number(COMMAND_BUSY), QString("%1#8#00#%2#8").arg(caller_where).arg(where));
}

void MessageDevice::sendWrongChecksum(const QString &message_id)
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
