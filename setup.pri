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
# CONF_FILE = ts_3_5|ts_10
# before including this file in the project

# Change target name, to something like BTouch.arm or BTouch.x86
TRGT_SUFFIX =

contains(LAYOUT, ts_10) {
	DEFINES += LAYOUT_TS_10
} else {
	DEFINES += LAYOUT_TS_3_5
}
TRGT_NAME = BTouch

contains(CONF_FILE, ts_10) {
	DEFINES += CONFIG_TS_10
	message(Using TS 10\'\' config file.)
} else {
	DEFINES += CONFIG_TS_3_5
	message(Using TS 3.5\'\' config file.)
}


!isArm() {
	message(x86 architecture detected.)

	LIBS += -L../../common_files/lib/x86 -lcommon
	OBJECTS_DIR = obj/x86
	MOC_DIR = moc/x86
	HARDWARE = x11
	TRGT_SUFFIX = $${TRGT_SUFFIX}.x86
} else {
	message(ARM architecture detected.)
	LIBS += -L../../common_files -lcommon
	OBJECTS_DIR = obj/arm
	MOC_DIR = moc/arm
	DEFINES += BT_EMBEDDED

	HARDWARE = embedded
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

INCLUDEPATH += . .. ../devices ../../stackopen/common_files ../../stackopen ../../stackopen/common_develer/lib
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
#DEFINES += TRACK_IMAGES_MEMORY

