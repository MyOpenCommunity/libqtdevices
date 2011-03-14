######################################################################
# Manual mantained!
#
# This project file is designed to compile the Graphical User Interface
# for the BTicino touchscreen 10''. It shares large pieces of code with
# the BTicino touchscreen 3.5'' code.

LAYOUT = ts_10
CONF_FILE = ts_10

# Test architecture depending on the compiler used.
# In this case we are searching for the substring 'arm'
TEST_ARCH = $$find(QMAKE_CXX,arm)

isEmpty(TEST_ARCH) {
	# x86
	DEFINES += OPENSERVER_ADDR=\\\"btouch_10\\\"
	DEFINES += XML_SERVER_ADDRESS=\\\"btouch_10\\\"
	DEFINES += BT_HARDWARE_X11
}
else {
	DEFINES += BT_HARDWARE_TS_10

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

HEADERS += favoriteswidget.h \
	headerwidget.h \
	iconsettings.h \
	messages.h \
	mount_watcher.h \
	multimedia.h \
	multimedia_buttons.h \
	multimedia_filelist.h \
	ipradio.h \
	scenevomanager.h \
	slideshow.h \
	sounddiffusionpage.h \
	vctcall.h \
	mediaplayerpage.h \
	videoplayer.h \
	audioplayer.h \
	webcam.h


SOURCES += favoriteswidget.cpp \
	headerwidget.cpp \
	iconsettings.cpp \
	messages.cpp \
	mount_watcher.cpp \
	multimedia.cpp \
	multimedia_buttons.cpp \
	multimedia_filelist.cpp \
	ipradio.cpp \
	scenevomanager.cpp \
	slideshow.cpp \
	sounddiffusionpage.cpp \
	vctcall.cpp \
	mediaplayerpage.cpp \
	videoplayer.cpp \
	audioplayer.cpp \
	webcam.cpp

OTHER_FILES += skin.xml
