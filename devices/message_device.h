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


#ifndef MESSAGE_DEVICE_H
#define MESSAGE_DEVICE_H

#include "device.h"

#include <QTimer>
#include <QDateTime>

/**
 * Message
 *
 * Structure used as container for date and message text.
 */
struct Message
{
	QDateTime datetime;
	QString text;
};
Q_DECLARE_METATYPE(Message);

inline bool operator==(const Message &m1, const Message &m2)
{
	return m1.datetime == m2.datetime && m2.text == m2.text;
}


/**
 * MessageDevicePrivate namespace
 *
 * The variables and the functions that resides in this namespace
 * should be into the implementation, but are here to make easier to test them.
 */
namespace MessageDevicePrivate
{
	static const int TIMEOUT = 3000;

	int checksum(const QString &string);
	Message parseMessage(const QString &raw_message);
}

/**
 * MessageDevice
 *
 * Class for messaging management from SCS and CDP (via open).
 * Only for receiving (for now).
 */
class MessageDevice : public device
{
friend class TestMessageDevice;

Q_OBJECT
public:

	enum Type
	{
		DIM_MESSAGE = 1,
	};

	explicit MessageDevice(int openserver_id = 0);

protected:
	virtual bool parseFrame(OpenMsg &msg, StatusList &status_list);

private slots:
	void timeout();

private:
	void sendReady();
	void sendBusy(const QString &caller_where);
	void sendWrongChecksum(const QString &message_id);
	void sendTimeout();

	void resetTimer();
	void cleanup();

	QString cdp_where;
	QString message;
	QTimer timer;
};

#endif // MESSAGE_DEVICE_H
