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


#ifndef TEST_DEVICE_H
#define TEST_DEVICE_H

#include <QObject>

class OpenServerMock;
class Client;


/**
 * The base class for all tests about device.
 *
 * The derived class should define tests using the following rules:
 * 1. receive* -> verify that the data structure built from the parsing of incoming frames
 *    from the server is correct.
 * 2. send* -> verify that the frame to be sent to the server is correctly created.
 * 3. test* -> tests that cannot be included in 1. & 2.
 */
class TestDevice : public QObject
{
public:
	TestDevice();
	void initTestDevice();
	virtual ~TestDevice();

protected:
	OpenServerMock *server;
	Client *client_command;
	Client *client_request;
	Client *client_monitor;
	void cleanBuffers();
};


#endif // TEST_DEVICE_H
