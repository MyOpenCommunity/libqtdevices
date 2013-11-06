TEMPLATE = lib
TARGET = libqtdevices

QT -= gui
QT += network

INCLUDEPATH += . ../common_files

# Input
HEADERS += airconditioning_device.h \
           antintrusion_device.h \
           automation_device.h \
           bacnet_device.h \
           bttime.h \
           definitions.h \
           delayedslotcaller.h \
           device.h \
           devices_cache.h \
           energy_device.h \
           frame_classes.h \
           frame_functions.h \
           lighting_device.h \
           loads_device.h \
           media_device.h \
           message_device.h \
           openclient.h \
           platform_device.h \
           probe_device.h \
           pulldevice.h \
           scenario_device.h \
           stopandgo_device.h \
           thermal_device.h \
           videodoorentry_device.h \

SOURCES += airconditioning_device.cpp \
           antintrusion_device.cpp \
           automation_device.cpp \
           bacnet_device.cpp \
           bttime.cpp \
           definitions.cpp \
           delayedslotcaller.cpp \
           device.cpp \
           devices_cache.cpp \
           energy_device.cpp \
           frame_classes.cpp \
           frame_functions.cpp \
           lighting_device.cpp \
           loads_device.cpp \
           media_device.cpp \
           message_device.cpp \
           openclient.cpp \
           platform_device.cpp \
           probe_device.cpp \
           pulldevice.cpp \
           scenario_device.cpp \
           stopandgo_device.cpp \
           thermal_device.cpp \
           videodoorentry_device.cpp \
