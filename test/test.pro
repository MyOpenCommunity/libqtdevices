QT += network xml testlib
OBJECTS_DIR = obj
MOC_DIR = moc

DEFINES += QT_QWS_EBX BT_EMBEDDED BTWEB

INCLUDEPATH+= . .. ../../bt_stackopen/common_files ../../bt_stackopen
INCLUDEPATH+= ../../bt_stackopen/common_develer/lib

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

LIBS+= -L ../../common_files/lib/x86 -lcommon -lssl

HEADERS += test_device.h \
           test_landevice.h \
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
           test_landevice.cpp \
           ../landevice.cpp \
           ../device.cpp \
           ../device_status.cpp \
           ../frame_interpreter.cpp \
           ../openclient.cpp \
           ../bttime.cpp \
           ../generic_functions.cpp \
           ../definitions.cpp


