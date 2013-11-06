QT += network xml testlib
OBJECTS_DIR = obj
MOC_DIR = moc

DEFINES += QT_QWS_EBX BT_EMBEDDED BTWEB QT_NO_DEBUG_OUTPUT DEBUG

INCLUDEPATH+= . .. ../../common_files
INCLUDEPATH+= ../../common_develer/lib
DEPENDPATH+= . ..

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += debug silent
CONFIG -= release

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter

TEMPLATE = app

LIBS+= -L ../../common_files/lib/x86 -lcommon -lssl -lexpat

VPATH = ..

HEADERS += \
device_tester.h \
openserver_mock.h \
test_air_conditioning_device.h \
test_antintrusion_device.h \
test_automation_device.h \
test_bacnet_device.h \
test_checkaddress.h \
test_device.h \
test_devicescache.h \
test_energy_device.h \
test_lighting_device.h \
test_loads_device.h \
test_main.h \
test_media_device.h \
test_message_device.h \
test_platform_device.h \
test_probe_device.h \
test_pull_manager.h \
test_scenario_device.h \
test_stopandgo_device.h \
test_thermal_device.h \
test_videodoorentry_device.h \
../airconditioning_device.h \
../antintrusion_device.h \
../automation_device.h \
../bacnet_device.h \
../bttime.h \
../definitions.h \
../delayedslotcaller.h \
../device.h \
../devices_cache.h \
../energy_device.h \
../frame_classes.h \
../frame_functions.h \
../lighting_device.h \
../loads_device.h \
../media_device.h \
../message_device.h \
../openclient.h \
../platform_device.h \
../probe_device.h \
../pulldevice.h \
../scenario_device.h \
../stopandgo_device.h \
../thermal_device.h \
../videodoorentry_device.h \

SOURCES += \
device_tester.cpp \
openserver_mock.cpp \
test_air_conditioning_device.cpp \
test_antintrusion_device.cpp \
test_automation_device.cpp \
test_bacnet_device.cpp \
test_checkaddress.cpp \
test_device.cpp \
test_devicescache.cpp \
test_energy_device.cpp \
test_lighting_device.cpp \
test_loads_device.cpp \
test_main.cpp \
test_media_device.cpp \
test_message_device.cpp \
test_platform_device.cpp \
test_probe_device.cpp \
test_pull_manager.cpp \
test_scenario_device.cpp \
test_stopandgo_device.cpp \
test_thermal_device.cpp \
test_videodoorentry_device.cpp \
../airconditioning_device.cpp \
../antintrusion_device.cpp \
../automation_device.cpp \
../bacnet_device.cpp \
../bttime.cpp \
../definitions.cpp \
../delayedslotcaller.cpp \
../device.cpp \
../devices_cache.cpp \
../energy_device.cpp \
../frame_classes.cpp \
../frame_functions.cpp \
../lighting_device.cpp \
../loads_device.cpp \
../media_device.cpp \
../message_device.cpp \
../openclient.cpp \
../platform_device.cpp \
../probe_device.cpp \
../pulldevice.cpp \
../scenario_device.cpp \
../stopandgo_device.cpp \
../thermal_device.cpp \
../videodoorentry_device.cpp \

