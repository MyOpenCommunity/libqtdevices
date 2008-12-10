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
           allarme.h \
           ambdiffson.h \
           amplificatori.h \
           antintrusion.h \
           automation.h \
           aux.h \
           bann2but.h \
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
           bannbut2icon.h \
           bannbuticon.h \
           bannciclaz.h \
           bannciclaz3but.h \
           banner.h \
           bannfrecce.h \
           bannfullscreen.h \
           bannon2scr.h \
           bannondx.h \
           bannonicons.h \
           bannonoff.h \
           bannonoff2scr.h \
           bannonoffcombo.h \
           bannpuls.h \
           bannregolaz.h \
           banntemperature.h \
           brightnesspage.h \
           brightnesscontrol.h \
           btbutton.h \
           btmain.h \
           bttime.h \
           buttons_bar.h \
           calibrate.h \
           carico.h \
           cleanscreen.h \
           contrpage.h \
           datetime.h \
           device.h \
           device_cache.h \
           device_status.h \
           fontmanager.h \
           frame_interpreter.h \
           generic_functions.h \
           genpage.h \
           homepage.h \
           icondispatcher.h \
           impostatime.h \
           lansettings.h \
           lighting.h \
           listbrowser.h \
           main.h \
           mediaplayer.h \
           multimedia_source.h \
           multisounddiff.h \
           openclient.h \
           page.h \
           plantmenu.h \
           playwindow.h \
           poweramplifier.h \
           poweramplifier_device.h \
           radio.h \
           scaleconversion.h \
           scenario.h \
           scenevocond.h \
           screensaver.h \
           settings.h \
           sorgentiaux.h \
           sorgentimedia.h \
           sorgentiradio.h \
           sottomenu.h \
           sounddiffusion.h \
           stopngo.h \
           supervisionmenu.h \
           sveglia.h \
           tastiera.h \
           thermalmenu.h \
           timescript.h \
           titlelabel.h \
           versio.h \
           videoentryphone.h \
           xmlconfhandler.h \
           xml_functions.h \

SOURCES += actuators.cpp \
           allarme.cpp \
           ambdiffson.cpp \
           amplificatori.cpp \
           antintrusion.cpp \
           automation.cpp \
           aux.cpp \
           bann4taslab.cpp \
           bann2but.cpp \
           bann2butlab.cpp \
           bann3but.cpp \
           bann4but.cpp \
           bann_antintrusion.cpp \
           bann_automation.cpp \
           bann_lighting.cpp \
           bann_scenario.cpp \
           bann_settings.cpp \
           bann_videoentryphone.cpp \
           bannbut2icon.cpp \
           bannbuticon.cpp \
           bannciclaz.cpp \
           bannciclaz3but.cpp \
           banner.cpp \
           bannfrecce.cpp \
           bannfullscreen.cpp \
           bannon2scr.cpp \
           bannondx.cpp \
           bannonicons.cpp \
           bannonoff.cpp \
           bannonoff2scr.cpp \
           bannonoffcombo.cpp \
           bannpuls.cpp \
           bannregolaz.cpp \
           banntemperature.cpp \
           btbutton.cpp \
           btmain.cpp \
           brightnesspage.cpp \
           brightnesscontrol.cpp \
           bttime.cpp \
           buttons_bar.cpp \
           calibrate.cpp \
           carico.cpp \
           cleanscreen.cpp \
           contrpage.cpp \
           datetime.cpp \
           device.cpp \
           device_cache.cpp \
           device_status.cpp \
           fontmanager.cpp \
           frame_interpreter.cpp \
           generic_functions.cpp \
           genpage.cpp \
           homepage.cpp \
           icondispatcher.cpp \
           impostatime.cpp \
           lansettings.cpp \
           lighting.cpp \
           listbrowser.cpp \
           main.cpp \
           mediaplayer.cpp \
           multimedia_source.cpp \
           multisounddiff.cpp \
           openclient.cpp \
           plantmenu.cpp \
           playwindow.cpp \
           poweramplifier.cpp \
           poweramplifier_device.cpp \
           radio.cpp \
           scaleconversion.cpp \
           scenario.cpp \
           scenevocond.cpp \
           screensaver.cpp \
           settings.cpp \
           sorgentiaux.cpp \
           sorgentimedia.cpp \
           sorgentiradio.cpp \
           sottomenu.cpp \
           sounddiffusion.cpp \
           stopngo.cpp \
           supervisionmenu.cpp \
           sveglia.cpp \
           tastiera.cpp \
           thermalmenu.cpp \
           timescript.cpp \
           titlelabel.cpp \
           versio.cpp \
           videoentryphone.cpp \
           xmlconfhandler.cpp \
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
