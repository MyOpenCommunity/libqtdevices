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


#include "test_media_device.h"
#include "openclient.h"
#include "openserver_mock.h"

#include <media_device.h>

#include <QtTest>


void TestSourceDevice::initTestCase()
{
	source_id = "1";
	dev = new SourceDevice(source_id);
}

void TestSourceDevice::cleanupTestCase()
{
	delete dev;
}

void TestSourceDevice::sendNextTrack()
{
	dev->nextTrack();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*9*2#%1##").arg(source_id));
}

void TestSourceDevice::sendPrevTrack()
{
	dev->prevTrack();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*10*2#%1##").arg(source_id));
}

void TestSourceDevice::sendTurnOn()
{
	QString area = "2";
	dev->turnOn(area);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*35#4#%2#%1*3#%2#0##").arg(source_id).arg(area));
}


void TestRadioSourceDevice::initTestCase()
{
	source_id = "1";
	dev = new RadioSourceDevice(source_id);
}

void TestRadioSourceDevice::cleanupTestCase()
{
	delete dev;
}

void TestRadioSourceDevice::sendFrequenceUp()
{
	dev->frequenceUp();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*5#*2#%1##").arg(source_id));
}

void TestRadioSourceDevice::sendFrequenceDown()
{
	dev->frequenceDown();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*6#*2#%1##").arg(source_id));
}

void TestRadioSourceDevice::sendSaveStation()
{
	dev->saveStation("6");
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*33#6*2#%1##").arg(source_id));
}

