######################################################################
# Manual mantained!
#
# This project file is designed to compile the Graphical User Interface
# for the BTicino touchscreen 10''. It shares large pieces of code with
# the BTicino touchscreen 3.5'' code.

LAYOUT = ts_10
CONF_FILE = ts_10

include(../setup.pri)

contains(HARDWARE, x11) {
	# x86
	DEFINES += OPENSERVER_ADDR=\\\"btouch_10\\\"
	DEFINES += XML_SERVER_ADDRESS=\\\"btouch_10\\\"
	DEFINES += BT_HARDWARE_X11
}
else {
	DEFINES += BT_HARDWARE_PXA270

	HEADERS += QWSMOUSE/qmouselinuxtp_qws.h QWSMOUSE/qmouse_qws.h
	SOURCES += QWSMOUSE/qmouselinuxtp_qws.cpp QWSMOUSE/qmouse_qws.cpp
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

HEADERS += audioplayer_ts10.h \
	favoriteswidget.h \
	headerwidget.h \
	mediaplayerpage.h \
	messages.h \
	mount_watcher.h \
	multimedia_buttons.h \
	multimedia_ts10.h \
	scenevomanager.h \
	settings_ts10.h \
	slideshow.h \
	vctcall.h \
	videoplayer.h \
	webcam.h


SOURCES += audioplayer_ts10.cpp \
	favoriteswidget.cpp \
	headerwidget.cpp \
	mediaplayerpage.cpp \
	messages.cpp \
	mount_watcher.cpp \
	multimedia_buttons.cpp \
	multimedia_ts10.cpp \
	scenevomanager.cpp \
	settings_ts10.cpp \
	slideshow.cpp \
	vctcall.cpp \
	videoplayer.cpp \
	webcam.cpp

OTHER_FILES += skin.xml
