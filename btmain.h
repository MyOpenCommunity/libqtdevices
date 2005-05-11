/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** BtMain.cpp
**
**Apertura pagina iniziale e associazione tasti-sottomenù
**
****************************************************************/


#ifndef BTMAIN_H
#define BTMAIN_H

#include <qwidget.h>
#include "homepage.h"
#include "sottomenu.h"
#include "diffsonora.h"
#include "openclient.h"
#include "antintrusione.h"
#include "versio.h"
#include "genpage.h"

#include "../bt_stackopen/common_files/openwebnet.h"
#include "../bt_stackopen/common_files/common_functions.h"


class  BtMain : public QObject
{
       Q_OBJECT
public:
    BtMain ( QWidget *parent=0, const char *name=0,QApplication*a=NULL);
    homePage * Home, *specPage;
    sottoMenu *illumino,*scenari,*carichi,*imposta,*automazioni,*termo,*sched;
    diffSonora *difSon;
    antintrusione* antintr;
    Client * client_comandi;
    Client *  client_monitor; 
    versio* datiGen;
signals:
    void freeze(bool);
//    void sendFrame(char*);
private slots:    
    void hom();
    void myMain();
    void init();
    void gesScrSav();
    void freezed(bool);
//    void specFunz();
    void setPwd(bool,char*);
    void testPwd(char*);
private:
    QTimer  *  tempo1;
    QTimer  *  tempo2;
    QTimer  *  tempo3;	
    QWidget* pagDefault;
    char	pwd[10];
    bool	pwdOn;
    tastiera *tasti;
    unsigned char firstTime,bloccato;
    genPage* screen;
    unsigned char tiposcreen;
    
//    char specialFrame[50];
};

#endif// BTMAIN_H
