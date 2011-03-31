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

HEADERS += xmldevice_tester.h \
	test_bttime.h \
	test_clientwriter.h \
	test_delayedslotcaller.h \
	test_imageselection.h \
	test_scenevodevicescond.h \
	test_xmlclient.h \
	test_xmldevice.h \
	test_functions.h \
	../devices/test/openserver_mock.h \
	../btbutton.h \
	../bttime.h \
	../devices_cache.h \
	../fontmanager.h \
	../frame_classes.h \
	../frame_functions.h \
	../generic_functions.h \
	../hardware_functions.h \
	../icondispatcher.h \
	../imageselectionhandler.h \
	../main.h \
	../openclient.h \
	../scaleconversion.h \
	../scenevodevicescond.h \
	../skinmanager.h \
	../xml_functions.h \
	../xmlclient.h \
	../delayedslotcaller.h \
	../xmldevice.h

SOURCES += xmldevice_tester.cpp \
	test_main.cpp \
	test_bttime.cpp \
	test_clientwriter.cpp \
	test_delayedslotcaller.cpp \
	test_imageselection.cpp \
	test_scenevodevicescond.cpp \
	test_xmlclient.cpp \
	test_xmldevice.cpp \
	test_functions.cpp \
	../devices/test/openserver_mock.cpp \
	../btbutton.cpp \
	../bttime.cpp \
	../devices_cache.cpp \
	../fontmanager.cpp \
	../frame_classes.cpp \
	../frame_functions.cpp \
	../generic_functions.cpp \
	../hardware_functions_x11.cpp \
	../icondispatcher.cpp \
	../imageselectionhandler.cpp \
	../openclient.cpp \
	../scaleconversion.cpp \
	../scenevodevicescond.cpp \
	../skinmanager.cpp \
	../xml_functions.cpp \
	../xmlclient.cpp \
	../delayedslotcaller.cpp \
	../xmldevice.cpp \
	../definitions.cpp

include(../devices.pri)
