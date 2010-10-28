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


/*!
	\ingroup Messages
	\brief Structure used as container for date and message text.
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


namespace MessageDevicePrivate
{
	static const int TIMEOUT = 5000;

	/*!
		\internal

		\brief Function to calculate the checksum of a message.
		\note The sum is performed on bytes, not characters.

		CHK2 = (1 + B1 + B2 + ... + Bn) mod 256
		CHK1 = [nxB1 + (n-1)xB2 + (n-2)xB3 + ... + Bn + n] mod 256
		CHK = (CHK1 CHK2)
	 */
	int checksum(const QString &string);

	/*!
		\internal

		\brief Function to create a message from a raw string.
	 */
	Message parseMessage(const QString &raw_message);
}

/*!
	\ingroup Messages
	\brief Manages messages from Guard Unit.

	\section MessageDevice-dimensions Dimensions
	\startdim
	\dim{DIM_MESSAGE,QString,,The received message}
	\enddim

	\note Only for receiving.

	\sa \ref device-dimensions
*/
class MessageDevice : public device
{
friend class TestMessageDevice;

Q_OBJECT
public:

	/*!
		\refdim{MessageDevice}
	*/
	enum Type
	{
		DIM_MESSAGE = 1,
	};

	/*!
		\brief Constructor

		Construct a new MessageDevice with the given \a openserver_id.
	 */
	explicit MessageDevice(int openserver_id = 0);

protected:
	/*!
		\section protocol Message protocol description:
		- begin
		- param (ignored)
		- continue (could arrive many times)
		- checksum
		- end

		\note Only one message a time can be processed, if the device receives
			a begin request while is processing one message, it responds with a
			busy response to the caller.

		\note If the device doesn't receive a request every 3 seconds after the
			begin, it sends a timeout response to the caller containing the
			number of bytes received.

		\sa device::parseFrame()
	 */
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);


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
