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
#include <QEventLoop>


void testSleep(int msec)
{
	QEventLoop event_loop;
	QTimer timer;

	timer.setSingleShot(true);
	QObject::connect(&timer, SIGNAL(timeout()), &event_loop, SLOT(quit()));

	// Wait for the timeout
	timer.start(msec);
	event_loop.exec();
}

TestDevice::TestDevice()
{
	// To use DeviceValues in signal/slots and watch them through QSignalSpy
	qRegisterMetaType<DeviceValues>("DeviceValues");
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

	QHash<int, Clients> clients;
	clients[0].command = client_command;
	clients[0].request = client_request;
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

void TestDevice::flushCompressedFrames(device *dev)
{
	foreach (int what, dev->compressed_frames.keys())
		dev->emitCompressedFrame(what);
	foreach (int what, dev->compressed_requests.keys())
		dev->emitCompressedInit(what);
}
