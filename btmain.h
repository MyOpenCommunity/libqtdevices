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
#include "termoregolaz.h"
#include "versio.h"
#include "genpage.h"
#include "calibrate.h"
#include "btlabel.h"

#include "../bt_stackopen/common_files/openwebnet.h"
#include "../bt_stackopen/common_files/common_functions.h"
#include <qmainwindow.h> 


#define BALL_NUM	5

class palla : public BtLabel
{
    Q_OBJECT
    public:
    palla(QWidget*parent=0, const char* name=0,unsigned int f=0);
    void clear();
    public slots:
        void paintEvent(QPaintEvent *); 
    private:
    int rx, ry;
};



class  BtMain : public QWidget
{
       Q_OBJECT
public:
    BtMain ( QWidget *parent=0, const char *name=0,QApplication*a=NULL);
    homePage * Home, *specPage;
    sottoMenu *illumino,*scenari,*carichi,*imposta,*automazioni,*sched;
    termoregolaz *termo;
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
    void testFiles();
     void svegl(bool);
    
private:
    QTimer  *  tempo1;
    QTimer  *  tempo2;
    QTimer  *  tempo3;	
    QWidget* pagDefault;
    char	pwd[10];
    bool	pwdOn,svegliaIsOn,alreadyCalibrated;
    tastiera *tasti;
    unsigned char firstTime,bloccato,backcol;
    genPage* screen;
    unsigned char tiposcreen;
     Calibrate* calib;
    QPixmap* Sfondo[12], *grab;
    BtLabel* screensav[12];//, *ball[10];
//    palla	*ball[BALL_NUM];
    BtLabel	*ball[BALL_NUM];
    int countScrSav,icx,icy,x[BALL_NUM],y[BALL_NUM],vx[BALL_NUM],vy[BALL_NUM],dim[BALL_NUM];
};

#endif// BTMAIN_H
