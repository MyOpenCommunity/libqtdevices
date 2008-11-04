######################################################################
# Manual mantained!

TEMPLATE = app

CONFIG+= qt debug warn_on
DEFINES += QT_QWS_EBX BT_EMBEDDED BTWEB
LIBS+= -L../common_files -lcommon -lssl
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
           antintrusione.h \
           automatismi.h \
           aux.h \
           bann2but.h \
           bann2butlab.h \
           bann3but.h \
           bann4but.h \
           bann4taslab.h \
           bann_antintrusione.h \
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
           bannsettings.h \
           banntemperature.h \
           btbutton.h \
           btlabel.h \
           btmain.h \
           btstyle.h \
           bttime.h \
           btwidgets.h \
           buttons_bar.h \
           calibrate.h \
           carico.h \
           cleanscreen.h \
           contrpage.h \
           device.h \
           device_cache.h \
           device_status.h \
           diffmulti.h \
           diffsonora.h \
           dimmers.h \
           fontmanager.h \
           frame_interpreter.h \
           genericfunz.h \
           genpage.h \
           homepage.h \
           icondispatcher.h \
           impostatime.h \
           lansettings.h \
           listbrowser.h \
           main.h \
           mediaplayer.h \
           multimedia_source.h \
           openclient.h \
           plantmenu.h \
           playwindow.h \
           postoext.h \
           poweramplifier.h \
           poweramplifier_device.h \
           radio.h \
           scaleconversion.h \
           scenari.h \
           scenevocond.h \
           screensaver.h \
           setitems.h \
           sorgentiaux.h \
           sorgentimedia.h \
           sorgentiradio.h \
           sottomenu.h \
           stopngo.h \
           supervisionmenu.h \
           sveglia.h \
           tastiera.h \
           thermalmenu.h \
           timescript.h \
           titlelabel.h \
           versio.h \
           videocitof.h \
           xmlconfhandler.h \
           xmlvarihandler.h \

SOURCES += actuators.cpp \
           allarme.cpp \
           ambdiffson.cpp \
           amplificatori.cpp \
           antintrusione.cpp \
           automatismi.cpp \
           aux.cpp \
           bann4taslab.cpp \
           bann2but.cpp \
           bann2butlab.cpp \
           bann3but.cpp \
           bann4but.cpp \
           bann_antintrusione.cpp \
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
           bannsettings.cpp \
           banntemperature.cpp \
           btbutton.cpp \
           btlabel.cpp \
           btmain.cpp \
           btstyle.cpp \
           bttime.cpp \
           btwidgets.cpp \
           buttons_bar.cpp \
           calibrate.cpp \
           carico.cpp \
           cleanscreen.cpp \
           contrpage.cpp \
           device.cpp \
           device_cache.cpp \
           device_status.cpp \
           diffmulti.cpp \
           diffsonora.cpp \
           dimmers.cpp \
           fontmanager.cpp \
           frame_interpreter.cpp \
           genericfunz.cpp \
           genpage.cpp \
           homepage.cpp \
           icondispatcher.cpp \
           impostatime.cpp \
           lansettings.cpp \
           listbrowser.cpp \
           main.cpp \
           mediaplayer.cpp \
           multimedia_source.cpp \
           openclient.cpp \
           plantmenu.cpp \
           playwindow.cpp \
           postoext.cpp \
           poweramplifier.cpp \
           poweramplifier_device.cpp \
           radio.cpp \
           scaleconversion.cpp \
           scenari.cpp \
           scenevocond.cpp \
           screensaver.cpp \
           setitems.cpp \
           sorgentiaux.cpp \
           sorgentimedia.cpp \
           sorgentiradio.cpp \
           sottomenu.cpp \
           stopngo.cpp \
           supervisionmenu.cpp \
           sveglia.cpp \
           tastiera.cpp \
           thermalmenu.cpp \
           timescript.cpp \
           titlelabel.cpp \
           versio.cpp \
           videocitof.cpp \
           xmlconfhandler.cpp \
           xmlvarihandler.cpp \

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
