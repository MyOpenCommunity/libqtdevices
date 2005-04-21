#include "versio.h"
#include <qwidget.h>
#include <qpixmap.h>
#include <qcursor.h>

versio::versio( QWidget *parent,const char *name , unsigned int f)
        : BtLabel( parent, name,  f)
{ 

     setGeometry(0,0,240,320);
     
     datiGen = new BtLabel(this, "DG");
     
     datiGen->setGeometry(15, 150, 210, 160);
     datiGen->setFrameStyle(QFrame::Panel | QFrame::Raised);
     
     BtLabel* myHome = new BtLabel(this, "MH");
     myHome->setGeometry(30, 12, 181, 128);     
     myHome->setFrameStyle(QFrame::Panel | QFrame::Raised);
     myHome->setAutoResize(TRUE);
     myHome->setPixmap(QPixmap("cfg/skin/my_home.png"));
     
     bticino = new BtLabel(this, "BT");
     bticino->setGeometry(129, 258, 92, 42);
     bticino->setFrameStyle( QFrame::Plain);
     bticino->setAutoResize(TRUE);
     bticino->setPixmap(QPixmap("cfg/skin/bticino.png"));

    
    

     datiGen->setLineWidth(3);
     datiGen->setText("art. H4684\n\nFIRMWARE\nPIC_REL\nHARDWARE");     

#if defined(BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
#endif
     datiGen->setFrameStyle(QFrame::Panel | QFrame::Raised);
     datiGen->setAlignment(AlignLeft|AlignTop);//VCenter);
       
     datiGen->setFont( QFont( "helvetica", 14, QFont::Bold) );
     datiGen-> setIndent(15);
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
	if (!strcmp(msg_open.Extract_dove(),"3"))
	{
	    hw=atoi(msg_open.Extract_valori(2));
	    aggiorna=1;	    
	}
    }
    if (aggiorna)
    {
	char scritta[100];
	sprintf(&scritta[100], "art. H4684\n\nFIRMWARE: %d.%d.%d\nPIC REL: %d.%d.%d\nHARDWARE: %d", vers, release, build, pic_version, pic_release, pic_build, hw);
	datiGen->setText(&scritta[100]);   
//	qDebug("scritto VERSIONE");
//	repaint(FALSE);
    }
 }

void versio::inizializza()
{
//       openwebnet msg_open;
      emit sendFrame("*#1013**6##"); 
       emit sendFrame("*#1013**3##");        
       emit sendFrame("*#13**16##");        
}
