/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**sottoMenu.cpp
**
**Sottomenù sottoMenu
**
****************************************************************/


#include "sottomenu.h"
#include "items.h"
#include "setitems.h"
#include "btbutton.h"
#include "diffsonora.h"
//#include "main.h"
#include <qfont.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <stdlib.h>
#include <qwidget.h>
#include <qcursor.h>


 
sottoMenu::sottoMenu( QWidget *parent, const char *name, uchar navBarMode,int wi,int hei, uchar n)
        : QWidget( parent, name )
{
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    setCursor (QCursor (blankCursor));
    showFullScreen();
#endif    
       numRighe=n;  
       hasNavBar=navBarMode;
       width=wi;
       height=hei;
       freez=FALSE;
       
       setGeometry(0,0,width,height);   
	
        bannNavigazione  = NULL;
		
       if (navBarMode)
       {
	   bannNavigazione  = new bannFrecce(this,"bannerfrecce",navBarMode);
    
	   bannNavigazione  ->setGeometry( 0 , height-height/numRighe ,width , height/numRighe );	  
	   connect(bannNavigazione  ,SIGNAL(backClick()),this,SIGNAL(Closed()));
	   connect(bannNavigazione  ,SIGNAL(upClick()),this,SLOT(goUp()));
	   connect(bannNavigazione  ,SIGNAL(downClick()),this,SLOT(goDown()));
	   connect(bannNavigazione  ,SIGNAL(forwardClick()),this,SIGNAL(goDx()));
       }
    indice=0;
}

void sottoMenu::setNavBarMode(uchar navBarMode,char* IconBut4)
{
    delete( bannNavigazione );
     if (navBarMode)
       {
	   bannNavigazione  = new bannFrecce(this,"bannerfrecce",navBarMode, IconBut4);
    
	   bannNavigazione  ->setGeometry( 0 , height-height/numRighe ,width , height/numRighe );
	   
	   bannNavigazione->setBGColor(backgroundColor());
  	   bannNavigazione->setFGColor(foregroundColor());
	   connect(bannNavigazione  ,SIGNAL(backClick()),this,SIGNAL(Closed()));
	   connect(bannNavigazione  ,SIGNAL(upClick()),this,SLOT(goUp()));
	   connect(bannNavigazione  ,SIGNAL(downClick()),this,SLOT(goDown()));
  	   connect(bannNavigazione  ,SIGNAL(forwardClick()),this,SIGNAL(goDx()));
       }
}

void sottoMenu::setBGColor(int r, int g, int b)
{	
    setBGColor (QColor :: QColor(r,g,b));    
}
void sottoMenu::setFGColor(int r, int g, int b)
{
    setFGColor (QColor :: QColor(r,g,b));
}

void sottoMenu::setBGColor(QColor c)
{
    setPaletteBackgroundColor(c);
    if ( bannNavigazione)
	bannNavigazione->setBGColor(c);
}
void sottoMenu::setFGColor(QColor c)	
{
    setPaletteForegroundColor(c);
    if ( bannNavigazione)
	bannNavigazione->setFGColor(c);
}
   
int sottoMenu::setBGPixmap(char* backImage)
{
    QPixmap Back; 
     if(Back.load(backImage))
    {
	 setPaletteBackgroundPixmap(Back);      
	 return (0);
    }    
     return (1);
}


int sottoMenu::addItem(char tipo, char* descrizione, void* indirizzo,char* IconaSx,char*  IconaDx,char *icon ,char *pressedIcon,int periodo, int numFrame, QColor SecondForeground)
 {
    switch (tipo){
    case ATTUAT_AUTOM:   elencoBanner.append(new attuatAutom(this,"banneruno",(char*)indirizzo, IconaSx, IconaDx,icon, pressedIcon,periodo,numFrame)); break;     
    case DIMMER:   elencoBanner.append(new dimmer(this,"bannerdue",(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon)); break;
    case SCENARIO:   elencoBanner.append(new scenario(this,"bannertre",(char*)indirizzo,IconaSx)); break;
    case GR_ATTUAT_AUTOM: elencoBanner.append(new grAttuatAutom(this,"bannerquattro",indirizzo,IconaSx, IconaDx,icon,periodo,numFrame)); break;  
    case GR_DIMMER: elencoBanner.append(new grDimmer(this,"bannerquattro",indirizzo,IconaSx, IconaDx,icon,pressedIcon)); break;  			 
    case CARICO: elencoBanner.append(new carico(this,"bannercinque",(char*)indirizzo,IconaSx)); break; 
    case ATTUAT_AUTOM_INT: elencoBanner.append(new attuatAutomInt(this,"banneruno",(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon,periodo,numFrame)); break;
    case ATTUAT_AUTOM_INT_SIC: elencoBanner.append(new attuatAutomIntSic(this,"banneruno",(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon,periodo,numFrame)); break;
    case ATTUAT_AUTOM_TEMP: elencoBanner.append(new attuatAutomTemp(this,"bannerotto",(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon,periodo,numFrame)); break;     
    case GR_ATTUAT_INT: elencoBanner.append(new grAttuatInt(this,"bannernove",indirizzo,IconaSx, IconaDx,icon,periodo,numFrame)); break;
    case ATTUAT_AUTOM_PULS:   elencoBanner.append(new attuatPuls(this,"bannernove",(char*)indirizzo,IconaSx, IconaDx,icon,AUTOMAZ,periodo,numFrame)); break;
    case ATTUAT_VCT_LS: elencoBanner.append(new attuatPuls(this,"bannernove",(char*)indirizzo,IconaSx, IconaDx,icon,VCT_LS,periodo,numFrame)); break;
    case ATTUAT_VCT_SERR: elencoBanner.append(new attuatPuls(this,"bannernove",(char*)indirizzo,IconaSx, IconaDx,icon,VCT_SERR,periodo,numFrame)); break;
    case SET_DATA_ORA : elencoBanner.append(new setDataOra(this,"bannerundici")); break;
    case AMPLIFICATORE:   elencoBanner.append(new amplificatore(this,"bannerdue",(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon)); break;
    case SORGENTE : elencoBanner.append(new sorgente(this,"bannerundici",(char*)indirizzo)); break;
    case SORGENTE_RADIO : elencoBanner.append(new banradio(this,"bannercentoundici",(char*)indirizzo)); break;
    case GR_AMPLIFICATORI: elencoBanner.append(new grAmplificatori(this,"bannerquattro",indirizzo,IconaSx, IconaDx,icon,pressedIcon)); break;  	
    case SET_SVEGLIA: elencoBanner.append(new impostaSveglia(this,"bannerventotto",(diffSonora*)indirizzo, IconaSx,IconaDx, icon,  periodo, numFrame)); break;
    case CALIBRAZIONE: elencoBanner.append(new calibration(this,"bannerventinove")); break;
    case TERMO: elencoBanner.append(new termoPage(this,"bannertrenta",(char*)indirizzo, IconaSx, IconaDx,icon, pressedIcon,SecondForeground)); break;		       
	  }
    
     connect(this, SIGNAL(gestFrame(char*)), elencoBanner.getLast(), SLOT(gestFrame(char*))); 
     connect(elencoBanner.getLast(), SIGNAL(sendFrame(char*)), this , SIGNAL(sendFrame(char*)));
     connect(elencoBanner.getLast(), SIGNAL(freeze(bool)), this , SIGNAL(freeze(bool))); 
     connect(this, SIGNAL(deFreez()), elencoBanner.getLast(), SLOT(deFreez())); 
     connect(elencoBanner.getLast(), SIGNAL(richStato(char*)), this, SIGNAL(richStato(char*))); 
     elencoBanner.getLast()->SetText(descrizione);
     elencoBanner.getLast()->setAnimationParams(periodo,numFrame);
     elencoBanner.getLast()->setBGColor(backgroundColor());
     elencoBanner.getLast()->setFGColor(foregroundColor());
     draw();
     return(1);    
 }


void sottoMenu::draw()
{
    uint idx,idy;
    
     for (idy=0;idy<elencoBanner.count();idy++)
	elencoBanner.at(idy)->hide();
     if (hasNavBar)
     {
	    for (idx=0;idx<numRighe;idx++)
	    {
		if  ( (elencoBanner.at(indice+idx)) || (elencoBanner.count()>numRighe) ) 
		{   
		    elencoBanner.at( (indice+idx) %(elencoBanner.count()))->setGeometry(0,idx*(QWidget::height()-MAX_HEIGHT/NUM_RIGHE)/numRighe,QWidget::width(),(QWidget::height()-MAX_HEIGHT/NUM_RIGHE)/numRighe);
		    elencoBanner.at( (indice+idx) %(elencoBanner.count()))->Draw();
		    elencoBanner.at( (indice+idx) %(elencoBanner.count()))->show();
		}
	    }
	  /*  if (parentWidget(FALSE)/*->parentWidget(FALSE))
		bannNavigazione->setGeometry(0,parentWidget(FALSE)->height()-parentWidget(FALSE)->height()/numRighe,parentWidget(FALSE)->width(),parentWidget(FALSE)->height()/numRighe);
	    else*/
//		bannNavigazione  ->setGeometry( 0 , height-height/numRighe ,width , height/numRighe );	
	    
	    
	    
//	    bannNavigazione  ->setGeometry( 0 , QWidget::height()/numRighe *(numRighe-1),QWidget::width() , QWidget::height()/numRighe );	
	    bannNavigazione  ->setGeometry( 0 , QWidget::height()-MAX_HEIGHT/NUM_RIGHE,QWidget::width() , MAX_HEIGHT/NUM_RIGHE);	
	    
	    
	    
	 //    qDebug("sottomenu CON navBar x:%d - y:%d - w:%d - h:%d ",0,idx*QWidget::height()/numRighe,QWidget::width(),QWidget::height()/numRighe);
	 //    qDebug("        %s",elencoBanner.at(0)->testo);
	 //   qDebug("height: %d - numRighe:%d",QWidget::height(),numRighe);
	    bannNavigazione->show();	
	}
     else
         {
	    for (idx=0;idx<numRighe;idx++)
	    {
		if  ( (elencoBanner.at(indice+idx)) || (elencoBanner.count()>=numRighe) ) 
		{   
		     elencoBanner.at( (indice+idx) %(elencoBanner.count()))->setGeometry(0,idx*QWidget::height()/numRighe,QWidget::width(),QWidget::height()/numRighe);
	//	     qDebug("sottomenu senza navBar x:%d - y:%d - w:%d - h:%d ",0,idx*QWidget::height()/numRighe,QWidget::width(),QWidget::height()/numRighe);
		    elencoBanner.at( (indice+idx) %(elencoBanner.count()))->Draw();
		    elencoBanner.at( (indice+idx) %(elencoBanner.count()))->show();
		}
	    }
	}
}

void sottoMenu::goUp()
{
    if (elencoBanner.count()>(numRighe))
    {
	indice=(++indice)%(elencoBanner.count());
	draw();
    }
}
    
void sottoMenu::goDown()
{
    if (elencoBanner.count()>(numRighe))
    {
	if (--indice<0)
	    indice=elencoBanner.count()-1;
	draw();
    }
}
void sottoMenu::setNumRighe(uchar n)
{
    numRighe=n;
}

banner* sottoMenu::getLast()
{
    return(elencoBanner.last());
}

void sottoMenu::inizializza()
{
    iniTim = new QTimer(this,"iniTimer");
    iniTim->start(300,TRUE);
     QObject::connect(iniTim,SIGNAL(timeout()), this,SLOT(init()));

}

void sottoMenu::init()
{
     for (char idx=0;idx<elencoBanner.count();idx++)     
	elencoBanner.at (idx) -> inizializza();     
}


bool sottoMenu::setPul(char* chi, char* where)
{
    for (char idx=0;idx<elencoBanner.count();idx++)     
    {
	if ( (!strcmp(elencoBanner.at(idx)->getChi(),chi)) && (!strcmp(elencoBanner.at(idx)->getAddress(),where)) )
	    elencoBanner.at(idx)->setPul();
    }
}
bool sottoMenu::setGroup(char* chi, char* where, bool* group)
{
    for (char idx=0;idx<elencoBanner.count();idx++)     
    {	
	if ( (!strcmp(elencoBanner.at(idx)->getChi(),chi)) && (!strcmp(elencoBanner.at(idx)->getAddress(),where)) )
	    elencoBanner.at(idx)->setGroup(group); 
    }
}

void sottoMenu::setIndex(char* indiriz)
{
    int idx;
     for (idx=0;idx<elencoBanner.count();idx++)     
    {
	if (!strcmp(elencoBanner.at(idx)->getAddress(),indiriz))
	{	    
	    qDebug("setindex trovato %s",indiriz);
	    break;
	}
    }
     if  (idx<=elencoBanner.count())
	 indice=idx;
}


void sottoMenu::setNumRig(uchar n)
  {
         numRighe=n;       
  }
void sottoMenu::setHeight(int h)
  {
       height=h;
  }
uchar sottoMenu::getNumRig()
  {
         return(numRighe);       
  }
int sottoMenu::getHeight()
  {
       return(height);
  }


/*void sottoMenu::mousePressEvent ( QMouseEvent *  )
{
    qDebug("Pressed");    
}*/
void sottoMenu::mouseReleaseEvent ( QMouseEvent *  )	
{
     qDebug("Released");   
     if (freez)
     {
	 freez=FALSE;
	 emit(freeze(freez));    
     }
}

void sottoMenu::freezed(bool f)
{
    freez=f;
    if (freez)
    {	   
	for(uchar idx=0;idx<elencoBanner.count();idx++)
	   {
//	    setUpdatesEnabled( FALSE );    
//	    elencoBanner.at(idx)->setEnabled(FALSE);
	    elencoBanner.at(idx)->setDisabled(TRUE);
//	   setUpdatesEnabled( TRUE );    
       }
    }
    else
    {
	for(uchar idx=0;idx<elencoBanner.count();idx++)
//	    elencoBanner.at(idx)->setEnabled(TRUE);
	    elencoBanner.at(idx)->setDisabled(FALSE);
	qDebug("Homepage DEfreezed");
	emit(deFreez());
    }
}






