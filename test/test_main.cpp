#include <QtTest/QtTest>

#include "test_landevice.h"


int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	TestLanDevice test_lan_device;
	QTest::qExec(&test_lan_device, argc, argv);
}

