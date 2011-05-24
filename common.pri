######################################################################
# Manual mantained!
#
# This file contains all the common files for the Graphical User Interface

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
	audioplayer.h \
	bann_amplifiers.h \
	antintrusion.h \
	audiostatemachine.h \
	automation.h \
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
	delayedslotcaller.h \
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
	frame_classes.h \
	frame_functions.h \
	generic_functions.h \
	hardware_functions.h \
	homepage.h \
	homewindow.h \
	icondispatcher.h \
	iconpage.h \
	iconwindow.h \
	imageselectionhandler.h \
	ipradio.h \
	itemlist.h \
	items.h \
	keypad.h \
	labels.h \
	lansettings.h \
	lighting.h \
	loads.h \
	load_management.h \
	main.h \
	mediaplayer.h \
	mediaplayerpage.h \
	multimedia.h \
	multimedia_buttons.h \
	multimedia_filelist.h \
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
	sounddiffusionpage.h \
	soundsources.h \
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
	treebrowser.h \
	version.h \
	videodoorentry.h \
	window.h \
	windowcontainer.h \
	xmlclient.h \
	xmldevice.h \
	xml_functions.h

SOURCES += actuators.cpp \
	airconditioning.cpp \
	alarmclock.cpp \
	alarmpage.cpp \
	audioplayer.cpp \
	bann_amplifiers.cpp \
	antintrusion.cpp \
	automation.cpp \
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
	delayedslotcaller.cpp \
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
	frame_classes.cpp \
	frame_functions.cpp \
	generic_functions.cpp \
	homepage.cpp \
	homewindow.cpp \
	icondispatcher.cpp \
	iconpage.cpp \
	iconwindow.cpp \
	imageselectionhandler.cpp \
	ipradio.cpp \
	itemlist.cpp \
	items.cpp \
	keypad.cpp \
	labels.cpp \
	lansettings.cpp \
	lighting.cpp \
	loads.cpp \
	load_management.cpp \
	main.cpp \
	mediaplayer.cpp \
	mediaplayerpage.cpp \
	multimedia.cpp \
	multimedia_buttons.cpp \
	multimedia_filelist.cpp \
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
	sounddiffusionpage.cpp \
	soundsources.cpp \
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
	treebrowser.cpp \
	version.cpp \
	videodoorentry.cpp \
	window.cpp \
	windowcontainer.cpp \
	xmlclient.cpp \
	xmldevice.cpp \
	xml_functions.cpp


CONFIG(examples) {
	message(Build examples)
	include(examples.pri)
}

OTHER_FILES += main.doc
OTHER_FILES += examples.doc

