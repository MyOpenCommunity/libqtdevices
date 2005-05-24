/****************************************************************
***
* BTicino Touch scren Colori art. H4686
**
** homePage.cpp
**
** Finestra principale
**
****************************************************************/

#include "homepage.h"
#include "main.h"
#include "btbutton.h"
#include "timescript.h"
#include "genericfunz.h"

#include <qfont.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtimer.h>

#include <qpixmap.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qstyle.h>

#include <stdlib.h>
#include <qrect.h>
#include <qstatusbar.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qmessagebox.h>
#include <qimage.h>
#include <qfile.h>

#include <qwidget.h>

homePage::homePage( QWidget *parent, const char *name, WFlags f )
        : QWidget( parent, name )
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
//    showFullScreen();
#endif
    setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
    xClock=xTemp=MAX_WIDTH+2;
    yClock=yTemp=MAX_HEIGHT+2;
    freez=FALSE;
    descrizione=NULL;
 }


/*void homePage::showPip()
{
    this->show();
}
*/

void homePage::addButton(int x, int y, char* iconName, char function, char* chix, char* cosax, char* dovex, char tipo)
{
    BtButton *b1;
    QPixmap Icon;
 //  b1 = new BtButton (this,"BelBottone");
    char nomeFile[MAX_PATH];
    
    
    elencoButtons.append(new BtButton (this,"BelBottone"));
    b1 = elencoButtons.getLast();
    
    if (function==SPECIAL)
	b1->setGeometry(x, y, DIM_SPECBUT_HOME, DIM_BUT_HOME );
    else
	b1->setGeometry(x, y, DIM_BUT_HOME, DIM_BUT_HOME );
    
    if (Icon.load(iconName))
    	 b1->setPixmap(Icon);
    b1->setPaletteBackgroundColor(backgroundColor());
    
    
    getPressName((char*)iconName, &nomeFile[0],sizeof(nomeFile));
/*   memset(nomeFile,'\000',sizeof(nomeFile));
    
    strncpy(nomeFile,iconName,strstr(iconName,".")-iconName);
    strcat(nomeFile,"p");
    strcat(nomeFile,strstr(iconName,"."));*/

   
    if (Icon.load(nomeFile))
    	 b1->setPressedPixmap(Icon);     

    

    switch (function){
//	case USCITA:   connect(b1,SIGNAL(clicked()), qApp, SLOT(quit()) );       break;
	case AUTOMAZIONE:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Automazione() )); break;
	case ILLUMINAZIONE:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Illuminazione() )); break;
	case ANTIINTRUSIONE:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Antiintrusione() )); break;
	case CARICHI:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Carichi() )); break;
	case TERMOREGOLAZIONE:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Termoregolazione()) ); break;		  
	case DIFSON:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Difson() )); break;
	case SCENARI:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Scenari() )); break;
	case IMPOSTAZIONI:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Settings() )); break;
	case SCHEDULAZIONI:  connect(b1,SIGNAL(clicked()),this, SIGNAL(Schedulazioni() )); break;
	case SPECIAL:
			     tipoSpecial=tipo;
                                               qDebug("tipoSpecial= %d",tipoSpecial);
			     strcpy(&chi[0],chix);
			     strcpy(&cosa[0],cosax);
			     strcpy(&dove[0],dovex);			     
			     if (tipoSpecial==PULSANTE)
			     {
				 qDebug("tipoSpecial= PULSANTE");
				 connect(b1, SIGNAL(pressed()), this, SLOT(specFunzPress()));
				 connect(b1, SIGNAL(released()), this, SLOT(specFunzRelease()));
			     }
			     else
				 connect(b1, SIGNAL(clicked()), this, SLOT(specFunz()));
			     break;
	case BACK: connect(b1, SIGNAL(clicked()), this, SIGNAL(Close()));break;			
    }
}

void homePage::setBGColor(int r, int g, int b)
{	
    setPaletteBackgroundColor( QColor :: QColor(r,g,b));    
}
void homePage::setFGColor(int r, int g, int b)
{
    setPaletteForegroundColor( QColor :: QColor(r,g,b));
}

int homePage::setBGPixmap(char* backImage)
{
    QPixmap Back; 
     if(Back.load(backImage))
    {
	 setPaletteBackgroundPixmap(Back);      
	 return (0);
    }    
     return (1);
}

void homePage::addClock(int x, int y,int width,int height,QColor bg, QColor fg, int style, int line)
{
     dataOra = new timeScript(this,"scrittaHomePage");
     dataOra->setGeometry(x,y,width,height);
     dataOra->setPaletteForegroundColor(fg);
     dataOra->setPaletteBackgroundColor(bg);

     dataOra->setFrameStyle( style );
     dataOra->setLineWidth(line);    
}    
void homePage::addClock(int x, int y)
{  
    addClock(x, y, 180, 35, foregroundColor(), backgroundColor(),QFrame::NoFrame, 0);
}	


void homePage::addDate(int x, int y,int width,int height,QColor bg, QColor fg, int style, int line)
{
     dataOra = new timeScript(this,"scrittaHomePage",uchar (25));
     dataOra->setGeometry(x,y,width,height);
     dataOra->setPaletteForegroundColor(fg);
     dataOra->setPaletteBackgroundColor(bg);

     dataOra->setFrameStyle( style );
     dataOra->setLineWidth(line);    
}    
void homePage::addDate(int x, int y)
{  
    addClock(x, y, 180, 35, foregroundColor(), backgroundColor(),QFrame::NoFrame, 0);
}	

void homePage::addTemp(char *z, int x, int y,int width,int height,QColor bg, QColor fg, int style, int line, char* text)
{
     strcpy(zonaTermo,z);
     temperatura = new QLCDNumber(this,"0.00 C");
     temperatura ->setGeometry(x,y,width,height-H_SCR_TEMP);
     temperatura ->setPaletteForegroundColor(fg);
     temperatura ->setPaletteBackgroundColor(bg);

     temperatura ->setFrameStyle( style );
     temperatura ->setLineWidth(line);    
     temperatura ->setNumDigits(6);
     temperatura -> display("0.00\272C");
     temperatura -> setSegmentStyle(QLCDNumber::Flat);    
     
     if (text)
     {
	 descrTemp = new BtLabel(this,text);
	 descrTemp ->setFont( QFont( "helvetica", 14, QFont::Bold ));
	 descrTemp ->setAlignment(AlignHCenter|AlignVCenter);
	 descrTemp ->setText(text);
	 descrTemp ->setGeometry(x,y+height-H_SCR_TEMP,width,H_SCR_TEMP);
	 descrTemp ->setPaletteForegroundColor(fg);
	 descrTemp ->setPaletteBackgroundColor(bg);
     }     
 }    

void homePage::addTemp(char *z, int x, int y)
{
     addTemp(z,x,y,180, 35, backgroundColor(), foregroundColor(), QFrame::NoFrame, 0,NULL);
}    


void homePage::addDescr(char *z, int x, int y,int width,int height,QColor bg, QColor fg, int style, int line)
{
    descrizione = new BtLabel(this,z);
    descrizione->setFont( QFont( "helvetica", 14, QFont::Bold ));
    descrizione->setAlignment(AlignHCenter|AlignVCenter);
    descrizione->setText(z);
    descrizione->setGeometry(x,y,width,height);
    descrizione->setPaletteForegroundColor(fg);
    descrizione->setPaletteBackgroundColor(bg);
    descrizione->setFrameStyle( style );
    descrizione->setLineWidth(line);       
 }    

void homePage::addDescr(char *z, int x, int y)
{
     addDescr(z,x,y,160, 20, backgroundColor(), foregroundColor(), QFrame::NoFrame, 0);
}    



void homePage::mouseReleaseEvent ( QMouseEvent * e )	
{
     qDebug("Released");   
     if (freez)
     {
	 freez=FALSE;
	 emit(freeze(freez));    
     }
     QWidget::mouseReleaseEvent ( e );
}

void homePage::freezed(bool f)
{
    freez=f;
//        	qDebug("%s freezed %d",name(),freez);			
    if (freez)
    {
	for(uchar idx=0;idx<elencoButtons.count();idx++)
	    elencoButtons.at(idx)->setEnabled(FALSE);
	if (descrizione)
	    descrizione->setEnabled(FALSE);
    }
    else
    {
	for(uchar idx=0;idx<elencoButtons.count();idx++)
	    elencoButtons.at(idx)->setEnabled(TRUE);
	if (descrizione)
	    descrizione->setEnabled(TRUE);
    }
 
}

void homePage::gestFrame(char* frame)
 {    
    openwebnet msg_open;
    char aggiorna;
    char dovex[30];

    
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
//qDebug("vedo frame in Homepage: ");
    
    if (!strcmp(msg_open.Extract_chi(),"4"))
    {
	strcpy(&dovex[0], msg_open.Extract_dove());
	if (dove[0]=='#')
	    strcpy(&dovex[0], &dovex[1]);
	
	if ( (! strcmp(&dovex[0],&zonaTermo[0]) ) )
	{	 
	     if   (!strcmp(msg_open.Extract_grandezza(),"0")) 
	    {
	       //Temperatura misurata
	       float icx;
	       char	tmp[10], temp[10];   
	       
	       icx=atoi(msg_open.Extract_valori(0));
	      qDebug("vedo temperatura per Temp in Homepage: %d",icx);
	       memset(temp,'\000',sizeof(temp));
	       if (icx>=1000)
	       {
		   strcat(temp,"-");
		   icx=icx-1000;
	       }
	       icx/=10;
	       sprintf(tmp,"%.1f",icx);
	       strcat(temp,tmp);
       	       strcat(temp,"\272C");
	       temperatura->display(&temp[0]);
	   }
	 }
    }
     if ( (!strcmp(msg_open.Extract_chi(),&chi[0])) && (tipoSpecial==CICLICO) )
    {
	if ( (! strcmp(&dove[0],msg_open.Extract_dove())) )
	{
	     strcpy(&cosa[0], msg_open.Extract_cosa());
	 }
    }
 }

void homePage::specFunz()
{    
      char specialFrame[50];
      
      if (tipoSpecial==CICLICO)
      {
	  if (strcmp(&cosa[0], "0"))
	      strcpy(&cosa[0],"0");
	  else
	      strcpy(&cosa[0],"1");
      }
	    
      strcpy(&specialFrame[0],"*");
      strcat(&specialFrame[0],&chi[0]);
      strcat(&specialFrame[0],"*");      
      strcat(&specialFrame[0],&cosa[0]);
      strcat(&specialFrame[0],"*");      
      strcat(&specialFrame[0],&dove[0]);      
      strcat(&specialFrame[0],"##");                  

      emit(sendFrame(&specialFrame[0]));      
}

void homePage::specFunzPress()
{  
    	 qDebug("tipoSpecial= PRESSED");
      char specialFrame[50];
      
      strcpy(&specialFrame[0],"*");
      strcat(&specialFrame[0],&chi[0]);
      strcat(&specialFrame[0],"*1*");      
      strcat(&specialFrame[0],&dove[0]);      
      strcat(&specialFrame[0],"##");          
      emit(sendFrame(&specialFrame[0]));
  }

void homePage::specFunzRelease()
{    
    	 qDebug("tipoSpecial= RELEASED");
      char specialFrame[50];
      
      strcpy(&specialFrame[0],"*");
      strcat(&specialFrame[0],&chi[0]);
      strcat(&specialFrame[0],"*0*");      
      strcat(&specialFrame[0],&dove[0]);      
      strcat(&specialFrame[0],"##");                  
      emit(sendFrame(&specialFrame[0]));
  }
