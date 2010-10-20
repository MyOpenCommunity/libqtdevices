QT += core gui network

TARGET = server
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    messagereceiver.cpp

HEADERS  += mainwindow.h \
    messagereceiver.h
