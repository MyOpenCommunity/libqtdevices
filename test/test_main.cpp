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

#include "test_scenevodevicescond.h"
#include "test_bttime.h"
#include "main.h"

// move this somewhere else (generic_functions.cpp?): we can't link main.cpp
QHash<GlobalFields, QString> bt_global::config;

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QList<QObject *> test_list;

	TestScenEvoDevicesCond test_scenevo_devices_cond;
	test_list << &test_scenevo_devices_cond;

	TestBtTime test_bttime;
	test_list << &test_bttime;

	QStringList arglist = app.arguments();
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

