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


namespace QTest
{
	template<> char *toString(const Message &message)
	{
		QByteArray ba = "Message(";
		ba += message.datetime.toString() + " - " + message.text;
		ba += ")";
		return qstrdup(ba.data());
	}
}


void TestMessageDevice::init()
{
	dev = new MessageDevice;
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

void TestMessageDevice::testParseMessage()
{
	Message message;
	message.datetime = QDateTime(QDate(2010, 3, 8), QTime(17, 32));
	message.text = "qualsiasi cosa";

	Message check = MessageDevicePrivate::parseMessage("\01608/03/10 17:32\017qualsiasi cosa");
	QCOMPARE(check, message);
}

void TestMessageDevice::sendReady()
{
	dev->where = "165";
	dev->guardunit_where = "350";
	dev->sendReady();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*8*9013*350#00#165##"));
}

void TestMessageDevice::sendBusy()
{
	dev->where = "165";
	dev->sendBusy(QString::number(350));
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*8*9014*350#00#165##"));
}

void TestMessageDevice::sendWrongChecksum()
{
	dev->where = "165";
	dev->guardunit_where = "350";
	dev->sendWrongChecksum("ID_MESSAGE");
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*8*9015#ID_MESSAGE*350#00#165##"));
}

void TestMessageDevice::sendTimeout()
{
	dev->where = "165";
	dev->guardunit_where = "350";
	dev->message = "test";
	dev->sendTimeout();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*8*9016#4*350#00#165##"));
}

void TestMessageDevice::connectFailed()
{
	QVERIFY(dev->guardunit_where.isEmpty());
	QVERIFY(dev->message.isEmpty());
	QVERIFY(!dev->timer.isActive());

	OpenMsg begin_msg("*8*9012#1001*165#00#350##");
	dev->manageFrame(begin_msg);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*8*9013*350#00#165##"));
	QCOMPARE(dev->guardunit_where, QString("350"));

	OpenMsg other_begin_msg("*8*9012#1001*165#00#666##");
	dev->manageFrame(other_begin_msg);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*8*9014*666#00#165##"));
	QCOMPARE(dev->guardunit_where, QString("350"));
}

void TestMessageDevice::receiveCompleteMessage()
{
	QVERIFY(dev->guardunit_where.isEmpty());
	QVERIFY(dev->message.isEmpty());
	QVERIFY(!dev->timer.isActive());

	OpenMsg begin_msg("*8*9012#1001*165#00#350##");
	dev->manageFrame(begin_msg);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*8*9013*350#00#165##"));
	QCOMPARE(dev->guardunit_where, QString("350"));


	OpenMsg param_msg("*#8*165#00#350*#9001*1*2***10**##");
	dev->manageFrame(param_msg);
	client_command->flush();

	QVERIFY(!dev->guardunit_where.isEmpty());


	OpenMsg continue_msg("*#8*165#00#350*#9002*14*48*56*47*48*51*47*49*48*32*49*55*58*51*50*15*113*117*97*108*115*105*97*115*105*32*99*111*115*97##");
	dev->manageFrame(continue_msg);
	client_command->flush();

	QCOMPARE(dev->message, QString("\01608/03/10 17:32\017qualsiasi cosa"));


	OpenMsg checksum_msg("*8*9017#34401*165#00#350##");
	dev->manageFrame(checksum_msg);
	client_command->flush();

	// Verify that the cleanup is not performed
	QVERIFY(!dev->guardunit_where.isEmpty());
	QVERIFY(!dev->message.isEmpty());
	QVERIFY(dev->timer.isActive());

	Message message;
	message.datetime = QDateTime(QDate(2010, 3, 8), QTime(17, 32));
	message.text = "qualsiasi cosa";
	DeviceTester tst(dev, MessageDevice::DIM_MESSAGE, DeviceTester::ONE_VALUE);
	tst.check("*8*9001*165#00#350##", message);
}

void TestMessageDevice::receiveWrongChecksum()
{
	QVERIFY(dev->guardunit_where.isEmpty());
	QVERIFY(dev->message.isEmpty());
	QVERIFY(!dev->timer.isActive());

	OpenMsg begin_msg("*8*9012#1001*165#00#350##");
	dev->manageFrame(begin_msg);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*8*9013*350#00#165##"));
	QCOMPARE(dev->guardunit_where, QString("350"));


	OpenMsg param_msg("*#8*165#00#350*#9001*1*2***10**##");
	dev->manageFrame(param_msg);
	client_command->flush();

	QVERIFY(!dev->guardunit_where.isEmpty());


	OpenMsg continue_msg("*#8*165#00#350*#9002*14*48*56*47*48*51*47*49*48*32*49*55*58*51*50*15*113*117*97*108*115*105*97*115*105*32*99*111*115*97##");
	dev->manageFrame(continue_msg);
	client_command->flush();

	QCOMPARE(dev->message, QString("\01608/03/10 17:32\017qualsiasi cosa"));


	OpenMsg checksum_msg("*8*9017#12345*165#00#350##");
	dev->manageFrame(checksum_msg);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*8*9015#12345*350#00#165##"));

	// Verify that the cleanup is performed
	QVERIFY(dev->guardunit_where.isEmpty());
	QVERIFY(dev->message.isEmpty());
	QVERIFY(!dev->timer.isActive());
}

void TestMessageDevice::receiveTimeout()
{
	QVERIFY(dev->guardunit_where.isEmpty());
	QVERIFY(dev->message.isEmpty());
	QVERIFY(!dev->timer.isActive());

	OpenMsg begin_msg("*8*9012#1001*165#00#350##");
	dev->manageFrame(begin_msg);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*8*9013*350#00#165##"));
	QCOMPARE(dev->guardunit_where, QString("350"));
	QVERIFY(dev->message.isEmpty());
	QVERIFY(dev->timer.isActive());

	dev->timeout();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*8*9016#0*350#00#165##"));

	// Verify that the cleanup is performed
	QVERIFY(dev->guardunit_where.isEmpty());
	QVERIFY(dev->message.isEmpty());
	QVERIFY(!dev->timer.isActive());
}

void TestMessageDevice::receiveBeginMessage()
{
	// where#lev#00..
	QVERIFY(dev->guardunit_where.isEmpty());
	QVERIFY(dev->message.isEmpty());
	QVERIFY(!dev->timer.isActive());

	OpenMsg begin_msg("*8*9012#1001*165#00#350##");
	dev->manageFrame(begin_msg);
	client_command->flush();

	QCOMPARE(dev->guardunit_where, QString("350"));

	dev->cleanup();
	// where#00..
	QVERIFY(dev->guardunit_where.isEmpty());
	QVERIFY(dev->message.isEmpty());
	QVERIFY(!dev->timer.isActive());

	OpenMsg begin_msg2("*8*9012#1001*165#00#350##");
	dev->manageFrame(begin_msg2);
	client_command->flush();

	QCOMPARE(dev->guardunit_where, QString("350"));
}

