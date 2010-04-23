QT += network xml testlib
OBJECTS_DIR = obj
MOC_DIR = moc

DEFINES += QT_QWS_EBX BT_EMBEDDED BTWEB QT_NO_DEBUG_OUTPUT DEBUG
DEFINES += BT_HARDWARE_X11

INCLUDEPATH+= . .. ../devices ../devices/test ../../stackopen/common_files ../../stackopen
INCLUDEPATH+= ../../stackopen/common_develer/lib
DEPENDPATH+= . .. ../devices ../devices/test

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += debug silent
CONFIG -= release

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter

TEMPLATE = app

LIBS+= -L ../../common_files/lib/x86 -lcommon -lssl

VPATH = ..

HEADERS += test_bttime.h \
	test_scenevodevicescond.h \
	../devices/test/openserver_mock.h \
	../btbutton.h \
	../bttime.h \
	../devices_cache.h \
	../fontmanager.h \
	../generic_functions.h \
	../hardware_functions.h \
	../icondispatcher.h \
	../main.h \
	../openclient.h \
	../scaleconversion.h \
	../scenevodevicescond.h \
	../skinmanager.h \
	../xml_functions.h

SOURCES += test_main.cpp \
	test_bttime.cpp \
	test_scenevodevicescond.cpp \
	../devices/test/openserver_mock.cpp \
	../btbutton.cpp \
	../bttime.cpp \
	../devices_cache.cpp \
	../fontmanager.cpp \
	../generic_functions.cpp \
	../hardware_functions_x11.cpp \
	../icondispatcher.cpp \
	../openclient.cpp \
	../scaleconversion.cpp \
	../scenevodevicescond.cpp \
	../skinmanager.cpp \
	../xml_functions.cpp \
	../definitions.cpp

include(../devices.pri)
