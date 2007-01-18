/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** banner.cpp
**
**Implementazione di un banner generico
**
****************************************************************/

#include <stdlib.h>
#include "banner.h"
#include "btbutton.h"
#include "main.h"
#include <qfont.h>
#include "btlabel.h"
#include <qpixmap.h>
#include <qfile.h>
#include <qapplication.h>


banner::banner( QWidget *parent,const char *name )
        : QWidget( parent, name )
{
    BannerIcon = NULL;
    BannerText = NULL;
    SecondaryText = NULL;
    sxButton = NULL;
    dxButton = NULL;
    csxButton = NULL;
    cdxButton = NULL;
    for (int idx=0;idx<MAX_NUM_ICON;idx++)
	Icon[idx] = NULL;
    for (int idx=0;idx<4;idx++)
	pressIcon[idx] = NULL;
    periodo = 0;
    numFrame = contFrame = 0;
    value=0;
    attivo=0;
    minValue=maxValue=0;
    step = 1;
    animationTimer=NULL;   
    numRighe=NUM_RIGHE;
    memset(chi,'\000',sizeof(chi));
    memset(group,FALSE,sizeof(group));
    pul=FALSE;
    serNum=1;
    stato=0;
}


banner::~banner()
{
    if (BannerIcon)
        delete (BannerIcon);
    if (BannerText)
        delete (BannerText);
    if (SecondaryText)
        delete (SecondaryText);
    
    for(int idx=0;idx<MAX_NUM_ICON;idx++)
    {
        if (Icon[idx])
            delete (Icon[idx]);
    }
    for (int idx=0;idx<4;idx++)
    {
        if (Icon[idx])
            delete (pressIcon[idx]);
    }
    
    if (dxButton)
        delete (dxButton);
    if (csxButton)
    {
 /*       disconnect(csxButton,SIGNAL(clicked()),this,SIGNAL(csxClick()));
        disconnect(csxButton,SIGNAL(pressed()),this,SIGNAL(csxPressed()));
        disconnect(csxButton,SIGNAL(released()),this,SIGNAL(csxReleased()));*/
      delete(csxButton);
    }
    if (cdxButton)
        
        delete (cdxButton);
    if (sxButton)
        delete (sxButton);
    BannerIcon = NULL;
    BannerText = NULL;
    SecondaryText = NULL;
    sxButton = NULL;
    dxButton = NULL;
    csxButton = NULL;
    cdxButton = NULL;
    for (int idx=0;idx<MAX_NUM_ICON;idx++)
        Icon[idx] = NULL;
    for (int idx=0;idx<4;idx++)
        pressIcon[idx] = NULL;
}


void banner::SetText( const char *text )
{
    memset(testo,'\000',sizeof(testo));
    strncpy(testo,text,MAX_PATH*2-1);
 }

void banner::SetSeconaryText( const char *text )
{
    memset(testoSecondario,'\000',sizeof(testoSecondario));
    strncpy(testoSecondario,text,MAX_TEXT_2-1);
 }

void banner::SetIcons( uchar idIcon, const char *actuallcon )
{
    char pressIconName[MAX_PATH];
    
    if (!(Icon[idIcon]))
        Icon[idIcon]= new QPixmap();
    Icon[idIcon]->load(actuallcon);
    
    getPressName((char*)actuallcon, & pressIconName[0],MAX_PATH);
    if (!pressIcon[idIcon])
        pressIcon[idIcon]= new QPixmap();
    pressIcon[idIcon]->load(&pressIconName[0]);
    
}
	
void banner::SetIcons( const char *actuallcon ,char tipo)
{
    char pressIconName[MAX_PATH];
    
    if (actuallcon)
    {
	getPressName((char*)actuallcon, &pressIconName[0],sizeof(pressIconName));
    }
     
    if (tipo==1)
      {	
	if(!Icon[0])
	  Icon[0] = new QPixmap();
	Icon[0]->load(actuallcon);
	if (QFile::exists(pressIconName))
	  {
	    if(!pressIcon[0])
	      pressIcon[0] = new QPixmap();
	    pressIcon[0]->load(pressIconName);
	}
	 
    }
    if (tipo==3)
    {
        if(!Icon[3])
	    Icon[3] = new QPixmap();
	Icon[3]->load(actuallcon);
	if (QFile::exists(pressIconName))
	{
	    if(!pressIcon[3])
	      pressIcon[3] = new QPixmap();
	    pressIcon[3]->load(pressIconName);
	}
    }
}
void banner::SetIcons( const char *sxIcon , const char *dxIcon)
{
    char pressIconName[MAX_PATH];
    
    if(!Icon[0])
      Icon[0] = new QPixmap();
    if(!Icon[1])
      Icon[1] = new QPixmap();
 
 
    if (sxIcon)
    {
	Icon[0]->load(sxIcon);
	
	getPressName((char*)sxIcon, &pressIconName[0],sizeof(pressIconName));
	if (QFile::exists(pressIconName))
	{
	    if(!pressIcon[0])
	        pressIcon[0] = new QPixmap();
	    pressIcon[0]->load(pressIconName);
	}
    }
    if (dxIcon)
    {
	getPressName((char*)dxIcon, &pressIconName[0],sizeof(pressIconName));
	Icon[1]->load(dxIcon);
	if (QFile::exists(pressIconName))
	{
	    if(!pressIcon[1])
	        pressIcon[1] = new QPixmap();
	    pressIcon[1]->load(pressIconName);
	}
    }
}
void banner::SetIcons( const char *sxIcon , const char *dxIcon,const char *centerIcon)
{
    char pressIconName[MAX_PATH];
    if(!Icon[0])
        Icon[0] = new QPixmap();
    if(!Icon[1])
        Icon[1] = new QPixmap();
    if(!Icon[3])
        Icon[3] = new QPixmap();
        
     if (sxIcon)
     {
	Icon[0]->load(sxIcon);  
	qDebug("Icon[0] <- %s", sxIcon);
	getPressName((char*)sxIcon, &pressIconName[0],sizeof(pressIconName));
	if (QFile::exists(pressIconName))
	{
	    if(!pressIcon[0])
	        pressIcon[0] = new QPixmap();
	    pressIcon[0]->load(pressIconName);
	}
    }
      if (dxIcon)
     {
	Icon[1]->load(dxIcon);
	qDebug("Icon[1] <- %s", dxIcon);
	getPressName((char*)dxIcon, &pressIconName[0],sizeof(pressIconName));
	if (QFile::exists(pressIconName))
	{
	     if(!pressIcon[1])
	         pressIcon[1] = new QPixmap();
	     pressIcon[1]->load(pressIconName);
	 }
    }
       if (centerIcon)
     {
	Icon[3]->load(centerIcon);
	qDebug("Icon[3] <- %s", centerIcon);
	getPressName((char*)centerIcon, &pressIconName[0],sizeof(pressIconName));
	qDebug("pressIconName is %s", pressIconName);
	if (QFile::exists(pressIconName))
	{
	    if(!pressIcon[3])
	        pressIcon[3] = new QPixmap();
	    pressIcon[3]->load(pressIconName);   
	    qDebug("pressIcon[3] <- %s", pressIconName);
	}
    }
       impostaAttivo(1);
}
void banner::SetIcons( const char *sxIcon , const char *dxIcon,const char*centerActiveIcon,const char*centerInactiveIcon)
{
    char pressIconName[MAX_PATH];
    
    if(!Icon[0])
      Icon[0] = new QPixmap();
    if(!Icon[1])
      Icon[1] = new QPixmap();
    if(!Icon[2])
      Icon[2] = new QPixmap();
    if(!Icon[3])
      Icon[3] = new QPixmap();
  
     if (sxIcon)
    {
	getPressName((char*)sxIcon, &pressIconName[0],sizeof(pressIconName));
	qDebug("Icon[0] <- %s", sxIcon);
	Icon[0]->load(sxIcon);
	if (QFile::exists(pressIconName))
	{
	    if(!pressIcon[0])
	        pressIcon[0] = new QPixmap(); 
	    pressIcon[0]->load(pressIconName);	
	}
    }
     if (dxIcon)
     {
	getPressName((char*)dxIcon, &pressIconName[0],sizeof(pressIconName));
	qDebug("Icon[1] <- %s", dxIcon);
	Icon[1]->load(dxIcon);
	if (QFile::exists(pressIconName))
	{
	    if(!pressIcon[1])
	        pressIcon[1] = new QPixmap();
	    pressIcon[1]->load(pressIconName);	
	}
    }     
      if (centerActiveIcon)
     {
	getPressName((char*)centerActiveIcon, &pressIconName[0],sizeof(pressIconName));
	qDebug("Icon[3] <- %s", centerActiveIcon);
	Icon[3]->load(centerActiveIcon);
	if (QFile::exists(pressIconName))
	{
	  if(!pressIcon[3])
	    pressIcon[3] = new QPixmap();
	  pressIcon[3]->load(pressIconName);   	
	}
    }
       if (centerInactiveIcon)
     {
	getPressName((char*)centerInactiveIcon, &pressIconName[0],sizeof(pressIconName));
	qDebug("Icon[2] <- %s", centerInactiveIcon);
	Icon[2]->load(centerInactiveIcon);
	if (QFile::exists(pressIconName))
	{
	  if(!pressIcon[2])
	    pressIcon[2] = new QPixmap();
	  pressIcon[2]->load(pressIconName);   	
	}
    }
    
}

void banner::SetIcons( const char *sxIcon , const char *dxIcon,const char*centerInactiveIcon,const char*centerUpIcon,const char*centerDownIcon)
{
    char pressIconName[MAX_PATH];
    
    if(!Icon[0])
      Icon[0] = new QPixmap();
    if(!Icon[1])
      Icon[1] = new QPixmap();
    if(!Icon[2])
      Icon[2] = new QPixmap();
    if(!Icon[3])
      Icon[3] = new QPixmap();
    if(!Icon[4])
      Icon[4] = new QPixmap();
  
     if (sxIcon)
    {
	getPressName((char*)sxIcon, &pressIconName[0],sizeof(pressIconName));
	Icon[0]->load(sxIcon);
	if (QFile::exists(pressIconName))
	{
	  if(!pressIcon[0])
	    pressIcon[0] = new QPixmap(); 
	  pressIcon[0]->load(pressIconName);	
	}
    }
     if (dxIcon)
     {
	getPressName((char*)dxIcon, &pressIconName[0],sizeof(pressIconName));
	Icon[1]->load(dxIcon);
	if (QFile::exists(pressIconName))
	{
	  if(!pressIcon[1])
	    pressIcon[1] = new QPixmap();
	  pressIcon[1]->load(pressIconName);	
	}
    }     
       if (centerInactiveIcon)
     {
	getPressName((char*)centerInactiveIcon, &pressIconName[0],sizeof(pressIconName));
	Icon[2]->load(centerInactiveIcon);
	if (QFile::exists(pressIconName))
	{
	  if(!pressIcon[2])
	    pressIcon[2] = new QPixmap();
	  pressIcon[2]->load(pressIconName);   	
	}
    }
      if (centerUpIcon)
     {
	getPressName((char*)centerUpIcon, &pressIconName[0],sizeof(pressIconName));
	Icon[3]->load(centerUpIcon);
	if (QFile::exists(pressIconName))
	{
	  if(!pressIcon[3])
	    pressIcon[3] = new QPixmap();
	  pressIcon[3]->load(pressIconName);   	
	}
    }
  if (centerDownIcon)
     {
	getPressName((char*)centerDownIcon, &pressIconName[0],sizeof(pressIconName));
	Icon[4]->load(centerDownIcon);
	if (QFile::exists(pressIconName))
	{
	  if(!pressIcon[4])
	    pressIcon[4] = new QPixmap();
	  pressIcon[4]->load(pressIconName);   	
	}
    }
      
}



void banner::SetIcons( const char *sxIcon , const char *dxIcon,const char*centerActiveIcon,const char*centerInactiveIcon,int period, int number)
{
char pressIconName[MAX_PATH];
    
    if(!Icon[0]) 
        Icon[0] = new QPixmap();
    if(!Icon[1])
        Icon[1] = new QPixmap();
    if(!Icon[2])
        Icon[2] = new QPixmap();    
    
     if (sxIcon)
    {
	getPressName((char*)sxIcon, &pressIconName[0],sizeof(pressIconName));
	Icon[0]->load(sxIcon);
	if (QFile::exists(pressIconName))
	{
	  if(!pressIcon[0])
	    pressIcon[0] = new QPixmap();
	  pressIcon[0]->load(pressIconName);	
	}
    }
     if (dxIcon)
     {
	getPressName((char*)dxIcon, &pressIconName[0],sizeof(pressIconName));
	Icon[1]->load(dxIcon);
	if (QFile::exists(pressIconName))
	{
	  if(!pressIcon[1])
	    pressIcon[1] = new QPixmap();
	  pressIcon[1]->load(pressIconName);	
	}
    }     
    if (centerInactiveIcon)
    {	
	getPressName((char*)centerInactiveIcon, &pressIconName[0],sizeof(pressIconName));
	Icon[2]->load(centerInactiveIcon);	
	if (QFile::exists(pressIconName))
	{
	  if(!pressIcon[2])
	    pressIcon[2] = new QPixmap();
	  pressIcon[2]->load(pressIconName);   	
	}
    } 
    if  ( (centerActiveIcon) && (number) )
     {
	for (uchar idx=1;idx<=number;idx++)
	{
	  if(!Icon[2+idx])
	    Icon[2+idx] = new QPixmap();
	    memset(pressIconName,'\000',sizeof(pressIconName));
	 
	    strncpy(pressIconName,centerActiveIcon,strstr(centerActiveIcon,".")-centerActiveIcon);
	    strcat(pressIconName,QString::number(idx));
	    strcat(pressIconName,strstr(centerActiveIcon,"."));
	    Icon[2+idx]->load(pressIconName,"PNG",Qt::ThresholdDither|Qt::DitherMode_Mask|Qt::AvoidDither);
	}	
    }
    else if (centerActiveIcon)
      {
	   if(!Icon[3])
	      Icon[3] = new QPixmap();
	   Icon[3]->load(centerActiveIcon);
	   getPressName((char*)centerActiveIcon, &pressIconName[0],sizeof(pressIconName));
	   if (QFile::exists(pressIconName))
	     {
	       if(!pressIcon[3])
		 pressIcon[3] = new QPixmap();
	       pressIcon[3]->load(pressIconName);   	
	     }
    }
    periodo=period;
    numFrame=number;
}

void banner::SetIcons( const char *sxIcon , const char *dxIcon,const char*centerActiveIcon,const char*centerInactiveIcon,char inactiveLevel)
{
    SetIcons( sxIcon , dxIcon,centerActiveIcon,centerInactiveIcon,(const char*)NULL,inactiveLevel);
}

void banner::SetIcons( const char *sxIcon , const char *dxIcon,const char*centerActiveIcon,const char*centerInactiveIcon, const char*breakIcon, char inactiveLevel)
{
   char nomeFile[MAX_PATH];
	
   if(!Icon[0])
     Icon[0] = new QPixmap();
   if(!Icon[1])
     Icon[1] = new QPixmap();
   
   
    if (sxIcon)
    {    
	Icon[0]->load(sxIcon);
	getPressName((char*)sxIcon, &nomeFile[0],sizeof(nomeFile));
	if (QFile::exists(nomeFile))
	{
	  if(!pressIcon[0])
	    pressIcon[0] = new QPixmap();
	  pressIcon[0]->load(nomeFile);
	 }
    }
    if (dxIcon)
    {    
	getPressName((char*)dxIcon, &nomeFile[0],sizeof(nomeFile));
	Icon[1]->load(dxIcon);
	if (QFile::exists(nomeFile))
	{
	  if(!pressIcon[1])
	    pressIcon[1] = new QPixmap();
	  pressIcon[1]->load(nomeFile);
	 }
    }
    
    if(!Icon[2])
      Icon[2] = new QPixmap();
    if(!Icon[3])
      Icon[3] = new QPixmap();
    memset(nomeFile,'\000',sizeof(nomeFile));
    strncpy(nomeFile,centerInactiveIcon,strstr(centerInactiveIcon,".")-centerInactiveIcon);
    strcat(nomeFile,"sxl0");
    strcat(nomeFile,strstr(centerInactiveIcon,"."));
    Icon[2]->load(nomeFile);	
    qDebug("Icon[2] <- %s", nomeFile);
    nomeFile[strstr(nomeFile,".")-nomeFile-4]='d';
    Icon[3]->load(nomeFile);		
    qDebug("Icon[3] <- %s", nomeFile);
    
    char idy=0;
    char suff[10];
 
    for (char idx=minValue;idx<=maxValue;idx+=step,idy++)
    {
        if(!Icon[4+idy*2])
	  Icon[4+idy*2]=new QPixmap();
	memset(nomeFile,'\000',sizeof(nomeFile));
	strncpy(nomeFile,centerActiveIcon,strstr(centerActiveIcon,".")-centerActiveIcon);
	sprintf(&suff[0],"sxl%d",idx);
	strcat(nomeFile,&suff[0]);
	strcat(nomeFile,strstr(centerActiveIcon,"."));
	Icon[4+idy*2]->load(nomeFile);	
	qDebug("Lev = %d, Icon[%d] <- %s", idx, 4+idy*2, nomeFile);
	if (inactiveLevel)
	{
	    if(!Icon[22+idy*2])
	      Icon[22+idy*2]=new QPixmap();
	    memset(nomeFile,'\000',sizeof(nomeFile));
	    strncpy(nomeFile,centerInactiveIcon,strstr(centerInactiveIcon,".")-centerInactiveIcon);
	    sprintf(&suff[0],"sxl%d",idx);
	    strcat(nomeFile,&suff[0]);
	    strcat(nomeFile,strstr(centerInactiveIcon,"."));
	    Icon[22+idy*2]->load(nomeFile);		
	    qDebug("Icon[%d] <- %s", 22+idy*2, nomeFile);
	}
    }
    for (char idx=minValue,idy=0;idx<=maxValue;idx+=step,idy++)
    {
        if(!Icon[5+idy*2])
	  Icon[5+idy*2]=new QPixmap();
	memset(nomeFile,'\000',sizeof(nomeFile));
	strncpy(nomeFile,centerActiveIcon,strstr(centerActiveIcon,".")-centerActiveIcon);
	sprintf(&suff[0],"dxl%d",idx);
	strcat(nomeFile,&suff[0]);
	strcat(nomeFile,strstr(centerActiveIcon,"."));
	Icon[5+idy*2]->load(nomeFile);	
	qDebug("Lev = %d, Icon[%d] <- %s", idx, 5+idy*2, nomeFile);
	if (inactiveLevel)
	{
	    if(!Icon[23+idy*2])
	      Icon[23+idy*2]=new QPixmap();
	    memset(nomeFile,'\000',sizeof(nomeFile));
	    strncpy(nomeFile,centerInactiveIcon,strstr(centerInactiveIcon,".")-centerInactiveIcon);
	    sprintf(&suff[0],"dxl%d",idx);
	    strcat(nomeFile,&suff[0]);
	    strcat(nomeFile,strstr(centerInactiveIcon,"."));
	    Icon[23+idy*2]->load(nomeFile);	
	    qDebug("Icon[%d] <- %s", 23+idy*2, nomeFile);
	}
    }

    if (breakIcon)
    {
        if(!Icon[44])
	    Icon[44] = new QPixmap();
	if(!Icon[45])
	    Icon[45] = new QPixmap();
	memset(nomeFile,'\000',sizeof(nomeFile));
	strncpy(nomeFile,breakIcon,strstr(breakIcon,".")-breakIcon);
	strcat(nomeFile,"sx");
	strcat(nomeFile,strstr(breakIcon,"."));
	Icon[44]->load(nomeFile);	
	qDebug("Icon[%d] <- %s", 44, nomeFile);
	nomeFile[strstr(nomeFile,".")-nomeFile-2]='d';
	Icon[45]->load(nomeFile);	
	qDebug("Icon[%d] <- %s", 45, nomeFile);
    }
    	    
}


void banner::addItem(char item,int x,int y,int dimX, int dimY)
{
    QWidget * Item=NULL;
 
    switch (item){
    case BUT1: sxButton = new BtButton(this,"Bottone di sinistra"); 
	    connect(sxButton,SIGNAL(clicked()),this,SIGNAL(sxClick()));
	    connect(sxButton,SIGNAL(pressed()),this,SIGNAL(sxPressed()));
  	    connect(sxButton,SIGNAL(released()),this,SIGNAL(sxReleased()));
	    Item = sxButton;
	    break;
    case BUT2: dxButton = new BtButton(this,"Bottone di destra");
	    connect(dxButton,SIGNAL(clicked()),this,SIGNAL(dxClick()));
	    connect(dxButton,SIGNAL(pressed()),this,SIGNAL(dxPressed()));
 	    connect(dxButton,SIGNAL(released()),this,SIGNAL(dxReleased()));
	    Item = dxButton;
	    break;
    case TEXT: BannerText = new BtLabel(this,"Testo"); Item = BannerText; break;
    case ICON: BannerIcon = new BtLabel(this,"Icona"); Item = BannerIcon; break;
    case ICON2: BannerIcon2 = new BtLabel(this, "Icona2"); 
	Item = BannerIcon2; break;
    case BUT3: csxButton = new BtButton(this,"Bottone di centrosinistra");  
	  connect(csxButton,SIGNAL(clicked()),this,SIGNAL(csxClick()));
	  connect(csxButton,SIGNAL(pressed()),this,SIGNAL(csxPressed()));
 	  connect(csxButton,SIGNAL(released()),this,SIGNAL(csxReleased()));
	  Item = csxButton;
	  break;
    case BUT4: cdxButton = new BtButton(this,"Bottone di centrodestra");
	  connect(cdxButton,SIGNAL(clicked()),this,SIGNAL(cdxClick()));
  	  connect(cdxButton,SIGNAL(pressed()),this,SIGNAL(cdxPressed()));
 	  connect(cdxButton,SIGNAL(released()),this,SIGNAL(cdxReleased()));
	  Item = cdxButton;
	  break;
      case TEXT2: SecondaryText = new BtLabel(this,"Testo secondario"); Item = SecondaryText; break;
	}
    Item->setGeometry(x,y,dimX,dimY);
 //   Item->setPaletteBackgroundColor( QColor :: QColor(BG_R,BG_G,BG_B));  
/*   if ((parentWidget(FALSE)  ->parentWidget(FALSE) ))
    {
	Item -> setPaletteBackgroundColor( (parentWidget(FALSE)  -> parentWidget(FALSE) ) -> backgroundColor());		
	Item -> setPaletteForegroundColor( (parentWidget(FALSE)  -> parentWidget(FALSE) ) -> foregroundColor());	
    }*/
    
}
void banner::nascondi(char item)
{
    switch (item){
    case BUT1: if(sxButton)
		   sxButton->hide();
	       break;
    case BUT2: if(dxButton)
		   dxButton->hide();
	    break;
    case TEXT: if(BannerText)        
 		   BannerText->hide();
 	       break;
    case ICON: if(BannerIcon)
		   BannerIcon->hide();
	       break;
    case BUT3: if(csxButton )
		   csxButton->hide();
	       break;
    case BUT4: if(cdxButton)
		   cdxButton->hide();
	       break;
     case TEXT2: if (SecondaryText)
		      SecondaryText->hide();
	       break;
	}
}
void banner::mostra(char item)
{
    switch (item){
    case BUT1: if(sxButton)
		   sxButton->show();
	       break;
    case BUT2: if(dxButton)
		   dxButton->show();
	    break;
    case TEXT: if(BannerText)
		   BannerText->show();
	       break;
    case ICON: if(BannerIcon)
		   BannerIcon->show();
	       break;
    case BUT3: if(csxButton )
		   csxButton->show();
	       break;
    case BUT4: if(cdxButton)
		   cdxButton->show();
	       break;
     case TEXT2: if (SecondaryText)
		    SecondaryText->show();
	       break;
	}
}   
    
void banner::Draw()
{
    qDebug("banner::Draw(), attivo = %d, value = %d", attivo, value);
    if ( (sxButton) && (Icon[0]) )
    {
	sxButton->setPixmap(*Icon[0]);
	if (pressIcon[0])
	    sxButton->setPressedPixmap(*pressIcon[0]);
    }
 
    if ( (dxButton) && (Icon[1]) )
    {
	dxButton->setPixmap(*Icon[1]);
	if (pressIcon[1])
	    dxButton->setPressedPixmap(*pressIcon[1]);
    }
          if (minValue==maxValue)
    {
	    QPixmap *pntIcon=NULL;
      
	    if  (attivo==1)
		pntIcon=Icon[3+contFrame];
    	    if  (attivo==2)
	    {
		pntIcon=Icon[4+contFrame];
	    }
	    else if (!attivo)
		pntIcon=Icon[2];
	    if ( (pntIcon) && (BannerIcon) )
	    {
		BannerIcon->repaint();
		BannerIcon->setPixmap(*pntIcon);    
		BannerIcon->repaint();
	    }
	    
	    if ( (Icon[2]) && (csxButton) )
	    {
		csxButton->setPixmap(*Icon[2]);
		if (pressIcon[2])
		    csxButton->setPressedPixmap(*pressIcon[2]);
	    }
 
	    if ( (cdxButton) && (Icon[3]) )
	    {
		cdxButton->setPixmap(*Icon[3]);
		if (pressIcon[3])
		    cdxButton->setPressedPixmap(*pressIcon[3]);
	    }
	   qApp-> flush();    
       }
      else
      {
	  if (attivo==1)
	    {
		    if ( (Icon[4+((value-step)/step)*2]) && (csxButton) )
		    {
			csxButton->setPixmap(*Icon[4+((value-step)/step)*2]);
			qDebug("* Icon[%d]", 4+((value-step)/step)*2);
		    }
		    if ( (cdxButton) && (Icon[5+((value-step)/step)*2]) )
		    {
			cdxButton->setPixmap(*Icon[5+((value-step)/step)*2]);
			qDebug("** Icon[%d]", 5+((value-step)/step)*2);
		    }
    	   }
	  else if (attivo==0)
	  {
		    if (Icon[22])
		    {
			if ( (Icon[22+(value-step)/step*2]) && (csxButton) )
			{
			    csxButton->setPixmap(*Icon[22+((value-step)/step)*2]);			
			    qDebug("*** Icon[%d]", 22+((value-step)/step)*2);
			}
			
			if ( (cdxButton) && (Icon[23+((value-step)/step)*2]) )
			{
			    cdxButton->setPixmap(*Icon[23+((value-step)/step)*2]);    				
			     qDebug("**** Icon[%d]", 23+((value-step)/step)*2);
			}
		    }	  	
		    else
		    {
			if ( (Icon[2]) && (csxButton) )
			{
			    csxButton->setPixmap(*Icon[2]);	
			    qDebug("***** Icon[%d]", 2);
			}
			
			if ( (cdxButton) && (Icon[3]) )
			{
			    cdxButton->setPixmap(*Icon[3]); 
			    qDebug("****** Icon[%d]", 3);
			}
		    }
		}
	  else if (attivo==2)
	  {
		    if ( (Icon[44]) && (csxButton) )
			{
			    csxButton->setPixmap(*Icon[44]);		    
			    qDebug("******* Icon[%d]", 44);
			}
			
			if ( (cdxButton) && (Icon[45]) )
			{
			    cdxButton->setPixmap(*Icon[45]);    
			    qDebug("******* Icon[%d]", 45);
			}
	}
	  
      }
     
        /*TODO
      icona animata -> armare timer per cambiare centerActiveIcon
     qui devo copiare period e number e in draw attivare il timer*/
      if ( (periodo) && (numFrame) )
      {	   
	  if (!animationTimer)
	  {
	      animationTimer = new QTimer(this,"clock");
	       connect(animationTimer,SIGNAL(timeout()),this,SLOT(animate()));
	   }
	  if ( !(animationTimer->isActive() ) && (attivo))
	      animationTimer->start(periodo);
	/* connect(animationTimer,SIGNAL(timeout()),this,SLOT(animate()));*/
      }
      
      
   
   if (BannerText)
   {
     BannerText->setAlignment(AlignHCenter|AlignVCenter);//AlignTop);
     BannerText->setFont( QFont( "helvetica", 14, QFont::Bold ) );
     BannerText->setText(testo);
//     qDebug("TESTO: %s", testo);
   }
   if (SecondaryText)
   {	
     SecondaryText->setAlignment(AlignHCenter|AlignVCenter);
     SecondaryText->setFont( QFont( "helvetica", 18, QFont::Bold ) );
     SecondaryText->setText(testoSecondario);
   }
}

void banner::impostaAttivo(char Attivo)
{
    attivo=Attivo;
    if ( (animationTimer) && (!Attivo) )
    {
	animationTimer->stop();
	  qDebug("KILLanimationTimer");   
    }
}

void banner::setAddress(char* indirizzo)
{
    strncpy (address,indirizzo,sizeof(address));
  //  address=new char (*indirizzo);
//    address=indirizzo;
}

void banner::gestFrame(char*){}


void banner::setBGColor(int r, int g, int b)
{	
   setBGColor( QColor :: QColor(r,g,b));    
}
void banner::setFGColor(int r, int g, int b)
{
    setFGColor(QColor :: QColor(r,g,b));
}

void banner::setBGColor(QColor c)
{
    setPaletteBackgroundColor(c);
    if (BannerIcon )
	BannerIcon ->setPaletteBackgroundColor(c) ;
    if (BannerText)
	BannerText->setPaletteBackgroundColor(c);
    if (SecondaryText)
	SecondaryText->setPaletteBackgroundColor(c);
    if (sxButton)
	sxButton->setPaletteBackgroundColor(c);
    if (dxButton)
	dxButton->setPaletteBackgroundColor(c);
    if (csxButton)
	csxButton->setPaletteBackgroundColor(c);
    if (cdxButton)
	cdxButton->setPaletteBackgroundColor(c);  
}
void banner::setFGColor(QColor c)	
{
    setPaletteForegroundColor(c);
    if (BannerIcon )
	BannerIcon ->setPaletteForegroundColor(c) ;
    if (BannerText)
	BannerText->setPaletteForegroundColor(c);
    if (SecondaryText)
	SecondaryText->setPaletteForegroundColor(c);
    if (sxButton)
	sxButton->setPaletteForegroundColor(c);
    if (dxButton)
	dxButton->setPaletteForegroundColor(c);
    if (csxButton)
	csxButton->setPaletteForegroundColor(c);
    if (cdxButton)
	cdxButton->setPaletteForegroundColor(c);  
}





void banner::setValue(char val)
{
    if ( (val>=minValue) && (val<=maxValue) )
	value=val;
}
void banner::aumValue()
{
    if (value<maxValue)
	value+=step;
}
void banner::decValue()
{
    if (value>minValue)
	value-=step;
}

void banner::setMaxValue(char val)
{
    maxValue=val;
}
void banner::setMinValue(char val)
{
    minValue=val;
}
void banner::setRange(char minval,char maxval)
{
    maxValue=maxval;
    minValue=minval;
}

void banner::setStep(char s)
{
    step = s;
}
    
unsigned char banner::isActive()
{
 return attivo;
/*    if (attivo)
	return(TRUE);
    return(FALSE);*/
}
    
char banner::getValue()
{
     return(value);
}

void banner::animate()
{
    contFrame++;
    if (contFrame>=numFrame)
	contFrame=0;
    Draw();
}

void banner::setAnimationParams(int per ,int num)
{
    periodo=per;
    numFrame=num;
}

void banner::getAnimationParams(int& per, int& num)
{
    per = periodo;
    num = numFrame;
}

void banner::setNumRighe( uchar n )
{
	numRighe=n;
}

char* banner::getAddress( )
{
    return (&address[0]);
}
    
void banner::setChi(char* indirizzo)
{
    strncpy(&chi[0],indirizzo,sizeof(chi));
}
char* banner::getChi( )
{
    return (&chi[0]);
}
void banner::setGroup(bool* gr)
{
   qDebug("%s setted GROUP",getAddress());
    for (char idx=0;idx<9;idx++)
    {
	group[idx]=gr[idx];
    }
}
bool* banner::getGroup( )
{
    return (&group[0]);
}
void banner::setPul( )
{
    pul=TRUE;
    qDebug("%s setted PUL",getAddress());
}
bool banner::getPul( )
{
    return(pul);
}

bool banner::isForMe(openwebnet& m)
{
    if(strcmp(m.Extract_chi(), "1")) return false ;
    if(!strcmp(m.Extract_dove(),getAddress())) return true;
    // BAH
    return (!getPul() && ((!strcmp(m.Extract_dove(),"0")) ||
			  ((strlen(m.Extract_dove())==1) && 
			   (!strncmp(m.Extract_dove(), getAddress(), 1)) ) || 
			  ((!strncmp(m.Extract_dove(),"#",1)) && 
			   *(getGroup()+(atoi(m.Extract_dove()+1))-1))));
}

void banner:: inizializza(bool forza){}

//char* banner::getChi(){return(NULL);}
void  banner::rispStato(char*){}



void banner::hide(){QWidget::hide(); }
void banner::show(){QWidget::show();}
void banner::openAckRx(void)
{
    qDebug("openAckRx()");
}
void banner::openNakRx(void)
{
    qDebug("openNakRx()");
}
void banner::setSerNum(int s){serNum=s;}
int    banner::getSerNum(){return(serNum);}
char banner::getId(){return(id);}
void banner::setId(char i){id=i;}
unsigned char banner::getState(){return (stato); }
char* banner::getManIcon(){return NULL;}
char* banner::getAutoIcon(){return NULL;}

void banner::ambChanged(char *, bool, void *) { };

void banner::parentChanged(QWidget *newParent) { };
void banner::grandadChanged(QWidget *newGrandad) { };
void banner::addAmb(char *) {};
