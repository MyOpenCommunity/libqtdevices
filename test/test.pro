QT += network xml testlib
OBJECTS_DIR = obj
MOC_DIR = moc

DEFINES += QT_QWS_EBX BT_EMBEDDED BTWEB QT_NO_DEBUG_OUTPUT DEBUG
DEFINES += BT_HARDWARE_X11
DEFINES += LAYOUT_TS_10 # to check the SCREENSAVER audio state on DisplayControl

INCLUDEPATH+= . .. ../ts_10 ../devices ../devices/test ../../stackopen/common_files ../../stackopen
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
	test_displaycontrol.h \
	test_imageselection.h \
	test_scenevodevicescond.h \
	test_xmlclient.h \
	test_xmldevice.h \
	test_functions.h \
	../devices/test/openserver_mock.h \
	../ts_10/audiostatemachine.h \
	../btbutton.h \
	../bttime.h \
	../delayedslotcaller.h \
	../devices_cache.h \
	../displaycontrol.h \
	../fontmanager.h \
	../frame_classes.h \
	../frame_functions.h \
	../generic_functions.h \
	../hardware_functions.h \
	../icondispatcher.h \
	../imageselectionhandler.h \
	../pagecontainer.h \
	../pagestack.h \
	../page.h \
	../main.h \
	../mediaplayer.h \
	../openclient.h \
	../scaleconversion.h \
	../scenevodevicescond.h \
	../screensaver.h \
	../skinmanager.h \
	../statemachine.h \
	../styledwidget.h \
	../timescript.h \
	../transitionwidget.h \
	../window.h \
	../windowcontainer.h \
	../xml_functions.h \
	../xmlclient.h \
	../xmldevice.h

SOURCES += xmldevice_tester.cpp \
	test_main.cpp \
	test_bttime.cpp \
	test_clientwriter.cpp \
	test_delayedslotcaller.cpp \
	test_displaycontrol.cpp \
	test_imageselection.cpp \
	test_scenevodevicescond.cpp \
	test_xmlclient.cpp \
	test_xmldevice.cpp \
	test_functions.cpp \
	../devices/test/openserver_mock.cpp \
	../ts_10/audiostatemachine_x11.cpp \
	../btbutton.cpp \
	../bttime.cpp \
	../delayedslotcaller.cpp \
	../devices_cache.cpp \
	../displaycontrol.cpp \
	../fontmanager.cpp \
	../frame_classes.cpp \
	../frame_functions.cpp \
	../generic_functions.cpp \
	../hardware_functions_x11.cpp \
	../icondispatcher.cpp \
	../imageselectionhandler.cpp \
	../pagecontainer.cpp \
	../pagestack.cpp \
	../page.cpp \
	../openclient.cpp \
	../mediaplayer.cpp \
	../scaleconversion.cpp \
	../scenevodevicescond.cpp \
	../screensaver.cpp \
	../skinmanager.cpp \
	../statemachine.cpp \
	../styledwidget.cpp \
	../timescript.cpp \
	../transitionwidget.cpp \
	../window.cpp \
	../windowcontainer.cpp \
	../xml_functions.cpp \
	../xmlclient.cpp \
	../xmldevice.cpp \
	../definitions.cpp

include(../devices.pri)
