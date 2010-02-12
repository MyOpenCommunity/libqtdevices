#include <QtTest/QtTest>
#include <QList>
#include <QRegExp>

#include "test_scenevodevicescond.h"
#include "main.h"

// move this somewhere else (generic_functions.cpp?): we can't link main.cpp
QHash<GlobalFields, QString> bt_global::config;

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QList<QObject *> test_list;

	TestScenEvoDevicesCond test_scenevo_devices_cond;
	test_list << &test_scenevo_devices_cond;

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

