######################################################################
# Manual mantained!
#
# This file contains all the common stuff for the Graphical User Interface


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
	message(Using TS 10\'\' config file.)
} else {
	DEFINES += CONFIG_TS_3_5
	message(Using TS 3.5\'\' config file.)
}


isEmpty(TEST_ARCH) {
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
DEFINES += QT_QWS_EBX BTWEB

LIBS += -lssl

INCLUDEPATH += . .. ../devices ../../stackopen/common_files ../../stackopen ../../stackopen/common_develer/lib
DEPENDPATH += . .. ../devices
QT += network xml

# note: do not use spaces to split values below
TARGET = $$TRGT_NAME$$TRGT_SUFFIX
message(Target name: $$TARGET)

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
QMAKE_CXXFLAGS_DEBUG += -O0 -g3 -ggdb
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

contains(HARDWARE, x11) {
	SOURCES += hardware_functions_x11.cpp
	SOURCES += audiostatemachine_x11.cpp
}
contains(HARDWARE, embedded) {
	SOURCES += hardware_functions.cpp
	SOURCES += audiostatemachine.cpp
	HEADERS += calibration.h
	SOURCES += calibration.cpp
	HEADERS += calibration_widget.h
	SOURCES += calibration_widget.cpp
}

# Input
HEADERS += actuators.h \
	airconditioning.h \
	alarmclock.h \
	alarmpage.h \
	audiosource.h \
	bann_amplifiers.h \
	antintrusion.h \
	audiostatemachine.h \
	automation.h \
	aux.h \
	bann1_button.h \
	bann2_buttons.h \
	bann3_buttons.h \
	bann4_buttons.h \
	bann_airconditioning.h \
	bann_antintrusion.h \
	bann_automation.h \
	bann_energy.h \
	bann_lighting.h \
	bann_scenario.h \
	bann_settings.h \
	bann_videodoorentry.h \
	banner.h \
	bannerpage.h \
	bannerfactory.h \
	bann_thermal_regulation.h \
	bannonoffcombo.h \
	btbutton.h \
	btmain.h \
	bttime.h \
	changedatetime.h \
	cleanscreen.h \
	contrast.h \
	datetime.h \
	devices_cache.h \
	displaycontrol.h \
	energy_data.h \
	energy_graph.h \
	energy_management.h \
	energy_rates.h \
	energy_view.h \
	feedmanager.h \
	feeditemwidget.h \
	feedparser.h \
	fileselector.h \
	fontmanager.h \
	frame_functions.h \
	generic_functions.h \
	hardware_functions.h \
	homepage.h \
	homewindow.h \
	icondispatcher.h \
	iconpage.h \
	iconwindow.h \
	imageselectionhandler.h \
	itemlist.h \
	itemlistpage.h \
	items.h \
	keypad.h \
	labels.h \
	lansettings.h \
	lighting.h \
	loads.h \
	load_management.h \
	main.h \
	mediaplayer.h \
	navigation_bar.h \
	openclient.h \
	page.h \
	pagecontainer.h \
	pagefactory.h \
	pagestack.h \
	plantmenu.h \
	poweramplifier.h \
	radio.h \
	ringtonesmanager.h \
	scaleconversion.h \
	scenario.h \
	scenevocond.h \
	scenevodevicescond.h \
	screensaver.h \
	screensaverpage.h \
	scrollablepage.h \
	sectionpage.h \
	singlechoicepage.h \
	skinmanager.h \
	specialpage.h \
	statemachine.h \
	state_button.h \
	stopngo.h \
	styledwidget.h \
	supervisionmenu.h \
	temperatureviewer.h \
	thermalmenu.h \
	thermalregulator.h \
	timescript.h \
	transitionwidget.h \
	version.h \
	videodoorentry.h \
	window.h \
	windowcontainer.h \
	xml_functions.h

SOURCES += actuators.cpp \
	airconditioning.cpp \
	alarmclock.cpp \
	alarmpage.cpp \
	audiosource.cpp \
	bann_amplifiers.cpp \
	antintrusion.cpp \
	automation.cpp \
	aux.cpp \
	bann1_button.cpp \
	bann2_buttons.cpp \
	bann3_buttons.cpp \
	bann4_buttons.cpp \
	bann_airconditioning.cpp \
	bann_antintrusion.cpp \
	bann_automation.cpp \
	bann_energy.cpp \
	bann_lighting.cpp \
	bann_scenario.cpp \
	bann_settings.cpp \
	bann_videodoorentry.cpp \
	banner.cpp \
	bannerpage.cpp \
	bannerfactory.cpp \
	bann_thermal_regulation.cpp \
	bannonoffcombo.cpp \
	btbutton.cpp \
	btmain.cpp \
	bttime.cpp \
	changedatetime.cpp \
	cleanscreen.cpp \
	contrast.cpp \
	datetime.cpp \
	definitions.cpp \
	devices_cache.cpp \
	displaycontrol.cpp \
	energy_data.cpp \
	energy_graph.cpp \
	energy_management.cpp \
	energy_rates.cpp \
	energy_view.cpp \
	feedmanager.cpp \
	feeditemwidget.cpp \
	feedparser.cpp \
	fileselector.cpp \
	fontmanager.cpp \
	frame_functions.cpp \
	generic_functions.cpp \
	homepage.cpp \
	homewindow.cpp \
	icondispatcher.cpp \
	iconpage.cpp \
	iconwindow.cpp \
	imageselectionhandler.cpp \
	itemlist.cpp \
	itemlistpage.cpp \
	items.cpp \
	keypad.cpp \
	labels.cpp \
	lansettings.cpp \
	lighting.cpp \
	loads.cpp \
	load_management.cpp \
	main.cpp \
	mediaplayer.cpp \
	navigation_bar.cpp \
	openclient.cpp \
	page.cpp \
	pagecontainer.cpp \
	pagefactory.cpp \
	pagestack.cpp \
	plantmenu.cpp \
	poweramplifier.cpp \
	radio.cpp \
	ringtonesmanager.cpp \
	scaleconversion.cpp \
	scenario.cpp \
	scenevocond.cpp \
	scenevodevicescond.cpp \
	screensaver.cpp \
	screensaverpage.cpp \
	scrollablepage.cpp \
	sectionpage.cpp \
	singlechoicepage.cpp \
	skinmanager.cpp \
	specialpage.cpp \
	statemachine.cpp \
	state_button.cpp \
	stopngo.cpp \
	styledwidget.cpp \
	supervisionmenu.cpp \
	temperatureviewer.cpp \
	thermalmenu.cpp \
	thermalregulator.cpp \
	timescript.cpp \
	transitionwidget.cpp \
	version.cpp \
	videodoorentry.cpp \
	window.cpp \
	windowcontainer.cpp \
	xml_functions.cpp

# Examples
SOURCES += examples/logoscreensaver/logoscreensaver.cpp
HEADERS += examples/logoscreensaver/logoscreensaver.h

