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


#include "test_message_device.h"

#include "openserver_mock.h"
#include "device_tester.h"
#include "openclient.h"
#include "message_device.h"

#include <QtTest>

void TestMessageDevice::init()
{
	dev = new MessageDevice("165");
}

void TestMessageDevice::cleanup()
{
	delete dev;
}

void TestMessageDevice::sendReady()
{
	dev->cdp_where = "350";
	dev->sendReady();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*8*9013*350#8#00#165#8##"));
}
