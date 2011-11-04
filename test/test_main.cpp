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


#include <QtTest/QtTest>
#include <QList>
#include <QRegExp>
#include <QApplication>

#include <iostream>
#include <logger.h>

#include "test_scenevodevicescond.h"
#include "test_bttime.h"
#include "test_imageselection.h"
#include "test_xmlclient.h"
#include "test_xmldevice.h"
#include "test_clientwriter.h"
#include "test_delayedslotcaller.h"
#include "test_displaycontrol.h"
#include "btmain.h"
#include "main.h"


BtStatus bt_global::status;
logger *app_logger;

// Objects required to replace the real objects and link successfully
class VCTCallPage  : public QObject
{
	Q_OBJECT
public:
	void closeCall();
};

void VCTCallPage::closeCall() { }

class IntercomCallPage  : public QObject
{
	Q_OBJECT
public:
	void closeCall();
};

void IntercomCallPage::closeCall() { }

#include "test_main.moc"


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QList<QObject *> test_list;

	// Initialize the config object for tests.
	bt_global::config = new QHash<GlobalField, QString>();
	(*bt_global::config)[TEMPERATURE_SCALE] = CELSIUS;
	(*bt_global::config)[TS_NUMBER] = "0";

	TestScenEvoDevicesCond test_scenevo_devices_cond;
	test_list << &test_scenevo_devices_cond;

	TestBtTime test_bttime;
	test_list << &test_bttime;

	TestImageSelection test_imageselection;
	test_list << &test_imageselection;

	TestXmlClient test_xmlclient;
	test_list << &test_xmlclient;

	TestXmlDevice test_xmldevice;
	test_list << &test_xmldevice;

	TestClientWriter test_clientwriter;
	test_list << &test_clientwriter;

	TestDelayedSlotCaller test_delayedslotcaller;
	test_list << &test_delayedslotcaller;

	TestDisplayControl test_displaycontrol;
	test_list << &test_displaycontrol;

	QStringList arglist = app.arguments();
	if (arglist.contains("--help"))
	{
		std::cout << "Options:" << std::endl;
		std::cout << " --test-class [REGEXP]\trun only tests that matches REGEXP" << std::endl;
		std::cout << " --help\t\t\tprint this help" << std::endl;
		std::cout << std::endl;
		std::cout << "Class List:" << std::endl;
		foreach (QObject *dev, test_list)
			std::cout << " " << dev->metaObject()->className() << std::endl;
		return 0;
	}

	QString testingClass;
	int custom_param_pos = arglist.indexOf("--test-class");
	if (custom_param_pos != -1 && custom_param_pos < arglist.size() - 1)
	{
		testingClass = arglist.at(custom_param_pos + 1);
		arglist.removeAt(custom_param_pos + 1);
		arglist.removeAt(custom_param_pos);
	}

	// use regular expressions to avoid writing the full class name each time
	QRegExp re(testingClass, Qt::CaseInsensitive);
	foreach (QObject *tester, test_list)
	{
		QString class_name = tester->metaObject()->className();
		if (testingClass.isEmpty() || class_name.contains(re))
			QTest::qExec(tester, arglist);
	}
}

