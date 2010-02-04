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
}

VPATH = ../devices ..
include(../common.pri)
include(../devices.pri)

HEADERS += favoriteswidget.h \
           headerwidget.h \
           iconsettings.h \
           multimedia.h \
           multimedia_buttons.h \
           multimedia_filelist.h \
           slideshow.h \
           vctcall.h \
           videoplayer.h \
           webcam.h

SOURCES += favoriteswidget.cpp \
           headerwidget.cpp \
           iconsettings.cpp \
           multimedia.cpp \
           multimedia_buttons.cpp \
           multimedia_filelist.cpp \
           slideshow.cpp \
           vctcall.cpp \
           videoplayer.cpp \
           webcam.cpp
