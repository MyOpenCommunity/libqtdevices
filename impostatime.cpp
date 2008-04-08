/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**impostaTime.cpp
**
**Sottomenù imposta data/ora
**
****************************************************************/

#include <qpixmap.h>
#include <qwidget.h>
#include <qcursor.h>
#include <qdatetime.h>

#include <qfile.h>

#include "impostatime.h"
#include "genericfunz.h"
#include "openclient.h"
extern unsigned char tipoData;

impostaTime::impostaTime( QWidget *parent, const char *name )
        : QFrame( parent, name )
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
//    showFullScreen();
#endif    
   char iconName[MAX_PATH];
   QPixmap* Icon1 = new QPixmap();
   QPixmap* Icon2 = NULL;
	 
    setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT); 
    setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));
    
    QDateTime OroTemp = QDateTime(QDateTime::currentDateTime(Qt::LocalTime));
    dataOra = new timeScript(this,"impostazioni",1,&OroTemp);
    
    
    
    dataOra->setGeometry(10,120,220,80);
    
    dataOra->setFrameStyle( QFrame::Plain );
    dataOra->setLineWidth(0);    
    dataOra->hide();

    memset(iconName,'\000',sizeof(iconName));
    strcpy(iconName,ICON_FRECCIA_SU);
    Icon1->load(iconName);	
    getPressName((char*)ICON_FRECCIA_SU, &iconName[0],sizeof(iconName));
 
        if (QFile::exists(iconName))
    {  	 
	Icon2 = new QPixmap();
	Icon2->load(iconName);
    }
    
    for (uchar idx=0;idx<3;idx++)
    {
	but[idx] = new BtButton(this,"freccia"/*+QString::number(idx)*/);

	but[idx] -> setGeometry(idx*80+10,60,60,60);
	but[idx] -> setAutoRepeat(true);
	but[idx] -> setPixmap(*Icon1);
	if (Icon2)
	{
	    but[idx] -> setPressedPixmap(*Icon2);
	}
    }
    free (Icon1);
    free (Icon2);
    Icon2=NULL;
    Icon1 = new QPixmap();
    Icon1->load(ICON_FRECCIA_GIU);	
     getPressName((char*)ICON_FRECCIA_GIU, &iconName[0],sizeof(iconName));
        if (QFile::exists(iconName))
    {  
	Icon2 = new QPixmap();
	Icon2->load(iconName);
    }
    
    for (uchar idx=3;idx<6;idx++)
    {
	but[idx] = new BtButton(this,"freccia"+QString::number(idx));
	but[idx] -> setGeometry((idx-3)*80+10,200,60,60);	
	but[idx] -> setAutoRepeat(true);
	but[idx] -> setPixmap(*Icon1);
	if (Icon2)
	    but[idx] -> setPressedPixmap(*Icon2);
    }	
     free (Icon1);
     free (Icon2);
     Icon2=NULL;
     Icon1 = new QPixmap();
     Icon1->load(ICON_OK);
     but[6] = new BtButton(this,"OK");
     but[6] -> setGeometry(90,260,60,60);
     but[6] -> setPixmap(*Icon1);
     getPressName((char*)ICON_OK, &iconName[0],sizeof(iconName));
     if (QFile::exists(iconName))
    {  
	Icon2 = new QPixmap();
	Icon2->load(iconName);
    }
     if (Icon2)
	    but[6] -> setPressedPixmap(*Icon2);
     // qDebug("-----34-----");
     
    Immagine = new BtLabel(this, "immaginetta superiore");
    Immagine->setGeometry(90,0,120,60);
    
    //dataOra=NULL;
}

void impostaTime::setFGColor(int r, int g, int b)
{
    setFGColor( QColor :: QColor(r,g,b)); 
}

void impostaTime::setBGColor(int r, int g, int b)
{
    setBGColor( QColor :: QColor(r,g,b)); 
}

void impostaTime::setBGColor(QColor c)
{	
    setPaletteBackgroundColor( c);    
    if (dataOra)
	dataOra->setPaletteBackgroundColor(backgroundColor());
    for (int idx=0;idx<7;idx++)
    {
	    if (but[idx])
		but[idx]->setPaletteBackgroundColor(backgroundColor());
    }
}
void impostaTime::setFGColor(QColor c)
{
    setPaletteForegroundColor( c );
    if (dataOra)
        dataOra->setPaletteForegroundColor(foregroundColor());
    for (int idx=0;idx<7;idx++)
   {
	 if (but[idx])   
	     but[idx]->setPaletteForegroundColor(foregroundColor());
    }
}


int impostaTime::setBGPixmap(char* backImage)
{
    QPixmap Back; 
     if(Back.load(backImage))
    {
	 setPaletteBackgroundPixmap(Back);      
	 return (0);
    }    
     return (1);
}

void impostaTime::OKTime()
{
    disconnect( but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumOra()));
    disconnect( but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumMin()));
    disconnect( but[2] ,SIGNAL(clicked()),dataOra,SLOT(aumSec()));
    disconnect( but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminOra()));
    disconnect( but[4] ,SIGNAL(clicked()),dataOra,SLOT(diminMin()));
    disconnect( but[5] ,SIGNAL(clicked()),dataOra,SLOT(diminSec()));
    disconnect( but[6] ,SIGNAL(clicked()),this,SLOT(OKTime()));
 
    dataOra->showDate();

    QPixmap* Icon1 = new QPixmap();
     Icon1->load(ICON_CALENDARIO);
           
     if (Icon1)
	 Immagine -> setPixmap(*Icon1); 
        if (tipoData==1)
	{
    connect( but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumDay()));
    connect( but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumMonth()));
    connect( but[2] ,SIGNAL(clicked()),dataOra,SLOT(aumYear()));
    connect( but[4] ,SIGNAL(clicked()),dataOra,SLOT(diminDay()));
    connect( but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminMonth()));
    connect( but[5] ,SIGNAL(clicked()),dataOra,SLOT(diminYear()));
    connect( but[6] ,SIGNAL(clicked()),this,SLOT(OKDate()));
}
   else
	{
    connect( but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumDay()));
    connect( but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumMonth()));
    connect( but[2] ,SIGNAL(clicked()),dataOra,SLOT(aumYear()));
    connect( but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminDay()));
    connect( but[4] ,SIGNAL(clicked()),dataOra,SLOT(diminMonth()));
    connect( but[5] ,SIGNAL(clicked()),dataOra,SLOT(diminYear()));
    connect( but[6] ,SIGNAL(clicked()),this,SLOT(OKDate()));
}
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
     openwebnet msg_open;
     char    pippo[50];
     QString s;

     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#13**#0*");
     s=dataOra->getDataOra().toString("hh");
     strcat(pippo,s.ascii());
     strcat(pippo,"*");
     s=dataOra->getDataOra().toString("mm");
     strcat(pippo,s.ascii());
     strcat(pippo,"*");
     s=dataOra->getDataOra().toString("ss");
     strcat(pippo,s.ascii());
     strcat(pippo,"**##");

     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
     emit sendFrame(msg_open.frame_open);
#endif
}

void impostaTime::OKDate()
{
       if (tipoData==1)
	{
    disconnect( but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumDay()));
    disconnect( but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumMonth()));
    disconnect( but[2] ,SIGNAL(clicked()),dataOra,SLOT(aumYear()));
    disconnect( but[4] ,SIGNAL(clicked()),dataOra,SLOT(diminDay()));
    disconnect( but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminMonth()));
    disconnect( but[5] ,SIGNAL(clicked()),dataOra,SLOT(diminYear()));
    disconnect( but[6] ,SIGNAL(clicked()),this,SLOT(OKDate()));
}
   else
	{
    disconnect( but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumDay()));
    disconnect( but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumMonth()));
    disconnect( but[2] ,SIGNAL(clicked()),dataOra,SLOT(aumYear()));
    disconnect( but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminDay()));
    disconnect( but[4] ,SIGNAL(clicked()),dataOra,SLOT(diminMonth()));
    disconnect( but[5] ,SIGNAL(clicked()),dataOra,SLOT(diminYear()));
    disconnect( but[6] ,SIGNAL(clicked()),this,SLOT(OKDate()));
}
    
    
     //QTextOStream (stdout)<<"\nOra impostata = "<<dataOra->getDataOra().toString();  
    
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
     openwebnet msg_open;
     char    pippo[50];
     QString s;
     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#13**#1*00*");
     s=dataOra->getDataOra().toString("dd");
     strcat(pippo,s.ascii());     
     strcat(pippo,"*");
     s=dataOra->getDataOra().toString("MM");
     strcat(pippo,s.ascii());     
     strcat(pippo,"*");
     s=dataOra->getDataOra().toString("yyyy");
     strcat(pippo,s.ascii());     
     strcat(pippo,"##");
     
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
     emit sendFrame(msg_open.frame_open);    
#endif
     
     //delete (dataOra);
     //dataOra = NULL;
     hide();
}

void impostaTime::mostra()
{
#if 0
      QDateTime OroTemp = QDateTime(QDateTime::currentDateTime());
    // QTextOStream (stdout)<<"\nmostra";

     dataOra = new timeScript(this,"scrittaHomePage",1,&OroTemp);
         
     
     
     dataOra->setGeometry(10,120,220,80);

     dataOra->setFrameStyle( QFrame::Plain );
     dataOra->setLineWidth(0);    
#else

     qDebug("impostaTime::mostra()");
     //dataOra->stopDate();
     dataOra->reset();
     dataOra->showTime();
     disconnect(but[0], 0, dataOra, 0);
     disconnect(but[1], 0, dataOra, 0);
     disconnect(but[2], 0, dataOra, 0);
     disconnect(but[3], 0, dataOra, 0);
     disconnect(but[4], 0, dataOra, 0);
     disconnect(but[5], 0, dataOra, 0);
     disconnect(but[6], 0, dataOra, 0);
#endif

     QPixmap *Icon1 = new QPixmap();
     Icon1->load(ICON_OROLOGIO);
           
    
     if (Icon1)
	 Immagine -> setPixmap(*Icon1); 
     
    connect( but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumOra()));
    connect( but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumMin()));
    connect( but[2] ,SIGNAL(clicked()),dataOra,SLOT(aumSec()));
    connect( but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminOra()));
    connect( but[4] ,SIGNAL(clicked()),dataOra,SLOT(diminMin()));
    connect( but[5] ,SIGNAL(clicked()),dataOra,SLOT(diminSec()));
    connect( but[6] ,SIGNAL(clicked()),this,SLOT(OKTime()));
    
    show();
    dataOra->show();
}

void impostaTime::hide()
{
    qDebug("impostaTime::hide()");
    QFrame::hide();
    if(dataOra)
	dataOra->hide();
}
