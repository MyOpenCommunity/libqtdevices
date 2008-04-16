/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannFrecce.cpp
**
**Riga con tasto ON OFF, icona centrale e scritta sotto
**
****************************************************************/

#include "bannfrecce.h"
#include "main.h"
//#include "btbutton.h"
#include <qpixmap.h>


bannFrecce::bannFrecce( QWidget *parent,const char *name ,uchar num,char* IconBut4)
        : banner( parent, name )
{
    qDebug("bannFrecce::bannFrecce(%d)", num);
    addItem(BUT1,(MAX_WIDTH/4-BUTFRECCE_DIM_X)/2,
	    (MAX_HEIGHT/numRighe-BUTFRECCE_DIM_Y)/2, BUTFRECCE_DIM_X , 
	    BUTFRECCE_DIM_Y );
    if ( (num>1) && (num<8) ) {
	addItem(BUT3 , MAX_WIDTH/4+(MAX_WIDTH/4-BUTFRECCE_DIM_X)/2 , 
		(MAX_HEIGHT/numRighe-BUTFRECCE_DIM_Y)/2 , 
		BUTFRECCE_DIM_X , BUTFRECCE_DIM_Y );
	if ((num>2) && (num<7) ) {	   
	    addItem(BUT4 , MAX_WIDTH/2+(MAX_WIDTH/4-BUTFRECCE_DIM_X)/2 , 
		    (MAX_HEIGHT/numRighe-BUTFRECCE_DIM_Y)/2 , 
		    BUTFRECCE_DIM_X, BUTFRECCE_DIM_Y );
	}
    }
    if  ( ((num>3) && (num<6) ) || (num==9) ) {
	addItem(BUT2, MAX_WIDTH*3/4+(MAX_WIDTH/4-BUTFRECCE_DIM_X)/2 , 
		(MAX_HEIGHT/numRighe-BUTFRECCE_DIM_Y)/2 , BUTFRECCE_DIM_X ,
		BUTFRECCE_DIM_Y );
    }
    if (num<5)
	SetIcons (ICON_FRECCIA_SX , IconBut4 , 
		  ICON_FRECCIA_SU , ICON_FRECCIA_GIU );
    else
	SetIcons ( ICON_OK ,IconBut4 , ICON_FRECCIA_SU , ICON_FRECCIA_GIU );

    connect(this, SIGNAL(sxClick()), this, SIGNAL(backClick()));
    connect(this, SIGNAL(dxClick()), this, SIGNAL(forwardClick()));
    connect(this, SIGNAL(csxClick()), this, SIGNAL(upClick()));
    connect(this, SIGNAL(cdxClick()), this, SIGNAL(downClick()));

    connect(this, SIGNAL(sxPressed()), this, SIGNAL(backPress()));
    connect(this, SIGNAL(dxPressed()), this, SIGNAL(forwardPress()));
    connect(this, SIGNAL(csxPressed()), this, SIGNAL(upPress()));
    connect(this, SIGNAL(cdxPressed()), this, SIGNAL(downPress()));

    connect(this, SIGNAL(sxReleased()), this, SIGNAL(backRelease()));
    connect(this, SIGNAL(dxReleased()), this, SIGNAL(forwardRelease()));
    connect(this, SIGNAL(csxReleased()), this, SIGNAL(upRelease()));
    connect(this, SIGNAL(cdxReleased()), this, SIGNAL(downRelease()));

    connect(this, SIGNAL(sxPressed()), this, SLOT(handleBackPress()));
    connect(this, SIGNAL(sxReleased()), this, SLOT(handleBackRelease()));
    connect(&press_timer, SIGNAL(timeout()), SIGNAL(longBackPress()));

    Draw();
}

void bannFrecce::handleBackPress()
{
	qDebug("***************handleBackPress");

	press_timer.start(3000, TRUE);
}

void bannFrecce::handleBackRelease()
{
	qDebug("***************handleBackRelease");

	press_timer.stop();
}
