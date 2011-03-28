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


#include "frame_receiver.h"
#include "openclient.h" // client_monitor

// Inizialization of static member
QHash<int, ClientReader*> FrameReceiver::clients_monitor;


/*!
 * \class FrameReceiver
 * \brief Provides a common interface for objects that would manage
 * frames from a socket
 *
 * To receive a frame the \a subscribe_monitor() method must be called with the
 * right \a "who" as parameter.
 *
 * The method \a manageFrame() must be defined in subclasses to define the
 * the object behviour when a frame arrives.
 */

/*!
 * \brief Constructor
 *
 * \note the default openserver id should be keep in sync with the define
 * MAIN_OPENSERVER
 */
FrameReceiver::FrameReceiver(int oid)
{
	subscribed = false;
	openserver_id = oid;
}

/*!
 * \brief Destructor
 */
FrameReceiver::~FrameReceiver()
{
	if (subscribed)
		clients_monitor[openserver_id]->unsubscribe(this);
}

/*!
 * \brief Sets the monitors
 */
void FrameReceiver::setClientsMonitor(const QHash<int, ClientReader*> &monitors)
{
	clients_monitor = monitors;
}

/*!
 * \fn FrameReceiver::manageFrame(OpenMsg &msg)
 * \brief Manages frames from the OpenServer
 *
 * This method is called every time that a frame with the \a "who" subscribed
 * arrives from the OpenServer.
 */

/*!
 * \brief Register to the monitor for receive frames
 *
 * Every child that would receive a frame for a \a "who" must subscribe itself
 * calling this method (unregistration is not needed).
 */
void FrameReceiver::subscribe_monitor(int who)
{
	Q_ASSERT_X(clients_monitor.contains(openserver_id), "FrameReceiver::subscribe_monitor",
		qPrintable(QString("Client monitor not set for id: %1!").arg(openserver_id)));
	clients_monitor[openserver_id]->subscribe(this, who);
	subscribed = true;
}
