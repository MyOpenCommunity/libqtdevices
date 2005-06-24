/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**antintrusione.cpp
**
**Sottomenù antiintrusione
**
****************************************************************/

#include "antintrusione.h"
extern unsigned char tipoData;

antintrusione::antintrusione( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
//    showFullScreen();
#endif     
   numRighe=NUM_RIGHE;  
   zone = new sottoMenu(this,"Zone",3,MAX_WIDTH, MAX_HEIGHT-MAX_HEIGHT/numRighe,/*numRighe-*/2); 
   impianto = new sottoMenu(this,"impianto",0,MAX_WIDTH, MAX_HEIGHT/numRighe,1); 
   allarmi = new sottoMenu(this,"Allarmi",3,MAX_WIDTH, MAX_HEIGHT,4); 

   
   allarmi->hide();
    setGeom(0,0,MAX_WIDTH,MAX_HEIGHT);
    connect(zone  ,SIGNAL(Closed()),this,SIGNAL(Closed()));
    
    connect(this,SIGNAL(gestFrame(char *)),zone,SIGNAL(gestFrame(char *)));
    connect(this,SIGNAL(gestFrame(char *)),impianto,SIGNAL(gestFrame(char *)));
    connect(zone,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*))); 
    connect(impianto,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*))); 	 
//    connect(allarmi,SIGNAL(Closed()),this , SLOT(show())); 	 
    
    connect(allarmi,SIGNAL(Closed()),allarmi , SLOT(hide())); 	     
    connect(allarmi,SIGNAL(Closed()),zone, SLOT(show())); 	     
    connect(allarmi,SIGNAL(Closed()),impianto , SLOT(show())); 	     
    
    connect(impianto, SIGNAL(goDx()), allarmi,SLOT(show())); 	 
//    connect(impianto, SIGNAL(goDx()), this,SLOT(hide())); 	     
    
    connect(this,SIGNAL(freezed(bool)),zone,SLOT(freezed(bool)));
    connect(this,SIGNAL(freezed(bool)),impianto,SLOT(freezed(bool)));        
    connect(this,SIGNAL(freezed(bool)),allarmi,SLOT(freezed(bool)));            
    connect(allarmi,SIGNAL(itemKilled()),this,SLOT(testranpo()));

}

void antintrusione::testranpo()
{
    QTimer *t=new  QTimer(this,"T");
    connect(t,SIGNAL(timeout()),this,SLOT( ctrlAllarm()));
    t->start(150,TRUE);
}

void antintrusione:: ctrlAllarm()
{
    qDebug("ctrlAllarm %d",allarmi->getCount());
    if (allarmi->getCount()>0)
        impianto->getLast()->mostra(BUT1);
    else
        impianto->getLast()->nascondi(BUT1);
}


void antintrusione::setBGColor(int r, int g, int b)
{	
    setPaletteBackgroundColor( QColor :: QColor(r,g,b));    
    if (impianto)
	impianto-> setBGColor(backgroundColor() );
    if (zone)
	zone-> setBGColor(backgroundColor() );
    if (allarmi)
	allarmi->setBGColor(backgroundColor() );
}
void antintrusione::setFGColor(int r, int g, int b)
{
    setPaletteForegroundColor( QColor :: QColor(r,g,b));
    if (impianto)
	impianto-> setFGColor(foregroundColor() );
    if (zone)    
	zone-> setFGColor(foregroundColor() );
    if (allarmi)
	allarmi->setFGColor(foregroundColor() );
}


void antintrusione::draw()
{	
    ctrlAllarm();
    if (impianto)
	impianto-> draw();
    if (zone)
	zone-> draw();
    if (allarmi)
	allarmi->draw();
}


int antintrusione::setBGPixmap(char* backImage)
{
    QPixmap Back; 
     if(Back.load(backImage))
    {
	 setPaletteBackgroundPixmap(Back);      
	 return (0);
    }    
     return (1);
}


int antintrusione::addItem(char tipo, char* descrizione, void* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *pressedIcon,int periodo, int numFrame, char* txt_tecnico, char* txt_intrusione, char* txt_manomissione, char* txt_panic)
 {        
    if (tipo== IMPIANTINTRUS)
    {
	impianto -> addItem(tipo, descrizione,indirizzo,  IconaSx, IconaDx, icon, pressedIcon);
	connect(impianto->getLast(),SIGNAL(impiantoInserito()),allarmi,SLOT(svuota()));
    memset(&testoManom[0],'\000',MAX_PATH);
    memset(&testoTecnico[0],'\000',MAX_PATH);
    memset(&testoIntrusione[0],'\000',MAX_PATH);
    memset(&testoPanic[0],'\000',MAX_PATH);  
    if (txt_manomissione)
        strncpy(&testoManom[0],txt_manomissione,MAX_PATH);
    if (txt_tecnico)
        strncpy(&testoTecnico[0],txt_tecnico,MAX_PATH);
    if (txt_intrusione)
        strncpy(&testoIntrusione[0],txt_intrusione,MAX_PATH);
    if (txt_panic)
        strncpy(&testoPanic[0],txt_panic,MAX_PATH);
    }
    else if (tipo== ZONANTINTRUS)
	zone->addItem(tipo , descrizione , indirizzo ,IconaSx,IconaDx, icon, pressedIcon);
    return(1);    
 }

void antintrusione::setNumRighe(uchar n)
{
    numRighe=n;
    zone->setNumRighe(n-1);
    impianto->setNumRighe(1);
    zone->draw();
    impianto->draw();
}


void antintrusione::inizializza()
{     
    zone-> inizializza();
    impianto->inizializza();
//     emit sendFrame("*16*53*100##");      frame per richiesta stato impianto
}


void antintrusione::gesFrame(char*frame)
{	
    emit gestFrame(frame);
    openwebnet msg_open;
    char aggiorna;
    
    aggiorna=0;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
   
    if (!strcmp(msg_open.Extract_chi(),"5"))
    {
	if ( (! strncmp(msg_open.Extract_cosa(),"12",2) ) || (! strncmp(msg_open.Extract_cosa(),"15",2) ) || \
	     (! strncmp(msg_open.Extract_cosa(),"16",2) ) || (! strncmp(msg_open.Extract_cosa(),"17",2) ) )     
	{
            
	    char descr[2*MAX_PATH],time[MAX_PATH];
	    char zona[3];       
        
        memset(&descr[0],'\000',2*MAX_PATH);
        
            if  ( (! strncmp(msg_open.Extract_cosa(),"12",2)) && (testoTecnico[0]) )
                strncpy(&descr[0],&testoTecnico[0],MAX_PATH);
             if  ( (! strncmp(msg_open.Extract_cosa(),"15",2)) && (testoIntrusione[0]) )
                strncpy(&descr[0],&testoIntrusione[0],MAX_PATH);
             if  ( (! strncmp(msg_open.Extract_cosa(),"16",2)) && (testoManom[0]) )
                strncpy(&descr[0],&testoManom[0],MAX_PATH);
             if  ( (! strncmp(msg_open.Extract_cosa(),"17",2)) && (testoPanic[0]) )
                strncpy(&descr[0],&testoPanic[0],MAX_PATH);

	    strcpy(&zona[0],msg_open.Extract_dove());
	    sprintf(&time[0], "\n%s   %s    Z %s", QDateTime::currentDateTime().toString("hh:mm").ascii(), \
			    QDateTime::currentDateTime().toString("dd.MM").ascii(), &zona[1]);	
        strncat(&descr[0],&time[0],MAX_PATH);
        allarmi->addItem(ALLARME, &descr[0], NULL, ICON_DEL);
        aggiorna=1;
	 }
    }    
    if (aggiorna)
    {
    qDebug("ARRIVATO ALLARME!!!!");
        allarmi->forceDraw();
        allarmi->show();
        impianto->hide();
        zone->hide();
        this->showFullScreen();
        ctrlAllarm();
    }
}




void antintrusione::setGeom(int x,int y,int w,int h)
{
      QWidget::setGeometry(x,y,w,h);
      if (impianto)
	impianto->setGeometry(x,y,w,h/numRighe);
    if(zone)
	zone->setGeometry(x,h/numRighe,w,h/numRighe*(numRighe-1));
    if (allarmi)
	allarmi->setGeometry(x,y,w,h);
}

void antintrusione::setNavBarMode(uchar c)
{
    zone->setNavBarMode(c);
}



