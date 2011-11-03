######################################################################
# Manual mantained!
#
# This file contains all the common stuff for the Graphical User Interface


# We use an empirical test to recognize the platform.
defineTest(isArm) {
	# In this case we are searching for the substring 'arm' in the QMAKE_CXX
	# predefined variable, which usually contains the compiler name
	TEST_QMAKE_CXX = $$find(QMAKE_CXX,arm)
	!isEmpty(TEST_QMAKE_CXX) {
		return(true)
	}
	# With Open Embedded builds, QMAKE_CXX is only a reference to the OE_QMAKE_CXX
	# environment variable, so we cannot use the above test, but we have to extract
	# the value from OE_QMAKE_CXX and test it.
	OECXX = $$(OE_QMAKE_CXX)
	TEST_OE_QMAKE_CXX = $$find(OECXX,arm)
	!isEmpty(TEST_OE_QMAKE_CXX) {
		return(true)
	}
	return (false)
}

TEMPLATE = app

# You have to define:
# LAYOUT = ts_3_5|ts_10
# before including this file in the project

# Change target name, to something like BTouch.arm or BTouch.x86
TRGT_SUFFIX =

contains(LAYOUT, ts_10) {
	DEFINES += LAYOUT_TS_10
} else {
	DEFINES += LAYOUT_TS_3_5
}

TRGT_NAME = BTouch

!isArm() {
	message(x86 architecture detected.)

	LIBS += -L../../stackopen/common_files/lib/x86 -lcommon
	INCLUDEPATH += ../../stackopen/common_files
	OBJECTS_DIR = obj/x86
	MOC_DIR = moc/x86
	isEmpty(HARDWARE) {
		HARDWARE = x11
	}
	TRGT_SUFFIX = $${TRGT_SUFFIX}.x86
} else {
	message(ARM architecture detected.)

	contains(HARDWARE, embedded-pxa255) {
		LIBS += -L../../stackopen/common_files-pxa255 -lcommon
		INCLUDEPATH += ../../stackopen/common_files-pxa255
	}
	contains(HARDWARE, embedded-dm365) {
		LIBS += -L../../stackopen/common_files -lcommon -lexpat
		INCLUDEPATH += ../../stackopen/common_files
	}
	contains(HARDWARE, embedded-pxa270) {
		LIBS += -L../../common_files -lcommon
		INCLUDEPATH += ../../common_files
	}

	OBJECTS_DIR = obj/arm
	MOC_DIR = moc/arm
	DEFINES += BT_EMBEDDED
	QMAKE_CXXFLAGS += -Wno-psabi

	TRGT_SUFFIX = $${TRGT_SUFFIX}.arm
}

CONFIG += qt warn_on silent
CONFIG -= debug_and_release

CONFIG(debug,debug|release) {
	message(*** Debug build)
	DEFINES += DEBUG
}

CONFIG(release,debug|release) {
	message(*** Release build)
	DEFINES += NO_QT_DEBUG_OUTPUT
}

CONFIG(examples) {
	DEFINES += BUILD_EXAMPLES
}

DEFINES += QT_QWS_EBX BTWEB

LIBS += -lssl

INCLUDEPATH += . .. ../devices
DEPENDPATH += . .. ../devices
QT += network xml

# note: do not use spaces to split values below
TARGET = $$TRGT_NAME$$TRGT_SUFFIX
message(Target name: $$TARGET)

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
QMAKE_CXXFLAGS_DEBUG += -O0 -g
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3


# Custom defines for BTouch tests
#
# show the usage of memory during the loading of images (slideshow)
#DEFINES += TRACK_IMAGES_MEMORY
#
# produces random data to simulate the graphs of energy.
#DEFINES += TEST_ENERGY_GRAPH

