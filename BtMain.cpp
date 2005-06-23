/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** BtMain.cpp
**
**Apertura pagina iniziale e associazione tasti-sottomenù
**
****************************************************************/

#include <qapplication.h>
#include <qobject.h>
#include <qaction.h>
//#include <qcursor.h>
#include <qwidget.h>
#include <qptrlist.h> 
#include <qcursor.h>
#include "qwaitcondition.h"

#include "main.h"
#include "btmain.h"
#include "homepage.h"
#include "sottomenu.h"
#include "impostatime.h"
#include "diffsonora.h"
#include "sveglia.h"
#include "genericfunz.h"
//#include "structureparser.h"
#include "xmlconfhandler.h"
#include "xmlvarihandler.h"
#include "calibrate.h"
#include "btlabel.h"

#include <sys/sysinfo.h>
#include <qfontdatabase.h>
#include <qfile.h>
#include <qxml.h>
#include <qwindowdefs.h>
#include <stdlib.h>
#include <math.h>
#include <qbitmap.h>






BtMain::BtMain(QWidget *parent, const char *name,QApplication* a)
    : QWidget( parent, name )
    {
    
    
    /*******************************************
v** Socket 
*******************************************/
    qDebug("parte BtMain");
    
    client_comandi = new  Client("127.0.0.1",20000,0);
    client_monitor	 = new  Client("127.0.0.1",20000,1); 
    connect(client_comandi, SIGNAL(frameToAutoread(char*)), client_monitor,SIGNAL(frameIn(char*)));
    
    setBacklight(TRUE);
    setContrast(0x80,FALSE);
    
    rearmWDT();
    
    firstTime=1;
    pagDefault=NULL;
    Home=specPage=NULL;
    illumino=scenari=carichi=imposta=automazioni=sched=NULL;
    termo=NULL;
    difSon=NULL;
    antintr=NULL;
    screen=NULL;
    
backcol=0;

    for (int idx=0;idx<12;idx++)
    {
        screensav[idx]=new BtLabel("",/*NULLscrsav*/this,"g");
       screensav[idx]->setFrameStyle(QFrame::Panel | QFrame::Raised);
       screensav[idx]-> hide();
       Sfondo[idx]=NULL;
    }
     for (int idx=0;idx<12;idx++)
    {
       ball[idx]=new BtLabel(this,"",0);
//              ball[idx]=new palla/*BtLabel*/(/*NULLscrsav*/this,"",0);
       ball[idx]->hide();       
   }
    grab=NULL;
        
    pwdOn=0;
   
    datiGen = new versio(NULL, "DG");
    struct sysinfo info;
    sysinfo(&info);
    qDebug("uptime: %d",info.uptime);
    qDebug("touch ago: %d",getTimePress());
    
    
    if (   (QFile::exists("/etc/pointercal")) && ( (info.uptime>200) || ( (info.uptime-1)<=getTimePress() )  ) )
    {
        tempo1 = new QTimer(this,"clock");
        tempo1->start(200);
        connect(tempo1,SIGNAL(timeout()),this,SLOT(hom()));
        datiGen->show();            
    }
    else
    {
        tempo1=NULL;
        /*Calibrate**/ calib = new Calibrate(NULL,"calibrazione",(unsigned char)0,(unsigned char)1);
                       calib->show(); 
                       connect(calib, SIGNAL(fineCalib()), this,SLOT(hom()));
#if defined (BTWEB) ||  defined (BT_EMBEDDED)              
                       connect(calib, SIGNAL(fineCalib()), datiGen,SLOT(showFullScreen()));
#endif
#if !defined (BTWEB) && !defined (BT_EMBEDDED)    
                       connect(calib, SIGNAL(fineCalib()), datiGen,SLOT(show()));
#endif                 
                       
                   }     
}

void BtMain::hom()
{
    if (tempo1)  
        delete(tempo1);   
    else
    {
            tempo1 = new QTimer(this,"clock");
            tempo1->start(200);
            connect(tempo1,SIGNAL(timeout()),this,SLOT(hom()));
            return;
        }
    
    /*   QFontInfo(Home->font());
         
    qDebug( "FONT FAMILY=%s",(Home->font()).family().ascii());
    qDebug( "FONT PIXEL SIZE=%d",(Home->font()).pixelSize());
    qDebug( "FONT POINT SIZE=%d",(Home->font()).pointSize());
    qDebug( "FONT ITALIC=%d",(Home->font()).italic());
    qDebug( "FONT WEIGHT=%d",(Home->font()).weight());
    qDebug( "FONT BOLD=%d",(Home->font()).bold());
    qDebug( "FONT WEIGHT=%d",(Home->font()).weight());
    
    QFontDatabase fdb;
    QStringList families = fdb.families();
    for ( QStringList::Iterator f = families.begin(); f != families.end(); ++f ) {
        QString family = *f;
        qDebug( family );
        QStringList styles = fdb.styles( family );
        for ( QStringList::Iterator s = styles.begin(); s != styles.end(); ++s ) {
            QString style = *s;
            QString dstyle = "\t" + style + " (";
            QValueList<int> smoothies = fdb.smoothSizes( family, style );
            for ( QValueList<int>::Iterator points = smoothies.begin();
                  points != smoothies.end(); ++points ) {
                dstyle += QString::number( *points ) + " ";
            }
            dstyle = dstyle.left( dstyle.length() - 1 ) + ")";
            qDebug( dstyle );
        }
    }
     qDebug( "FINE ANALISI FONT");
     
//    qDebug( "FONT FIXED PITCH=%d",(Home.font()).fixedPitch());
//    qDebug( "FONT RAWMODE=%d",(Home.font()).rawMode());
//    qDebug( "FONT EXACTMATCH=%d",(Home.font()).exactMatch());
*/	 
    datiGen->inizializza();    	 
    
    
    //--------------------------------------------------
    QColor *bg, *fg1, *fg2;
    QFile * xmlFile;
    bg=fg1=fg2=NULL;
    
    xmlskinhandler *handler1 = new xmlskinhandler(&bg, &fg1, &fg2);
    xmlFile = new QFile("cfg/extra.xml");
    QXmlInputSource source1( xmlFile );
    QXmlSimpleReader reader1;
    reader1.setContentHandler( handler1 );
    reader1.parse( source1 );
    delete handler1;
    delete xmlFile;
    /*  if ( (bg) && (fg1) && (fg2) )
      qDebug("COLORI:\n%d - %d - %d\n%d - %d - %d\n%d - %d - %d",bg->red(),bg->green(),bg-> blue(),fg1->red(), fg1->green(), fg1-> blue(),fg2->red(), fg2->green(), fg2-> blue());*/
    if (!bg)
        bg=new QColor(77,61,66);
    if (!fg1)
        fg1=new QColor(205,205,205);
    if (!bg)
        fg2=new QColor(7,151,254);
    
    //-----------------------------------------------
    xmlconfhandler  * handler2=new xmlconfhandler(this, &Home,&specPage, &illumino,&scenari,&carichi,&imposta, &automazioni, &termo,&difSon, &antintr,&pagDefault, client_comandi, client_monitor, datiGen, &sched,bg, fg1, fg2);
    
    setBackgroundColor(*bg);
     for (int idx=0;idx<12;idx++)
    {
       ball[idx] -> setBackgroundColor(*bg);  
       ball[idx] -> setBackgroundMode(Qt::NoBackground);
   }
    //setForegroundColor(*fg1);
    
    xmlFile = new QFile("cfg/conf.xml");
    QXmlInputSource source2( xmlFile );
    QXmlSimpleReader reader2;
    qDebug("parte parsing");
    reader2.setContentHandler( handler2 );
    reader2.parse( source2 );
    qDebug("finito parsing");
    delete handler2;
    delete xmlFile;
    
    qApp->setMainWidget( Home);   
    hide();
    setGeometry(0,0,240,320);
    setCursor (QCursor (blankCursor));
    setBackgroundColor(QColor(255,255,255));
        
    tempo3 = new QTimer(this,"clock");
    tempo3->start(10);
    connect(tempo3,SIGNAL(timeout()),this,SLOT(myMain()));
}


void BtMain::init()
{
    connect(client_monitor,SIGNAL(frameIn(char *)),datiGen,SLOT(gestFrame(char *))); 
    connect(datiGen,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));       
    
    if (datiGen)
        datiGen->inizializza();
    if(illumino)
        illumino->inizializza();
    if(automazioni)
        automazioni->inizializza();
    if(antintr)
        antintr->inizializza();
    if(difSon)
        difSon->inizializza();     
    if(scenari)
        scenari->inizializza();
    if(imposta)
        imposta->inizializza();
    if(termo)
        termo->inizializza();
    //    rearmWDT();
    
    struct sysinfo info;
    sysinfo(&info);
    //    qDebug("uptime= %d - timePress= %d", info.uptime, getTimePress() );
    if ( (info.uptime<200) && ( (info.uptime-1)>getTimePress() )  ) 
    {	
        calib = new Calibrate(NULL,"calibrazione",0,1);
        
#if defined (BTWEB) ||  defined (BT_EMBEDDED)          
        Home->hide();
        calib->show();//FullScreen(); con fullscreen non va!
        connect(calib, SIGNAL(fineCalib()),Home,SLOT(showFullScreen()));
#endif
#if !defined (BTWEB) && !defined (BT_EMBEDDED)    
        calib->show(); 
        //  connect(calib, SIGNAL(fineCalib()), Home,SLOT(show()));
#endif              
    }         
}




void BtMain::myMain()
{
    qDebug("entro MyMain");
    
    delete(tempo3);
    
    Home->showFullScreen();
    datiGen->hide();
    
    connect(client_monitor,SIGNAL(monitorSu()),this,SLOT(init()));
    client_monitor->connetti();
    
    tempo1 = new QTimer(this,"clock");
    tempo1->start(2000);
    disconnect(tempo1,SIGNAL(timeout()),this,SLOT(hom()));
    connect(tempo1,SIGNAL(timeout()),this,SLOT(gesScrSav()));
    
    tempo2 = new QTimer(this,"clock");
    tempo2->start(3000);
    connect(tempo2,SIGNAL(timeout()),this,SLOT(testFiles()));      
}

void BtMain::testFiles()
{
    if (QFile::exists(FILE_TEST1))
    { 
        if ( (screen) && (tiposcreen!=RED))
        {
            delete(screen);
            screen=NULL;
        }
        else if (!screen)
        {
                tiposcreen=RED;
                screen = new genPage(NULL,"red",RED);
                screen->show();
                qDebug("TEST1");
                setBacklight(TRUE);
                tempo1->stop();                
            }
    }
    else if (QFile::exists(FILE_TEST2))
    {
        if ( (screen)&& (tiposcreen!=GREEN))	
        {
            delete(screen);
            screen=NULL;
        }
        else if (!screen)
        {
                tiposcreen=GREEN;
                screen = new genPage(NULL,"green",GREEN);
                screen->show();
                qDebug("TEST2");
                setBacklight(TRUE);	
                tempo1->stop();                
            }
    }
    else if (QFile::exists(FILE_TEST3))
    {
        if ( (screen)&& (tiposcreen!=BLUE))	
        {
            delete(screen);
            screen=NULL;
        }
        else if(!screen)
        {
                tiposcreen=BLUE;
                screen = new genPage(NULL,"blu",BLUE);
                screen->show();
                qDebug("TEST3");
                setBacklight(TRUE);
                tempo1->stop();
            }
    }
    else if (QFile::exists(FILE_AGGIORNAMENTO))
    {
        if ((screen)&& (tiposcreen!=IMAGE))	
        {
            delete(screen);
            screen=NULL;
        }
        else if (!screen)
        {
                screen = new genPage(NULL,"blu",IMAGE,"cfg/skin/dwnpage.png");
                tiposcreen=IMAGE;	    
                screen->show();
                qDebug("AGGIORNAMENTO");
                setBacklight(TRUE);	
                tempo1->stop();
            }	
    }
    else
    {
        if (screen)
        {
                delete(screen);
                screen=NULL;
                tiposcreen=NONE;
                tempo1->start(2000);
            }
    }
}


void BtMain::gesScrSav()
{
    unsigned long tiempo;
    rearmWDT();  
    
    tiempo= getTimePress();
    
    if (!firstTime)
    {
        if  ( (tiempo>=30) && (getBacklight())) 
        {
#ifndef BACKLIGHT_SEMPRE_ON  
            setBacklight(FALSE);
            emit freeze(TRUE);
            bloccato=01;
            tempo1->changeInterval(500);
#endif		    
        }
        else if ( (tiempo<=5) && (bloccato) )
        {
            emit freeze(FALSE);
            bloccato=0;
            tempo1->changeInterval(2000);
            freezed(FALSE);
        }
        if  ( (tiempo>=60) )
        {
            if (pagDefault)
            {
                if ( (pagDefault->isHidden ())/* && (scrsav->isHidden() )*/ ) 
                {
                    if (illumino)
                        illumino -> hide();
                    if (scenari)
                        scenari -> hide();
                    if (carichi)
                        carichi -> hide();
                    if (imposta)
                        imposta -> hide();
                    if (automazioni)
                        automazioni -> hide();
                    if (termo)
                        termo -> hide();
                    if (difSon)
                        difSon -> hide();
                    if (antintr)
                        antintr -> hide();
                    if (specPage )
                        specPage -> hide();
                    if (pagDefault)
                        pagDefault -> showFullScreen();
                }
            }    
            if  ( (tiempo>=65) && isHidden())
            {
                for (int idx=0;idx<12;idx++)
                {
                    if (Sfondo[idx])
                        delete(Sfondo[idx]);
                        if (pagDefault)
                        {
                            Sfondo[idx] =  new QPixmap(QPixmap::grabWidget(pagDefault,(idx%3)*MAX_WIDTH/3,(int)(idx/3)*MAX_HEIGHT/4,MAX_WIDTH,MAX_HEIGHT/4)); 
                    }
                    else
                    {
                            Sfondo[idx] =  new QPixmap(QPixmap::grabWidget(Home,(idx%3)*MAX_WIDTH/3,(int)(idx/3)*MAX_HEIGHT/4,MAX_WIDTH,MAX_HEIGHT/4));     
                    }
                    screensav[idx]->setGeometry((idx%3)*80,(int)(idx/3)*80,80,80);
                    screensav[idx]->setPixmap(*Sfondo[idx]);
                    screensav[idx]->hide();show();
                }                           
                if (grab)
                    delete(grab);
                if(pagDefault)
                    grab= new QPixmap(QPixmap::grabWidget(pagDefault,0,0,MAX_WIDTH,MAX_HEIGHT)); 
                else
                    grab= new QPixmap(QPixmap::grabWidget(Home,0,0,MAX_WIDTH,MAX_HEIGHT)); 
                //grab= new QPixmap(QPixmap::grabWindow(pagDefault->winId(),0,0,MAX_WIDTH,MAX_HEIGHT)); 
                show();   
                countScrSav=0;
                }  
            
            if (isShown())
            {
        //        qDebug("tiempo: %d",tiempo);
                if ( 0/*(tiempo/100)%2 */)
                {                   
                    if(backcol>2)
                    {
                        tempo1->changeInterval(500);
                        for (int idx=0;idx<12;idx++)
                        {
                            screensav[idx]->show();
                        }
                         for (int idx=0;idx<BALL_NUM;idx++)
    {
                        ball[idx]->hide();
                    }
                    }
                countScrSav=(countScrSav+1)%8;
                 switch (countScrSav){
                    case 1:
                    icx=(int) (12.0*rand() / (RAND_MAX+1.0));
                    icy=(int) (12.0*rand() / (RAND_MAX+1.0));
                    screensav[icx]->hide();
                    break;
                    case 2:
                    screensav[icy]->hide();
                    break;
                    case 3:
                    setBackgroundColor(QColor(255,0,0));
                    break;
                case 4:                  
                        QRect r;
                        r=screensav[icx]->geometry();
                        screensav[icx]->setGeometry(screensav[icy]->geometry());
                        screensav[icy]->setGeometry(r);
                        if (backcol)
                            setBackgroundColor(QColor(0,255,0));
                        else
                            setBackgroundColor(QColor(0,0,255));
                        backcol=(backcol+1)%2;
                        screensav[icx]->show(); 
                        screensav[icy]->show(); 
                    break;
                    repaint();
                    show();                               
                }
             }
                 else
                 {
                    if (backcol<3)
                    {
                        backcol=4;
                        for (int idx=0;idx<12;idx++)
                        {
                            screensav[idx]->hide();
                        }
                        setPaletteBackgroundPixmap(*grab);        
                         for (int idx=0;idx<BALL_NUM;idx++)
    {
                        x[idx]=(int) (200.0*rand() / (RAND_MAX+1.0));
                        y[idx]=(int) (200.0*rand() / (RAND_MAX+1.0));
                        vx[idx]=(int) (30.0*rand() / (RAND_MAX+1.0)) -15;
                        vy[idx]=(int) (30.0*rand() / (RAND_MAX+1.0)) -15;
                            if (!vy[idx])
                                vy[idx]=1;
                            if (!vx[idx])
                                vx[idx]=1;                            
                        dim[idx]=(int) (10.0*rand() / (RAND_MAX+1.0))+15;
                          
                        QBitmap Maschera=QBitmap(dim[idx],dim[idx],TRUE);
                        QPainter p( &Maschera );
                        p.setBrush(QBrush ( Qt::color1/*Qt::black*/, Qt::SolidPattern ));
                        for(int idy=2;idy<=dim[idx];idy++)
                            p.drawEllipse ( (dim[idx]-idy)/2,(dim[idx]-idy)/2,idy,idy);
                        ball[idx]->setMask(Maschera);
                        
                    //    ball[idx]->setPaletteBackgroundColor(QColor((int) (200.0*rand() / (RAND_MAX+1.0))+56,(int) (200.0*rand() / (RAND_MAX+1.0))+56,(int) (200.0*rand() / (RAND_MAX+1.0))+56));
                        //    ball[idx]->setPaletteBackgroundColor(backgroundColor());
                 //       ball[idx]->hide();
                 //       ball[idx]->setGeometry(x[idx],y[idx],dim[idx],dim[idx]);
                 //      ball[idx]->show();
                    }
                        tempo1->changeInterval(100);
                    }
                    else
                    {
                        backcol++;
                        if (backcol==9)
                        {
                            backcol=4;
                            if (grab)
                                delete(grab);
                            if(pagDefault)
                                grab= new QPixmap(QPixmap::grabWidget(pagDefault,0,0,MAX_WIDTH,MAX_HEIGHT)); 
                            else
                                grab= new QPixmap(QPixmap::grabWidget(Home,0,0,MAX_WIDTH,MAX_HEIGHT)); 
                            setPaletteBackgroundPixmap(*grab);   
                        }
                        
                                                
                         for (int idx=0;idx<BALL_NUM;idx++)
    {
                    //        ball[idx] -> setBackgroundMode(Qt::NoBackground);
                        x[idx]+=vx[idx];
                        y[idx]+=vy[idx];
                       
                        if  (x[idx]<=0) 
                        {
                            vx[idx]=(int) (10.0*rand() / (RAND_MAX+1.0))+5 ;
                            x[idx]=0;
    //                        dim[idx]=(int) (20.0*rand() / (RAND_MAX+1.0));
                          ball[idx]->setPaletteBackgroundColor(QColor((int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150));
                            
   //                          ball[idx]->setPaletteForegroundColor(QColor((int) (200.0*rand() / (RAND_MAX+1.0))+56,(int) (200.0*rand() / (RAND_MAX+1.0))+56,(int) (200.0*rand() / (RAND_MAX+1.0))+56));
                            // repaint(0,0,width(),height(),TRUE);
                        }
                        if  (y[idx]>(MAX_HEIGHT-dim[idx])) 
                        {
                           vy[idx]=(int) (10.0*rand() / (RAND_MAX+1.0)) -15;
   //                         dim[idx]=(int) (20.0*rand() / (RAND_MAX+1.0));
                            y[idx]=MAX_HEIGHT-dim[idx];
                          ball[idx]->setPaletteBackgroundColor(QColor((int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150));
     //                       ball[idx]->setPaletteForegroundColor(QColor((int) (200.0*rand() / (RAND_MAX+1.0))+56,(int) (200.0*rand() / (RAND_MAX+1.0))+56,(int) (200.0*rand() / (RAND_MAX+1.0))+56));
                          //   repaint(0,0,width(),height(),TRUE);
                        }
                        if   (y[idx]<=0) 
                        {                         
                            vy[idx]=(int) (10.0*rand() / (RAND_MAX+1.0))+5 ;
                            if (!vy[idx])
                                vy[idx]=1;
                            y[idx]=0;
   //                         dim[idx]=(int) (20.0*rand() / (RAND_MAX+1.0))+5;
                          ball[idx]->setPaletteBackgroundColor(QColor((int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150));

    //                        ball[idx]->setPaletteForegroundColor(QColor((int) (256.0*rand() / (RAND_MAX+1.0)),(int) (256.0*rand() / (RAND_MAX+1.0)),(int) (256.0*rand() / (RAND_MAX+1.0))));
                        //    repaint(0,0,width(),height(),TRUE);
                        }
                   if  (x[idx]>(MAX_WIDTH-dim[idx])) 
                        {
                            vx[idx]=(int) (10.0*rand() / (RAND_MAX+1.0)) -15;         
                            if (!vx[idx])
                                vx[idx]=1;
       //                     dim[idx]=(int) (20.0*rand() / (RAND_MAX+1.0))+5;
                            x[idx]=MAX_WIDTH-dim[idx];
                          ball[idx]->setPaletteBackgroundColor(QColor((int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150,(int) (100.0*rand() / (RAND_MAX+1.0))+150));
   //                          ball[idx]->setPaletteForegroundColor(QColor((int) (200.0*rand() / (RAND_MAX+1.0))+56,(int) (200.0*rand() / (RAND_MAX+1.0))+56,(int) (200.0*rand() / (RAND_MAX+1.0))+56));

                      //       repaint(0,0,width(),height(),TRUE);
                        }
                    ball[idx]->setGeometry(x[idx],y[idx],dim[idx],dim[idx]);
                    ball[idx]->show();
               }
                    }
                }        
            }
        }	
        
    }
    else if  ( (tiempo>=120)  )
    {
#ifndef BACKLIGHT_SEMPRE_ON 	    
        setBacklight(FALSE);
        emit freeze(TRUE);	   
        tempo1->changeInterval(500);
#endif	    
        firstTime=0;
        bloccato=1;	
    }
    else  if  (tiempo<=5)
    {
        firstTime=0;
        setBacklight(TRUE);    
        tempo1->changeInterval(2000);
        bloccato=0;
    }
    
}

void BtMain::freezed(bool b)
{
    bloccato=0;
    if (b)
        bloccato=1;
    //qDebug("BLOCCATO   : %d",bloccato);
    if  (!b) 
        
    {
        setBacklight(TRUE);
        //qDebug("BtMain freezed FALSE");
        hide();
        if (pwdOn)
        {
            tasti = new tastiera(NULL,"tast");
            tasti->setBGColor(Home->backgroundColor());     
            tasti->setFGColor(Home->foregroundColor()); 
            tasti -> showTastiera();
            connect(tasti, SIGNAL(Closed(char*)), this, SLOT(testPwd(char*)));
        }
    }
}


void BtMain::setPwd(bool b ,char* p)
{
    pwdOn=b;
    strcpy(&pwd[0],p);
    qDebug("BtMain nuova pwd = %s - %d",&pwd[0],pwdOn );
}

void BtMain::testPwd(char* p)
{
    if(p)
    {
        if (strcmp(p,&pwd[0]))
        {
            tasti -> showTastiera();
            qDebug("pwd ko %s  doveva essere %s",p,&pwd[0]);
        }
        else
        {
            delete (tasti);
        }
    }
    else
        tasti -> showTastiera();
}




palla::palla(QWidget*parent, const char* name,unsigned int f) : BtLabel(parent, name, f){}

void palla::paintEvent(QPaintEvent *)  {
        QPainter paint(this);

//        paint.fillRect ( 0,0,width(),height(),QBrush ( backgroundColor(), Qt::SolidPattern ));
                
   /*     rx+=(int)(3.0*rand() / (RAND_MAX+1.0)) -1;
        if (rx<8)
            rx=8;
        if (rx>width())
            rx=width();
        
        ry+=(int) (3.0*rand() / (RAND_MAX+1.0)) -1;
        if (ry<8)
            ry=8;
        if (ry>height())
            ry=height();

      //  setFixedSize(rx,ry);
 */
        paint.setBrush(QBrush ( foregroundColor(), Qt::SolidPattern ));

//       paint.drawEllipse ( 0,0,rx,ry);
       paint.drawEllipse ( 0,0,width(),height());
       //ball[idx]->height(),ball[idx]->width());
    };
void palla ::clear()
{
QPainter paint(this);
        paint.fillRect ( 0,0,width(),height(),QBrush ( backgroundColor(), Qt::SolidPattern ));
  }
