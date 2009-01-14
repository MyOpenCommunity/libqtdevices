######################################################################
# Manually maintained
######################################################################

TEMPLATE = app

CONFIG+= qt debug warn_on
DEFINES += QT_QWS_EBX BT_EMBEDDED BTWEB
LIBS+= -L../common_files -lcommon -lssl
INCLUDEPATH+= . QWSMOUSE ../bt_stackopen/common_files ../bt_stackopen ../bt_stackopen/common_develer/lib

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
           btlabelevo.h \
           btmain.h \
           bttime.h \
           btwidgets.h \
           buttons_bar.h \
           calibrate.h \
           carico.h \
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
           supervisionMenu.h \
           sveglia.h \
           tastiera.h \
           thermalmenu.h \
           timescript.h \
           titlelabel.h \
           versio.h \
           videocitof.h \
           xmlconfhandler.h \
           xmlvarihandler.h \
           QWSMOUSE/qmouse_qws.h \
           QWSMOUSE/qmouselinuxtp_qws.h

SOURCES += actuators.cpp \
           allarme.cpp \
           ambdiffson.cpp \
           amplificatori.cpp \
           antintrusione.cpp \
           automatismi.cpp \
           aux.cpp \
           ban4tasLab.cpp \
           bann2But.cpp \
           bann2butLab.cpp \
           bann3But.cpp \
           bann4But.cpp \
           bann_antintrusione.cpp \
           bannBut2Icon.cpp \
           bannButIcon.cpp \
           bannciclaz.cpp \
           bannciclaz3but.cpp \
           banner.cpp \
           bannFrecce.cpp \
           bannfullscreen.cpp \
           bannOn2scr.cpp \
           bannOnDx.cpp \
           bannOnIcons.cpp \
           bannOnOff.cpp \
           bannOnOff2scr.cpp \
           bannonoffcombo.cpp \
           bannPuls.cpp \
           bannRegolaz.cpp \
           bannsettings.cpp \
           banntemperature.cpp \
           BTButton.cpp \
           BtLabel.cpp \
           BtMain.cpp \
           bttime.cpp \
           btwidgets.cpp \
           buttons_bar.cpp \
           calibrate.cpp \
           carico.cpp \
           contrPage.cpp \
           device.cpp \
           device_cache.cpp \
           device_status.cpp \
           diffmulti.cpp \
           diffSonora.cpp \
           dimmers.cpp \
           fontmanager.cpp \
           frame_interpreter.cpp \
           genericFunz.cpp \
           genPage.cpp \
           homePage.cpp \
           icondispatcher.cpp \
           impostatime.cpp \
           listbrowser.cpp \
           main.cpp \
           mediaplayer.cpp \
           multimedia_source.cpp \
           openClient.cpp \
           plantmenu.cpp \
           playwindow.cpp \
           postoext.cpp \
           poweramplifier.cpp \
           poweramplifier_device.cpp \
           radio.cpp \
           scaleconversion.cpp \
           scenari.cpp \
           scenEvoCond.cpp \
           screensaver.cpp \
           setItems.cpp \
           sorgentiaux.cpp \
           sorgentimedia.cpp \
           sorgentiradio.cpp \
           sottoMenu.cpp \
           stopngo.cpp \
           supervisionMenu.cpp \
           sveglia.cpp \
           tastiera.cpp \
           thermalmenu.cpp \
           timeScript.cpp \
           titlelabel.cpp \
           versio.cpp \
           videocitof.cpp \
           xmlconfhandler.cpp \
           xmlvarihandler.cpp \
           QWSMOUSE/qmouse_qws.cpp \
           QWSMOUSE/qmouselinuxtp_qws.cpp

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
