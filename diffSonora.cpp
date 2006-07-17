/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**diffSonora.cpp
**
**Sottomenù diffusione sonora
**
****************************************************************/

#include "diffsonora.h"
/*#include "sottomenu.h"
#include "items.h"
#include "main.h"
#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qlabel.h>
*/
 
diffSonora::diffSonora( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
  //    showFullScreen();
#endif  
   numRighe=NUM_RIGHE;  
   sorgenti = new sottoMenu(this,"Sorgenti",0,MAX_WIDTH, MAX_HEIGHT/numRighe,1); 
   amplificatori = new sottoMenu(this,"Amplificatori",3,MAX_WIDTH, MAX_HEIGHT-MAX_HEIGHT/numRighe,/*numRighe-*/2); 

   setGeom(0,0,MAX_WIDTH,MAX_HEIGHT);
   //sorgenti -> setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT/numRighe);
//   amplificatori -> setGeometry (0, MAX_HEIGHT/numRighe, MAX_WIDTH, MAX_HEIGHT- MAX_HEIGHT/numRighe );
     	
    connect(amplificatori  ,SIGNAL(Closed()),this,SLOT(fineVis()));
    BtLabel* linea = new BtLabel(this,NULL,0);
    linea->setGeometry(0,77,240,3);
    linea->setPaletteForegroundColor(QColor::QColor(0,0,0));

    connect(this,SIGNAL(gesFrame(char *)),sorgenti,SIGNAL(gestFrame(char *)));
    connect(this,SIGNAL(gesFrame(char *)),amplificatori,SIGNAL(gestFrame(char *)));
    connect(sorgenti,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*))); 
    connect(amplificatori,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*))); 	 
    
    connect(this,SIGNAL(freezed(bool)),amplificatori,SLOT(freezed(bool)));
    connect(this,SIGNAL(freezed(bool)),sorgenti,SLOT(freezed(bool)));    
}


void diffSonora::setBGColor(int r, int g, int b)
{	
    setPaletteBackgroundColor( QColor :: QColor(r,g,b));    
    if (sorgenti)
	sorgenti-> setBGColor(backgroundColor() );
    if (amplificatori)
	amplificatori-> setBGColor(backgroundColor() );
}
void diffSonora::setFGColor(int r, int g, int b)
{
    setPaletteForegroundColor( QColor :: QColor(r,g,b));
    if (sorgenti)
	sorgenti-> setFGColor(foregroundColor() );
    if (amplificatori)    
	amplificatori-> setFGColor(foregroundColor() );
}

int diffSonora::setBGPixmap(char* backImage)
{
    QPixmap Back; 
     if(Back.load(backImage))
    {
	 setPaletteBackgroundPixmap(Back);      
	 return (0);
    }    
     return (1);
}


int diffSonora::addItem(char tipo, char* descrizione, void* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *pressedIcon,int modo, int numFrame, char *ambdescr)
 {    
    if  ( (tipo==SORGENTE) || (tipo==SORGENTE_RADIO))
    {     	
	sorgenti-> setBGColor(backgroundColor() );
	sorgenti-> setFGColor(foregroundColor() );	
	sorgenti-> addItem(tipo , descrizione , indirizzo , icon, pressedIcon, 
			   NULL, NULL, modo);	
    } else if ((tipo == SORG_RADIO) || (tipo == SORG_AUX)) {
	sorgenti-> setBGColor(backgroundColor() );
	sorgenti-> setFGColor(foregroundColor() );	
	sorgenti-> addItem(tipo , descrizione , indirizzo , IconaSx, IconaDx, 
			   icon, NULL, modo, 0, QColor(0,0,0), ambdescr); 
	banner *b = sorgenti->getLast();
	connect(b, SIGNAL(csxClick()), sorgenti, SLOT(goDown()));
    }
    else 
    {     
	amplificatori->addItem(tipo , descrizione , indirizzo ,IconaSx,IconaDx, icon, pressedIcon);
    }
     return(1);    
 }

void diffSonora::setNumRighe(uchar n)
{
    numRighe=n;
    amplificatori->setNumRighe(n-2);
    sorgenti->setNumRighe(1);
    amplificatori->draw();
    sorgenti->draw();
}


void diffSonora::inizializza()
{     
	amplificatori-> inizializza();     
}


void diffSonora::gestFrame(char*frame)
{	
    emit gesFrame(frame);
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
		   sorgenti->setIndex(msg_open.Extract_dove());   
		   aggiorna=1;
		   qDebug("accesa sorg:%s",msg_open.Extract_dove());
		}	  
	}
    }    
    if (aggiorna)
    {
	sorgenti->draw();
	if(isVisual)
	{
	    QWidget::show();	    
	}
    }
}

void diffSonora::show()
{
     openwebnet msg_open;
     emit sendFrame("*16*53*100##"); 
     sorgenti->draw();
     amplificatori->draw();
     isVisual=1;

/*      if (sorgenti)
	sorgenti-> show();
      if (amplificatori)
	amplificatori-> show();*/
     QWidget::show();
}   

void diffSonora::draw()
{	
      if (sorgenti)
	sorgenti-> draw();
      if (amplificatori)
	amplificatori-> draw();
  }

void diffSonora::forceDraw()
{	
      if (sorgenti)
	sorgenti-> forceDraw();
      if (amplificatori)
	amplificatori-> forceDraw();
 }

void diffSonora::hide()
{	
/*      if (sorgenti)
	sorgenti-> hide();
      if (amplificatori)
	amplificatori-> hide();*/
    if (amplificatori)
	amplificatori->setIndice(0);
      QWidget::hide();
  }

void diffSonora::setGeom(int x,int y,int w,int h)
{
      QWidget::setGeometry(x,y,w,h);
      if (sorgenti)
	sorgenti->setGeometry(x,y,w,h/numRighe);
    if(amplificatori)
	amplificatori->setGeometry(x,h/numRighe,w,h/numRighe*(numRighe-1));
}

void diffSonora::setNavBarMode(uchar c)
{
    amplificatori->setNavBarMode(c);
}

void diffSonora::fineVis()
 {
    isVisual=0;
    emit(Closed());
    emit(closed(this));
}


#if 0
// diffMulti implementation
diffMulti::diffMulti( QWidget *parent, const char *name ) : 
    diffSonora(parent, name)
{  
}

int diffMulti::addItem(char tipo, char* descrizione, void* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *pressedIcon,int periodo, int numFrame)
{


}
#endif
