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
#include "versio.h"
#include <qfont.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <stdlib.h>
#include <qwidget.h>
#include <qcursor.h>
 #include <unistd.h>

 
sottoMenu::sottoMenu( QWidget *parent, const char *name, uchar navBarMode,int wi,int hei, uchar n)
        : QWidget( parent, name )
{
       numRighe=n;  
       hasNavBar=navBarMode;
       width=wi;
       height=hei;
       freez=FALSE;
       
       setGeometry(0,0,width,height);   
       setFixedSize(QSize(width, height));
	
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
       
       elencoBanner.setAutoDelete( TRUE );
    indice=0;
    indicold=100;
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
    if (!parentWidget())
	showFullScreen();
#endif       
}

void sottoMenu::setNavBarMode(uchar navBarMode,char* IconBut4)
{   
     
    if(navBarMode!=hasNavBar)
    {
       if(bannNavigazione)
        {	
            //free( bannNavigazione );
            bannNavigazione->hide();
            delete( bannNavigazione );
            bannNavigazione=NULL;
        }
        if (navBarMode)
        {
            bannNavigazione  = new bannFrecce(this,"bannerfrecce",navBarMode, IconBut4);
            
            bannNavigazione -> setGeometry( 0, height-height/NUM_RIGHE/*numRighe*/, width, height/NUM_RIGHE/*numRighe */);
            bannNavigazione -> setBGColor(backgroundColor());
            bannNavigazione -> setFGColor(foregroundColor());
            connect(bannNavigazione, SIGNAL(backClick()), this, SIGNAL(Closed()));
            connect(bannNavigazione, SIGNAL(upClick()), this, SLOT(goUp()));
            connect(bannNavigazione, SIGNAL(downClick()), this, SLOT(goDown()));
            connect(bannNavigazione, SIGNAL(forwardClick()), this, SIGNAL(goDx()));
        }
        hasNavBar=navBarMode;
        strncpy(&iconName[0],IconBut4,sizeof(&iconName[0]));
    }
    else if (strcmp(IconBut4,&iconName[0]))
    {        
        strncpy(&iconName[0],IconBut4,MAX_PATH);
        qDebug("strcmp(IconBut4,&iconName[0]) : %s - %s", &iconName[0], IconBut4);
        bannNavigazione -> SetIcons(1,&iconName[0]);
        bannNavigazione -> Draw();
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


int sottoMenu::addItem(char tipo, char* descrizione, void* indirizzo,char* IconaSx,char*  IconaDx,char *icon ,char *pressedIcon,int periodo, int numFrame, \
		       QColor SecondForeground,char* descr1,char* descr2,char* descr3,char* descr4, char* icoEx1, char* icoEx2, char* icoEx3,int par3,int par4, QPtrList<QString> *lt, QPtrList<scenEvo_cond> *lc, QString action, 
QString light, QString key, QString unknown)
 {
     qDebug("sottoMenu::addItem (%p)", lt);
    switch (tipo){
    case ATTUAT_AUTOM:   elencoBanner.append(new attuatAutom(this,descrizione ,(char*)indirizzo, IconaSx, IconaDx,icon, pressedIcon,periodo,numFrame)); break;     
    case DIMMER:   elencoBanner.append(new dimmer(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon, icoEx1)); break;
    case DIMMER_100: elencoBanner.append(new dimmer100(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon, icoEx1, par3, par4)); break;
    case SCENARIO:   elencoBanner.append(new scenario(this,descrizione ,(char*)indirizzo,IconaSx)); break;
    case GR_ATTUAT_AUTOM: elencoBanner.append(new grAttuatAutom(this,descrizione ,indirizzo,IconaSx, IconaDx,icon,periodo,numFrame)); break;  
    case GR_DIMMER: elencoBanner.append(new grDimmer(this,descrizione ,indirizzo,IconaSx, IconaDx,icon,pressedIcon)); break;  			 
    case CARICO: elencoBanner.append(new carico(this,descrizione ,(char*)indirizzo,IconaSx)); break; 
    case ATTUAT_AUTOM_INT: elencoBanner.append(new attuatAutomInt(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon,periodo,numFrame)); break;
    case ATTUAT_AUTOM_INT_SIC: elencoBanner.append(new attuatAutomIntSic(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon,periodo,numFrame)); break;
    case ATTUAT_AUTOM_TEMP: elencoBanner.append(new attuatAutomTemp(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon,periodo,numFrame,lt)); break;     
    case ATTUAT_AUTOM_TEMP_NUOVO_N: elencoBanner.append(new attuatAutomTempNuovoN(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon,periodo,numFrame, lt)); break; 
    case ATTUAT_AUTOM_TEMP_NUOVO_F: 
      if(!lt->count()) 
	lt->append(new QString(""));
      elencoBanner.append(new attuatAutomTempNuovoF(this,descrizione, (char*)indirizzo,IconaSx, IconaDx, icon, lt->at(0)->ascii())); break;
    case GR_ATTUAT_INT: elencoBanner.append(new grAttuatInt(this,descrizione ,indirizzo,IconaSx, IconaDx,icon,periodo,numFrame)); break;
    case ATTUAT_AUTOM_PULS:   elencoBanner.append(new attuatPuls(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,/*icon,*/AUTOMAZ,periodo,numFrame)); break;
    case ATTUAT_VCT_LS: elencoBanner.append(new attuatPuls(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,/*icon,*/VCT_LS,periodo,numFrame)); break;
    case ATTUAT_VCT_SERR: elencoBanner.append(new attuatPuls(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,/*icon,*/VCT_SERR,periodo,numFrame)); break;
    case SET_DATA_ORA : elencoBanner.append(new setDataOra(this,descrizione )); break;
    case AMPLIFICATORE:   elencoBanner.append(new amplificatore(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon)); break;
    case SORGENTE : elencoBanner.append(new sorgente(this,descrizione ,(char*)indirizzo)); break;
    case SORGENTE_RADIO : elencoBanner.append(new banradio(this,descrizione ,(char*)indirizzo)); break;
    case GR_AMPLIFICATORI: elencoBanner.append(new grAmplificatori(this,descrizione ,indirizzo,IconaSx, IconaDx,icon,pressedIcon)); break;  	
    case SET_SVEGLIA: elencoBanner.append(new impostaSveglia(this,descrizione ,(diffSonora*)indirizzo, IconaSx,IconaDx, icon, pressedIcon, periodo, numFrame,descr1,descr2,descr3,descr4,icoEx1,par3)); break;
    case CALIBRAZIONE: elencoBanner.append(new calibration(this,descrizione ,IconaSx)); break;
    case TERMO: elencoBanner.append(new termoPage(this,descrizione ,(char*)indirizzo, IconaSx, IconaDx,icon, pressedIcon,icoEx1,icoEx2,SecondForeground)); break;		       
    case ZONANTINTRUS: elencoBanner.append(new zonaAnti(this,descrizione ,(char*)indirizzo, IconaSx, IconaDx)); break;
    case IMPIANTINTRUS:  elencoBanner.append(new impAnti(this,descrizione ,(char*)indirizzo, IconaSx, IconaDx, icon, pressedIcon)); break;       
    case SUONO: elencoBanner.append(new impBeep(this,descrizione , IconaSx,IconaDx, icon)); break;
    case CONTRASTO: elencoBanner.append(new impContr(this,descrizione , IconaSx,IconaDx)); break;
    case VERSIONE: elencoBanner.append(new machVers(this,descrizione , (versio*)indirizzo, IconaSx)); break;
    case ALLARME: elencoBanner.append(new allarme(this,descrizione ,(char*)indirizzo, IconaSx));break;
//    case PROTEZIONE: elencoBanner.append(new impPassword(this,descrizione , (char*)indirizzo, IconaSx,IconaDx,icon,pressedIcon)); break;		  		 
    case PROTEZIONE: elencoBanner.append(new impPassword(this,descrizione ,IconaDx,icon,pressedIcon, IconaSx, periodo)); break;		  		 
    case MOD_SCENARI:  elencoBanner.append(new gesModScen(this, descrizione ,(char*)indirizzo, IconaSx,IconaDx,icon,pressedIcon, icoEx1, icoEx2, icoEx3));break;
    case SCENARIO_EVOLUTO: elencoBanner.append(new scenEvo(this, descrizione, lc, IconaSx, IconaDx, icon, pressedIcon, NULL, NULL, NULL, action, par3));
      break;
    case SCENARIO_SCHEDULATO: elencoBanner.append(new scenSched(this, descrizione, IconaSx, IconaDx, icon, pressedIcon, descr1, descr2, descr3, descr4)); break;							
      // FIXME: VERIFICA I PARAMETRI !!!
    case POSTO_ESTERNO: elencoBanner.append(new postoExt(this, descrizione, IconaSx, IconaDx, icon, pressedIcon, (char *)indirizzo, (char *)light.ascii(), (char *)key.ascii(), (char *)unknown.ascii())); break;
    }
    connect(this, SIGNAL(gestFrame(char*)), elencoBanner.getLast(), SLOT(gestFrame(char*))); 
    connect(elencoBanner.getLast(), SIGNAL(sendFrame(char*)), this , SIGNAL(sendFrame(char*)));
    connect(elencoBanner.getLast(), SIGNAL(freeze(bool)), this , SIGNAL(freeze(bool))); 
    connect(elencoBanner.getLast(), SIGNAL(svegl(bool)), this , SIGNAL(svegl(bool))); 
    connect( this , SIGNAL(frez(bool)), elencoBanner.getLast(), SIGNAL(freezed(bool)));      
    //     connect(this, SIGNAL(deFreez()), elencoBanner.getLast(), SLOT(deFreez())); 
    connect(elencoBanner.getLast(), SIGNAL(richStato(char*)), this, SIGNAL(richStato(char*))); 
    connect(elencoBanner.getLast(), SIGNAL(killMe(banner*)), this , SLOT(killBanner(banner*)));      
    elencoBanner.getLast()->SetText(descrizione);
    elencoBanner.getLast()->setAnimationParams(periodo,numFrame);
    elencoBanner.getLast()->setBGColor(backgroundColor());
    elencoBanner.getLast()->setFGColor(foregroundColor());
    elencoBanner.getLast()->setId(tipo);
    for (int idx=elencoBanner.count()-2;idx>=0;idx--)
      {
	if (elencoBanner.at(idx)->getId()==tipo)
	  {
	    elencoBanner.getLast()->setSerNum(elencoBanner.at(idx)->getSerNum()+1);
	    idx=-1;
	  }
      }
    
    //     draw();
    return(1);    
 }

void sottoMenu::draw()
{
    uint idx,idy;
    if (!(indicold==indice))
    {
//        qDebug("indicold=%d - indice=%d",indicold,indice);
	for (idy=0;idy<elencoBanner.count();idy++)
	    elencoBanner.at(idy)->hide();
	if (hasNavBar)
	{
		for (idx=0;idx<numRighe;idx++)
		{
		    if  ( (elencoBanner.at(indice+idx)) || (elencoBanner.count()>numRighe) ) 
		    {   			
			elencoBanner.at( (indice+idx) %(elencoBanner.count()))->setGeometry(0,idx*(height-MAX_HEIGHT/NUM_RIGHE)/numRighe,width,(height-MAX_HEIGHT/NUM_RIGHE)/numRighe);			
			elencoBanner.at( (indice+idx) %(elencoBanner.count()))->Draw();
			elencoBanner.at( (indice+idx) %(elencoBanner.count()))->show();
		    }
		}		
		bannNavigazione  ->setGeometry( 0 ,height-MAX_HEIGHT/NUM_RIGHE,width , MAX_HEIGHT/NUM_RIGHE);		
        bannNavigazione->Draw();
		bannNavigazione->show();	
	    }
	else
	{
		for (idx=0;idx<numRighe;idx++)
		{
		    if  ( (elencoBanner.at(indice+idx)) || (elencoBanner.count()>=numRighe) ) 
		    {   
			elencoBanner.at( (indice+idx) %(elencoBanner.count()))->setGeometry(0,idx*QWidget::height()/numRighe,QWidget::width(),QWidget::height()/numRighe);
			elencoBanner.at( (indice+idx) %(elencoBanner.count()))->Draw();
			elencoBanner.at( (indice+idx) %(elencoBanner.count()))->show();
		    }
		}
	    }
    indicold=indice;
    }
}

void sottoMenu::forceDraw()
{
    indicold=indice+1;
    draw();
}

void sottoMenu::goUp()
{
    if (elencoBanner.count()>(numRighe))
    {
	indicold=indice;
	indice=(++indice)%(elencoBanner.count());
	draw();
    }
}
    
void sottoMenu::goDown()
{
    if (elencoBanner.count()>(numRighe))
    {
	indicold=indice;
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

banner* sottoMenu::getCurrent()
{
    return(elencoBanner.at(indice));
}

banner* sottoMenu::getNext()
{        
        if (indice==(elencoBanner.count()-1))
            return(elencoBanner.at(0));
        return(elencoBanner.at(indice+1));
}

banner* sottoMenu::getPrevious()
{
        if (indice==0)
            return(elencoBanner.at(elencoBanner.count()-1));
        return(elencoBanner.at(indice-1));
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
    unsigned char p=0;
    
    for (char idx=0;idx<elencoBanner.count();idx++)     
    {
        if ( (!strcmp(elencoBanner.at(idx)->getChi(),chi)) && (!strcmp(elencoBanner.at(idx)->getAddress(),where)) )
        {
            elencoBanner.at(idx)->setPul();
            p=1;
          }
    }
    if (p)
        return TRUE;
    return FALSE;
}

bool sottoMenu::setGroup(char* chi, char* where, bool* group)
{
    unsigned char p=0;
    
    for (char idx=0;idx<elencoBanner.count();idx++)     
    {	
        if ( (!strcmp(elencoBanner.at(idx)->getChi(),chi)) && (!strcmp(elencoBanner.at(idx)->getAddress(),where)) )
        {
            elencoBanner.at(idx)->setGroup(group); 
            p=1;
          }
    }
    if (p)
        return TRUE;
    return FALSE;  
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

void sottoMenu::mouseReleaseEvent ( QMouseEvent *  e)	
{
     qDebug("Released");   
     if (freez)
     {
	 freez=FALSE;
	 emit(freeze(freez));    
     }
     QWidget::mouseReleaseEvent ( e );
}

void sottoMenu::freezed(bool f)
{
    freez=f;
    
    qDebug("%s freezed %d",name(),freez);
	
    if (freez)
    {	   
	for(uchar idx=0;idx<elencoBanner.count();idx++)
	    elencoBanner.at(idx)->setEnabled(FALSE);
	if(hasNavBar)
	    bannNavigazione->setEnabled(FALSE);
    }
    else
    {
	for(uchar idx=0;idx<elencoBanner.count();idx++)
	    elencoBanner.at(idx)->setEnabled(TRUE);
    	if(hasNavBar)
	    bannNavigazione->setEnabled(TRUE);
    }
    emit (frez(f));
}

void sottoMenu::setGeometry(int x, int y, int w, int h)
{
    //purtroppo in QTE se da un figlio faccio height() o width() mi da le dimensioni del padre...
    height=h; 
    width=w;
    QWidget::setGeometry(x, y, w, h);
    
}

void  sottoMenu::killBanner(banner* b)
{
    int icx=elencoBanner.findRef( b ) ;
//    int icx=elencoBanner.find( b ) ;    
    
    if ( icx  != -1 )
    {
        elencoBanner.at(icx)->hide();
        elencoBanner.remove(icx);
        //qDebug("ti scrivo icx %d", icx);    
        indice=0;
        indicold=100;
        //	qDebug("bannerCount= %d", elencoBanner.count());
        draw();
        if ((elencoBanner.count()==0)&&(parentWidget()) )
        {
            emit(Closed());//hide();
            parentWidget()->showFullScreen();
        }
    }
}

void sottoMenu::hide()
{
    indice=0;
    QWidget::hide();
    forceDraw();
}

void sottoMenu::svuota()
{
    elencoBanner.clear();
    draw();
}

uint sottoMenu::getCount()
{
    return(elencoBanner.count());
}



void  sottoMenu::setIndice(char c)
{
    if (c<=elencoBanner.count())
        indice=c;
}

/*void sottoMenu::show()
{
    draw();
    QWidget::show();
}*/
