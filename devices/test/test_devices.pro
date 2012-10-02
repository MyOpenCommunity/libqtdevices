QT += network xml testlib
OBJECTS_DIR = obj
MOC_DIR = moc

DEFINES += QT_QWS_EBX BT_EMBEDDED BTWEB QT_NO_DEBUG_OUTPUT DEBUG

INCLUDEPATH+= . .. ../.. ../../../stackopen/common_files ../../../stackopen
INCLUDEPATH+= ../../../stackopen/common_develer/lib
DEPENDPATH+= . .. ../..

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += debug silent
CONFIG -= release

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter

TEMPLATE = app

LIBS+= -L ../../../stackopen/common_files/lib/x86 -lcommon -lssl

VPATH = ../.. ..

HEADERS += device_tester.h \
	test_air_conditioning_device.h \
	test_antintrusion_device.h \
	test_automation_device.h \
	test_checkaddress.h \
	test_scenario_device.h \
	test_thermal_device.h \
	test_device.h \
	test_devicescache.h \
	test_lighting_device.h \
	test_platform_device.h \
	test_loads_device.h \
	test_media_device.h \
	test_energy_device.h \
	test_pull_manager.h \
	test_probe_device.h \
	test_message_device.h \
	test_stopandgo_device.h \
	test_videodoorentry_device.h \
	openserver_mock.h \
	../../bttime.h \
	../../openclient.h \
	../../frame_classes.h \
	../../frame_functions.h \
	../../xml_functions.h \
	../../main.h \
	../../delayedslotcaller.h \
	../../devices_cache.h


SOURCES += test_main.cpp \
	device_tester.cpp \
	test_air_conditioning_device.cpp \
	test_antintrusion_device.cpp \
	test_automation_device.cpp \
	test_checkaddress.cpp \
	test_scenario_device.cpp \
	test_thermal_device.cpp \
	test_device.cpp \
	test_devicescache.cpp \
	test_platform_device.cpp \
	test_loads_device.cpp \
	test_media_device.cpp \
	test_energy_device.cpp \
	test_lighting_device.cpp \
	test_pull_manager.cpp \
	test_probe_device.cpp \
	test_message_device.cpp \
	test_stopandgo_device.cpp \
	test_videodoorentry_device.cpp \
	openserver_mock.cpp \
	../../bttime.cpp \
	../../openclient.cpp \
	../../frame_classes.cpp \
	../../frame_functions.cpp \
	../../xml_functions.cpp \
	../../definitions.cpp \
	../../delayedslotcaller.cpp \
	../../devices_cache.cpp

include(../../devices.pri)
