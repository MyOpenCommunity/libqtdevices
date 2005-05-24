/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** items.cpp
**
**
**definizione dei vari items
****************************************************************/
#include "items.h"
#include "openclient.h"
#include "sottomenu.h"

//#include "btbutton.h"
#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <stdlib.h>



/*****************************************************************
**dimmer
****************************************************************/

dimmer::dimmer( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *inactiveIcon,char* breakIcon )
        : bannRegolaz( parent, name )
{ 
     setRange(1,9);
     SetIcons( IconaSx,IconaDx,icon, inactiveIcon,breakIcon,(char)0 );
     setAddress(indirizzo);
/*      impostaAttivo(1);
      setValue(5);*/
    connect(this,SIGNAL(sxClick()),this,SLOT(Accendi()));
    connect(this,SIGNAL(dxClick()),this,SLOT(Spegni()));
    connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
    connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
}



void dimmer::gestFrame(char* frame)
 {
    openwebnet msg_open;
    char aggiorna;
    
    aggiorna=0;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

    
    if (!strcmp(msg_open.Extract_chi(),"1"))
    {
	if ( (! strcmp(msg_open.Extract_dove(),getAddress()) ) ||     \
	                 ( !getPul() &&  (  \
	                                      (! strcmp(msg_open.Extract_dove(),"0")) ||   \
			      ( ( strlen(msg_open.Extract_dove())==1) && (! strncmp(msg_open.Extract_dove(),getAddress(),1)) ) ||     \
			      ( (! strncmp(msg_open.Extract_dove(),"#",1))  && *(getGroup()+(atoi(msg_open.Extract_dove()+1))-1) )        )   )  )
	{
	    if (!strcmp(msg_open.Extract_cosa(),"1")) 		
	    {
		if (isActive()!=1)
		{
		    impostaAttivo(1);
		    aggiorna=1;
		}
	    }
	    else if (!strcmp(msg_open.Extract_cosa(),"0")) 
	    {
	                if (isActive()!=0)
		{
		    impostaAttivo(0);
		    aggiorna=1;
		}
	    }
//	    else if (!strcmp(msg_open.Extract_cosa(),"2")) 
	    else if ( (atoi(msg_open.Extract_cosa()))<11) 
	    {
		    impostaAttivo(1);
		    setValue(atoi(msg_open.Extract_cosa())-1);
		    qDebug("imposto livello : %d",(atoi(msg_open.Extract_cosa())-1));		    
		    aggiorna=1;
	    }
	    else if (!strcmp(msg_open.Extract_cosa(),"19")) 
	    {
	              //DIMMER out of work
		    if (isActive()!=2)
		    {
			impostaAttivo(2);
			aggiorna=1;
		    }
	    }
	}
    }    
    if (aggiorna)
	Draw();
 }
void dimmer:: Accendi()
{
   openwebnet msg_open;
  
  msg_open.CreateNullMsgOpen();     
  msg_open.CreateMsgOpen("1", "1",getAddress(),"");
  emit sendFrame(msg_open.frame_open);   
 }
void dimmer:: Spegni()
{
   openwebnet msg_open;
  
  msg_open.CreateNullMsgOpen();     
  msg_open.CreateMsgOpen("1", "0",getAddress(),"");
  emit sendFrame(msg_open.frame_open);   
 }
void dimmer:: Aumenta()
{
   openwebnet msg_open;
  
  msg_open.CreateNullMsgOpen();     
  msg_open.CreateMsgOpen("1", "30",getAddress(),"");
  emit sendFrame(msg_open.frame_open);   
 }
void dimmer:: Diminuisci()	
{
        openwebnet msg_open;
  
  msg_open.CreateNullMsgOpen();     
  msg_open.CreateMsgOpen("1", "31",getAddress(),"");
  emit sendFrame(msg_open.frame_open);   
 }

void dimmer::inizializza()
{   
    openwebnet msg_open;
     char    pippo[50];
     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#1*");
     strcat(pippo,getAddress());
     strcat(pippo,"##");
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
     emit richStato(msg_open.frame_open);    
}
/*****************************************************************
**attuatAutom
****************************************************************/

attuatAutom::attuatAutom( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *pressedIcon ,int period,int number )
        : bannOnOff( parent, name )
{       
     SetIcons( IconaSx,IconaDx ,icon, pressedIcon,period ,number );
     setAddress(indirizzo);
     connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
     connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));                       
     setChi("1");
}


void attuatAutom::gestFrame(char* frame)
 {    
    openwebnet msg_open;
    char aggiorna;
    
    aggiorna=0;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

    
    if (!strcmp(msg_open.Extract_chi(),"1"))
    {
	if ( (! strcmp(msg_open.Extract_dove(),getAddress()) ) ||     \
	                 ( !getPul() &&  (  \
	                                      (! strcmp(msg_open.Extract_dove(),"0")) ||   \
			      ( ( strlen(msg_open.Extract_dove())==1) && (! strncmp(msg_open.Extract_dove(),getAddress(),1)) ) ||     \
			      ( (! strncmp(msg_open.Extract_dove(),"#",1))  && *(getGroup()+(atoi(msg_open.Extract_dove()+1))-1) )        )   )  )
	{
	    if (!strcmp(msg_open.Extract_cosa(),"1")) 		
	    {
		if (!isActive())
		{
		    impostaAttivo(1);
		    aggiorna=1;
		}
	    }
	    else if (!strcmp(msg_open.Extract_cosa(),"0")) 
	    {
	                if (isActive())
		{
		    impostaAttivo(0);
		    aggiorna=1;
		}
	    }
	}
    }    
    if (aggiorna)
	Draw();
 }


void attuatAutom::Attiva()
 {
  openwebnet msg_open;
  
  msg_open.CreateNullMsgOpen();     
  msg_open.CreateMsgOpen("1", "1",getAddress(),"");
   emit sendFrame(msg_open.frame_open);
 }

void attuatAutom::Disattiva()
 {
    openwebnet msg_open;
  
    msg_open.CreateNullMsgOpen();     
    msg_open.CreateMsgOpen("1", "0",getAddress(),"");
    emit sendFrame(msg_open.frame_open);
 }



char* attuatAutom::getChi()
{
   return("1");
}

void attuatAutom::inizializza()
{ 
     openwebnet msg_open;
     char    pippo[50];
     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#1*");
     strcat(pippo,getAddress());
     strcat(pippo,"##");
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
     emit richStato(msg_open.frame_open);    
}

/*****************************************************************
**gruppo di attuatAutom
****************************************************************/

grAttuatAutom::grAttuatAutom( QWidget *parent,const char *name,void *indirizzi, char* IconaSx,char* IconaDx,char *icon ,int period,int number )
        : bannOnOff( parent, name )
{     
     SetIcons( IconaSx, IconaDx,NULL,icon,period ,number );
      setAddress(indirizzi);
     connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
     connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
}



void grAttuatAutom::setAddress(void*indirizzi)
{
    elencoDisp=*((QPtrList<QString>*)indirizzi);        
//      elencoDisp=new (QPtrList<char>*)*((QPtrList<char>*)indirizzi);        
}


void grAttuatAutom::Attiva()
{
    openwebnet msg_open;
  
    for(uchar idx=0; idx<elencoDisp.count();idx++)
    {
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("1", "1",(char*)elencoDisp.at(idx)->ascii(),"");
	emit sendFrame(msg_open.frame_open);
    }
 }

void grAttuatAutom::Disattiva()
 {
    openwebnet msg_open;
  
   for(uchar idx=0; idx<elencoDisp.count();idx++)
    {
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("1", "0",(char*)elencoDisp.at(idx)->ascii(),"");
	emit sendFrame(msg_open.frame_open);
    }
}

/*****************************************************************
**gruppo di dimmer
****************************************************************/

grDimmer::grDimmer( QWidget *parent,const char *name,void *indirizzi, char* IconaSx,char* IconaDx,char *iconsx ,char* icondx,int period,int number )
        : bannRegolaz( parent, name )
{ 
//     setRange(1,1);
     SetIcons(  IconaSx, IconaDx ,icondx,iconsx );
     setAddress(indirizzi);
    connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
    connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
    connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
    connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
}



void grDimmer::setAddress(void*indirizzi)
{
    elencoDisp=*((QPtrList<QString>*)indirizzi);        
}


void grDimmer::Attiva()
 {
   openwebnet msg_open;
  
    for(uchar idx=0; idx<elencoDisp.count();idx++)
    {
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("1", "1",(char*)elencoDisp.at(idx)->ascii(),"");
	emit sendFrame(msg_open.frame_open);
    }
 }

void grDimmer::Disattiva()
 {
   openwebnet msg_open;
  
    for(uchar idx=0; idx<elencoDisp.count();idx++)
    {
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("1", "0",(char*)elencoDisp.at(idx)->ascii(),"");
	emit sendFrame(msg_open.frame_open);
    }
 }

void grDimmer::Aumenta()
 {
   openwebnet msg_open;
  
    for(uchar idx=0; idx<elencoDisp.count();idx++)
    {
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("1", "30",(char*)elencoDisp.at(idx)->ascii(),"");
	emit sendFrame(msg_open.frame_open);
    }
 }

void grDimmer::Diminuisci()
 {
   openwebnet msg_open;
  
    for(uchar idx=0; idx<elencoDisp.count();idx++)
    {
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("1", "31",(char*)elencoDisp.at(idx)->ascii(),"");
	emit sendFrame(msg_open.frame_open);
    }
 }

void grDimmer::inizializza(){}

/*****************************************************************
**scenario
****************************************************************/

scenario::scenario( sottoMenu *parent,const char *name,char* indirizzo,char* IconaSx )
        : bannOnSx( parent, name )
{   
     SetIcons( IconaSx,1);    
     setAddress(indirizzo);
     connect(this,SIGNAL(click()),this,SLOT(Attiva()));
}

void scenario::Attiva()
{
    openwebnet msg_open;
   char cosa[15];
   
   strncpy(cosa,getAddress(),sizeof(cosa));
   if (strstr(&cosa[0],"*"))
   {
       memset(index(&cosa[0],'*'),'\000',sizeof(cosa)-(index(&cosa[0],'*')-&cosa[0]));
       msg_open.CreateNullMsgOpen();          
       msg_open.CreateMsgOpen("0",&cosa[0],strstr(getAddress(),"*")+1,"");
       emit sendFrame(msg_open.frame_open);    
   }
}
void scenario::inizializza(){}
/*****************************************************************
**carico
****************************************************************/

carico::carico( sottoMenu *parent,const char *name,char* indirizzo ,char* IconaSx)
        : bannOnSx( parent, name )
{
   
     SetIcons( IconaSx,1);
     setAddress(indirizzo);
      connect(this,SIGNAL(click()),this,SLOT(Attiva()));
}

void carico::gestFrame(char*)
{
    
}
void carico::Attiva()
{
   openwebnet msg_open;
  
   msg_open.CreateNullMsgOpen();     
   msg_open.CreateMsgOpen("3", "2",getAddress(),"");
   emit sendFrame(msg_open.frame_open);
}

void carico::inizializza(){}
/*****************************************************************
**attuatAutomInt
****************************************************************/

attuatAutomInt::attuatAutomInt( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *StopIcon ,int period,int number )
        : bannOnOff( parent, name )
{     
      memset(nomeFile1,'\000',sizeof(nomeFile1));
      memset(nomeFile2,'\000',sizeof(nomeFile2));
      strncpy(nomeFile1,icon,strstr(icon,".")-icon);
      strncpy(nomeFile2,icon,strstr(icon,".")-icon);
      strcat(nomeFile1,"o");
      strcat(nomeFile2,"c");
      strcat(nomeFile1,strstr(icon,"."));
      strcat(nomeFile2,strstr(icon,"."));
      SetIcons( IconaSx, IconaDx , icon,nomeFile1,nomeFile2);
      strncpy(nomeFile3,StopIcon,sizeof(nomeFile3));
      strncpy(nomeFile1,IconaSx,sizeof(nomeFile3));
      strncpy(nomeFile2,IconaDx,sizeof(nomeFile3));

     setAddress(indirizzo);
     connect(this,SIGNAL(sxClick()),this,SLOT(analizzaUp()));
     connect(this,SIGNAL(dxClick()),this,SLOT(analizzaDown()));

    uprunning = dorunning = 0;    
}


void attuatAutomInt::gestFrame(char* frame)
 {  
    openwebnet msg_open;
    char aggiorna;
    
    aggiorna=0;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

    
    if (!strcmp(msg_open.Extract_chi(),"2"))
    {
	if ( (! strcmp(msg_open.Extract_dove(),getAddress()) ) ||     \
	                 ( !getPul() &&  (  \
	                                      (! strcmp(msg_open.Extract_dove(),"0")) ||   \
			      ( ( strlen(msg_open.Extract_dove())==1) && (! strncmp(msg_open.Extract_dove(),getAddress(),1)) ) ||     \
			      ( (! strncmp(msg_open.Extract_dove(),"#",1))  && *(getGroup()+(atoi(msg_open.Extract_dove()+1))-1) )        )   )  )
	{
	    if (!strcmp(msg_open.Extract_cosa(),"1")) 		
	    {
		if (!isActive())
		{
		    impostaAttivo(1);
    		    dorunning=0;
		    uprunning=1;
		    aggiorna=1;
		    SetIcons((uchar)0,nomeFile3 );
		}
	    }
	    else if (!strcmp(msg_open.Extract_cosa(),"0")) 
	    {
	                if (isActive())
		{
		    impostaAttivo(0);
		    uprunning=dorunning=0;
		    aggiorna=1;
    		    SetIcons((uchar)0,nomeFile1 );
    		    SetIcons((uchar)1,nomeFile2 );
		}
	    }
	    else if (!strcmp(msg_open.Extract_cosa(),"2")) 
	    {
	                if (!isActive())
		{
		    impostaAttivo(2);
		    dorunning=1;
		    uprunning=0;
		    aggiorna=1;
    		    SetIcons((uchar)1,nomeFile3 );
		}
	    }
	}
    }    
    if (aggiorna)
	Draw();   
 }

void attuatAutomInt::analizzaUp()
{
    if (!dorunning)
    {
	if (uprunning)
	{
	      openwebnet msg_open;
  
	      msg_open.CreateNullMsgOpen();     
	      msg_open.CreateMsgOpen("2", "0",getAddress(),"");
	      emit sendFrame(msg_open.frame_open);
	}
	else
	{
	  openwebnet msg_open;
  
	  msg_open.CreateNullMsgOpen();     
	  msg_open.CreateMsgOpen("2", "1",getAddress(),"");
	  emit sendFrame(msg_open.frame_open);	 
	}
    }
}

void attuatAutomInt::analizzaDown()
{
    if (!uprunning)
    {
	if (dorunning)
	{
	      openwebnet msg_open;
  
	      msg_open.CreateNullMsgOpen();     
	      msg_open.CreateMsgOpen("2", "0",getAddress(),"");
	      emit sendFrame(msg_open.frame_open);
	}
	else
	{
	        openwebnet msg_open;
 
	        msg_open.CreateNullMsgOpen();     
	        msg_open.CreateMsgOpen("2", "2",getAddress(),"");
	       emit sendFrame(msg_open.frame_open);
	}
    }
}


void attuatAutomInt::inizializza()
{ 
     openwebnet msg_open;
     char    pippo[50];
     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#1*");
     strcat(pippo,getAddress());
     strcat(pippo,"##");
     qDebug("mando frame attuat autom int %s",pippo);
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
     emit sendFrame(msg_open.frame_open);    
}
/*****************************************************************
**attuatAutomIntSic
****************************************************************/

attuatAutomIntSic::attuatAutomIntSic( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *StopIcon ,int period,int number )
        : bannOnOff( parent, name )
{     
      memset(nomeFile1,'\000',sizeof(nomeFile1));
      memset(nomeFile2,'\000',sizeof(nomeFile2));
      strncpy(nomeFile1,icon,strstr(icon,".")-icon);
      strncpy(nomeFile2,icon,strstr(icon,".")-icon);
      strcat(nomeFile1,"o");
      strcat(nomeFile2,"c");
      strcat(nomeFile1,strstr(icon,"."));
      strcat(nomeFile2,strstr(icon,"."));
      SetIcons( IconaSx, IconaDx , icon,nomeFile1,nomeFile2);
      strncpy(nomeFile3,StopIcon,sizeof(nomeFile3));
      strncpy(nomeFile1,IconaSx,sizeof(nomeFile3));
      strncpy(nomeFile2,IconaDx,sizeof(nomeFile3));
	

     setAddress(indirizzo);
     connect(this,SIGNAL(sxPressed()),this,SLOT(upPres()));
     connect(this,SIGNAL(dxPressed()),this,SLOT(doPres()));
     connect(this,SIGNAL(sxReleased()),this,SLOT(upRil()));
     connect(this,SIGNAL(dxReleased()),this,SLOT(doRil()));

    uprunning = dorunning = 0;
}



void attuatAutomIntSic::gestFrame(char* frame)
 {  
    openwebnet msg_open;
    char aggiorna;
    
    aggiorna=0;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

    
    if (!strcmp(msg_open.Extract_chi(),"2"))
    {
	if ( (! strcmp(msg_open.Extract_dove(),getAddress()) ) ||     \
	                 ( !getPul() &&  (  \
	                                      (! strcmp(msg_open.Extract_dove(),"0")) ||   \
			      ( ( strlen(msg_open.Extract_dove())==1) && (! strncmp(msg_open.Extract_dove(),getAddress(),1)) ) ||     \
			      ( (! strncmp(msg_open.Extract_dove(),"#",1))  && *(getGroup()+(atoi(msg_open.Extract_dove()+1))-1) )        )   )  )
	{
	    if (!strcmp(msg_open.Extract_cosa(),"1")) 		
	    {
		if (!isActive())
		{
		    impostaAttivo(1);
    		    dorunning=0;
		    uprunning=1;
		    aggiorna=1;
		    SetIcons((uchar)0,nomeFile3 );
		}
	    }
	    else if (!strcmp(msg_open.Extract_cosa(),"0")) 
	    {
	                if (isActive())
		{
		    impostaAttivo(0);
		    uprunning=dorunning=0;
		    aggiorna=1;
		    SetIcons((uchar)0,nomeFile1 );
    		    SetIcons((uchar)1,nomeFile2 );
		}
	    }
	    else if (!strcmp(msg_open.Extract_cosa(),"2")) 
	    {
	                if (!isActive())
		{
		    impostaAttivo(2);
		    dorunning=1;
		    uprunning=0;
		    aggiorna=1;
		    SetIcons((uchar)1,nomeFile3 );
		}
	    }
	}
    }    
    if (aggiorna)
	Draw();   
 }

void attuatAutomIntSic::upPres()
{
     if (!dorunning)
    {
	      openwebnet msg_open;
  
	      msg_open.CreateNullMsgOpen();     
	      msg_open.CreateMsgOpen("2", "1",getAddress(),"");
	      emit sendFrame(msg_open.frame_open);
    } 
}

void attuatAutomIntSic::doPres()
{
        if (!uprunning)
    {
	      openwebnet msg_open;
  
	      msg_open.CreateNullMsgOpen();     
	      msg_open.CreateMsgOpen("2", "2",getAddress(),"");
	      emit sendFrame(msg_open.frame_open);
  } 
}
void attuatAutomIntSic::upRil()
{
        if (!dorunning)
    {
	      openwebnet msg_open;
  
	      msg_open.CreateNullMsgOpen();     
	      msg_open.CreateMsgOpen("2", "0",getAddress(),"");
	      emit sendFrame(msg_open.frame_open);
    } 
}
void attuatAutomIntSic::doRil()
{
       if (!uprunning)
    {
	      openwebnet msg_open;
  
	      msg_open.CreateNullMsgOpen();     
	      msg_open.CreateMsgOpen("2", "0",getAddress(),"");
	      emit sendFrame(msg_open.frame_open);
    } 
}

void attuatAutomIntSic::inizializza()
{ 
     openwebnet msg_open;
     char    pippo[50];
     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#1*");
     strcat(pippo,getAddress());
     strcat(pippo,"##");
      qDebug("mando frame attuat autom intSic %s",pippo);
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
     emit sendFrame(msg_open.frame_open);    
}


/*****************************************************************
**attuatAutomTemp
****************************************************************/

attuatAutomTemp::attuatAutomTemp( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *pressedIcon ,int period,int number )
        : bannOnOff2scr( parent, name )
{     
     SetIcons( IconaDx, IconaSx ,icon, pressedIcon,period ,number );
     setAddress(indirizzo);
     cntTempi=0;
     SetSeconaryText(tempi[cntTempi]);
     connect(this,SIGNAL(dxClick()),this,SLOT(Attiva()));
     connect(this,SIGNAL(sxClick()),this,SLOT(CiclaTempo()));
}



void attuatAutomTemp::gestFrame(char* frame)
 {  
    openwebnet msg_open;
    char aggiorna;
    
    aggiorna=0;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

    
    if (!strcmp(msg_open.Extract_chi(),"1"))
    {
	if ( (! strcmp(msg_open.Extract_dove(),getAddress()) ) ||     \
	                 ( !getPul() &&  (  \
	                                      (! strcmp(msg_open.Extract_dove(),"0")) ||   \
			      ( ( strlen(msg_open.Extract_dove())==1) && (! strncmp(msg_open.Extract_dove(),getAddress(),1)) ) ||     \
			      ( (! strncmp(msg_open.Extract_dove(),"#",1))  && *(getGroup()+(atoi(msg_open.Extract_dove()+1))-1) )        )   )  )
	{
	    if (!strcmp(msg_open.Extract_cosa(),"1")) 		
	    {
		if (!isActive())
		{
		    impostaAttivo(1);
		    aggiorna=1;
		}
	    }
	    else if (!strcmp(msg_open.Extract_cosa(),"0")) 
	    {
	                if (isActive())
		{
		    impostaAttivo(0);
		    aggiorna=1;
		}
	    }
	}
    }    
    if (aggiorna)
	Draw();
 }

void attuatAutomTemp::Attiva()
 {
  openwebnet msg_open;
  char cosa[15];
  
  memset(&cosa[0],'\000',sizeof(cosa));
  sprintf(&cosa[0],"%d",(11+cntTempi));
  
  msg_open.CreateNullMsgOpen();     
  msg_open.CreateMsgOpen("1",&cosa[0],getAddress(),"");
   emit sendFrame(msg_open.frame_open);
 }

void attuatAutomTemp::CiclaTempo()
 {
    cntTempi= (cntTempi+1)%(sizeof(tempi)/sizeof(char*));
    SetSeconaryText(tempi[cntTempi]);
    Draw();
 }

void attuatAutomTemp::inizializza()
{ 
     openwebnet msg_open;
     char    pippo[50];
     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#1*");
     strcat(pippo,getAddress());
     strcat(pippo,"##");
      qDebug("mando frame attuat autom Temp %s",pippo);
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
     emit sendFrame(msg_open.frame_open);    
}
/*****************************************************************
**gruppo di attuatInt
****************************************************************/

grAttuatInt::grAttuatInt( QWidget *parent,const char *name,void *indirizzi,char* IconaSx,char* IconaDx, char *icon ,int period,int number )
        : bann3But( parent, name )
{     
     SetIcons(  IconaSx,IconaDx ,NULL,icon,period ,number );
     setAddress(indirizzi);
     connect(this,SIGNAL(sxClick()),this,SLOT(Alza()));
     connect(this,SIGNAL(dxClick()),this,SLOT(Abbassa()));
     connect(this,SIGNAL(centerClick()),this,SLOT(Ferma()));
}



void grAttuatInt::setAddress(void*indirizzi)
{
    elencoDisp=*((QPtrList<QString>*)indirizzi);        
}


void grAttuatInt::Alza()
 {
    openwebnet msg_open;
  
    for(uchar idx=0; idx<elencoDisp.count();idx++)
    {
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("2", "1",(char*)elencoDisp.at(idx)->ascii(),"");
	emit sendFrame(msg_open.frame_open);
    }
 }

void grAttuatInt::Abbassa()
 {
     openwebnet msg_open;
  
    for(uchar idx=0; idx<elencoDisp.count();idx++)
    {
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("2", "2",(char*)elencoDisp.at(idx)->ascii(),"");
	emit sendFrame(msg_open.frame_open);
    }
 }

void grAttuatInt::Ferma()
 {
     openwebnet msg_open;
  
    for(uchar idx=0; idx<elencoDisp.count();idx++)
    {
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("2", "0",(char*)elencoDisp.at(idx)->ascii(),"");
	emit sendFrame(msg_open.frame_open);
    }
 }

void grAttuatInt::inizializza()
{ 
}
/*****************************************************************
**attuatPuls
****************************************************************/

attuatPuls::attuatPuls( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,/*char* IconaDx,*/char *icon ,char tipo ,int period,int number )
        : bannPuls( parent, name )
{               
     SetIcons(  IconaSx,NULL,icon,NULL,period ,number );
     setAddress(indirizzo);
     connect(this,SIGNAL(sxPressed()),this,SLOT(Attiva()));
     connect(this,SIGNAL(sxReleased()),this,SLOT(Disattiva()));
     type=tipo;
     impostaAttivo(1);
}



void attuatPuls::Attiva()
 {    
     openwebnet msg_open;
     switch (type) {
                case  AUTOMAZ: 
	                    msg_open.CreateNullMsgOpen();     
		    msg_open.CreateMsgOpen("1", "1",getAddress(),"");
		    emit sendFrame(msg_open.frame_open);
		    break;
                case  VCT_SERR:
		    msg_open.CreateNullMsgOpen();     
		    msg_open.CreateMsgOpen("6", "10",getAddress(),"");
		    emit sendFrame(msg_open.frame_open);
		    break;	 
                case  VCT_LS:
		    msg_open.CreateNullMsgOpen();     
		    msg_open.CreateMsgOpen("6", "12",getAddress(),"");
		    emit sendFrame(msg_open.frame_open);
		    break;	 
    } 
 }

void attuatPuls::Disattiva()
 {
    openwebnet msg_open;
     switch (type) {
                case  AUTOMAZ:
	                    msg_open.CreateNullMsgOpen();     
		    msg_open.CreateMsgOpen("1", "0",getAddress(),"");
		    emit sendFrame(msg_open.frame_open);
		    break;	 
                case  VCT_SERR: 
		  /*  msg_open.CreateNullMsgOpen();     
		    msg_open.CreateMsgOpen("6", "1",getAddress(),"");
		    emit sendFrame(msg_open.frame_open);*/
		    break;	 
                case  VCT_LS: 
		    msg_open.CreateNullMsgOpen();     
		    msg_open.CreateMsgOpen("6", "11",getAddress(),"");
		    emit sendFrame(msg_open.frame_open);
		    break;	 
    }  
 }

void attuatPuls::inizializza()
{ 
}
/*****************************************************************
**amplificatore
****************************************************************/

amplificatore::amplificatore( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *inactiveIcon )
        : bannRegolaz( parent, name )
{ 
     setRange(1,9);
     SetIcons( IconaSx, IconaDx ,icon, inactiveIcon,(char)1 );
     qDebug("%s - %s - %s - %s", IconaSx, IconaDx, icon, inactiveIcon);
     setAddress(indirizzo);
    connect(this,SIGNAL(sxClick()),this,SLOT(Accendi()));
    connect(this,SIGNAL(dxClick()),this,SLOT(Spegni()));
    connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
    connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
    setValue(5);
}

int trasformaVol(int vol)
{
    if (vol<0)
	return(-1);
    
    if (vol<=3) 
                 return(1);
    if (vol<=7) 
                 return(2);
    if (vol<=11) 
                 return(3);
    if (vol<=14) 
                 return(4);
    if (vol<=17) 
                 return(5);
    if (vol<=20) 
                 return(6);
    if (vol<=23) 
                 return(7);
    if (vol<=27) 
                 return(8);    
    if (vol<=31) 
                 return(9);    
    return(-1);	
}
void amplificatore::gestFrame(char* frame)
 {
    openwebnet msg_open;
    char aggiorna;
    
    aggiorna=0;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
   
    if (!strcmp(msg_open.Extract_chi(),"16"))
    {
	if ( (! strcmp(msg_open.Extract_dove(),getAddress()) ) ||     \
	                 ( !getPul() &&  (  \
	                                      (! strcmp(msg_open.Extract_dove(),"0")) ||   \
			      ( ( strlen(msg_open.Extract_dove())==1) && (! strncmp(msg_open.Extract_dove(),getAddress(),1)) )          )   )  )
	{
	    if (!msg_open.IsMeasureFrame())
	    {
		if (!strcmp(msg_open.Extract_cosa(),"13")) 		
		{
		    if (isActive())
		    {
			impostaAttivo(0);
			aggiorna=1;
		    }
		}
		else if ((!strcmp(msg_open.Extract_cosa(),"0")) || (!strcmp(msg_open.Extract_cosa(),"3")))
		{
		    if (!isActive())
		    {
			impostaAttivo(1);
			aggiorna=1;
		    }
		}
	    }
	    else
	    {
		if ( !strcmp(msg_open.Extract_grandezza(),"1")) 
		{
		    int vol;
		    
		    vol=atoi(msg_open.Extract_valori(0))&0x1F;
		    vol=trasformaVol(vol);
		    setValue(vol);
		    qDebug("imposto livello : %d",vol);		    
		    aggiorna=1;
		}
	    }
	}
    }    
    if (aggiorna)
	Draw();
 }
void amplificatore:: Accendi()
{
      openwebnet msg_open;
  
      msg_open.CreateNullMsgOpen();     
      msg_open.CreateMsgOpen("16", "3",getAddress(),"");
      emit sendFrame(msg_open.frame_open);     
 }
void amplificatore:: Spegni()
{
      openwebnet msg_open;
  
      msg_open.CreateNullMsgOpen();     
      msg_open.CreateMsgOpen("16", "13",getAddress(),"");
      emit sendFrame(msg_open.frame_open);     
 }
void amplificatore:: Aumenta()
{
       openwebnet msg_open;
  
       msg_open.CreateNullMsgOpen();     
       msg_open.CreateMsgOpen("16", "1001",getAddress(),"");
       emit sendFrame(msg_open.frame_open);     
 }
void amplificatore:: Diminuisci()	
{
       openwebnet msg_open;
  
      msg_open.CreateNullMsgOpen();     
      msg_open.CreateMsgOpen("16", "1101",getAddress(),"");
      emit sendFrame(msg_open.frame_open);     
 }
void amplificatore::inizializza()
{ 
     openwebnet msg_open;
     char    pippo[50];
     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#16*");
     strcat(pippo,getAddress());
     strcat(pippo,"*1##");
      qDebug("mando frame ampli %s",pippo);
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
     emit sendFrame(msg_open.frame_open);    
     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#16*");
     strcat(pippo,getAddress());
     strcat(pippo,"*5##");
      qDebug("mando frame ampli %s",pippo);
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
     emit sendFrame(msg_open.frame_open);    
     
}
/*****************************************************************
**gruppo di amplificatori
****************************************************************/

grAmplificatori::grAmplificatori( QWidget *parent,const char *name,void *indirizzi,char* IconaSx,char* IconaDx, char *iconsx ,char* icondx,int period,int number )
        : bannRegolaz( parent, name )
{ 
//     setRange(1,1);
        SetIcons(IconaSx,IconaDx ,icondx,iconsx );
     setAddress(indirizzi);
    connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
    connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
    connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
    connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
   }



void grAmplificatori::setAddress(void*indirizzi)
{
    elencoDisp=*((QPtrList<QString>*)indirizzi);        
}


void grAmplificatori::Attiva()
 {
           openwebnet msg_open;
	   
 for(uchar idx=0; idx<elencoDisp.count();idx++)
    {
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("16", "3",(char*)elencoDisp.at(idx)->ascii(),"");
	emit sendFrame(msg_open.frame_open);
    }
 }

void grAmplificatori::Disattiva()
 {
           openwebnet msg_open;
	   
 for(uchar idx=0; idx<elencoDisp.count();idx++)
    {
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("16", "13",(char*)elencoDisp.at(idx)->ascii(),"");
	emit sendFrame(msg_open.frame_open);
    }
 }

void grAmplificatori::Aumenta()
 {
           openwebnet msg_open;
	   
 for(uchar idx=0; idx<elencoDisp.count();idx++)
    {
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("16", "1001",(char*)elencoDisp.at(idx)->ascii(),"");
	emit sendFrame(msg_open.frame_open);
    }
 }

void grAmplificatori::Diminuisci()
 {
           openwebnet msg_open;
	   
 for(uchar idx=0; idx<elencoDisp.count();idx++)
    {
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("16", "1101",(char*)elencoDisp.at(idx)->ascii(),"");
	emit sendFrame(msg_open.frame_open);
    }
 }
void grAmplificatori::inizializza()
{ 
}

/*****************************************************************
**sorgente
****************************************************************/


sorgente::sorgente( QWidget *parent,const char *name,char* indirizzo )
        : bannCiclaz( parent, name )
{
   
     SetIcons( ICON_CICLA,NULL,ICON_FFWD,ICON_REW);
     setAddress(indirizzo);
 //     connect(this,SIGNAL(click()),this,SLOT(Attiva()));
     connect(this  ,SIGNAL(sxClick()),this,SLOT(ciclaSorg()));
     connect(this  ,SIGNAL(csxClick()),this,SLOT(decBrano()));
     connect(this  ,SIGNAL(cdxClick()),this,SLOT(aumBrano()));
     nascondi(BUT3);
}

void sorgente::gestFrame(char*)
{
}

void sorgente::ciclaSorg()
{
    openwebnet msg_open;
          
     msg_open.CreateMsgOpen("16","23","100","");	     
     emit sendFrame(msg_open.frame_open);      
}

void sorgente::decBrano()
{
   openwebnet msg_open;
          
     msg_open.CreateMsgOpen("16","6101",getAddress(),"");	     
     emit sendFrame(msg_open.frame_open);      
}

void sorgente::aumBrano()
{
   openwebnet msg_open;
          
     msg_open.CreateMsgOpen("16","6001",getAddress(),"");	     
     emit sendFrame(msg_open.frame_open);      
}

void sorgente::inizializza()
{ 
}

/*****************************************************************
**sorgente_Radio
****************************************************************/


banradio::banradio( QWidget *parent,const char *name,char* indirizzo )
        : bannCiclaz( parent, name )
{     
     SetIcons( ICON_CICLA,ICON_IMPOSTA,ICON_FFWD,ICON_REW);
     setAddress(indirizzo);
     myRadio = new radio(NULL,"radio");
     myRadio->setRDS("");
     myRadio->setFreq(0.00);
     
//     myRadio-> setBGColor(parentWidget(TRUE)->backgroundColor() );
//     myRadio-> setFGColor(parentWidget(TRUE)->foregroundColor() );	
     
     
     myRadio->setStaz((uchar)1);
     
     connect(this  ,SIGNAL(sxClick()),this,SLOT(ciclaSorg()));
     connect(this  ,SIGNAL(csxClick()),this,SLOT(decBrano()));
     connect(this  ,SIGNAL(cdxClick()),this,SLOT(aumBrano()));
     
     connect(this  ,SIGNAL(dxClick()),myRadio,SLOT(showRadio()));
     connect(this , SIGNAL(dxClick()),this,SLOT(startRDS()));
     
     connect(this  ,SIGNAL(dxClick()),this->parentWidget(FALSE)->parentWidget(FALSE),SLOT(hide()));
     connect(myRadio,SIGNAL(Closed()),this->parentWidget(FALSE)->parentWidget(FALSE),SLOT(show()));
     connect(myRadio,SIGNAL(Closed()),myRadio,SLOT(hide()));
     connect(myRadio,SIGNAL(Closed()),this,SLOT(stopRDS()));     
     
     
     connect(myRadio,SIGNAL(decFreqAuto()),this,SLOT(decFreqAuto()));
     connect(myRadio,SIGNAL(aumFreqAuto()),this,SLOT(aumFreqAuto()));
     connect(myRadio,SIGNAL(decFreqMan()),this,SLOT(decFreqMan()));
     connect(myRadio,SIGNAL(aumFreqMan()),this,SLOT(aumFreqMan()));
     connect(myRadio,SIGNAL(changeStaz()),this,SLOT(changeStaz()));
     connect(myRadio,SIGNAL(memoFreq(uchar)),this,SLOT(memoStaz(uchar)));
     connect(myRadio,SIGNAL(richFreq()),this,SLOT(richFreq()));
     
//     myRadio->hide();
}



void banradio::gestFrame(char*frame)
{
    openwebnet msg_open;
    char aggiorna;
    
    aggiorna=0;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
   
    if (!strcmp(msg_open.Extract_chi(),"16"))
    {
	if ( (! strcmp(msg_open.Extract_dove(),getAddress()) )  )
	{
	    if (!msg_open.IsMeasureFrame())
	    {
	    }
	    else
	    {
		if ( !strcmp(msg_open.Extract_grandezza(),"6")) 
		{
		    float freq;
		    		
		    freq=atoi(msg_open.Extract_valori(1));
		    freq=freq/1000;
		    myRadio->setFreq(freq);
		    aggiorna=1;
		}
		if ( !strcmp(msg_open.Extract_grandezza(),"7")) 
		{
		    int staz;
		    
		    staz=atoi(msg_open.Extract_valori(1));
		    myRadio->setStaz((uchar)staz);
		    aggiorna=1;
		}
		if ( !strcmp(msg_open.Extract_grandezza(),"8")) 
		{
		    char rds[9];
		    for(int idx=0;idx<8;idx++)
		    {
			rds[idx]=atoi(msg_open.Extract_valori(idx));
		    }
		    rds[8]='\000';		    
		    myRadio->setRDS(&rds[0]);
		    aggiorna=1;
		}
	    }
	}
    }    
    if (aggiorna)
	myRadio->draw();
}

void banradio::show()
{
    
    openwebnet msg_open;
     char    pippo[50];
     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#16*");
     strcat(pippo,getAddress());
     strcat(pippo,"*6##");
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
		      
     emit sendFrame(msg_open.frame_open);
           
     QWidget::show();
    
      if (parentWidget()->parentWidget()->parentWidget(TRUE))
	nascondi(BUT2);
      else
	mostra(BUT2);
}

void banradio::hide()
{
    if (myRadio->isShown())
	stopRDS();
    myRadio->hide();
    QWidget::hide();
}

void banradio::SetText( const char *text )
{
/*    memset(testo,'\000',sizeof(testo));
    strncpy(testo,text,MAX_TEXT-1);*/
    banner::SetText(text);
    myRadio->setName((char*)text);
 }

void banradio::ciclaSorg()
{
    openwebnet msg_open;
          
     msg_open.CreateMsgOpen("16","23","100","");	     
     emit sendFrame(msg_open.frame_open);      
}

void banradio::decBrano()
{
   openwebnet msg_open;
          
     msg_open.CreateMsgOpen("16","6101",getAddress(),"");	     
     emit sendFrame(msg_open.frame_open);      
}

void banradio::aumBrano()
{
   openwebnet msg_open;
          
     msg_open.CreateMsgOpen("16","6001",getAddress(),"");	     
     emit sendFrame(msg_open.frame_open);      
}
void banradio::aumFreqAuto()
{
      openwebnet msg_open;
          
     myRadio->setFreq(0.00);
     myRadio->setRDS("- - - - ");
     myRadio->draw();
     msg_open.CreateMsgOpen("16","5000",getAddress(),"");	     
     emit sendFrame(msg_open.frame_open);   
 }
void banradio::decFreqAuto()
{
       openwebnet msg_open;
          
     myRadio->setFreq(0.00);
     myRadio->setRDS("- - - - ");
     myRadio->draw();
     msg_open.CreateMsgOpen("16","5100",getAddress(),"");	     
     emit sendFrame(msg_open.frame_open);       
}
void banradio::aumFreqMan()
{
 /*   static bool flag;
    
    if (flag)
    {*/
	openwebnet msg_open;
	float f;
	
	f=myRadio->getFreq();
	if (f<108.00)
	    f+=0.05;
	else
	    f=87.50;
	myRadio->setFreq(f);
	myRadio->draw();
	msg_open.CreateMsgOpen("16","5001",getAddress(),"");	     
	emit sendFrame(msg_open.frame_open);   
/*	flag=FALSE;
    }
    else
	flag=TRUE;*/
}
void banradio::decFreqMan()
{
  /*  static bool flag;
    
    if (flag)
    {*/
	openwebnet msg_open;
	float f;
	
	f=myRadio->getFreq();
	if (f>87.50)
	    f-=0.05;
	else
	    f=108.00;
	myRadio->setFreq(f);
	myRadio->draw();
	msg_open.CreateMsgOpen("16","5101",getAddress(),"");	     
	emit sendFrame(msg_open.frame_open);   
/* 	flag=FALSE;
   }
    else
	flag=TRUE;*/
}
void banradio::changeStaz()
{
       openwebnet msg_open;
          
     msg_open.CreateMsgOpen("16","6001",getAddress(),"");	     
     emit sendFrame(msg_open.frame_open);   
}
    
void banradio::memoStaz(uchar st)
{
       openwebnet msg_open;
     char    pippo[50],pippa[10];
     unsigned int ic;
     double f;
     
     memset(pippo,'\000',sizeof(pippo));
     memset(pippa,'\000',sizeof(pippa));
     strcat(pippo,"*#16*");
     strcat(pippo,getAddress());
     strcat(pippo,"*#7*");//     strcat(pippo,"*#9*0*");
     memset(pippa,'\000',sizeof(pippa));
//     f=myRadio->getFreq()*1000;
//     sprintf(pippa,"%.0f",f);
//     qDebug("pippa2  :%s",&pippa[0]);
//     strcat(pippo,pippa);
//     strcat(pippo,"*");
     ic=(unsigned int)st;
     sprintf(pippa,"%01hu",st);
     qDebug("pippa1  :%01hu",&pippa[0]);
     strcat(pippo,pippa);
     strcat(pippo,"##");
     qDebug(msg_open.frame_open);
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
		      
     emit sendFrame(msg_open.frame_open);   
}
void banradio::startRDS()
{
      openwebnet msg_open;
     char    pippo[50];
     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*16*101*");
     strcat(pippo,getAddress());
//     strcat(pippo,"*8##");
     strcat(pippo,"##");
     
     qDebug(msg_open.frame_open);
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
		      
     emit sendFrame(msg_open.frame_open);   
}
void banradio::stopRDS()
{
     openwebnet msg_open;
     char    pippo[50];
     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*16*102*");
     strcat(pippo,getAddress());
     strcat(pippo,"##");
     
     qDebug(msg_open.frame_open);
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
		      
     emit sendFrame(msg_open.frame_open);   
}

void banradio::richFreq()
{
    openwebnet msg_open;
     char    pippo[50];
     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#16*");
     strcat(pippo,getAddress());
     strcat(pippo,"*6##");
     
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
		      
     emit sendFrame(msg_open.frame_open); 
}

void banradio::setBGColor(int r, int g , int b)
{
    setBGColor(QColor(r,g,b));   
}
void banradio::setFGColor(int r, int g, int b)
{
   setFGColor(QColor(r,g,b));   
}
void banradio::setBGColor(QColor c)
{
    if (c.isValid())
    {
 myRadio->setBGColor(c);   
 banner::setBGColor(c); 
}
}
void banradio::setFGColor(QColor c)
{
    if (c.isValid())
    {
 myRadio->setFGColor(c);       
 banner::setFGColor(c); 
}
}



void banradio::inizializza()
{ 
}


/*****************************************************************
**termoPage
****************************************************************/
termoPage::termoPage ( QWidget *parent, const char *name ,char*indirizzo,char* IconaMeno,char* IconaPiu,char*IconaMan,char*IconaAuto, QColor SecondForeground)
        : bannTermo( parent, name, SecondForeground)
{       
     SetIcons( IconaMeno, IconaPiu , ICON_SONDAOFF,ICON_SONDAANTI);/*IconaMan, IconaAuto );*/
     setAddress(indirizzo);
     connect(this,SIGNAL(dxClick()),this,SLOT(aumSetpoint()));
     connect(this,SIGNAL(sxClick()),this,SLOT(decSetpoint()));                       
     setChi("4");
}


void termoPage::gestFrame(char* frame)
 {    
    openwebnet msg_open;
    char aggiorna;
    char dove[30];
    char pippo[50];    
    aggiorna=0;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

    
    if (!strcmp(msg_open.Extract_chi(),"4"))
    {
	strcpy(&dove[0], msg_open.Extract_dove());
	if (dove[0]=='#')
	    strcpy(&dove[0], &dove[1]);
	
	if ( (! strcmp(&dove[0],"0") ) )
	{

//Richiesta via centrale     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#4*#");
     strcat(pippo,getAddress());
     strcat(pippo,"##");
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));     
     emit sendFrame(msg_open.frame_open);   
		
		
	}		
	if ( (! strcmp(&dove[0],getAddress()) ) )
	{
	    if ( (!strcmp(msg_open.Extract_cosa(),"110")) || (!strcmp(msg_open.Extract_cosa(),"210")) || (!strcmp(msg_open.Extract_cosa(),"310")) )
	    {
		    if  (statoSonda!=S_MAN) 
		    {
			((sottoMenu*)parentWidget())->setNavBarMode(4,ICON_MANUAL_ON);
			statoSonda=S_MAN;
			mostra(BUT1);
			mostra(BUT2);
			nascondi(ICON);
			tempImp->show();
			aggiorna=1;
			((sottoMenu*)parentWidget())->draw();
		    }
	    }
	    else if  ( (!strcmp(msg_open.Extract_cosa(),"111")) || (!strcmp(msg_open.Extract_cosa(),"211")) || (!strcmp(msg_open.Extract_cosa(),"311")) )
	    {
		    if  (statoSonda!=S_AUTO) 
		    {
			((sottoMenu*)parentWidget())->setNavBarMode(4,ICON_AUTO_ON);
			nascondi(BUT1);
			nascondi(BUT2);
			nascondi(ICON);
			tempImp->show();
			statoSonda=S_AUTO;
			aggiorna=1;
			((sottoMenu*)parentWidget())->draw();
		    }
	    }
	    else if   (!strcmp(msg_open.Extract_cosa(),"102")) 
	    {
	       //sonda in antigelo	       
  	       nascondi(BUT1);
	       nascondi(BUT2);
	       mostra(ICON);
	       tempImp->hide();
	       impostaAttivo(1);
       	       aggiorna=1;
	       statoSonda=S_ANTIGELO;
	    }
    	    else if   (!strcmp(msg_open.Extract_cosa(),"202")) 
	    {
	       //sonda in protezione termica
         	       nascondi(BUT1);
	       nascondi(BUT2);
       	       mostra(ICON);
	       tempImp->hide();
	       impostaAttivo(1);
	       aggiorna=1;	       
	       statoSonda=S_OFF;
	    }
    	    else if   (!strcmp(msg_open.Extract_cosa(),"103")) 
	    {
	       //sonda in OFF
       	       mostra(ICON);
  	       nascondi(BUT1);
	       nascondi(BUT2);	       
	       tempImp->hide();
	       impostaAttivo(0);
	       aggiorna=1;	       
	    }    
    	    else if   (!strcmp(msg_open.Extract_grandezza(),"13")) 
	    {
	       switch (atoi(msg_open.Extract_valori(0))){
	       case 0:	val_imp=3;isOff=0;isAntigelo=0;break;
	       case 1:	val_imp=4;isOff=0;isAntigelo=0;break;
	       case 2:	val_imp=5;isOff=0;isAntigelo=0;break;
	       case 3:	val_imp=6;isOff=0;isAntigelo=0;break;
	       case 11:	val_imp=2;isOff=0;isAntigelo=0;break;
	       case 12:	val_imp=1;isOff=0;isAntigelo=0;break;
	       case 13:	val_imp=0;isOff=0;isAntigelo=0;break;
	       case 4:	val_imp=0;isOff=1;isAntigelo=0;/*OFF*/break;
	       case 5:	val_imp=0;isOff=0;isAntigelo=1;/*LOCAL PROTECTION*/break;			
		    }
     	       aggiorna=1;
	   }
	     else if   (!strcmp(msg_open.Extract_grandezza(),"0")) 
	    {
	       //Temperatura misurata
	       float icx;
	       char	tmp[10];   
	       
	       icx=atoi(msg_open.Extract_valori(0));
	       qDebug("temperatura misurata: %d",icx);
	       memset(temp,'\000',sizeof(temp));
	       if (icx>=1000)
	       {
		   strcat(temp,"-");
		   icx=icx-1000;
	       }
	       icx/=10;
	       sprintf(tmp,"%.1f",icx);
	       qDebug("-1: tmp: %.1f - %s",icx, &tmp[0]);
	       strcat(temp,tmp);
	       qDebug("-1: temp: %s", &temp[0]);
	       strcat(temp,"\272C");
	       qDebug("-2: temp: %s", &temp[0]);
	       aggiorna=1;	       
	       
	       memset(pippo,'\000',sizeof(pippo));
	       strcat(pippo,"*#4*");
	       strcat(pippo,getAddress());
	       strcat(pippo,"*14");
	       strcat(pippo,"##");
	       msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));     
	       emit sendFrame(msg_open.frame_open);         	       
////	       
	       //Richiesta via centrale     
	       memset(pippo,'\000',sizeof(pippo));
	       strcat(pippo,"*#4*#");
	       strcat(pippo,getAddress());
	       strcat(pippo,"##");
	       msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));     
	       emit sendFrame(msg_open.frame_open);   
/////	       

	   }
	    else if   (!strcmp(msg_open.Extract_grandezza(),"14")) 
	    {
	       //Set Point
	       float icx;
	       char	tmp[10];   
	       
	       qDebug("temperatura setpoint: %d",icx);
	       icx=atoi(msg_open.Extract_valori(0));
	       memset(setpoint,'\000',sizeof(setpoint));
	       if (icx>=1000)
	       {
		   strcat(setpoint,"-");
		   icx=icx-1000;
	       }
	       icx/=10;
	       sprintf(tmp,"%.1f",icx);
	       strcat(setpoint,tmp);
	       strcat(setpoint,"\272C");
	       aggiorna=1;	       
	    }
	}
    }    
    if (aggiorna)
	Draw();
 }


char* termoPage::getChi()
{
   return("4");
}

void termoPage::autoMan()
{
         openwebnet msg_open;
     char    pippo[50];

     memset(pippo,'\000',sizeof(pippo));


    if    (statoSonda==S_MAN)
     {
	strcat(pippo,"*4*311*#");
	strcat(pippo,getAddress());
    }
    else 
    {
	int icx;
	
	icx=atoi(strstr(&setpoint[0],".")+1);
	if(setpoint[0]=='-')
	    icx=-icx;
	icx+=atoi(&setpoint[0])*10;
	if (icx<0)
	    icx=(abs(icx))+1000;
	
	strcat(pippo,"*#4*#");
	strcat(pippo,getAddress());
	strcat(pippo,"*#14*");
     	sprintf(pippo,"%s%04d",pippo,icx);
	strcat(pippo,"*3");
    }
     
     strcat(pippo,"##");
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));     
     emit sendFrame(msg_open.frame_open);   
}

void termoPage::hide()
{
    disconnect(parentWidget(),SIGNAL(goDx()), this, SLOT(autoMan()));    
    banner::hide();
}

void termoPage::show()
{
    disconnect(parentWidget(),SIGNAL(goDx()), this, SLOT(autoMan()));    /*mi assicuro che ci sia un solo collegamento*/
    connect(parentWidget(),SIGNAL(goDx()), this, SLOT(autoMan()));
    banner::show();
}



void termoPage::aumSetpoint()
{
    openwebnet msg_open;
     char    pippo[50];
     int 	icx;

      memset(pippo,'\000',sizeof(pippo));
      icx=atoi(strstr(&setpoint[0],".")+1);
      if(setpoint[0]=='-')
	  icx=-icx;
      icx+=atoi(&setpoint[0])*10;
      if (icx<0)
	  icx=(abs(icx))+995;
      else
	  icx=(abs(icx))+5;
     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#4*#");
     strcat(pippo,getAddress());
     strcat(pippo,"*#14*");
     sprintf(pippo,"%s%04d",&pippo[0],icx);
     strcat(pippo,"*3##");
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));         
    emit sendFrame(msg_open.frame_open);   
}
     
void termoPage::decSetpoint()
{
      openwebnet msg_open;
     char    pippo[50];
     int 	icx;

      memset(pippo,'\000',sizeof(pippo));
      icx=atoi(strstr(&setpoint[0],".")+1);
      if(setpoint[0]=='-')
	  icx=-icx;
      icx+=atoi(&setpoint[0])*10;
      if (icx<0)
	  icx=(abs(icx))+1005;
      else
	  icx=(abs(icx))-5;
     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#4*#");
     strcat(pippo,getAddress());
     strcat(pippo,"*#14*");
     sprintf(pippo,"%s%04d",&pippo[0],icx);
     strcat(pippo,"*3##");
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));         
    emit sendFrame(msg_open.frame_open);   
}     
     
void termoPage::inizializza()
{ 
     openwebnet msg_open;
     char    pippo[50];
     
//Richiesta via centrale     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#4*#");
     strcat(pippo,getAddress());
     strcat(pippo,"##");
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));     
     emit sendFrame(msg_open.frame_open);   
     
     
//Richiesta alla sonda 
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#4*");
     strcat(pippo,getAddress());
     strcat(pippo,"##");
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));     
     emit sendFrame(msg_open.frame_open);   
 }


/*****************************************************************
**zonaAnti
****************************************************************/

zonaAnti::zonaAnti( QWidget *parent,const char *name,char* indirizzo,char* IconActive,char* IconDisactive,/*char *icon ,char *pressedIcon ,*/int period,int number )
        : bannOnOff( parent, name )
{       
    char    pippo[MAX_PATH];
    char    pluto[MAX_PATH];
    
     setAddress(indirizzo);
     getZoneName(IconActive, &pippo[0], indirizzo, sizeof(pippo));
     getZoneName(IconDisactive, &pluto[0], indirizzo, sizeof(pluto));
     SetIcons( NULL, NULL,&pippo[0],&pluto[0]);    
     setChi("5");
}


void zonaAnti::gestFrame(char* frame)
 {    
    openwebnet msg_open;
    char aggiorna;
    
    aggiorna=0;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

    
    if (!strcmp(msg_open.Extract_chi(),"5"))
    {
	if ( (! strcmp(msg_open.Extract_dove(),getAddress()) )   )   
	{
	    if (!strcmp(msg_open.Extract_cosa(),"11")) 		
	    {
		if (!isActive())
		{
		    impostaAttivo(1);
		    aggiorna=1;
		}
	    }
	    else if (!strcmp(msg_open.Extract_cosa(),"18")) 
	    {
	                if (isActive())
		{
		    impostaAttivo(0);
		    aggiorna=1;
		}
	    }
	}
    }    
    if (aggiorna)
	Draw();
 }



char* zonaAnti::getChi()
{
   return("5");
}

void zonaAnti::inizializza()
{ 
     openwebnet msg_open;
     char    pippo[50];
     
     memset(pippo,'\000',sizeof(pippo));
     strcat(pippo,"*#5*");
     strcat(pippo,getAddress());
     strcat(pippo,"##");
     msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
     emit sendFrame(msg_open.frame_open);    
}


/*****************************************************************
**impAnti
****************************************************************/


impAnti::impAnti( QWidget *parent,const char *name,char* indirizzo,char* IconOn, char* IconOff, char* IconInfo, char* IconActive, int ,int  )
        :  bann2butLab( parent, name )
{      
    char pippo[MAX_PATH];
    
     memset(pippo,'\000',sizeof(pippo));
     if (IconActive)
	 strncpy(&pippo[0],IconActive,strstr(IconActive,".")-IconActive-3);

     strcat(pippo,"dis");
     strcat(pippo,strstr(IconActive,"."));
     SetIcons(  IconInfo,IconOff,&pippo[0],IconOn,IconActive);
     setChi("5");
     nascondi(BUT2);
     impostaAttivo(2);
     connect(this,SIGNAL(dxClick()),this,SLOT(Inserisci()));
     connect(this,SIGNAL(cdxClick()),this,SLOT(Disinserisci()));               
//     connect(this,SIGNAL(sxClick()),this,SIGNAL(CodaAllarmi()));                
     connect(this,SIGNAL(sxClick()), parentWidget(),SIGNAL(goDx()));
}


void impAnti::gestFrame(char* frame)
 {    
    openwebnet msg_open;
    char aggiorna;
    
    aggiorna=0;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

    
    if (!strcmp(msg_open.Extract_chi(),"5"))
    {
	if  (! strcmp(msg_open.Extract_dove(),"0" )   )   
	{
	    if (!strcmp(msg_open.Extract_cosa(),"8")) 		
	    {
		if (!isActive())
		{
		    impostaAttivo(2);
		    nascondi(BUT4);
		    mostra(BUT2);
		    aggiorna=1;
		    emit(impiantoInserito());
		}
	    }
	    else if (!strcmp(msg_open.Extract_cosa(),"9")) 
	    {
	                if (isActive())
		{
		    impostaAttivo(0);
		    nascondi(BUT2);
		    mostra(BUT4);		    
		    aggiorna=1;
		}
	    }
	}
    }    
    if (aggiorna)
	Draw();
 }

void impAnti::Inserisci()
{
    tasti = new tastiera(NULL, "");
    connect(tasti, SIGNAL(Closed(char*)), this, SLOT(Insert(char*)));
    tasti->setBGColor(backgroundColor());
    tasti->setFGColor(foregroundColor());
    tasti->showTastiera();
    parentWidget()->hide();
//    this->hide();
}
void impAnti::Disinserisci()
{
    tasti = new tastiera(NULL, "");
    connect(tasti, SIGNAL(Closed(char*)), this, SLOT(DeInsert(char*)));
    tasti->setBGColor(backgroundColor());
    tasti->setFGColor(foregroundColor());    
    tasti->showTastiera();
    parentWidget()->hide();
    
//    this->hide();    
}

 
void impAnti::Insert(char* pwd)
{   
    if (pwd)
    {
	openwebnet msg_open;
	char    pippo[50];
	
	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*5*36#");
	strcat(pippo,pwd);
	strcat(pippo,"*0##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);    
    }
        parentWidget()->show();       
    
 }
void impAnti::DeInsert(char* pwd)
{
    if (pwd)
    {
	openwebnet msg_open;
	char    pippo[50];
	
	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*5*36#");
	strcat(pippo,pwd);
	strcat(pippo,"*0##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);    
    }    
    parentWidget()->show();   
//    qDebug("disinserisco con %s", pwd);
}

char* impAnti::getChi()
{
   return("5");
}

void impAnti::inizializza()
{ 
     emit sendFrame("*#5*0##");    
}


/*****************************************************************
**allarme
****************************************************************/

allarme::allarme( sottoMenu  *parent,const char *name,char* indirizzo,char* IconaDx )
        : bannOnDx( parent, name )
{   
     SetIcons( IconaDx,1);    
 //    setAddress(indirizzo);
     connect(this,SIGNAL(click()),this,SLOT(muoio()));
//     connect(this,SIGNAL(click()),parent,SIGNAL(itemKilled()));
//     qDebug(parent->name());
       connect(this,SIGNAL(itemKilled()),parent,SIGNAL(itemKilled()));
}

void allarme::muoio()
{    
    emit (itemKilled());
    emit killMe(this);
}


/*****************************************************************
**gesModScen
****************************************************************/

gesModScen::gesModScen( QWidget *parent, const char *name ,char*indirizzo,char* IcoSx,char* IcoDx,char* IcoCsx,char* IcoCdx,char* IcoDes, char* IcoSx2, char* IcoDx2)
	:  bann4tasLab(parent, name)
{
    strcpy(&iconOn[0],IcoSx);
    strcpy(&iconStop[0],IcoSx2);       
    strcpy(&iconInfo[0],IcoDx);
    strcpy(&iconNoInfo[0],IcoDx2);       

    SetIcons (IcoSx, IcoDx, IcoCsx, IcoCdx, IcoDes);
    nascondi(BUT3);
    nascondi(BUT4);    

    memset(&cosa[0],'\000',sizeof(&cosa[0]) );
    memset(&dove[0],'\000',sizeof(&dove[0]) );    
 //   strcpy(&cosa[0], indirizzo);
    
//   *strstr(&cosa[0],"*")='\000';
    if (strstr(indirizzo,"*"))
    {
	strncpy(&cosa[0], indirizzo, strstr(indirizzo,"*")-indirizzo);

	strcpy(&dove[0], strstr(indirizzo,"*")+1);    
    }
    sendInProgr=0;

     setAddress(indirizzo);
     impostaAttivo(2);   
    connect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
    connect(this,SIGNAL(dxClick()),this,SLOT(enterInfo()));
    connect(this,SIGNAL(csxClick()),this,SLOT(startProgScen()));
    connect(this,SIGNAL(cdxClick()),this,SLOT(cancScen()));    

}

void gesModScen::attivaScenario()
{
    openwebnet msg_open;
    char    pippo[50];
	
    memset(pippo,'\000',sizeof(pippo));
    strcat(pippo,"*0*");
    strcat(pippo,&cosa[0]);
    strcat(pippo,"*");
    strcat(pippo,&dove[0]);
    strcat(pippo,"##");
    msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
    emit sendFrame(msg_open.frame_open);    
}
void gesModScen::enterInfo()
{
      nascondi(ICON);
      mostra(BUT4);    
      mostra(BUT3);      
      SetIcons(uchar(1),&iconNoInfo[0]);
      qDebug(&iconNoInfo[0]);
      disconnect(this,SIGNAL(dxClick()),this,SLOT(enterInfo()));
      connect(this,SIGNAL(dxClick()),this,SLOT(exitInfo())); 
      Draw();
}
void gesModScen::exitInfo()
{
      mostra(ICON);
      nascondi(BUT4);    
      nascondi(BUT3);      
      SetIcons(uchar(1),&iconInfo[0]);
        qDebug(&iconInfo[0]);
      connect(this,SIGNAL(dxClick()),this,SLOT(enterInfo()));
      disconnect(this,SIGNAL(dxClick()),this,SLOT(exitInfo()));     
      Draw();
}
void gesModScen::startProgScen()
{
    openwebnet msg_open;
    char    pippo[50];
	
    memset(pippo,'\000',sizeof(pippo));
    strcat(pippo,"*0*40#");
    strcat(pippo,&cosa[0]);
    strcat(pippo,"*");
    strcat(pippo,&dove[0]);
    strcat(pippo,"##");
    msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
    emit sendFrame(msg_open.frame_open);    
    sendInProgr=1;
}
void gesModScen::stopProgScen()
{
    openwebnet msg_open;
    char    pippo[50];
	
    memset(pippo,'\000',sizeof(pippo));
    strcat(pippo,"*0*41#");
    strcat(pippo,&cosa[0]);
    strcat(pippo,"*");
    strcat(pippo,&dove[0]);
    strcat(pippo,"##");
    msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
    emit sendFrame(msg_open.frame_open);    
    sendInProgr=0;
}
void gesModScen::cancScen()
{
    openwebnet msg_open;
    char    pippo[50];
	
    memset(pippo,'\000',sizeof(pippo));
    strcat(pippo,"*0*42#");
    strcat(pippo,&cosa[0]);
    strcat(pippo,"*");
    strcat(pippo,&dove[0]);
    strcat(pippo,"##");
    msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
    emit sendFrame(msg_open.frame_open);    
}
void gesModScen::gestFrame(char* frame)
{
    openwebnet msg_open;
    char aggiorna;
    
    aggiorna=0;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

    
    if (!strcmp(msg_open.Extract_chi(),"0"))
    {
	if ( (! strcmp(msg_open.Extract_dove(),&dove[0])) )
	{
	    if (!strncmp(msg_open.Extract_cosa(),"40",2)) 		
	    {
		if (sendInProgr)
		{	
		    SetIcons(uchar(0),&iconStop[0]);       
		    disconnect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
		    connect(this,SIGNAL(sxClick()),this,SLOT(stopProgScen()));
		    Draw();
		}
		else
		{
		    nascondi(BUT2);
		    exitInfo();
		}
	    }
	    else if ( (!strncmp(msg_open.Extract_cosa(),"43",2)) || (!strncmp(msg_open.Extract_cosa(),"45",2)))
	    {
		nascondi(BUT2);
		exitInfo();
	    }
	    else if  (!strncmp(msg_open.Extract_cosa(),"41",2)) 
	    {
		SetIcons(uchar(0),&iconOn[0]);       
		connect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
		disconnect(this,SIGNAL(sxClick()),this,SLOT(stopProgScen()));
		mostra(BUT2);
		Draw();
	    }
	    
	    else if (!strcmp(msg_open.Extract_cosa(),"0")) 
	    {
	    }
	}
    }    
     if (!strcmp(msg_open.Extract_chi(),"1001"))
    {
	if ( (! strcmp(msg_open.Extract_dove(),&dove[0])) )
	{	
	    if ( (! strcmp(msg_open.Extract_grandezza(),"1")) )
	    {
		if ( (! strcmp(msg_open.Extract_valori(0),"55")) )
		    mostra(BUT2);	
	    }
	}
    }

    if (aggiorna)
	Draw();
}
void gesModScen::inizializza()
{   
  	openwebnet msg_open;
	char    pippo[50];
	
	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1001*");
	strcat(pippo,&dove[0]);
	strcat(pippo,"*1##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);    	
	nascondi(BUT2);     
}
