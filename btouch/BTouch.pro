######################################################################
# Manual mantained!
#
# This project file is designed to compile the Graphical User Interface
# for the BTicino touchscreen 3.5''. It shares large pieces of code with
# the BTicino touchscreen 10'' code.

LAYOUT = btouch
CONF_FILE = btouch

# Test architecture depending on the compiler used.
# In this case we are searching for the substring 'arm'
TEST_ARCH = $$find(QMAKE_CXX,arm)

isEmpty(TEST_ARCH) {
	# x86
	DEFINES += OPENSERVER_ADDR=\\\"btouch\\\"
}
else {
	HEADERS += QWSMOUSE/qmouse_qws.h \
		QWSMOUSE/qmouselinuxevent-2-6_qws.h

	SOURCES += QWSMOUSE/qmouse_qws.cpp \
		QWSMOUSE/qmouselinuxevent-2-6_qws.cpp

	INCLUDEPATH += QWSMOUSE
}

VPATH = ../devices ..
include(../common.pri)
include(../devices.pri)

SOURCES += bannercontent.cpp
