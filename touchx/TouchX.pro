######################################################################
# Manual mantained!
#
# This project file is designed to compile the Graphical User Interface
# for the BTicino touchscreen 10''. It shares large pieces of code with
# the BTicino touchscreen 3.5'' code.

LAYOUT = touchx
CONF_FILE = touchx

# Test architecture depending on the compiler used.
# In this case we are searching for the substring 'arm'
TEST_ARCH = $$find(QMAKE_CXX,arm)

isEmpty(TEST_ARCH) {
	# x86
	DEFINES += OPENSERVER_ADDR=\\\"touchx\\\"
	DEFINES += BT_HARDWARE_X11
}
else {
	DEFINES += BT_HARDWARE_TOUCHX

	HEADERS += QWSMOUSE/qmouselinuxtp_qws.h QWSMOUSE/qmouse_qws.h
	SOURCES += QWSMOUSE/qmouselinuxtp_qws.cpp QWSMOUSE/qmouse_qws.cpp
}

VPATH = ../devices ..
include(../common.pri)
include(../devices.pri)

HEADERS += favoriteswidget.h \
           headerwidget.h \
           iconsettings.h \
           messages.h \
           mount_watcher.h \
           multimedia.h \
           multimedia_buttons.h \
           multimedia_filelist.h \
           radio.h \
           scenevomanager.h \
           slideshow.h \
           sounddiffusionpage.h \
           vctcall.h \
           videoplayer.h \
           webcam.h


SOURCES += favoriteswidget.cpp \
           headerwidget.cpp \
           iconsettings.cpp \
           messages.cpp \
           mount_watcher.cpp \
           multimedia.cpp \
           multimedia_buttons.cpp \
           multimedia_filelist.cpp \
           radio.cpp \
           scenevomanager.cpp \
           slideshow.cpp \
           sounddiffusionpage.cpp \
           vctcall.cpp \
           videoplayer.cpp \
           webcam.cpp
