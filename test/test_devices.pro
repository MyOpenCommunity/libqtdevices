QT += network xml testlib
OBJECTS_DIR = obj
MOC_DIR = moc

DEFINES += QT_QWS_EBX BT_EMBEDDED BTWEB QT_NO_DEBUG_OUTPUT DEBUG

INCLUDEPATH+= . .. ../../stackopen/common_files ../../stackopen
INCLUDEPATH+= ../../stackopen/common_develer/lib
DEPENDPATH+= . ..

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += debug silent
CONFIG -= release

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter

TEMPLATE = app

LIBS+= -L ../../stackopen/common_files/lib/x86 -lcommon -lssl

VPATH = ..

HEADERS += *.h
HEADERS += ../*.h

SOURCES += *.cpp
SOURCES += ../*.cpp

