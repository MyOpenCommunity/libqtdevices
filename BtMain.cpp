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
#include "calibrate.h"

#include <qfontdatabase.h>
#include <qfile.h>
#include <qxml.h>
#include <qwindowdefs.h>




BtMain::BtMain(QWidget *parent, const char *name,QApplication* a)
	: QObject( parent, name )
{

    
  /*******************************************
** Socket 
*******************************************/
      qDebug("parte BtMain");
      
      client_comandi = new  Client("127.0.0.1",20000,0);
      client_monitor	 = new  Client("127.0.0.1",20000,1); 
  

     setBacklight(TRUE);
     setContrast(0x80,FALSE);
     
     rearmWDT();
     
     firstTime=1;
     pagDefault=NULL;
     Home=specPage=NULL;
     illumino=scenari=carichi=imposta=automazioni=termo=NULL;
     difSon=NULL;
     antintr=NULL;
     screen=NULL;
     
     datiGen = new versio(NULL, "DG");
   

       
     if (QFile::exists("/etc/pointercal"))
     {
	      tempo1 = new QTimer(this,"clock");
	      tempo1->start(200);
	      connect(tempo1,SIGNAL(timeout()),this,SLOT(hom()));
	      datiGen->show();
     }
     else
     {
	      tempo1=NULL;
	      Calibrate* calib = new Calibrate(NULL,"calibrazione",(unsigned char)0,(unsigned char)1);
	      calib->show(); 
	      connect(calib, SIGNAL(fineCalib()), this,SLOT(hom()));
	      connect(calib, SIGNAL(fineCalib()), datiGen,SLOT(show()));
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
	 
  xmlconfhandler  * handler=new xmlconfhandler(this, &Home,&specPage, &illumino,&scenari,&carichi,&imposta, &automazioni, &termo,&difSon, &antintr,&pagDefault, client_comandi, client_monitor, datiGen);
  

  
  QFile * xmlFile;
  xmlFile = new QFile("cfg/conf.xml");
  QXmlInputSource source( xmlFile );
  QXmlSimpleReader reader;
  qDebug("parte parsing");
  reader.setContentHandler( handler );
  reader.parse( source );
   qDebug("finito parsing");
  delete handler;
  delete xmlFile;
   
    
    qApp->setMainWidget( Home);    


    tempo3 = new QTimer(this,"clock");
    tempo3->start(10);
    connect(tempo3,SIGNAL(timeout()),this,SLOT(myMain()));

}
	
////////non più eseguito////////
void BtMain::init()
{
    qDebug("parte init");
    connect(client_monitor,SIGNAL(frameIn(char *)),datiGen,SLOT(gestFrame(char *))); 
     connect(datiGen,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));       

     
     if(illumino)
	 illumino->inizializza();
     if(automazioni)
	 automazioni->inizializza();
     if(antintr)
	 antintr->inizializza();
     if(termo)
	 termo->inizializza();
     if(difSon)
	 difSon->inizializza();
     if (datiGen)
	 datiGen->inizializza();
     if(scenari)
	 scenari->inizializza();
     if(imposta)
	 imposta->inizializza();
     qDebug("fine init");
}




void BtMain::myMain()
{
    qDebug("entro MyMain");
    
    delete(tempo3);
    
      Home->show();
      datiGen->hide();


      connect(client_monitor,SIGNAL(monitorSu()),this,SLOT(init()));
      client_monitor->connetti();
	
      tempo1 = new QTimer(this,"clock");
      tempo1->start(2000);
      disconnect(tempo1,SIGNAL(timeout()),this,SLOT(hom()));
      connect(tempo1,SIGNAL(timeout()),this,SLOT(gesScrSav()));
}


void BtMain::gesScrSav()
{
    unsigned long tiempo;
    rearmWDT();  
    
    if (QFile::exists("/MODALITA_TEST1"))
    {
	if ( (screen) && (tiposcreen!=RED))
	    delete(screen);
	else if (!screen)
	{
	    tiposcreen=RED;
	    screen = new genPage(NULL,"red",RED);
	    screen->show();
	    qDebug("TEST1");
	    setBacklight(TRUE);
	}
    }
    else if (QFile::exists("/MODALITA_TEST2"))
    {
	if ( (screen)&& (tiposcreen!=GREEN))	
	    delete(screen);
	else if (!screen)
	{
	    tiposcreen=GREEN;
	    screen = new genPage(NULL,"green",GREEN);
	    screen->show();
	    qDebug("TEST2");
	    setBacklight(TRUE);	
	}
    }
    else if (QFile::exists("/MODALITA_TEST3"))
    {
	if ( (screen)&& (tiposcreen!=BLUE))	
	    delete(screen);
	else if(!screen)
	{
	    tiposcreen=BLUE;
	    screen = new genPage(NULL,"blu",BLUE);
	    screen->show();
	    qDebug("TEST3");
	    setBacklight(TRUE);
	}
    }
    else if (QFile::exists("/MODALITA_AGGIORNAMENTO"))
    {
	if ((screen)&& (tiposcreen!=IMAGE))	
	    delete(screen);
	else if (!screen)
	{
	    screen = new genPage(NULL,"blu",IMAGE,"cfg/skin/dwnpage.png");
	    tiposcreen=IMAGE;	    
	    screen->show();
	    qDebug("AGGIORNAMENTO");
	    setBacklight(TRUE);	
	}
	
    }
    else
    {
	if (screen)
	    delete(screen);
	screen=NULL;
	tiposcreen=NONE;
	
	tiempo= getTimePress();
	
	if (!firstTime)
	{
	    if  (tiempo<=5)
		firstTime=0;
	    
	    if  ( (tiempo>=16) && (getBacklight())) 
	    {
		    setBacklight(FALSE);
		    //    qDebug("BtMain emetto freezed TRUE");
		    emit freeze(TRUE);
		    tempo1->changeInterval(500);
		    //  qDebug("Cambiato tempo intervento");
		}
	    else if ( (tiempo<=5) && (!getBacklight()) )
	    {
		    setBacklight(TRUE);
		    //  qDebug("BtMain emetto freezed FALSE");	    
		    emit freeze(FALSE);
		    tempo1->changeInterval(2000);
		    //  qDebug("Cambiato tempo intervento");	    
		}
	    else if  ( (tiempo>=60) )
	    {
		    if (pagDefault)
		    {
			if (pagDefault->isHidden ()) 
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
		}	
	}
	else if  ( (tiempo>=120)  )
	{
	    setBacklight(FALSE);
	    emit freeze(TRUE);
	    firstTime=0;
	    tempo1->changeInterval(500);
	}
    }
}

void BtMain::freezed(bool b)
{
    if  (!b) 
    {
	setBacklight(TRUE);
	//qDebug("BtMain freezed FALSE");
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
//    qDebug("nuova pwd = %s",&pwd[0] );
}

void BtMain::testPwd(char* p)
{
    if(p)
    {
	if (strcmp(p,&pwd[0]))
	{
	    tasti -> showTastiera();
	    //	    qDebug("pwd ko %s  doveva essere %s",p,&pwd[0]);
	}
	else
	{
	    delete (tasti);
	}
    }
    else
	tasti -> showTastiera();
}
