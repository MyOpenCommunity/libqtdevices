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


#include "test_device.h"
#include "openserver_mock.h"
#include "openclient.h"

#include <device.h>
#include <frame_receiver.h>

#include <QVariant>
#include <QMetaType>


TestDevice::TestDevice()
{
	// To use StatusList in signal/slots and watch them through QSignalSpy
	qRegisterMetaType<StatusList>("StatusList");
	server = new OpenServerMock;
}

void TestDevice::initTestDevice()
{
	client_command = server->connectCommand();
	client_request = server->connectRequest();
	client_monitor = server->connectMonitor();

	QHash<int, Client*> monitors;
	monitors[0] = client_monitor;
	FrameReceiver::setClientsMonitor(monitors);

	QHash<int, QPair<Client*, Client*> > clients;
	clients[0].first = client_command;
	clients[0].second = client_request;
	device::setClients(clients);
}

TestDevice::~TestDevice()
{
	delete server;
}

void TestDevice::cleanBuffers()
{

	client_command->flush();
	client_request->flush();
	client_monitor->flush();
	server->frameCommand(1);
	server->frameRequest(1);
}

