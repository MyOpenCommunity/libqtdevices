/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannTermo.cpp
**
**Pagina termoregolaizone
**
****************************************************************/

#include "banntermo.h"
#include "main.h"
#include "sottomenu.h"
#include <qfont.h>
#include <qpixmap.h>


bannTermo::bannTermo( QWidget *parent,const char *name,QColor SecondForeground )
        : banner( parent, name )
{
    
    int h=(MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE-BUTMENPLUS_DIM_Y-DESCR_DIM_Y-TEMPMIS_Y-OFFSET_Y)/5;
    for (int idx=0;idx<7;idx++)
    {	
	texts[idx] = new BtLabel(this, "");
	texts[idx] -> setGeometry(idx*MAX_WIDTH/7,h+DESCR_DIM_Y+TEMPMIS_Y,MAX_WIDTH/7,OFFSET_Y);
    }
    tempMis = new BtLabel(this,NULL);
    tempMis -> setGeometry(0,h+DESCR_DIM_Y,MAX_WIDTH,TEMPMIS_Y);
    addItem( BUT1 , 0 , h*3+DESCR_DIM_Y+ TEMPMIS_Y+OFFSET_Y , BUTMENPLUS_DIM_X ,  BUTMENPLUS_DIM_Y);
    addItem( BUT2 , MAX_WIDTH-BUTMENPLUS_DIM_X ,h*3+DESCR_DIM_Y+ TEMPMIS_Y+ OFFSET_Y, BUTMENPLUS_DIM_X , BUTMENPLUS_DIM_Y );
    addItem( TEXT , 0 ,h, MAX_WIDTH , DESCR_DIM_Y );    
    addItem( ICON ,BUTMENPLUS_DIM_X ,h*3+DESCR_DIM_Y+ TEMPMIS_Y+OFFSET_Y, MAX_WIDTH -2*BUTMENPLUS_DIM_X, BUTMENPLUS_DIM_Y);    
    nascondi(ICON);
    
    tempImp= new BtLabel(this,"0.00\272C");
    tempImp -> setGeometry(BUTMENPLUS_DIM_X ,h*3+DESCR_DIM_Y+ TEMPMIS_Y+OFFSET_Y, MAX_WIDTH -2*BUTMENPLUS_DIM_X, BUTMENPLUS_DIM_Y);
    
    sondoffanti = new BtLabel(this, "");
    sondoffanti -> setGeometry(0,h+DESCR_DIM_Y+TEMPMIS_Y,MAX_WIDTH,OFFSET_Y);
    
    memset(temp,'\000',sizeof(temp));
    strcpy(&temp[0],"-23.5\272C");
    strcpy(&setpoint[0],&temp[0]);
    secondForeground=QColor(SecondForeground);
    
    val_imp=3;
    ((sottoMenu*)parentWidget())->setNavBarMode(4,ICON_MANUAL_ON);
    isOff=0;
    isAntigelo=0;
}


void bannTermo::Draw()
{
    tempMis -> setFont( QFont( "Times", 40, QFont::Bold ) );
    tempMis -> setAlignment(AlignHCenter|AlignVCenter);
    tempMis -> setText(&temp[0]);
    tempImp -> setFont( QFont( "Times", 25, QFont::Bold ) );
    tempImp -> setPaletteForegroundColor(secondForeground);
    tempImp -> setAlignment(AlignHCenter|AlignVCenter);
    tempImp -> setText(&setpoint[0]);
    if (isOff)
    {
	for (int idx=0;idx<7;idx++)
	{
	    texts[idx] -> hide();
	}
	sondoffanti -> setAlignment(AlignHCenter|AlignVCenter);
	sondoffanti ->setPaletteForegroundColor(foregroundColor());
	sondoffanti -> setText("OFF");
	sondoffanti -> show();   
	sondoffanti ->setPaletteForegroundColor(foregroundColor());
	sondoffanti -> setFont( QFont( "Times", 20, QFont::Bold ) );
    }
    else   if (isAntigelo)
    {
	for (int idx=0;idx<7;idx++)
	{
	    texts[idx] -> hide();
	}
	sondoffanti -> setAlignment(AlignHCenter|AlignVCenter);
	sondoffanti ->setPaletteForegroundColor(foregroundColor());
	sondoffanti -> setText("*");
	sondoffanti -> show();   
	sondoffanti ->setPaletteForegroundColor(foregroundColor());
	sondoffanti -> setFont( QFont( "Times", 40, QFont::Bold ) );
    }
    else
    {
	for (int idx=0;idx<7;idx++)
	{
	    char app[3];
	    memset(app,'\000',sizeof(app));
	    sprintf(&app[0],"%+d",idx-3);
	    texts[idx] -> setText((char*)&app[0]);
	    texts[idx] -> setFont( QFont( "Times", 20, QFont::Bold ) );
	    texts[idx] -> setAlignment(AlignHCenter|AlignVCenter);
	    texts[idx] -> show();
	    if (val_imp==idx)
		texts[idx] ->setPaletteForegroundColor(secondForeground);
	    else
		texts[idx] ->setPaletteForegroundColor(foregroundColor());
	}  
	sondoffanti->hide();
    }
  
    banner::Draw();
}


/*
 if (manual)
       ((sottoMenu*)parentWidget())->setNavBarMode(4,ICON_MANUAL_ON);
   else
       ((sottoMenu*)parentWidget())->setNavBarMode(4,ICON_AUTO_ON);   
       */
