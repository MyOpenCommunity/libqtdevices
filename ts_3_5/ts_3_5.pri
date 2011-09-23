######################################################################
# Manual mantained!
#
# This project file is designed to compile the Graphical User Interface
# for the BTicino touchscreen 3.5''. It shares large pieces of code with
# the BTicino touchscreen 10'' code.

LAYOUT = ts_3_5

include(../setup.pri)

# check if the hardware (passed using the HARDWARE var from the .pro file) is one
# of the supported platform and the qmake used is coherent with that.
defineTest(isHardwareSupported) {
	contains(HARDWARE, embedded-pxa255) | contains(HARDWARE, embedded-dm365) {
		isArm() {
			return(true)
		}
	}
	contains(HARDWARE, x11) {
		!isArm() {
			return (true)
		}
	}
	return (false)
}


!isHardwareSupported() {
	error(The hardware for ts3 is not supported or the qmake choose is not the right one.)
}

contains(HARDWARE, embedded-pxa255) {
	# The hardware platform for ts3 (PXA255).
	DEFINES += BT_HARDWARE_PXA255

	INCLUDEPATH += QWSMOUSE
	HEADERS += QWSMOUSE/qmouse_qws.h \
		QWSMOUSE/qmouselinuxevent-2-6_qws.h

	SOURCES += QWSMOUSE/qmouse_qws.cpp \
		QWSMOUSE/qmouselinuxevent-2-6_qws.cpp
}
contains(HARDWARE, embedded-dm365) {
	# The hardware platform for ts3 (DM365).
	DEFINES += BT_HARDWARE_DM365
}
contains(HARDWARE, x11) {
	# x11
	DEFINES += OPENSERVER_ADDR=\\\"btouch_3_5\\\"
	DEFINES += XML_SERVER_ADDRESS=\\\"btouch_3_5\\\"
	DEFINES += BT_HARDWARE_X11
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

HEADERS +=  audioplayer_ts3.h \
	aux.h \
	brightnesspage.h \
	displaypage.h \
	multimedia_ts3.h \
	scenevomanager.h \
	settings_ts3.h \
	transitioneffects.h

SOURCES += audioplayer_ts3.cpp \
	aux.cpp \
	brightnesspage.cpp \
	displaypage.cpp \
	multimedia_ts3.cpp \
	scenevomanager.cpp \
	settings_ts3.cpp \
	transitioneffects.cpp

OTHER_FILES = skin.xml


