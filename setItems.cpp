/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** setItems.cpp
**
**
**definizione dei vari items
****************************************************************/
#include "setitems.h"
#include "main.h"

//#include "btbutton.h"
#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>


/*****************************************************************
**setdataOra
****************************************************************/

setDataOra::setDataOra( QWidget *parent,const char *name )
        : bannOnDx( parent, name )
{   
     SetIcons( ICON_OK_80,1);
 //     connect(this,SIGNAL(click()),this,SLOT(Attiva()));
      
//    settalora =  impostaTime (NULL,"impOra");
     
     settalora.setBGColor(backgroundColor()/*BG_R, BG_G, BG_B*/);  
     settalora.setFGColor(foregroundColor()/*255-BG_R, 255-BG_G, 255-BG_B*/);  
     connect(this,SIGNAL(click()),&settalora,SLOT(mostra()));      
}

/*****************************************************************
**impostaSveglia
****************************************************************/

impostaSveglia::impostaSveglia( QWidget *parent,const char *name,diffSonora* diso, char* icon1, char*icon2, char* frame, int freq, int tipo )
        : bann2But( parent, name )
{   

     strncpy(&iconOn[0], icon1, sizeof(iconOn));
     strncpy(&iconOff[0], icon2, sizeof(iconOff));
     
     SetIcons( &iconOff[0] ,ICON_INFO);
	  
     svegliolina = new sveglia(NULL,"svegliolina",(uchar) freq, (uchar) tipo,diso, frame);
     
     svegliolina->setBGColor(backgroundColor()/*BG_R, BG_G, BG_B*/);  
     svegliolina->setFGColor(foregroundColor()/*255-BG_R, 255-BG_G, 255-BG_B*/);  
     svegliolina->hide();
     connect(this,SIGNAL(dxClick()),svegliolina,SLOT(mostra()));      
     connect(this,SIGNAL(sxClick()),this,SLOT(toggleAbil()));    
     
//    connect(this , SIGNAL(gestFrame(char*)),svegliolina,SLOT(gestFrame(char*))); 
     connect(parentWidget() , SIGNAL(gestFrame(char*)),svegliolina,SLOT(gestFrame(char*))); 
     connect(svegliolina,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*)));      
     connect(svegliolina,SIGNAL(ImClosed()),this , SLOT(show()));      
     connect(svegliolina,SIGNAL(freeze(bool)),this , SIGNAL(freeze(bool)));     
     connect(this, SIGNAL(spegniSveglia()), svegliolina,SLOT(spegniSveglia()));     
}

void impostaSveglia::gestFrame(char* frame)
{	
//       qDebug("SLOT SVEGLIA GESTFRAME");
   //      emit gFrame(fr);
}

void impostaSveglia::setAbil(bool b) 
{
    svegliolina->activateSveglia(b);
     show();
}
void impostaSveglia::toggleAbil()
{
    bool b;
    b=svegliolina->getActivation()^1;
    qDebug("impostaSveglia b= %d",b);
    svegliolina-> activateSveglia(b);
    show();
}      
void  impostaSveglia::show()
{
    if (svegliolina->getActivation())
	SetIcons(uchar(0),&iconOn[0]/*ICON_SVEGLIA_ON_60*/);
//	SetIcons( ICON_SVEGLIA_ON ,ICON_INFO);  
   else
       	SetIcons(uchar(0),&iconOff[0]/*ICON_SVEGLIA_OFF_60*/);
//                SetIcons( ICON_SVEGLIA_OFF ,ICON_INFO);
   Draw();
    QWidget::show();
}

void impostaSveglia::deFreez()
{
   emit( spegniSveglia());
}

/*****************************************************************
**calibration
****************************************************************/
#include "calibrate.h"

calibration::calibration( QWidget *parent,const char *name )
        : bannOnDx( parent, name )
{   
     SetIcons( ICON_OK_80,1);
     
     //settalora.setBGColor(backgroundColor()/*BG_R, BG_G, BG_B*/);  
     //settalora.setFGColor(foregroundColor()/*255-BG_R, 255-BG_G, 255-BG_B*/);  
     connect(this,SIGNAL(click()),this,SLOT(doCalib()));      
}

void calibration::doCalib()
{
    calib = new Calibrate();
}
