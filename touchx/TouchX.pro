######################################################################
# Manual mantained!

LAYOUT = touchx
CONF_FILE = touchx

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

SOURCES += bannercontent_touchx.cpp \
           favoriteswidget.cpp \
           headerwidget.cpp \
           iconsettings.cpp \
           multimedia.cpp \
           multimedia_buttons.cpp \
           multimedia_filelist.cpp \
           slideshow.cpp \
           vctcall.cpp \
           videoplayer.cpp \
           webcam.cpp
