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
    connect(impianto, SIGNAL(goDx()), allarmi,SLOT(show())); 	 
//    connect(impianto, SIGNAL(goDx()), this,SLOT(hide())); 	     
    
    connect(this,SIGNAL(freezed(bool)),zone,SLOT(freezed(bool)));
    connect(this,SIGNAL(freezed(bool)),impianto,SLOT(freezed(bool)));        
    connect(this,SIGNAL(freezed(bool)),allarmi,SLOT(freezed(bool)));            
/*  QTimer *tiempo = new QTimer(this,"clock");
    tiempo->start(3000,TRUE);
    connect(tiempo,SIGNAL(timeout()),this,SLOT(tiempout()));*/
}

 
 
void antintrusione::tiempout()
{ 
    /*---fuffoide inizializzazione di elenco Allarmi---*/
   char descr[25];
    for(unsigned int idx=0; idx<10; idx++)
    {	
	sprintf(&descr[0], "%s - %s z%d", QDateTime::currentDateTime(Qt::LocalTime).toString("hh:mm:ss").ascii(), QDateTime::currentDateTime(Qt::LocalTime).toString("dd:MM").ascii(), idx);	
	allarmi->addItem(ALLARME, &descr[0], NULL, ICON_MENO);
    }
    allarmi->draw();
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


int antintrusione::addItem(char tipo, char* descrizione, void* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *pressedIcon,int periodo, int numFrame)
 {        
    if (tipo== IMPIANTINTRUS)
	impianto -> addItem(tipo, descrizione,indirizzo,  IconaSx, IconaDx, icon, pressedIcon);
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
	    char descr[25];
                    sprintf(&descr[0], "%s - %s z%d", QDateTime::currentDateTime().toString("hh:mm:ss").ascii(), \
			    QDateTime::currentDateTime().toString("dd:MM").ascii(), msg_open.Extract_dove());	
                    allarmi->addItem(ALLARME, &descr[0], NULL, ICON_MENO);
	 }
    }    
    if (aggiorna)
	allarmi->draw();
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



