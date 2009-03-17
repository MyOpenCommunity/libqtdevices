QT += network xml testlib
OBJECTS_DIR = obj
MOC_DIR = moc

DEFINES += QT_QWS_EBX BT_EMBEDDED BTWEB

INCLUDEPATH+= . .. ../../bt_stackopen/common_files ../../bt_stackopen
INCLUDEPATH+= ../../bt_stackopen/common_develer/lib

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter

TEMPLATE = app

LIBS+= -L ../../common_files/lib/x86 -lcommon -lssl

HEADERS += device_tester.h \
           test_device.h \
           test_landevice.h \
           test_energy_device.h \
           test_poweramplifier_device.h \
           openserver_mock.h \
           ../energy_device.h \
           ../poweramplifier_device.h \
           ../landevice.h \
           ../device.h \
           ../device_status.h \
           ../frame_interpreter.h \
           ../openclient.h \
           ../bttime.h \
           ../generic_functions.h \
           ../main.h

SOURCES += test_main.cpp \
           test_device.cpp \
           device_tester.cpp \
           test_landevice.cpp \
           test_energy_device.cpp \
           test_poweramplifier_device.cpp \
           openserver_mock.cpp \
           ../energy_device.cpp \
           ../poweramplifier_device.cpp \
           ../landevice.cpp \
           ../device.cpp \
           ../device_status.cpp \
           ../frame_interpreter.cpp \
           ../openclient.cpp \
           ../bttime.cpp \
           ../generic_functions.cpp \
           ../definitions.cpp


