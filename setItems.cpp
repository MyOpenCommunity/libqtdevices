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
     SetIcons( ICON_INFO,1);
 //     connect(this,SIGNAL(click()),this,SLOT(Attiva()));
      
//    settalora =  impostaTime (NULL,"impOra");
     qDebug("SETTAORA");
     settalora.setBGColor(backgroundColor()/*BG_R, BG_G, BG_B*/);  
     settalora.setFGColor(foregroundColor()/*255-BG_R, 255-BG_G, 255-BG_B*/);  
     connect(this,SIGNAL(click()),&settalora,SLOT(mostra()));      
     connect(&settalora,SIGNAL(sendFrame(char*)), this, SIGNAL(sendFrame(char*)));
          qDebug("SETTAORA - END");
}

/*****************************************************************
**impostaSveglia
****************************************************************/

impostaSveglia::impostaSveglia( QWidget *parent,const char *name,diffSonora* diso, char* icon1, char*icon2, char* frame, int freq, int tipo , char* descr1, char* descr2, char* descr3, char* descr4)
        : bann2But( parent, name )
{   
     qDebug("IMPOST SVEGLIA");
     strcpy(&iconOn[0], icon1);
     qDebug("IMPOST SVEGLIA 1");
     strcpy(&iconOff[0], icon2);
     qDebug("IMPOST SVEGLIA 2");
/*    strncpy(&iconOn[0], icon1, sizeof(iconOn));
     strncpy(&iconOff[0], icon2, sizeof(iconOff));*/
     SetIcons( &iconOff[0] ,ICON_INFO);
	qDebug("IMPOST SVEGLIA 3");  
     svegliolina = new sveglia(NULL,"svegliolina",(uchar) freq, (uchar) tipo,diso, frame,descr1,descr2,descr3,descr4);
     qDebug("IMPOST SVEGLIA 4");
     svegliolina->setBGColor(backgroundColor());  
     svegliolina->setFGColor(foregroundColor());  
     svegliolina->hide();
     qDebug("IMPOST SVEGLIA 5");
     connect(this,SIGNAL(dxClick()),svegliolina,SLOT(mostra()));      
     connect(this,SIGNAL(sxClick()),this,SLOT(toggleAbil()));    
     
     connect(parentWidget() , SIGNAL(gestFrame(char*)),svegliolina,SLOT(gestFrame(char*))); 
     connect(svegliolina,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*)));      
     connect(svegliolina,SIGNAL(ImClosed()),this , SLOT(show()));      
     connect(svegliolina, SIGNAL(freeze(bool)),this , SIGNAL(freeze(bool)));     
     connect(this, SIGNAL(freezed(bool)), svegliolina,SLOT(spegniSveglia(bool)));     
          qDebug("IMPOST SVEGLIA - END");
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
}

/*void impostaSveglia::deFreez()
{
   emit( spegniSveglia());
}
*/
/*****************************************************************
**calibration
****************************************************************/
//#include "calibrate.h"

calibration::calibration( QWidget *parent,const char *name, const char* icon )
        : bannOnDx( parent, name )
{   
          qDebug("CALIBRATION");
//     SetIcons(icon,1);
      SetIcons( ICON_INFO,1);
     connect(this,SIGNAL(click()),this,SLOT(doCalib()));      
          qDebug("CLIBRATION-END");
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

impBeep::impBeep( QWidget *parent,const char *name ,const char * icon1, const char *icon2)
        : bannOnSx( parent, name )
{   
         qDebug("BEEP");
     strncpy(&iconOn[0], icon1, sizeof(iconOn));
     strncpy(&iconOff[0], icon2, sizeof(iconOff));
     SetIcons( &iconOff[0],1);
     setBeep(FALSE);//non va bene, bisogna prendere il valore di partenza dal costruttore
     connect(this,SIGNAL(click()),this,SLOT(toggleBeep()));      
          qDebug("BEEP_END");
}

void impBeep::toggleBeep()
{
    if(getBeep())
    {
	setBeep(FALSE);
	SetIcons(uchar(0), &iconOff[0]);
    }
    else
    {
	setBeep(TRUE);
	SetIcons(uchar(0), &iconOn[0]);	
    }
    Draw();
}

/*****************************************************************
**contrasto
****************************************************************/

impContr::impContr( QWidget *parent,const char *name ,char* val, const char * icon1)
        : bannOnDx( parent, name )
{   
     SetIcons( icon1,1);
     connect(this,SIGNAL(click()),this,SLOT(showContr()));      
     setContrast(atoi(val));
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
    delete(contrasto);
}


/*****************************************************************
**machVers
****************************************************************/

machVers::machVers( QWidget *parent,const char *name, versio * ver, const char* icon1)
        : bannOnDx( parent, name )
{   
         qDebug("VERSIONE");
    SetIcons( icon1,1);
    connect(this,SIGNAL(click()),this,SLOT(showVers()));   
    v=ver;
         qDebug("VRSIONE _END");
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

impPassword ::impPassword ( QWidget *parent,const char *name, char* attiva,char* password, char* icon1, char*icon2,char* icon3)
        : bann2But( parent, name )
{   
     qDebug("PWD");
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
     
     if (!strcmp(attiva,"1"))
	 active=TRUE;
      else
 	 active=FALSE;      
      emit(setPwd(active,&paswd[0]));	 
//     connect(this, SIGNAL(freezed(bool)), tasti,SLOT(freezed(bool)));     --da mettere
           qDebug("PWD _ END");
}


void impPassword::toggleActivation()
{

    if (active)
	active=FALSE;
    else
	active=TRUE;
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
    QWidget::show();
}

void  impPassword::reShow1(char* c)
{
    if (strcmp(&paswd[0],c))
    {
	   show();
	   qDebug("password errata");
	   sb=getBeep();
	   setBeep(TRUE);
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
    connect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow1(char*)));  
    disconnect(tasti,SIGNAL(Closed(char*)),this , SLOT(reShow2(char*)));  
    strcpy(&paswd[0],c);
    emit(setPwd(active,&paswd[0]));
    show();
    tasti->setMode(HIDDEN);
}

void impPassword::tiempout()
{
    delete(tiempo);
    setBeep(sb);	   
}
