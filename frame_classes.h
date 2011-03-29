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


#ifndef FRAME_CLASSES_H
#define FRAME_CLASSES_H

#include <QObject>
#include <QHash>

class ClientReader;
class ClientWriter;
class OpenMsg;

struct Clients
{
	ClientWriter *command;
	ClientWriter *request;
	ClientReader *supervisor;
	Clients()
	{
		command = 0;
		request = 0;
		supervisor = 0;
	}
};


/*!
	\ingroup Core
	\brief Provides a common interface for objects that would manage
	frames from a socket

	To receive a frame the \a subscribeMonitor() method must be called with the
	right \a "who" as parameter.

	The method \a manageFrame() must be defined in subclasses to define the
	the object behaviour when a frame arrives.
 */
class FrameReceiver
{
friend class ClientReader;
public:
	FrameReceiver(int openserver_id = 0);
	virtual ~FrameReceiver();

	/*!
		\brief Sets the monitors
	 */
	static void setClientsMonitor(const QHash<int, ClientReader*> &monitors);

	/*!
		\brief Manages frames from the OpenServer

		This method is called every time that a frame with the \a "who" subscribed
		arrives from the OpenServer.
	 */
	virtual void manageFrame(OpenMsg &msg) = 0;

protected:
	/*!
		\brief Register to the monitor for receive frames

		Every child that would receive a frame for a \a "who" must subscribe itself
		calling this method (unregistration is not needed).
	 */
	void subscribeMonitor(int who);

	static QHash<int, ClientReader*> clients_monitor;

private:
	bool subscribed;
	int openserver_id;
};


/*!
	\ingroup Core
	\brief An abstract base class that includes all the stuff required to send
	frames.
*/
class FrameSender
{
friend class ClientWriter;
public:
	FrameSender(int openserver_id = 0);

	virtual ~FrameSender();

	/*!
		\brief Sets the Client
	*/
	static void setClients(const QHash<int, Clients> &c);

	/*!
		\brief Send (without delay) a \a frame to the Openserver with the given \a id.
	 */
	static void sendCommandFrame(int openserver_id, const QString &frame);

	/*!
		\brief Set this flag to delay the frames sent using the sendFrame()
		sendInit() methods.

		\sa sendFrame(), sendFrameNow(), sendInit(), sendInitNow()
	*/
	static void delayFrames(bool delay);

protected:
	/*!
		\brief Sends frames to the openserver, using the Client::COMMAND channel.

		The frame can be really sent after a delay depending on the delayFrames()
		setting.

		\sa delayFrames(), sendFrameNow()
	*/
	void sendFrame(const QString &frame) const;

	/*!
		\brief Sends frames to the openserver, using the Client::COMMAND channel.

		The frame is sent immediatelly not taking care of the delayFrames()
		setting.

		\sa delayFrames(), sendFrame()
	 */
	void sendFrameNow(const QString &frame) const;

	/*!
		\brief Sends frames to the openserver, using the Client::REQUEST channel.

		The frame can be really sent after a delay depending on the delayFrames()
		setting.

		\sa delayFrames(), sendInitNow()
	*/
	void sendInit(const QString &frame) const;

	/*!
		\brief Sends frames to the openserver, using the Client::REQUEST channel.

		The frame is sent immediatelly not taking care of the delayFrames()
		setting.

		\sa delayFrames(), sendFrame()
	 */
	void sendInitNow(const QString &frame) const;

	/*!
		\brief Request to receive ack/nack for the specified 'who'.

		For each frame sent, the openserver response with ack to indicate that
		the command is successfully processed or with nak for the opposite. Calling
		this method the FrameSender subclass can manage both the situations.
		\sa manageAck(), manageNak()
	*/
	void subscribeAck(int who);

	/*!
		\brief Manages ack from the OpenServer

		This method is called every time that an ack relatives to a frame with the
		\a "who" subscribed arrives from the Openserver.
		\sa subscribeAck(), manageNak()
	 */
	virtual void manageAck(OpenMsg &msg) {}

	/*!
		\brief Manages nak from the OpenServer

		This method is called every time that a nak relatives to a frame with the
		\a "who" subscribed arrives from the Openserver.
		\sa subscribeAck(), manageAck()
	 */
	virtual void manageNak(OpenMsg &msg) {}

	static QHash<int, Clients> clients;

private:
	int openserver_id;
	bool subscribed;
};


#endif // FRAME_CLASSES_H
