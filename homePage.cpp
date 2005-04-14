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
#if defined(BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
//    showFullScreen();
#endif
    setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
    xClock=xTemp=MAX_WIDTH+2;
    yClock=yTemp=MAX_HEIGHT+2;
    freez=FALSE;
 }


/*void homePage::showPip()
{
    this->show();
}
*/

void homePage::addButton(int x, int y, char* iconName, char function)
{
    BtButton *b1;
    QPixmap Icon;
 //  b1 = new BtButton (this,"BelBottone");
    char nomeFile[MAX_PATH];
    
    
    elencoButtons.append(new BtButton (this,"BelBottone"));
    b1 = elencoButtons.getLast();
    
    b1->setGeometry(x,y,DIM_BUT_HOME ,DIM_BUT_HOME );
    if (Icon.load(iconName))
    	 b1->setPixmap(Icon);
    b1->setPaletteBackgroundColor(backgroundColor());
   memset(nomeFile,'\000',sizeof(nomeFile));
    
    strncpy(nomeFile,iconName,strstr(iconName,".")-iconName);
    strcat(nomeFile,"P");
    strcat(nomeFile,strstr(iconName,"."));

   
    if (Icon.load(nomeFile))
    	 b1->setPressedPixmap(Icon);     


    switch (function){
	case USCITA:   connect(b1,SIGNAL(clicked()), qApp, SLOT(quit()) );       break;
	case AUTOMAZIONE:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Automazione() )); break;
	case ILLUMINAZIONE:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Illuminazione() )); break;
	case ANTIINTRUSIONE:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Antiintrusione() )); break;
	case CARICHI:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Carichi() )); break;
	case TERMOREGOLAZIONE:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Termoregolazione()) ); break;		  
	case DIFSON:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Difson() )); break;
	case SCENARI:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Scenari() )); break;
	case IMPOSTAZIONI:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Settings() )); break;
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
     dataOra = new timeScript(this,"scrittaHomePage");
     dataOra->setGeometry(x,y,180,35);
     dataOra->setPaletteForegroundColor(foregroundColor());
}	

/*void homePage::mousePressEvent ( QMouseEvent *  )
{
    qDebug("Pressed");    
}*/
void homePage::mouseReleaseEvent ( QMouseEvent *  )	
{
     qDebug("Released");   
     if (freez)
     {
	 freez=FALSE;
	 emit(freeze(freez));    
     }
}

void homePage::freezed(bool f)
{
    freez=f;
    if (freez)
    {
	for(uchar idx=0;idx<elencoButtons.count();idx++)
	    elencoButtons.at(idx)->setEnabled(FALSE);
	qDebug("Homepage freezed");	
    }
    else
    {
	for(uchar idx=0;idx<elencoButtons.count();idx++)
	    elencoButtons.at(idx)->setEnabled(TRUE);
	qDebug("Homepage DEfreezed");
    }
 
}
