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
#include "openclient.h"
#include "genericfunz.h"
#include "sottomenu.h"

//#include "btbutton.h"
#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>


/*****************************************************************
**setdataOra
****************************************************************/

setDataOra::setDataOra( sottoMenu *parent,const char *name )
        : bannOnDx( parent, name )
{   
     SetIcons( ICON_INFO,1);
 //     connect(this,SIGNAL(click()),this,SLOT(Attiva()));
      
//    settalora =  impostaTime (NULL,"impOra");
     settalora.setBGColor(backgroundColor()/*BG_R, BG_G, BG_B*/);  
     settalora.setFGColor(foregroundColor()/*255-BG_R, 255-BG_G, 255-BG_B*/);  
     connect(this,SIGNAL(click()),&settalora,SLOT(mostra()));      
     connect(&settalora,SIGNAL(sendFrame(char*)), this, SIGNAL(sendFrame(char*)));
}

/*****************************************************************
**impostaSveglia
****************************************************************/

impostaSveglia::impostaSveglia( QWidget *parent,const char *name,diffSonora* diso, char*h, char*m, char* icon1, char*icon2, int enabled , int freq , char* descr1, char* descr2, char* descr3, char* descr4,char* frame, int tipo)
        : bann2But( parent, name )
{   
     strcpy(&iconOn[0], icon1);
     strcpy(&iconOff[0], icon2);
/*    strncpy(&iconOn[0], icon1, sizeof(iconOn));
     strncpy(&iconOff[0], icon2, sizeof(iconOff));*/
     SetIcons( &iconOff[0] ,ICON_INFO);
     svegliolina = new sveglia(NULL,"svegliolina",(uchar) freq, (uchar) tipo,diso, frame,descr1,descr2,descr3,descr4,h,m);
     svegliolina->setBGColor(backgroundColor());  
     svegliolina->setFGColor(foregroundColor());  
     svegliolina->hide();
     if (enabled==1)
	 setAbil(TRUE);
     else
	 setAbil(FALSE);	 
     connect(this,SIGNAL(dxClick()),svegliolina,SLOT(mostra()));      
     connect(this,SIGNAL(dxClick()),parentWidget(),SLOT(hide()));           
     connect(this,SIGNAL(sxClick()),this,SLOT(toggleAbil()));    
     
     connect(parentWidget() , SIGNAL(gestFrame(char*)),svegliolina,SLOT(gestFrame(char*))); 
     connect(svegliolina,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*)));      
     connect(svegliolina,SIGNAL(ImClosed()),parentWidget() , SLOT(showFullScreen()));      
     connect(svegliolina, SIGNAL(freeze(bool)),this , SIGNAL(freeze(bool)));     
     connect(this, SIGNAL(freezed(bool)), svegliolina,SLOT(spegniSveglia(bool)));     
}

void impostaSveglia::gestFrame(char* frame)
{	
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
//    qDebug("impostaSveglia b= %d",b);
    svegliolina-> activateSveglia(b);
    show();
}      
void  impostaSveglia::show()
{
    if (svegliolina->getActivation())
    {
	SetIcons(uchar(0),&iconOn[0]);
    }
   else
    {
       	SetIcons(uchar(0),&iconOff[0]);
    }
   Draw();
    QWidget::show();
    svegliolina->setSerNum(getSerNum());
}

void impostaSveglia::inizializza()
{
    svegliolina->inizializza();
}


/*****************************************************************
**calibration
****************************************************************/
//#include "calibrate.h"

calibration::calibration( sottoMenu  *parent,const char *name, const char* icon )
        : bannOnDx( parent, name )
{   
//     SetIcons(icon,1);
      SetIcons( ICON_INFO,1);
     connect(this,SIGNAL(click()),this,SLOT(doCalib()));      
}

void calibration::doCalib()
{
    calib = new Calibrate();
    calib->show();
    hide();
    connect(calib,SIGNAL(fineCalib()), this, SLOT(fineCalib()));
}
void calibration::fineCalib()
{
    show();
    delete(calib);
}

/*****************************************************************
**beep
****************************************************************/

impBeep::impBeep( sottoMenu  *parent,const char *name ,char* val, const char * icon1, const char *icon2)
        : bannOnSx( parent, name )
{   
     strncpy(&iconOn[0], icon1, sizeof(iconOn));
     strncpy(&iconOff[0], icon2, sizeof(iconOff));
     SetIcons( &iconOff[0],1);
//     setBeep(FALSE);//non va bene, bisogna prendere il valore di partenza dal costruttore
     connect(this,SIGNAL(click()),this,SLOT(toggleBeep()));      
     if (strcmp(val,"1"))
	 setBeep(FALSE,FALSE);
     else
	 setBeep(TRUE,FALSE);
}

void impBeep::toggleBeep()
{
    if(getBeep())
    {
	setBeep(FALSE,TRUE);
	SetIcons(uchar(0), &iconOff[0]);
    }
    else
    {
	setBeep(TRUE,TRUE);
	SetIcons(uchar(0), &iconOn[0]);	
	beep();
    }
    Draw();
}

/*****************************************************************
**contrasto
****************************************************************/

impContr::impContr( sottoMenu  *parent,const char *name ,char* val, const char * icon1)
        : bannOnDx( parent, name )
{   
     SetIcons( icon1,1);
     connect(this,SIGNAL(click()),this,SLOT(showContr()));      
     setContrast(atoi(val),FALSE);
}

void impContr::showContr()
{
    hide();
    contrasto = new contrPage(NULL,"contr"); 
    connect(contrasto, SIGNAL(Close()), this, SLOT(contrMade()));

    contrasto->setBGColor(backgroundColor());  
    contrasto->setFGColor(foregroundColor());  
    contrasto->show();
}

void impContr::contrMade()
{
    show();
    contrasto->hide();
    setContrast(getContrast(),TRUE);
    delete(contrasto);
}


/*****************************************************************
**machVers
****************************************************************/

machVers::machVers( sottoMenu  *parent,const char *name, versio * ver, const char* icon1)
        : bannOnDx( parent, name )
{   
         SetIcons( icon1,1);
    connect(this,SIGNAL(click()),this,SLOT(showVers()));   
    v=ver;
     }

void machVers::showVers()
{
     v->setPaletteBackgroundColor(backgroundColor());  
     v->setPaletteForegroundColor(foregroundColor());  
     v->show();
     
    tiempo = new QTimer(this,"clock");
    tiempo->start(10000,TRUE);
    connect(tiempo,SIGNAL(timeout()),this,SLOT(tiempout()));
 }
void machVers::tiempout()
{
    v->hide();
    delete(tiempo);
}



/*****************************************************************
**impPassword 
****************************************************************/

//impPassword ::impPassword ( QWidget *parent,const char *name, char* attiva,char* password, char* icon1, char*icon2,char* icon3)
impPassword ::impPassword ( QWidget *parent,const char *name, char* icon1, char*icon2,char* icon3, char* password,int attiva)	
        : bann2But( parent, name )
{   

     strncpy(&iconOn[0], icon1, sizeof(iconOn));
     strncpy(&iconOff[0], icon2, sizeof(iconOff));
     strncpy(&paswd[0], password, sizeof(paswd));     
   
     
     SetIcons( &iconOff[0] ,icon3);
	  
     tasti = new tastiera(NULL,"tast");
     
     tasti->setBGColor(backgroundColor());  
     tasti->setFGColor(foregroundColor());  
     tasti->hide();
     tasti->setMode(HIDDEN);
     connect(this,SIGNAL(dxClick()),tasti,SLOT(showTastiera()));      
     connect(this,SIGNAL(sxClick()),this,SLOT(toggleActivation()));    
     
     connect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));      
     connect(this, SIGNAL(setPwd(bool,char*)), parentWidget(), SIGNAL(setPwd(bool,char*)));
     
/*     if (!strcmp(attiva,"1"))
	 active=TRUE;
      else
 	 active=FALSE;      */
     if (attiva==1)
	 active=TRUE;
     else
	 active=FALSE;
      
      emit(setPwd(active,&paswd[0]));	 
//     connect(this, SIGNAL(freezed(bool)), tasti,SLOT(freezed(bool)));     --da mettere
 }


void impPassword::toggleActivation()
{

    if (active)
    {
	active=FALSE;
	setCfgValue(PROTEZIONE, "enabled","0",getSerNum());
    }
    else
    {
	active=TRUE;
	setCfgValue(PROTEZIONE, "enabled","1",getSerNum());	
    }
    emit(setPwd(active,&paswd[0]));
    show();
}      

void  impPassword::show()
{
    if (active)
	SetIcons(uchar(0),&iconOn[0]);
   else
       	SetIcons(uchar(0),&iconOff[0]);
   Draw();
   qDebug("passwd = %s %d", &paswd[0], paswd[0]);
   if (paswd[0]=='\000')
   {
       qDebug("passwd = ZERO");
       disconnect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));
       disconnect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow2(char*)));
       tasti->setMode(CLEAN);	
       connect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow2(char*))); 
   }
   else
   {
       disconnect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));  
       disconnect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow2(char*)));  
       tasti->setMode(HIDDEN);       
       connect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*))); 
   }
    QWidget::show();
}

void  impPassword::reShow1(char* c)
{
    if (c==NULL)
    {
	show();
	return;
    }
    if (strcmp(&paswd[0],c))
    {
	   show();
	   qDebug("password errata doveva essere %s",&paswd[0]);
	   sb=getBeep();
	   setBeep(TRUE,FALSE);
	   beep(1000);
	   tiempo = new QTimer(this,"clock");
	   tiempo->start(1100,TRUE);
	   connect(tiempo,SIGNAL(timeout()),this,SLOT(tiempout()));
    }
    else
    {
	connect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow2(char*)));  
	disconnect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));  
	tasti->setMode(CLEAN);	
	tasti->showTastiera();
	qDebug("password giusta");
    }
}

void  impPassword::reShow2(char* c)
{
    if (c)
    {
	connect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));  
	disconnect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow2(char*)));  
	strcpy(&paswd[0],c);
	setCfgValue(PROTEZIONE, "value",&paswd[0],getSerNum());
	emit(setPwd(active,&paswd[0]));
    }
    show();
    tasti->setMode(HIDDEN);
}

void impPassword::tiempout()
{
    delete(tiempo);
    setBeep(sb,FALSE);	   
}
