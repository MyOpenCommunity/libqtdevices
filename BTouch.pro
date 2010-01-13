######################################################################
# Manual mantained!

TEMPLATE = app

LAYOUT += btouch
#For compatibility reasons, default to BTouch conf file
CONF_FILE += btouch
# Change target name, to something like BTouch.arm or BTouch.x86.touchx
TRGT_NAME = BTouch
TRGT_SUFFIX =


contains(LAYOUT, touchx) {
	DEFINES += LAYOUT_TOUCHX
	DEST_PREFIX = touchx
	TRGT_NAME = TouchX
} else {
	DEFINES += LAYOUT_BTOUCH
	DEST_PREFIX = .
}
DESTDIR = $$DEST_PREFIX

contains(CONF_FILE, touchx) {
	message(Using TouchX config file.)
	TRGT_SUFFIX = $${TRGT_SUFFIX} ".touchx"
} else {
	DEFINES += CONFIG_BTOUCH
	message(Using BTouch config file.)
}

# test architecture depending on the compiler used.
# in this case we are searching for the substring 'arm'
TEST_ARCH = $$find(QMAKE_CXX,arm)
isEmpty(TEST_ARCH) {
	message(x86 architecture detected.)

	contains(LAYOUT, touchx) {
		DEFINES += OPENSERVER_ADDR=\\\"touchx\\\"
	} else {
		DEFINES += OPENSERVER_ADDR=\\\"btouch\\\"
	}

	DEFINES += MEDIASERVER_PATH=\\\"/video/mp3/bticino_test/\\\"
	LIBS += -L../common_files/lib/x86 -lcommon
	OBJECTS_DIR = $$DEST_PREFIX/obj/x86
	MOC_DIR = $$DEST_PREFIX/moc/x86
	HARDWARE = x11
	DEFINES += BT_HARDWARE_X11
	TRGT_SUFFIX = $${TRGT_SUFFIX}.x86
} else {
	message(ARM architecture detected.)
	LIBS += -L../common_files -lcommon
	OBJECTS_DIR = $$DEST_PREFIX/obj/arm
	MOC_DIR = $$DEST_PREFIX/moc/arm
	DEFINES += BT_EMBEDDED

	contains(LAYOUT, touchx) {
		# use the default Qt handler
	} else {
		HEADERS += QWSMOUSE/qmouse_qws.h \
			QWSMOUSE/qmouselinuxevent-2-6_qws.h

		SOURCES += QWSMOUSE/qmouse_qws.cpp \
			QWSMOUSE/qmouselinuxevent-2-6_qws.cpp
	}

	HARDWARE = btouch
	DEFINES += BT_HARDWARE_BTOUCH
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

INCLUDEPATH += . QWSMOUSE ../stackopen/common_files ../stackopen ../stackopen/common_develer/lib $(ARMLINUX)
QT += network xml

# note: do not use spaces to split values below
TARGET = $$TRGT_NAME$$TRGT_SUFFIX
message(Target name: $$TARGET)

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
QMAKE_CXXFLAGS_DEBUG += -O0 -g3 -ggdb
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

# Input
HEADERS += actuators.h \
           airconditioning.h \
           airconditioning_device.h \
           alarmclock.h \
           alarmsounddiff_device.h \
           alarmpage.h \
           ambdiffson.h \
           amplificatori.h \
           antintrusion.h \
           automation.h \
           automation_device.h \
           aux.h \
           bann1_button.h \
           bann2_buttons.h \
           bann3_buttons.h \
           bann4_buttons.h \
           bann_airconditioning.h \
           bann_antintrusion.h \
           bann_automation.h \
           bann_lighting.h \
           bann_scenario.h \
           bann_settings.h \
           bann_videoentryphone.h \
           bannciclaz.h \
           banner.h \
           bannercontent.h \
           bannerfactory.h \
           bannfrecce.h \
           bann_thermal_regulation.h \
           bannonoffcombo.h \
           bannregolaz.h \
           banntemperature.h \
           brightnesspage.h \
           btbutton.h \
           btmain.h \
           bttime.h \
           buttons_bar.h \
           calibrate.h \
           changedatetime.h \
           cleanscreen.h \
           contrast.h \
           datetime.h \
           device.h \
           device_status.h \
           devices_cache.h \
           displaycontrol.h \
           displaypage.h \
           enabling_button.h \
           energy_data.h \
           energy_device.h \
           energy_graph.h \
           energy_view.h \
           entryphone_device.h \
           favoriteswidget.h \
           feedmanager.h \
           feeditemwidget.h \
           feedparser.h \
           filebrowser.h \
           fileselector.h \
           fontmanager.h \
           frame_interpreter.h \
           frame_receiver.h \
           generic_functions.h \
           genpage.h \
           gridcontent.h \
           hardware_functions.h \
           headerwidget.h \
           homepage.h \
           homewindow.h \
           icondispatcher.h \
           iconpage.h \
           iconsettings.h \
           iconwindow.h \
           items.h \
           keypad.h \
           lansettings.h \
           lighting.h \
           lighting_device.h \
           listbrowser.h \
           loads.h \
           main.h \
           mediaplayer.h \
           multimedia.h \
           multimedia_buttons.h \
           multimedia_filelist.h \
           multimedia_source.h \
           multisounddiff.h \
           navigation_bar.h \
           openclient.h \
           page.h \
           pagecontainer.h \
           pagefactory.h \
           plantmenu.h \
           platform_device.h \
           playwindow.h \
           poweramplifier.h \
           poweramplifier_device.h \
           probe_device.h \
           pulldevice.h \
           radio.h \
           scaleconversion.h \
           scenario.h \
           scenario_device.h \
           scenevocond.h \
           screensaver.h \
           screensaverpage.h \
           sectionpage.h \
           settings.h \
           singlechoicecontent.h \
           singlechoicepage.h \
           skinmanager.h \
           slideshow.h \
           sorgentiaux.h \
           sorgentimedia.h \
           sorgentiradio.h \
           sottomenu.h \
           sounddiffusion.h \
           specialpage.h \
           stopngo.h \
           styledwidget.h \
           supervisionmenu.h \
           temperatureviewer.h \
           thermal_device.h \
           thermalmenu.h \
           timescript.h \
           titlelabel.h \
           transitionwidget.h \
           version.h \
           videoentryphone.h \
           vctcall.h \
           videoplayer.h \
           window.h \
           windowcontainer.h \
           xml_functions.h

SOURCES += actuators.cpp \
           airconditioning.cpp \
           airconditioning_device.cpp \
           alarmclock.cpp \
           alarmsounddiff_device.cpp \
           alarmpage.cpp \
           ambdiffson.cpp \
           amplificatori.cpp \
           antintrusion.cpp \
           automation.cpp \
           automation_device.cpp \
           aux.cpp \
           bann1_button.cpp \
           bann2_buttons.cpp \
           bann3_buttons.cpp \
           bann4_buttons.cpp \
           bann_airconditioning.cpp \
           bann_antintrusion.cpp \
           bann_automation.cpp \
           bann_lighting.cpp \
           bann_scenario.cpp \
           bann_settings.cpp \
           bann_videoentryphone.cpp \
           bannciclaz.cpp \
           banner.cpp \
           bannercontent.cpp \
           bannercontent_touchx.cpp \         
           bannerfactory.cpp \
           bannfrecce.cpp \
           bann_thermal_regulation.cpp \
           bannonoffcombo.cpp \
           bannregolaz.cpp \
           banntemperature.cpp \
           btbutton.cpp \
           btmain.cpp \
           brightnesspage.cpp \
           bttime.cpp \
           buttons_bar.cpp \
           calibrate.cpp \
           changedatetime.cpp \
           cleanscreen.cpp \
           contrast.cpp \
           datetime.cpp \
           definitions.cpp \
           device.cpp \
           device_status.cpp \
           devices_cache.cpp \
           displaycontrol.cpp \
           displaypage.cpp \
           enabling_button.cpp \
           energy_data.cpp \
           energy_device.cpp \
           energy_graph.cpp \
           energy_view.cpp \
           entryphone_device.cpp \
           favoriteswidget.cpp \
           feedmanager.cpp \
           feeditemwidget.cpp \
           feedparser.cpp \
           filebrowser.cpp \
           fileselector.cpp \
           fontmanager.cpp \
           frame_interpreter.cpp \
           frame_receiver.cpp \
           generic_functions.cpp \
           genpage.cpp \
           gridcontent.cpp \
           headerwidget.cpp \
           homepage.cpp \
           homewindow.cpp \
           icondispatcher.cpp \
           iconpage.cpp \
           iconsettings.cpp \
           iconwindow.cpp \
           items.cpp \
           keypad.cpp \
           lansettings.cpp \
           lighting.cpp \
           lighting_device.cpp \
           listbrowser.cpp \
           loads.cpp \
           main.cpp \
           mediaplayer.cpp \
           multimedia.cpp \
           multimedia_buttons.cpp \
           multimedia_filelist.cpp \
           multimedia_source.cpp \
           multisounddiff.cpp \
           navigation_bar.cpp \
           openclient.cpp \
           page.cpp \
           pagecontainer.cpp \
           pagefactory.cpp \
           plantmenu.cpp \
           platform_device.cpp \
           playwindow.cpp \
           poweramplifier.cpp \
           poweramplifier_device.cpp \
           probe_device.cpp \
           pulldevice.cpp \
           radio.cpp \
           scaleconversion.cpp \
           scenario.cpp \
           scenario_device.cpp \
           scenevocond.cpp \
           screensaver.cpp \
           screensaverpage.cpp \
           sectionpage.cpp \
           settings.cpp \
           singlechoicecontent.cpp \
           singlechoicepage.cpp \
           skinmanager.cpp \
           slideshow.cpp \
           sorgentiaux.cpp \
           sorgentimedia.cpp \
           sorgentiradio.cpp \
           sottomenu.cpp \
           sounddiffusion.cpp \
           specialpage.cpp \
           stopngo.cpp \
           styledwidget.cpp \
           supervisionmenu.cpp \
           temperatureviewer.cpp \
           thermal_device.cpp \
           thermalmenu.cpp \
           timescript.cpp \
           titlelabel.cpp \
           transitionwidget.cpp \
           version.cpp \
           videoentryphone.cpp \
           vctcall.cpp \
           videoplayer.cpp \
           window.cpp \
           windowcontainer.cpp \
           xml_functions.cpp

contains(HARDWARE, x11) {
	SOURCES += hardware_functions_x11.cpp
}
contains(HARDWARE, btouch) {
	SOURCES += hardware_functions.cpp
}

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
