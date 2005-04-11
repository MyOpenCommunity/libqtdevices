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
#include "qwaitcondition.h"

#include "main.h"
#include "btmain.h"
#include "homepage.h"
#include "sottomenu.h"
#include "impostatime.h"
#include "diffsonora.h"
#include "sveglia.h"
/*
#include "openclient.h"

#include "../bt_stackopen/common_files/openwebnet.h"
#include "../bt_stackopen/common_files/common_functions.h"
*/

#define VERBOSITY_LEVEL_DEFAULT 	0
char* Suffisso="<Qto>";
int VERBOSITY_LEVEL=VERBOSITY_LEVEL_DEFAULT;
FILE* StdLog=stdout;
char* My_Parser="bt_xmlparser";


/*int main( int argc, char **argv )
{
    QApplication a( argc, argv );
        
    QTextOStream (stdout)<<"\nQTouch Vivo\n";   */
BtMain::BtMain(QWidget *parent, const char *name,QApplication* a)
	: QObject( parent, name )
{
  // !!!raf XML   
#define   DEFAULT_FILETIWEB 		 	"/home/httpd/html/myhome/TiWeb.xml"
  char buff[20]="";
  GetConfigValue(DEFAULT_FILETIWEB, "configuratore", "setup", "scs", "coordinate_scs", "scs_addr"," ",buff, "in", "out");
  QTextOStream (stdout)<<"GETCO" << buff << "\n"; 
  
/*******************************************
** Socket 
*******************************************/
     
     client_comandi = new  Client("127.0.0.1",20000,0);
     client_monitor	 = new  Client("127.0.0.1",20000,1); 
  
/**********************************************/
     
     page0 = new QWidget(NULL,"PAGE0");
     page0->setGeometry(0,0,240,320);
     page0->setPaletteBackgroundColor(QColor(77,75,100));
     page0->setPaletteForegroundColor(QColor(205,200,195));
     BtLabel* datiGen = new BtLabel(page0, "DG");
     datiGen->setGeometry(15, 150, 210, 160);
     datiGen->setFrameStyle(QFrame::Panel | QFrame::Raised);
     datiGen->setAlignment(AlignLeft|AlignTop);//VCenter);
     datiGen->setFont( QFont( "Times", 13, QFont::Bold ) );
     datiGen->setIndent(15);
     datiGen->setPaletteForegroundColor(page0->backgroundColor());
     datiGen->setPaletteBackgroundColor(page0->foregroundColor());
     datiGen->setLineWidth(4);
     datiGen->setText("art. H4684\n\nFIRMWARE: 0.0.0\nHARDWARE: 0");   
     BtLabel* myHome = new BtLabel(page0, "MH");
     myHome->setGeometry(30, 12, 181, 128);     
     myHome->setFrameStyle(QFrame::Panel | QFrame::Raised);
     myHome->setAutoResize(TRUE);
     myHome->setPixmap(QPixmap("cfg/skin/my_home.png"));
     
     BtLabel* bticino = new BtLabel(page0, "BT");
     bticino->setGeometry(130, 260, 92, 42);
     bticino->setFrameStyle( QFrame::Plain);
     bticino->setAutoResize(TRUE);
     bticino->setPixmap(QPixmap("cfg/skin/bticino.png"));
     bticino->setPaletteBackgroundColor(page0->foregroundColor());
     
     
     page0->show();
     tempo1 = new QTimer(this,"clock");
     tempo1->start(100);
    connect(tempo1,SIGNAL(timeout()),this,SLOT(hom()));
 }

void BtMain::hom()
{
    delete(tempo1);
         Home = new homePage(NULL,"homepage",Qt::WType_TopLevel | Qt::WStyle_Maximize | Qt::WRepaintNoErase);    
    // !!!raf font
     QFontInfo(Home->font());

/*    qDebug( "FONT FAMILY=%s",(Home.font()).family().ascii());
    qDebug( "FONT PIXEL SIZE=%d",(Home.font()).pixelSize());
    qDebug( "FONT POINT SIZE=%d",(Home.font()).pointSize());
    qDebug( "FONT ITALIC=%d",(Home.font()).italic());
    qDebug( "FONT WEIGHT=%d",(Home.font()).weight());
    qDebug( "FONT BOLD=%d",(Home.font()).bold());
    qDebug( "FONT WEIGHT=%d",(Home.font()).weight());*/
//    qDebug( "FONT FIXED PITCH=%d",(Home.font()).fixedPitch());
//    qDebug( "FONT RAWMODE=%d",(Home.font()).rawMode());
//    qDebug( "FONT EXACTMATCH=%d",(Home.font()).exactMatch());
    
    Home->setBGColor(BG_R, BG_G, BG_B);
    Home->setFGColor(205,205,205);
      
//    Home->addButton(0,35,ICON_USCITA_80,USCITA);
    Home->addButton(0,125,ICON_ILLUMINAZ_80,ILLUMINAZIONE);
    Home->addButton(80,5,ICON_SCENARI_80 ,SCENARI);
    Home->addButton(80,90,ICON_AUTOMAZIONE_80 	,AUTOMAZIONE);
    Home->addButton(80,175,ICON_DIFSON_80 ,DIFSON);
    Home->addButton(160,35,ICON_CARICHI_80,CARICHI);
    Home->addButton(160,125,ICON_IMPOSTAZIONI_80 ,IMPOSTAZIONI);
    Home->addButton(0,35,ICON_TERMOREGOL_80,TERMOREGOLAZIONE);
//     Home.addButton(12,12,ICON_AUTOMAZIONE_80,DIFSON);
    
    Home->addClock(30,265,180,40,QColor :: QColor(BG_R, BG_G, BG_B),QColor :: QColor(205, 205, 205),QFrame::Plain,3);
    
//    Home.setShown(TRUE);
 
    Home->hide();
    
    qDebug("home.show()"); 
    qApp->setMainWidget( Home);    

    tempo2 = new QTimer(this,"clock");
    tempo2->start(3000);
    connect(tempo2,SIGNAL(timeout()),this,SLOT(showHome()));
    
    tempo3 = new QTimer(this,"clock");
    tempo3->start(50);
    connect(tempo3,SIGNAL(timeout()),this,SLOT(myMain()));
    
    
}
	
void BtMain::showHome()
{
    delete(tempo2);
    Home->show();
    delete(page0);    
}




void BtMain::myMain()
{
/*******************************************
** homePage
********************************************/
    delete(tempo3);

/*------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------*/
    
 /****************************************
** variabili fuffoidi da inizializare in modo umano in futuro
*****************************************/   
    char* indirizzo="sscamuz";
    QPtrList<char> elDispGroup;
/*------------------------------------------------------------------------------------------*/    
    
/*******************************************
** illuminazione - definizione
********************************************/

    illumino = new sottoMenu (NULL,"ILLUMINAZIONE");	 

    illumino->setBGColor(BG_R, BG_G, BG_B);            
    illumino->setFGColor(205,205,205);

     illumino->addItem(ATTUAT_AUTOM, "TINELLO",(void*)"11",ICON_ON_60 , ICON_OFF_60 , ICON_LUCE_ACCESA,ICON_LUCE_SPENTA);
     illumino->addItem(ATTUAT_AUTOM, "CUCINA",(void*)"13",ICON_ON_60 , ICON_OFF_60 , ICON_LUCE_ACCESA,ICON_LUCE_SPENTA);
     illumino->addItem(ATTUAT_AUTOM, "CAMERA", (void*)"28" ,ICON_ON_60 , ICON_OFF_60 , ICON_VENTIL_ACC , ICON_VENTIL_SPE);
     illumino->addItem(DIMMER , "ATRIO" ,  (void*)"99" , ICON_ON_60 , ICON_OFF_60 ,ICON_DIMMER_ACC , ICON_DIMMER_SPE);
     elDispGroup.clear();
     elDispGroup.append( (char*) "94");
     elDispGroup.append(((char*) "95"));
     illumino->addItem(GR_DIMMER , "Dimmer's Group" , &elDispGroup ,ICON_ON_60 , ICON_OFF_60 , ICON_DIMMERS_SX, ICON_DIMMERS_DX);  
     elDispGroup.clear();
     elDispGroup.append(((char*) "0"));
     illumino->addItem(GR_ATTUAT_AUTOM, "GENERALE",&elDispGroup, ICON_ON_60 , ICON_OFF_60 ,ICON_LUCI,NULL);
     elDispGroup.clear();
     elDispGroup.append( (char*) "11");
     elDispGroup.append(((char*) "#1"));
     elDispGroup.append(((char*) "6"));
     illumino->addItem(GR_ATTUAT_AUTOM, "GRUPPETTINO",&elDispGroup, ICON_ON_60 , ICON_OFF_60 ,ICON_LUCI,NULL);
     elDispGroup.clear();      

     illumino->addItem(ATTUAT_VCT_SERR, "Serr VCT",(void*)"4000",ICON_ON_60 , ICON_KEY_60, ICON_KEY_60,NULL);
     
     illumino->addItem(ATTUAT_VCT_LS, "Luci VCT",(void*)"0",ICON_ON_60 , ICON_LUCE_ACCESA,ICON_LUCE_ACCESA,NULL);
     
     illumino->addItem(ATTUAT_AUTOM_TEMP, "Temporizzato",(void*)"48", ICON_CICLA_60 , ICON_ON_60 ,ICON_LUCE_ACCESA2,ICON_LUCE_SPENTA2);  

     
/*******************************************
** Automazioni - definizione
********************************************/

     automazioni = new sottoMenu (NULL,"AUTOMAZIONE");	 

    automazioni->setBGColor(BG_R, BG_G, BG_B);         
    automazioni->setFGColor(205,205,205);
    
     automazioni->addItem(ATTUAT_AUTOM_INT , "AttuatInterbloccato" , (void*)"49", ICON_UP_60 , ICON_DOWN_60,ICON_TENDA,ICON_STOP_MOV);
     automazioni->addItem(ATTUAT_AUTOM_INT_SIC , "AttuatInterbloccatoSicuro" , (void*)"79",ICON_UP_SIC_60 , ICON_DOWN_SIC_60,ICON_CANCELLO,ICON_STOP_MOV);
     elDispGroup.clear();
     elDispGroup.append( (char*) "49");
     elDispGroup.append(((char*) "79"));
     automazioni->addItem(GR_ATTUAT_INT, "GRUP_Interbloc",&elDispGroup,ICON_UP_60 , ICON_DOWN_60, ICON_STOP_MOVS,NULL);     
     automazioni->addItem(ATTUAT_AUTOM_TEMP, "Temporizzato",(void*)"11",ICON_CICLA_60 , ICON_ON_60 , ICON_RUBINETTO_ACCESO2,ICON_RUBINETTO_SPENTO2);
     
 /*******************************************
** Scenari - definizione
********************************************/   

     scenari = new sottoMenu (NULL,"SCENARI");	 
 
     scenari->setBGColor(BG_R, BG_G, BG_B);        
     scenari->setFGColor(205,205,205);
//     scenari.setNumRighe(6);
   char  ind[15] = "1*11\000";
     
     scenari->addItem(SCENARIO , "SCENARIO 1" , (void*)ind,ICON_ON_60);
     memset(ind,'\000',sizeof(ind));
     scenari->addItem(SCENARIO , "SCENARIO 2" , indirizzo,ICON_ON_60);
     scenari->addItem(SCENARIO , "SCENARIO 3" , indirizzo,ICON_ON_60);
     scenari->addItem(SCENARIO , "SCENARIO 4" , indirizzo,ICON_ON_60);

 /*******************************************
** Carichi - definizione
********************************************/   

     carichi = new sottoMenu (NULL,"CARICHI");	 
 
     carichi->setBGColor(BG_R, BG_G, BG_B);        
     carichi->setFGColor(205,205,205); 
	 
     carichi->addItem(CARICO , "CARICO 1" , indirizzo,ICON_ON_60);
     carichi->addItem(CARICO , "CARICO 2" , indirizzo,ICON_ON_60);
     carichi->addItem(CARICO , "CARICO 3" , indirizzo,ICON_ON_60);
     carichi->addItem(CARICO , "CARICO 4" , indirizzo,ICON_ON_60);
     
/*******************************************
** Diffusione sonora - definizione
********************************************/   
     difSon = new diffSonora (NULL,"DIFFUSIONE_SONORA");	 	 

     difSon->setBGColor(BG_R, BG_G, BG_B);     
     difSon->setFGColor(205,205,205); 
    
    difSon->addItem(SORGENTE_RADIO,"Radio Cantina",(void*)"101");
     difSon->addItem(SORGENTE,"Hi Fi",(void*)"102");

     difSon->addItem(AMPLIFICATORE,"Ampli 1",(void*)"78", ICON_ON_60 , ICON_OFF_60 , ICON_AMPLI_ACC , ICON_AMPLI_SPE);
//     difSon.addItem(AMPLIFICATORE,"Ampli 2",indirizzo, ICON_ON_60 , ICON_OFF_60 , ICON_AMPLI_ACC , ICON_AMPLI_SPE);
//     difSon.addItem(AMPLIFICATORE,"Ampli 3",indirizzo, ICON_ON_60 , ICON_OFF_60 , ICON_AMPLI_ACC , ICON_AMPLI_SPE);
     elDispGroup.clear();
     elDispGroup.append( (char*) "48");
     elDispGroup.append(((char*) "7"));
     difSon->addItem(GR_AMPLIFICATORI,"Ampli 4",&elDispGroup, ICON_ON_60 , ICON_OFF_60 , ICON_AMPLIS_SX,  ICON_AMPLIS_DX);     
//     difSon.addItem(AMPLIFICATORE,"Ampli 5",indirizzo, ICON_ON_60 , ICON_OFF_60 , ICON_AMPLI_ACC , ICON_AMPLI_SPE);
     
	  
/*******************************************
** Impostazioni - definizione
********************************************/   
    imposta = new sottoMenu (NULL,"IMPOSTAZIONE");	 
 
     imposta->setBGColor(BG_R, BG_G, BG_B);       
     imposta->setFGColor(205,205,205);
     
     imposta->addItem(SET_DATA_ORA,"set Ora",NULL,NULL,NULL);
     imposta->addItem(SET_SVEGLIA,"Sveglia 1",(void*)difSon,ICON_SVEGLIA_ON_60,ICON_SVEGLIA_OFF_60,NULL,NULL,(int)SEMPRE, (int)DI_SON );
     imposta->addItem(SET_SVEGLIA,"Sched  1",NULL,ICON_AUTO_ON, ICON_AUTO_OFF,"*0*3*11##",NULL, (int)SEMPRE, (int)FRAME );
     imposta->addItem(CALIBRAZIONE,"Calibration",NULL,NULL,NULL);
 
/*******************************************
** Termoregolazione
********************************************/        

        termo = new sottoMenu( NULL, "TERMOREGOLAZIONE", 4, MAX_WIDTH, MAX_HEIGHT,1);
	
        termo->setBGColor(BG_R, BG_G, BG_B);       	
        termo->setFGColor(205,205,205);
        termo->addItem(TERMO,"Zona DUE",(void*)"2",ICON_PIU, ICON_MENO, ICON_MANUAL_ON, ICON_AUTO_ON,0,0,QColor(255,0,0));	
        termo->addItem(TERMO,"Zona numero uno",(void*)"1",ICON_PIU, ICON_MENO, ICON_MANUAL_ON, ICON_AUTO_ON,0,0,QColor(255,0,0));	
	

/*------------------------------------------------------------------------------------------*/     
     
   
   /* bool pep[9]={FALSE,TRUE,FALSE,TRUE,FALSE,FALSE,TRUE,FALSE,FALSE};
     illumino->setPul("1","11");
     illumino->setGroup("1","28", pep);
     */
/*******************************************
** connessioni 
********************************************/
    
    
    
     connect(Home,SIGNAL(Illuminazione()),illumino,SLOT(show()));
     connect(illumino,SIGNAL(Closed()),illumino,SLOT(hide()));
     connect(illumino,SIGNAL(Closed()),Home,SLOT(show()));
     
     connect(Home,SIGNAL(Automazione()),automazioni,SLOT(show()));
     connect(automazioni,SIGNAL(Closed()),automazioni,SLOT(hide()));
     connect(automazioni,SIGNAL(Closed()),Home,SLOT(show()));
     
     connect(Home,SIGNAL(Scenari()),scenari,SLOT(show()));
     connect(scenari,SIGNAL(Closed()),scenari,SLOT(hide()));
     connect(scenari,SIGNAL(Closed()),Home,SLOT(show()));
     
     connect(Home,SIGNAL(Carichi()),carichi,SLOT(show()));
     connect(carichi,SIGNAL(Closed()),carichi,SLOT(hide()));
     connect(carichi,SIGNAL(Closed()),Home,SLOT(show()));
     
     connect(Home,SIGNAL(Settings()),imposta,SLOT(show()));
     connect(imposta,SIGNAL(Closed()),imposta,SLOT(hide()));
     connect(imposta,SIGNAL(Closed()),Home,SLOT(show()));
     
     connect(Home,SIGNAL(Difson()),difSon,SLOT(show()));
     connect(difSon,SIGNAL(Closed()),difSon,SLOT(hide()));
     connect(difSon,SIGNAL(Closed()),Home,SLOT(show()));
     
     connect(Home,SIGNAL(Termoregolazione()),termo,SLOT(show()));
     connect(termo,SIGNAL(Closed()),termo,SLOT(hide()));
     connect(termo,SIGNAL(Closed()),Home,SLOT(show()));
     
     
     connect(client_monitor,SIGNAL(frameIn(char *)),illumino,SIGNAL(gestFrame(char *)));
     connect(client_monitor,SIGNAL(frameIn(char *)),automazioni,SIGNAL(gestFrame(char *)));
     connect(client_monitor,SIGNAL(frameIn(char *)),difSon,SLOT(gestFrame(char *)));
     connect(client_monitor,SIGNAL(frameIn(char *)),imposta,SIGNAL(gestFrame(char *)));
     connect(client_monitor,SIGNAL(frameIn(char *)),termo,SIGNAL(gestFrame(char *)));
     
//     QObject::connect(client_comandi,SIGNAL(frameIn(char *)),&illumino,SIGNAL(gestFrame(char *)));
     connect(illumino,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));     
     connect(automazioni,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));     
     connect(scenari,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));     
     connect(carichi,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));     
     connect(imposta,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));        
     connect(difSon,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));          
     connect(termo,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));          
     connect(illumino,SIGNAL(richStato(char *)),client_comandi,SLOT(richStato(char *)));     
     
//--------     
    connect(illumino,SIGNAL(freeze(bool)),this,SIGNAL(freeze(bool)));     
     connect(automazioni,SIGNAL(freeze(bool)),this,SIGNAL(freeze(bool)));     
     connect(scenari,SIGNAL(freeze(bool)),this,SIGNAL(freeze(bool)));     
     connect(carichi,SIGNAL(freeze(bool)),this,SIGNAL(freeze(bool)));          
    connect(imposta,SIGNAL(freeze(bool)),this,SIGNAL(freeze(bool)));     
     connect(difSon,SIGNAL(freeze(bool)),this,SIGNAL(freeze(bool)));     
     connect(Home,SIGNAL(freeze(bool)),this,SIGNAL(freeze(bool)));     
     connect(termo,SIGNAL(freeze(bool)),this,SIGNAL(freeze(bool)));     
   
    connect(this,SIGNAL(freeze(bool)),illumino,SLOT(freezed(bool)));     
     connect(this,SIGNAL(freeze(bool)),automazioni,SLOT(freezed(bool)));     
     connect(this,SIGNAL(freeze(bool)),scenari,SLOT(freezed(bool)));     
     connect(this,SIGNAL(freeze(bool)),carichi,SLOT(freezed(bool)));          
     connect(this,SIGNAL(freeze(bool)),imposta,SLOT(freezed(bool)));     
     connect(this,SIGNAL(freeze(bool)),difSon,SLOT(freezed(bool)));          
     connect(this,SIGNAL(freeze(bool)),Home,SLOT(freezed(bool)));               
     connect(this,SIGNAL(freeze(bool)),termo,SLOT(freezed(bool)));               

     
/*******************************************/     
  /*  QWaitCondition pisul;
     pisul.wait(1000);*/

     illumino->inizializza();
     automazioni->inizializza();
     /*scenari->inizializza();*/
     difSon->inizializza();
     termo->inizializza();     
   
//     Home.showFullScreen();
 

//    return a.exec();
}
