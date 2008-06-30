/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** versio.h
**
**definizioni della pagine di visualizzazione versioni
**
****************************************************************/

#include "versio.h"
#include "main.h"
#include "openclient.h"
#include "fontmanager.h"

#include <qwidget.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <stdlib.h>

versio::versio( QWidget *parent,const char *name , unsigned int f)
        : BtLabel( parent, name,  f)
{ 

     setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
     setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));
     
     datiGen = new BtLabel(this, "DG");
     
     datiGen->setGeometry(15, 150, 210, 160);
     datiGen->setFrameStyle(QFrame::Panel | QFrame::Raised);
     
     BtLabel* myHome = new BtLabel(this, "MH");
     myHome->setGeometry(30, 12, 181, 128);     
     myHome->setFrameStyle(QFrame::Panel | QFrame::Raised);
     myHome->setAutoResize(TRUE);
     myHome->setPixmap(QPixmap(IMG_PATH "my_home.png"));
     
     bticino = new BtLabel(this, "BT");
     bticino->setGeometry(129, 258, 92, 42);
     bticino->setFrameStyle( QFrame::Plain);
     bticino->setAutoResize(TRUE);
     bticino->setPixmap(QPixmap(IMG_PATH "bticino.png"));

     datiGen->setLineWidth(3);
     //datiGen->setText(" H/L/N/NT4684");     
     datiGen->setText( model );
//     datiGen->setText("art. H4684\n\nFIRMWARE\nPIC_REL\nHARDWARE");       
     
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
     setCursor (QCursor (blankCursor));
#endif
     datiGen->setFrameStyle(QFrame::Panel | QFrame::Raised);
     datiGen->setAlignment(AlignHCenter|AlignVCenter);
	QFont aFont;
	FontManager::instance()->getFont( font_versio_datiGen, aFont );
	datiGen->setFont( aFont );
	indDisp=0;
}

void   versio::setPaletteForegroundColor( const QColor & c )
{
         datiGen->setPaletteBackgroundColor(c);
         bticino->setPaletteBackgroundColor(c);
        BtLabel::setPaletteForegroundColor(c);
}
void   versio::setPaletteBackgroundColor( const QColor &  c)
{
      datiGen->setPaletteForegroundColor(c);
       bticino->setPaletteForegroundColor(c);
      BtLabel::setPaletteBackgroundColor(c);      
}

void versio::gestFrame(char* frame)
 {
    openwebnet msg_open;
    char aggiorna;
    
    aggiorna=0;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    
    
    if (!strcmp(msg_open.Extract_chi(),"13"))
    {
	if (!strcmp(msg_open.Extract_grandezza(),"16"))
	{
	    vers=atoi(msg_open.Extract_valori(0));
	    release=atoi(msg_open.Extract_valori(1));
	    build=atoi(msg_open.Extract_valori(2));	  
	    aggiorna=1;
	}
    }
    if (!strcmp(msg_open.Extract_chi(),"1013"))
    {
	if (!strcmp(msg_open.Extract_grandezza(),"6"))
	{
	    pic_version=atoi(msg_open.Extract_valori(0));
	    pic_release=atoi(msg_open.Extract_valori(1));
	    pic_build=atoi(msg_open.Extract_valori(2));	    
    	    aggiorna=1;
	}
	if (!strcmp(msg_open.Extract_grandezza(),"3"))
	{
	    hw_version=atoi(msg_open.Extract_valori(0));
	    hw_release=atoi(msg_open.Extract_valori(1));
	    hw_build=atoi(msg_open.Extract_valori(2));
	    aggiorna=1;	    
	    qDebug("presa vers HW = %d.%d.%d",hw_version, hw_release, hw_build);
	}
    }
    if (aggiorna)
    {
	char scritta[100];
	QFont aFont;
	FontManager::instance()->getFont( font_versio_datiGenFw, aFont );

	datiGen->setFont( aFont );
	datiGen-> setIndent(15);
	datiGen->setAlignment(AlignLeft|AlignTop);    
	sprintf(&scritta[100], "art. %s\n\nFIRMWARE: %d.%d.%d\nPIC REL: %d.%d.%d\nHARDWARE: %d.%d.%d\nT.S. n. %d", model.ascii(), vers, release, build, pic_version, pic_release, pic_build, hw_version, hw_release, hw_build, indDisp);
	
	datiGen->setText(&scritta[100]); // FIXME da tradurre??
        qDebug("setta scritte versio");
    }
 }

void versio::inizializza()
{
//       openwebnet msg_open;

    qDebug("*************** versio::inizializza() ***************");

      emit sendFrame("*#1013**6##"); 
       emit sendFrame("*#1013**3##");        
       emit sendFrame("*#13**16##");        
}


void versio::setAddr(int a)
{
    indDisp=a;
}

void versio::setModelU(const QString & m)
{
    model = m;
    datiGen->setText( model );
}
