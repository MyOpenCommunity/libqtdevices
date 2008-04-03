/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannRegolaz.cpp
**
**Riga con tasto ON OFF, icona centrale divisa in due per regolazione
**
****************************************************************/

#include "bannregolaz.h"
#include "main.h"

#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qtimer.h>

bannRegolaz::bannRegolaz( QWidget *parent,const char *name )
        : banner( parent, name )
{
    addItem( BUT2 , 0 , 0 , BANREGOL_BUT_DIM , BANREGOL_BUT_DIM );
    addItem( BUT1 , MAX_WIDTH-BANREGOL_BUT_DIM , 0 , BANREGOL_BUT_DIM , BANREGOL_BUT_DIM );
    addItem( BUT3 , (MAX_WIDTH-BUTREGOL_ICON_DIM_X)/2 , 0,BUTREGOL_ICON_DIM_X/2 , BUTREGOL_ICON_DIM_Y );
    addItem( BUT4 , MAX_WIDTH/2 , 0,BUTREGOL_ICON_DIM_X/2 , BUTREGOL_ICON_DIM_Y );
    addItem( TEXT , 0 , BANREGOL_BUT_DIM , MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE-BANREGOL_BUT_DIM );
    
    timRip=NULL;
    
    connect(this,SIGNAL(cdxPressed()),this,SLOT(armTimRipdx()));
    connect(this,SIGNAL(cdxReleased()),this,SLOT(killTimRipdx()));    
    connect(this,SIGNAL(csxPressed()),this,SLOT(armTimRipsx()));
    connect(this,SIGNAL(csxReleased()),this,SLOT(killTimRipsx()));    
    
}


void bannRegolaz::armTimRipdx()
{
    if  (!timRip) 
	timRip = new QTimer(this,"clock");
   if (!(timRip->isActive()) )
    {
	    timRip->start(TIME_RIP_REGOLAZ);
   	    disconnect(timRip,SIGNAL(timeout()),this,SIGNAL(csxClick()));
	    connect(timRip,SIGNAL(timeout()),this,SIGNAL(cdxClick()));
	}
}
void bannRegolaz::armTimRipsx()
{
     if  (!timRip) 
	timRip = new QTimer(this,"clock");
      if (!(timRip->isActive()) )
    {
	    timRip->start(TIME_RIP_REGOLAZ);
    	    disconnect(timRip,SIGNAL(timeout()),this,SIGNAL(cdxClick()));
	    connect(timRip,SIGNAL(timeout()),this,SIGNAL(csxClick()));
	}
}
void bannRegolaz::killTimRipdx()
{
    if  (timRip) 
	timRip->stop();
}
void bannRegolaz::killTimRipsx()
{
     if  (timRip) 
	timRip->stop();
}
