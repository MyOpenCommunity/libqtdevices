######################################################################
# Manual mantained!
#
# This project file is designed to compile the Graphical User Interface
# for the BTicino touchscreen 3.5''. It shares large pieces of code with
# the BTicino touchscreen 10'' code.

LAYOUT = ts_3_5
CONF_FILE = ts_3_5

# Test architecture depending on the compiler used.
# In this case we are searching for the substring 'arm'
TEST_ARCH = $$find(QMAKE_CXX,arm)

isEmpty(TEST_ARCH) {
	# x86
	DEFINES += OPENSERVER_ADDR=\\\"btouch_3_5\\\"
	DEFINES += BT_HARDWARE_X11
}
else {
	DEFINES += BT_HARDWARE_TS_3_5
	INCLUDEPATH += QWSMOUSE
	HEADERS += QWSMOUSE/qmouse_qws.h \
		QWSMOUSE/qmouselinuxevent-2-6_qws.h

	SOURCES += QWSMOUSE/qmouse_qws.cpp \
		QWSMOUSE/qmouselinuxevent-2-6_qws.cpp
}

VPATH = ../devices ..
include(../common.pri)
include(../devices.pri)

TRANSLATIONS += linguist-ts/btouch_ar.ts \
		linguist-ts/btouch_de.ts \
		linguist-ts/btouch_def.ts \
		linguist-ts/btouch_en.ts \
		linguist-ts/btouch_es.ts \
		linguist-ts/btouch_fr.ts \
		linguist-ts/btouch_it.ts \
		linguist-ts/btouch_nl.ts \
		linguist-ts/btouch_pt.ts \
		linguist-ts/btouch_ru.ts \
		linguist-ts/btouch_zh_CN.ts \
		linguist-ts/btouch_zh_TW.ts \
		linguist-ts/btouch_hr.ts \
		linguist-ts/btouch_hu.ts \
		linguist-ts/btouch_sl.ts \
		linguist-ts/btouch_el.ts \
		linguist-ts/btouch_pl.ts \
		linguist-ts/btouch_tr.ts

# Add SOURCES and HEADERS with TS 3.5''-specific source files

HEADERS += displaypage.h \
	brightnesspage.h \
	scenevomanager.h \
	settings.h

SOURCES += displaypage.cpp \
	brightnesspage.cpp \
	scenevomanager.cpp \
	settings.cpp

OTHER_FILES = skin.xml
