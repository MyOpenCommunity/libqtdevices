QT += network xml testlib
OBJECTS_DIR = obj
MOC_DIR = moc

DEFINES += QT_QWS_EBX BT_EMBEDDED BTWEB QT_NO_DEBUG_OUTPUT

INCLUDEPATH+= . .. ../../stackopen/common_files ../../stackopen
INCLUDEPATH+= ../../stackopen/common_develer/lib

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += debug silent
CONFIG -= release

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter

TEMPLATE = app

LIBS+= -L ../../common_files/lib/x86 -lcommon -lssl

HEADERS += device_tester.h \
            test_automation_device.h \
            test_air_conditioning_device.h \
            test_checkaddress.h \
            test_scenario_device.h \
            test_device.h \
            test_lighting_device.h \
            test_landevice.h \
            test_energy_device.h \
            test_poweramplifier_device.h \
            test_pull_manager.h \
            test_probe_device.h \
            openserver_mock.h \
            ../energy_device.h \
            ../lighting_device.h \
            ../poweramplifier_device.h \
            ../landevice.h \
            ../pulldevice.h \
            ../automation_device.h \
            ../airconditioning_device.h \
            ../scenario_device.h \
            ../probe_device.h \
            ../device.h \
            ../device_status.h \
            ../frame_interpreter.h \
            ../openclient.h \
            ../bttime.h \
            ../generic_functions.h \
            ../xml_functions.h \
            ../main.h

SOURCES += test_main.cpp \
            device_tester.cpp \
            test_automation_device.cpp \
            test_air_conditioning_device.cpp \
            test_checkaddress.cpp \
            test_scenario_device.cpp \
            test_device.cpp \
            test_landevice.cpp \
            test_energy_device.cpp \
            test_lighting_device.cpp \
            test_poweramplifier_device.cpp \
            test_pull_manager.cpp \
            test_probe_device.cpp \
            openserver_mock.cpp \
            ../energy_device.cpp \
            ../lighting_device.cpp \
            ../poweramplifier_device.cpp \
            ../landevice.cpp \
            ../pulldevice.cpp \
            ../automation_device.cpp \
            ../airconditioning_device.cpp \
            ../scenario_device.cpp \
            ../probe_device.cpp \
            ../device.cpp \
            ../device_status.cpp \
            ../frame_interpreter.cpp \
            ../openclient.cpp \
            ../bttime.cpp \
            ../generic_functions.cpp \
            ../xml_functions.cpp \
            ../definitions.cpp
