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
           xmlvarihandler.h
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
           bann3but.cpp \
           bann4But.cpp \
           bann_antintrusione.cpp \
           bannBut2Icon.cpp \
           bannButIcon.cpp \
           bannciclaz.cpp \
           bannciclaz3but.cpp \
           banner.cpp \
           bannfrecce.cpp \
           bannfullscreen.cpp \
           bannon2scr.cpp \
           bannOnDx.cpp \
           bannOnIcons.cpp \
           bannonoff.cpp \
           bannonoff2scr.cpp \
           bannonoffcombo.cpp \
           bannpuls.cpp \
           bannRegolaz.cpp \
           bannsettings.cpp \
           banntemperature.cpp \
           btbutton.cpp \
           btlabel.cpp \
           BtMain.cpp \
           bttime.cpp \
           btwidgets.cpp \
           buttons_bar.cpp \
           calibrate.cpp \
           carico.cpp \
           contrpage.cpp \
           device.cpp \
           device_cache.cpp \
           device_status.cpp \
           diffmulti.cpp \
           diffSonora.cpp \
           dimmers.cpp \
           fontmanager.cpp \
           frame_interpreter.cpp \
           genericfunz.cpp \
           genpage.cpp \
           homePage.cpp \
           icondispatcher.cpp \
           impostatime.cpp \
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
           ../bt_stackopen/common_files/openwebnet.cpp \
           ../bt_stackopen/common_files/common_functions.cpp \
           ../bt_stackopen/common_files/gestione_socket.cpp

TRANSLATIONS += cfg/extra/2/btouch_it.ts \
                cfg/extra/2/btouch_de.ts \
                cfg/extra/2/btouch_es.ts \
                cfg/extra/2/btouch_fr.ts \
                cfg/extra/2/btouch_nl.ts \
                cfg/extra/2/btouch_pt.ts \
                cfg/extra/2/btouch_ru.ts \
                cfg/extra/2/btouch_ar.ts \
                cfg/extra/2/btouch_zh_CN.ts \
                cfg/extra/2/btouch_zh_TW.ts

