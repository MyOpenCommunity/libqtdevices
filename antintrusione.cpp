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
#if defined(BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
//    showFullScreen();
#endif     
   numRighe=NUM_RIGHE;  
   zone = new sottoMenu(this,"Zone",3,MAX_WIDTH, MAX_HEIGHT-MAX_HEIGHT/numRighe,/*numRighe-*/2); 
   impianto = new sottoMenu(this,"impianto",0,MAX_WIDTH, MAX_HEIGHT/numRighe,1); 

   setGeom(0,0,MAX_WIDTH,MAX_HEIGHT);
   
    connect(zone  ,SIGNAL(Closed()),this,SIGNAL(Closed()));
    QLabel* linea = new QLabel(this,NULL,0);
    linea->setGeometry(0,77,240,3);
    linea->setPaletteForegroundColor(QColor::QColor(0,0,0));
    
    connect(this,SIGNAL(gesFrame(char *)),zone,SIGNAL(gestFrame(char *)));
    connect(this,SIGNAL(gesFrame(char *)),impianto,SIGNAL(gestFrame(char *)));
    connect(zone,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*))); 
    connect(impianto,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*))); 	 
    
    connect(this,SIGNAL(freezed(bool)),zone,SLOT(freezed(bool)));
    connect(this,SIGNAL(freezed(bool)),impianto,SLOT(freezed(bool)));        
//    impianto-> addItem(IMPIANTINTRUS , "ImpAntiintr" , (void*)"0" , ICON_IMP_INS, ICON_IMP_DIS);	       
}


void antintrusione::setBGColor(int r, int g, int b)
{	
    setPaletteBackgroundColor( QColor :: QColor(r,g,b));    
    if (impianto)
	impianto-> setBGColor(backgroundColor() );
    if (zone)
	zone-> setBGColor(backgroundColor() );
}
void antintrusione::setFGColor(int r, int g, int b)
{
    setPaletteForegroundColor( QColor :: QColor(r,g,b));
    if (impianto)
	impianto-> setFGColor(foregroundColor() );
    if (zone)    
	zone-> setFGColor(foregroundColor() );
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
    else
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
    
    
//     emit sendFrame("*16*53*100##");      frame per richiesta stato impianto
}


void antintrusione::gestFrame(char*frame)
{	
 /*   emit gesFrame(frame);
    openwebnet msg_open;
    char aggiorna;
    
    aggiorna=0;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
   
    if (!strcmp(msg_open.Extract_chi(),"16"))
    {
	if ( (! strncmp(msg_open.Extract_dove(),"100",2) ))     
	{
	    if ((!strcmp(msg_open.Extract_cosa(),"0")) || (!strcmp(msg_open.Extract_cosa(),"3")))
		{
		   impianto->setIndex(msg_open.Extract_dove());   
		   aggiorna=1;
		   qDebug("accesa sorg:%s",msg_open.Extract_dove());
		}	  
	}
    }    
    if (aggiorna)
	impianto->draw();*/
}

/*void antintrusione::show()
{
     openwebnet msg_open;
     emit sendFrame("*16*53*100##"); 
     QWidget::show();
     impianto->draw();
     zone->draw();
}   
*/


void antintrusione::setGeom(int x,int y,int w,int h)
{
      QWidget::setGeometry(x,y,w,h);
      if (impianto)
	impianto->setGeometry(x,y,w,h/numRighe);
      qDebug("x:%d-y:%d-w:%d-h:%d",x,y,w,h/numRighe);
    if(zone)
	zone->setGeometry(x,h/numRighe,w,h/numRighe*(numRighe-1));
    
}

void antintrusione::setNavBarMode(uchar c)
{
    zone->setNavBarMode(c);
}

/*void antintrusione::freezed(bool)
 {
    qDebug("ANTINT FREEZED");
}*/
