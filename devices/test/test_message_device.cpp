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
#include "openmsg.h"

#include <QtTest>

#define TIMEOUT_DELAY 1000


void TestMessageDevice::init()
{
	dev = new MessageDevice("165");
}

void TestMessageDevice::cleanup()
{
	delete dev;
}

void TestMessageDevice::testChecksum()
{
	int check = MessageDevicePrivate::checksum(QString("Bticino%1").arg(QChar(0xf0e2)));

	QCOMPARE(check, 0xE49B);
}

void TestMessageDevice::sendReady()
{
	dev->cdp_where = "350";
	dev->sendReady();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*8*9013*350#8#00#165#8##"));
}

void TestMessageDevice::sendBusy()
{
	dev->sendBusy(QString::number(350));
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*8*9014*350#8#00#165#8##"));
}

void TestMessageDevice::sendWrongChecksum()
{
	dev->cdp_where = "350";
	dev->sendWrongChecksum("ID_MESSAGE");
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*8*9015#ID_MESSAGE*350#8#00#165#8##"));
}

void TestMessageDevice::sendTimeout()
{
	dev->cdp_where = "350";
	dev->message = "test";
	dev->sendTimeout();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*8*9016#4*350#8#00#165#8##"));
}

void TestMessageDevice::connectFailed()
{
	QVERIFY(dev->cdp_where.isEmpty());
	QVERIFY(dev->message.isEmpty());
	QVERIFY(!dev->timer.isActive());

	OpenMsg begin_msg("*8*9012#1001*165#8#00#350#8##");
	dev->manageFrame(begin_msg);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*8*9013*350#8#00#165#8##"));
	QCOMPARE(dev->cdp_where, QString("350"));

	OpenMsg other_begin_msg("*8*9012#1001*165#8#00#666#8##");
	dev->manageFrame(other_begin_msg);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*8*9014*666#8#00#165#8##"));
	QCOMPARE(dev->cdp_where, QString("350"));
}

void TestMessageDevice::receiveCompleteMessage()
{
	QVERIFY(dev->cdp_where.isEmpty());
	QVERIFY(dev->message.isEmpty());
	QVERIFY(!dev->timer.isActive());

	OpenMsg begin_msg("*8*9012#1001*165#8#00#350#8##");
	dev->manageFrame(begin_msg);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*8*9013*350#8#00#165#8##"));
	QCOMPARE(dev->cdp_where, QString("350"));


	OpenMsg param_msg("*#8*165#8#00#350#8*#9001*1*2***10**##");
	dev->manageFrame(param_msg);
	client_command->flush();

	QVERIFY(!dev->cdp_where.isEmpty());


	OpenMsg continue_msg("*#8*165#8#00#350#8*#9002*14*48*56*47*48*51*47*49*48*32*49*55*58*51*50*15*113*117*97*108*115*105*97*115*105*32*99*111*115*97##");
	dev->manageFrame(continue_msg);
	client_command->flush();

	QCOMPARE(dev->message, QString("\01608/03/10 17:32\017qualsiasi cosa"));


	OpenMsg checksum_msg("*8*9017#34401*165#8#00#350#8##");
	dev->manageFrame(checksum_msg);
	client_command->flush();

	// Verify that the cleanup is not performed
	QVERIFY(!dev->cdp_where.isEmpty());
	QVERIFY(!dev->message.isEmpty());
	QVERIFY(dev->timer.isActive());


	DeviceTester tst(dev, MessageDevice::DIM_MESSAGE, DeviceTester::ONE_VALUE);
	tst.check("*8*9001*165#8#00#350#8##", "\01608/03/10 17:32\017qualsiasi cosa");
}

void TestMessageDevice::recevieWrongChecksum()
{
	QVERIFY(dev->cdp_where.isEmpty());
	QVERIFY(dev->message.isEmpty());
	QVERIFY(!dev->timer.isActive());

	OpenMsg begin_msg("*8*9012#1001*165#8#00#350#8##");
	dev->manageFrame(begin_msg);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*8*9013*350#8#00#165#8##"));
	QCOMPARE(dev->cdp_where, QString("350"));


	OpenMsg param_msg("*#8*165#8#00#350#8*#9001*1*2***10**##");
	dev->manageFrame(param_msg);
	client_command->flush();

	QVERIFY(!dev->cdp_where.isEmpty());


	OpenMsg continue_msg("*#8*165#8#00#350#8*#9002*14*48*56*47*48*51*47*49*48*32*49*55*58*51*50*15*113*117*97*108*115*105*97*115*105*32*99*111*115*97##");
	dev->manageFrame(continue_msg);
	client_command->flush();

	QCOMPARE(dev->message, QString("\01608/03/10 17:32\017qualsiasi cosa"));


	OpenMsg checksum_msg("*8*9017#12345*165#8#00#350#8##");
	dev->manageFrame(checksum_msg);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*8*9015#12345*350#8#00#165#8##"));

	// Verify that the cleanup is performed
	QVERIFY(dev->cdp_where.isEmpty());
	QVERIFY(dev->message.isEmpty());
	QVERIFY(!dev->timer.isActive());
}

void TestMessageDevice::receiveTimeout()
{
	QVERIFY(dev->cdp_where.isEmpty());
	QVERIFY(dev->message.isEmpty());
	QVERIFY(!dev->timer.isActive());

	OpenMsg begin_msg("*8*9012#1001*165#8#00#350#8##");
	dev->manageFrame(begin_msg);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*8*9013*350#8#00#165#8##"));
	QCOMPARE(dev->cdp_where, QString("350"));
	QVERIFY(dev->message.isEmpty());
	QVERIFY(dev->timer.isActive());

	testSleep(MessageDevicePrivate::TIMEOUT + TIMEOUT_DELAY);

	QCOMPARE(server->frameCommand(), QString("*8*9016#0*350#8#00#165#8##"));

	// Verify that the cleanup is performed
	QVERIFY(dev->cdp_where.isEmpty());
	QVERIFY(dev->message.isEmpty());
	QVERIFY(!dev->timer.isActive());
}
