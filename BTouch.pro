######################################################################
# Manual mantained!

TEMPLATE = app

# determine the target platform on the basis of environment variable TARGET
PLATFORM = $$(ARCH)

contains(PLATFORM, arm) {
	LIBS+= -L../common_files -lcommon
	OBJECTS_DIR = obj/arm
	MOC_DIR = moc/arm
}

contains(PLATFORM, x86) {
	DEFINES += OPENSERVER_ADDR=\\\"btouch\\\"
	LIBS+= -L../common_files/lib/x86 -lcommon
	OBJECTS_DIR = obj/x86
	MOC_DIR = moc/x86
}

LIBS+= -lssl
CONFIG+= qt debug warn_on
DEFINES += QT_QWS_EBX BT_EMBEDDED BTWEB
INCLUDEPATH+= . QWSMOUSE ../bt_stackopen/common_files ../bt_stackopen ../bt_stackopen/common_develer/lib
QT += network xml

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
QMAKE_CXXFLAGS_DEBUG += -O0 -g3 -ggdb
QMAKE_CXXFLAGS_RELEASE += -O2
QMAKE_CXXFLAGS += -fexceptions

# Input
HEADERS += actuators.h \
           alarmclock.h \
           allarme.h \
           ambdiffson.h \
           amplificatori.h \
           antintrusion.h \
           automation.h \
           aux.h \
           bann1_button.h \
           bann2_buttons.h \
           bann2butlab.h \
           bann3but.h \
           bann4but.h \
           bann4taslab.h \
           bann_antintrusion.h \
           bann_automation.h \
           bann_lighting.h \
           bann_scenario.h \
           bann_settings.h \
           bann_videoentryphone.h \
           bannciclaz.h \
           banner.h \
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
           cleanscreen.h \
           contrast.h \
           datetime.h \
           device.h \
           device_status.h \
           devices_cache.h \
           displaycontrol.h \
           displaypage.h \
           energy_data.h \
           fontmanager.h \
           frame_interpreter.h \
           generic_functions.h \
           genpage.h \
           homepage.h \
           icondispatcher.h \
           impostatime.h \
           keypad.h \
           landevice.h \
           lansettings.h \
           lighting.h \
           listbrowser.h \
           loads.h \
           main.h \
           mediaplayer.h \
           multimedia_source.h \
           multisounddiff.h \
           openclient.h \
           page.h \
           pagecontainer.h \
           plantmenu.h \
           playwindow.h \
           poweramplifier.h \
           poweramplifier_device.h \
           radio.h \
           scaleconversion.h \
           scenario.h \
           scenevocond.h \
           screensaver.h \
           screensaverpage.h \
           settings.h \
           singlechoicepage.h \
           sorgentiaux.h \
           sorgentimedia.h \
           sorgentiradio.h \
           sottomenu.h \
           sounddiffusion.h \
           specialpage.h \
           stopngo.h \
           supervisionmenu.h \
           temperatureviewer.h \
           thermalmenu.h \
           timescript.h \
           titlelabel.h \
           version.h \
           videoentryphone.h \
           xml_functions.h \

SOURCES += actuators.cpp \
           alarmclock.cpp \
           allarme.cpp \
           ambdiffson.cpp \
           amplificatori.cpp \
           antintrusion.cpp \
           automation.cpp \
           aux.cpp \
           bann1_button.cpp \
           bann2_buttons.cpp \
           bann4taslab.cpp \
           bann2butlab.cpp \
           bann3but.cpp \
           bann4but.cpp \
           bann_antintrusion.cpp \
           bann_automation.cpp \
           bann_lighting.cpp \
           bann_scenario.cpp \
           bann_settings.cpp \
           bann_videoentryphone.cpp \
           bannciclaz.cpp \
           banner.cpp \
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
           cleanscreen.cpp \
           contrast.cpp \
           datetime.cpp \
           device.cpp \
           device_status.cpp \
           devices_cache.cpp \
           displaycontrol.cpp \
           displaypage.cpp \
           energy_data.cpp \
           fontmanager.cpp \
           frame_interpreter.cpp \
           generic_functions.cpp \
           genpage.cpp \
           homepage.cpp \
           icondispatcher.cpp \
           impostatime.cpp \
           keypad.cpp \
           landevice.cpp \
           lansettings.cpp \
           lighting.cpp \
           listbrowser.cpp \
           loads.cpp \
           main.cpp \
           mediaplayer.cpp \
           multimedia_source.cpp \
           multisounddiff.cpp \
           openclient.cpp \
           page.cpp \
           pagecontainer.cpp \
           plantmenu.cpp \
           playwindow.cpp \
           poweramplifier.cpp \
           poweramplifier_device.cpp \
           radio.cpp \
           scaleconversion.cpp \
           scenario.cpp \
           scenevocond.cpp \
           screensaver.cpp \
           screensaverpage.cpp \
           settings.cpp \
           singlechoicepage.cpp \
           sorgentiaux.cpp \
           sorgentimedia.cpp \
           sorgentiradio.cpp \
           sottomenu.cpp \
           sounddiffusion.cpp \
           specialpage.cpp \
           stopngo.cpp \
           supervisionmenu.cpp \
           temperatureviewer.cpp \
           thermalmenu.cpp \
           timescript.cpp \
           titlelabel.cpp \
           version.cpp \
           videoentryphone.cpp \
           xml_functions.cpp \

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
                linguist-ts/btouch_zh_TW.ts
