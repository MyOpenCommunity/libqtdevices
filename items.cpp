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
    setRange(10,90);
    setStep(10);
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
    
    
    if (isForMe(msg_open))
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
	    setValue(10 * (atoi(msg_open.Extract_cosa())-1));
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
    qDebug("dimmer::inizializza");

    memset(pippo,'\000',sizeof(pippo));
    strcat(pippo,"*#1*");
    strcat(pippo,getAddress());
    strcat(pippo,"##");
    msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
    emit richStato(msg_open.frame_open);    
}


/*****************************************************************
**dimmer 100 livelli
****************************************************************/

dimmer100::dimmer100( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *inactiveIcon,char* breakIcon,
		      int sstart, int sstop)
  : dimmer( parent, name, indirizzo, IconaSx, IconaDx, icon, 
	    inactiveIcon, breakIcon)
{ 
    qDebug("costruttore dimmer100, name = %s", name);
    softstart = sstart;
    qDebug("softstart = %d", softstart);
    softstop = sstop;
    qDebug("softstop = %d", softstop);
    setRange(5,100);
    setStep(5);
    setValue(0);
    qDebug("IconaSx = %s", IconaSx);
    qDebug("IconaDx = %s", IconaDx);
    qDebug("icon = %s", icon);
    qDebug("inactiveIcon = %s", inactiveIcon);
    qDebug("breakIcon = %s", breakIcon);
    SetIcons( IconaSx,IconaDx,icon, inactiveIcon,breakIcon,(char)0 );
}


bool dimmer100::decCLV(openwebnet& msg, char& code, char& lev, char& speed,
		       char& h, char &m, char &s)
{
    // Message is a new one if it has the form:
    // *#1*where*1*lev*speed##
    // which is a measure frame
    bool out = msg.IsMeasureFrame();
    if(!out) return out;
    code = atoi(msg.Extract_grandezza());
    qDebug("dimmer100::decCLV, code = %d", code);
    if(code == 2) {
	h = atoi(msg.Extract_valori(0));
	m = atoi(msg.Extract_valori(1));
	s = atoi(msg.Extract_valori(2));
    } else if(code == 1) {
	lev = atoi(msg.Extract_valori(0)) - 100;
	speed = atoi(msg.Extract_valori(1));
    }
    return true;
}

void dimmer100:: Accendi()
{
    //*#1*where*#1*lev*speed
    openwebnet msg_open;
    msg_open.CreateNullMsgOpen();
    char s[100];
#if 0
    sprintf(s, "*#1*%s*#1*%d*%d##", getAddress(), last_on_lev, softstart);
#else
    //*1*0#velocita*dove## 
    sprintf(s, "*1*1#%d*%s##", softstart, getAddress());
#endif
    msg_open.CreateMsgOpen(s, strlen(s));
    emit sendFrame(msg_open.frame_open);   
}

void dimmer100:: Spegni()
{
    if(spento) return;
    openwebnet msg_open;
    msg_open.CreateNullMsgOpen();
    char s[100];
    last_on_lev = value;
    //*1*0#velocita*dove## 
    sprintf(s, "*1*0#%d*%s##", softstop, getAddress());
    msg_open.CreateMsgOpen(s, strlen(s));
    emit sendFrame(msg_open.frame_open);   
}

void dimmer100:: Aumenta()
{
    openwebnet msg_open;
    if(spento) return;
    msg_open.CreateNullMsgOpen();     
    //msg_open.CreateMsgOpen("1", "30",getAddress(),"");
    char cosa[100];
    // Simone agresta il 4/4/2006
    // per l'incremento e il decremento prova ad usare il valore di velocit? di
    // default 255.
    sprintf(cosa, "30#5#255");
    msg_open.CreateMsgOpen("1", cosa, getAddress(), "");
    emit sendFrame(msg_open.frame_open);   
}

void dimmer100:: Diminuisci()	
{
    if(spento) return;
    openwebnet msg_open;
    char cosa[100];
    sprintf(cosa, "31#5#255", speed);
    msg_open.CreateNullMsgOpen(); 
    msg_open.CreateMsgOpen("1", cosa ,getAddress(),"");
    emit sendFrame(msg_open.frame_open);   
}



void dimmer100::gestFrame(char* frame)
{
    openwebnet msg_open;
    char aggiorna;
    char livello, velocita, codice, h, m, s;
    
    aggiorna=0;
    
    qDebug("dimmer100::gestFrame");

    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    
    bool nuovo = decCLV(msg_open, codice, livello, velocita, h, m, s);

    if(isForMe(msg_open)) {	
	if(!nuovo) {
	    // Frame vecchie

	    }
	} else {
	    // Frame nuove
	    if(codice == 1) {
		qDebug("value = %d, velocita = %d", livello, velocita);
		spento = livello ? false : true ;
		if(livello > 100) 
		    livello = 100;
		if(value != livello) { 
		    value = livello;
		    aggiorna = 1;
		}
		speed = velocita;
		setValue(livello);
		if(!livello) {
		    if(isActive()) {
			impostaAttivo(0);
			spento = true ;
			aggiorna = 1;
		    }
		}
		else {
		    if(!isActive()) {
			impostaAttivo(1);
			spento = false ;
			aggiorna = 1;
		    }
		}
	    } else if(codice == 2) {
		qDebug("Frame temp: %d %d %d", h, m ,s);
		if((h == 255) && (m == 255) && (s == 255)) {
		    if(isActive()) {
			impostaAttivo(0);
			aggiorna = 1;
		    }
		} else if((h == 0) && (m == 0) && (s == 0)) {
		    qDebug("MAMMA MIA: frame temporizzata con tempo 0");
		    if(!isActive()) {
			// Legge il livello. Senno` come fa ?
			//inizializza();
			//qDebug("REINIZIALIZZO IL DIMMER 100");
			impostaAttivo(1);
			aggiorna = 1;
		    }
		} else {
		    qDebug("Devo chiedere livello e velocita`");
		    inizializza();
		    return;
		}
	    }
	}
    if (aggiorna)
	  Draw();
}

void dimmer100::inizializza()
{   
    openwebnet msg_open;
    char    pippo[50];    
    memset(pippo,'\000',sizeof(pippo));
    strcat(pippo,"*#1*");
    strcat(pippo,getAddress());
    strcat(pippo,"*1##"); 
    msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0])); 
    emit richStato(msg_open.frame_open);
}



void dimmer100::Draw()
{
  if(value > 100)
    value = 100;
    qDebug("dimmer100::Draw(), attivo = %d, value = %d", attivo, value);
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
	if ( (Icon[2]) && (csxButton) )
	{
	    csxButton->setPixmap(*Icon[2]);
	    qDebug("*** Icon[%d]", 2);
	}
	if ( (cdxButton) && (Icon[3]) )
	{
	    cdxButton->setPixmap(*Icon[3]);
	    qDebug("**** Icon[%d]", 3);
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

grAttuatAutom::grAttuatAutom( QWidget *parent,const char *name,void *indirizzi, char* IconaDx,char* IconaSx,char *icon ,int period,int number )
        : bannOnOff( parent, name )
        {     
    SetIcons( IconaDx, IconaSx,NULL,icon,period ,number );
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
   // qDebug("mando frame attuat autom int %s",pippo);
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
 //   qDebug("mando frame attuat autom intSic %s",pippo);
    msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
    emit sendFrame(msg_open.frame_open);    
}


/*****************************************************************
**attuatAutomTemp
****************************************************************/

attuatAutomTemp::attuatAutomTemp( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *pressedIcon ,int period,int number, QPtrList<QString> *lt)
        : bannOnOff2scr( parent, name )
        {     
    SetIcons( IconaDx, IconaSx ,icon, pressedIcon,period ,number );
    setAddress(indirizzo);
    cntTempi=0;
    static const char *t[] =  { "1'", "2'", "3'", "4'", "5'", "10'", "15'" } ;
    tempi = new QPtrList<QString>;
    tempi->clear();
    qDebug("***** lt->count = %d *****", lt->count());
    int nt = lt->count() ? lt->count() : sizeof(t) / sizeof(char *) ;
    for(int i = 0; i < nt ; i++) {
	QString *s;
	if(lt->count() && i < lt->count())
	    s = lt->at(i);
	else
	    s = new QString(t[i]) ;
	tempi->append(s);
    }
    //SetSeconaryText((tempi->at(cntTempi))->ascii());
    assegna_tempo_display();
    SetSeconaryText(tempo_display);
    connect(this, SIGNAL(dxClick()), this, SLOT(Attiva()));
    connect(this, SIGNAL(sxClick()), this, SLOT(CiclaTempo()));
}

void attuatAutomTemp::leggi_tempo(char *&out)
{
   if(cntTempi >= ntempi())
      out = (char *)"";
   else
      out = (char *)(tempi->at(cntTempi))->ascii();
}

uchar attuatAutomTemp::ntempi()
{
    return tempi->count();
}

#if 0
bool attuatAutomTemp::isForMe(openwebnet& m)
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
#endif

void attuatAutomTemp::gestFrame(char* frame)
{  
    openwebnet msg_open;
    char aggiorna;
    
    aggiorna=0;
    
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    
    if(isForMe(msg_open)) {
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
    cntTempi= (cntTempi+1) % ntempi();
    qDebug("ntempi = %d", ntempi());
    qDebug("cntTempi = %d", cntTempi);
    assegna_tempo_display();
    SetSeconaryText(tempo_display);
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
  //  qDebug("mando frame attuat autom Temp %s",pippo);
    msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
    emit sendFrame(msg_open.frame_open);    
}

void attuatAutomTemp::assegna_tempo_display()
{ 
    char *s ; leggi_tempo(s) ; 
    strcpy(tempo_display, s);
}

attuatAutomTemp::~attuatAutomTemp()
{
    int i;
    for(i=0; i<tempi->count(); i++)
	delete tempi->at(i);
    delete tempi;
}

/*****************************************************************
**attuatAutomTempNuovoN
****************************************************************/

attuatAutomTempNuovoN::attuatAutomTempNuovoN( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *pressedIcon ,int period,int number , QPtrList<QString> *lt)
        : attuatAutomTemp( parent, name, indirizzo, IconaSx, IconaDx, icon,
			   pressedIcon, period, number, lt)
{     
    assegna_tempo_display();
    stato_noto = false ;
    SetSeconaryText(tempo_display);
}

void attuatAutomTempNuovoN::gestFrame(char* frame)
{  
    openwebnet msg_open;
    char aggiorna;

    aggiorna=0;
    qDebug("attuatAutomTempNuovoN::gestFrame()");
    
    // *#1*dove*2*ore*min*sec## 
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    
    if(isForMe(msg_open)) {
	if(msg_open.IsMeasureFrame()) {
	    uchar c, h, m, s;
	    c = atoi(msg_open.Extract_grandezza());
	    qDebug("Frame misura (%d)", c);
	    if(c == 2) {
		qDebug("Frame temporizzata nuova");
		h = atoi(msg_open.Extract_valori(0));
		m = atoi(msg_open.Extract_valori(1));
		s = atoi(msg_open.Extract_valori(2));
		qDebug("Frame temporizzata con h = %d, m = %d, s = %d", 
		       h, m, s);
		if((h == 255) && (m == 255) && (s == 255)) {
		    stato_noto = true ;
		    if(isActive()) {
			impostaAttivo(0);
			aggiorna = 1;
		    }
		} else if((h == 0) && (m == 0) && (s == 0)) {
		    qDebug("Frame temporizzata con tempo 0");
		    // No timing, ask for state
		    if(!stato_noto) {
			attuatAutomTemp::inizializza();
			aggiorna = 0;
		    } else
			aggiorna = 1;
		} else {
		    stato_noto = true ;
		    if(!isActive()) {
			impostaAttivo(1);
			aggiorna = 1;
		    }
		}
	    } else if(c == 1) {
		qDebug("Misura da dimmer 100 livelli");
		uchar l, v;
		l = atoi(msg_open.Extract_valori(0)) - 100;
		v = atoi(msg_open.Extract_valori(1));
		qDebug("Livello = %d, velocita` = %d", l, v);
		stato_noto = true ;
		if(l) {
		    if(!isActive()) {
			impostaAttivo(1);
			aggiorna = 1;
			inizializza();
		    }
		} else {
		    if(isActive()) {
			impostaAttivo(0);
			aggiorna = 1;
		    }
		}
	    } else {
		qDebug("Frame misura con cosa = %d. Ignoro !!", c);
		return;
	    }
	} else if(msg_open.IsNormalFrame()) {
	    int c = atoi(msg_open.Extract_cosa());
	    qDebug("Comando con cosa = %d", c);
	    if(!c) {
		// OFF illuminazione
		if(isActive()) {
		    impostaAttivo(0);
		    aggiorna = 1;
		    stato_noto = true ;
		}
	    } else if((c >= 1) && (c <= 30)) {
		// ON illuminazione, ON percentuale, vecchie temporizzate
		if(!isActive()) {
		    impostaAttivo(1);
		    aggiorna = 1;
		    stato_noto = true ;
		}
	    } 
	}
    }
    if (aggiorna) {
	qDebug("invoco Draw con value = %d", value);
	Draw();
    }
}

void attuatAutomTempNuovoN::Attiva()
{
    openwebnet msg_open;
    char frame[100];
    char *t ; leggi_tempo(t);
    sprintf(frame, "*#1*%s*#2*%s##", getAddress(), t);
    msg_open.CreateNullMsgOpen();     
    msg_open.CreateMsgOpen(frame, strlen(frame));
    emit sendFrame(msg_open.frame_open);
}

// *#1*dove*2## 
void attuatAutomTempNuovoN::inizializza()
{ 
    openwebnet msg_open;
    char    pippo[50];
    
    memset(pippo,'\000',sizeof(pippo));
    strcat(pippo,"*#1*");
    strcat(pippo,getAddress());
    strcat(pippo,"*2##");
    //  qDebug("mando frame attuat autom Temp %s",pippo);
    msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
    emit sendFrame(msg_open.frame_open);    
}

void attuatAutomTempNuovoN::assegna_tempo_display()
{
    char tmp[50];
    int hh , mm, ss;
    char *ptr;
    leggi_tempo(ptr);
    strcpy(tmp, ptr);
    // Prende solo hh e min
    ptr = strtok(tmp, "*");
    hh = strtol(ptr, NULL, 10);
    ptr = strtok(NULL, "*");
    mm = strtol(ptr, NULL, 10);
    ptr = strtok(NULL, "*");
    ss = strtol(ptr, NULL, 10);
    qDebug("tempo = %d %d %d", hh, mm, ss);
    if(!hh && !mm) {
	// Time in secs
	sprintf(tempo_display, "%d\"", ss);
    } else if (!hh) {
	// Time in mins'
	sprintf(tempo_display, "%d'", mm);
    } else if(hh < 10) {
	// Time in hh:mm
	sprintf(tempo_display, "%d:%d", hh, mm);
    } else {
	// Time in hh h
	sprintf(tempo_display, "%dh", hh);
    }
}


/*****************************************************************
**attuatAutomTempNuovoF
****************************************************************/

#define NTIMEICONS 9

attuatAutomTempNuovoF::attuatAutomTempNuovoF( QWidget *parent,const char *name,char* indirizzo,char* IconaCentroSx,char* IconaCentroDx,char *IconaDx, const char *t)
        : bannOn2scr( parent, name)
{     
    attuatAutomTempNuovoF::SetIcons( IconaCentroSx, IconaCentroDx, IconaDx );
    setAddress(indirizzo);
    SetSeconaryText("????");
    strncpy(tempo, t ? t : "0*0*0", sizeof(tempo));
    char *ptr ; 
    char tmp1[50];
    strcpy(tmp1, tempo);
    ptr = strtok(tmp1, "*");
    h = strtol(ptr, NULL, 10);
    ptr = strtok(NULL, "*");
    m = strtol(ptr, NULL, 10);
    ptr = strtok(NULL, "*");
    s = strtol(ptr, NULL, 10);
    val = 0;
    qDebug("tempo = %d %d %d", h, m, s);
    char tmp[50];
    if(!h && !m) {
	// Time in secs
	sprintf(tmp, "%d\"", s);
    } else if (!h) {
	// Time in mins'
	sprintf(tmp, "%d'", m);
    } else if(h < 10) {
	// Time in hh:mm
	sprintf(tmp, "%d:%d", h, m);
    } else {
	// Time in hh h
	sprintf(tmp, "%dh", h);
    }
    myTimer = new QTimer(this,"periodic_refresh");
    stato_noto = false ;
    temp_nota = false ;
    connect(myTimer,SIGNAL(timeout()),this,SLOT(update()));
    SetSeconaryText(tmp);
    connect(this,SIGNAL(dxClick()),this,SLOT(Attiva())); 
}

void attuatAutomTempNuovoF::gestFrame(char* frame)
{  
    openwebnet msg_open;
    char aggiorna;

    aggiorna=0;

    qDebug("attuatAutomTempNuovoF::gestFrame");

    // *#1*dove*2*ore*min*sec## 
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    
    if(isForMe(msg_open)) {
	if(msg_open.IsMeasureFrame()) {
	    uchar c, hnow, mnow, snow;
	    c = atoi(msg_open.Extract_grandezza());
	    if(c == 2) {
		hnow = atoi(msg_open.Extract_valori(0));
		mnow = atoi(msg_open.Extract_valori(1));
		snow = atoi(msg_open.Extract_valori(2));
		qDebug("Frame temporizzata con h = %d, m = %d, s = %d", hnow, 
		       mnow, snow);
		if((hnow == 255) && (mnow == 255) && (snow == 255)) {
		    if(isActive()) {
			stato_noto = true ;
			temp_nota = true ;
			impostaAttivo(0);
			aggiorna = 1;
			myTimer->stop();
		    }
		} else if((hnow == 0) && (mnow == 0) && (snow == 0)) {
		    qDebug("Frame temporizzazione con tempo 0");
		    myTimer->stop();
		    // Legge lo stato, non c'e` temporizzazione
		    if(!stato_noto)
			chiedi_stato();
		    temp_nota = true ;
		    value = 0;
		    aggiorna = 1;
		} else {
		    if(mnow > 60) {
			qDebug("Minuti non ammissibili, lascio perdere");
			return;
		    }
		    if(snow > 60) {
			qDebug("Secondi non ammissibili, lascio perdere");
			return;
		    }
		    int tmpval = (hnow * 3600) + (mnow * 60) + snow;
		    if(tmpval == val) return;
		    val = tmpval ;
		    impostaAttivo((val != 0));
		    if(!val) myTimer->stop();
		    stato_noto = true;
		    temp_nota = true ;
		    qDebug("tempo = %d %d %d", hnow, mnow, snow);
		    aggiorna = 1;
		}
	    } else if(c == 1) {
		qDebug("Misura da dimmer 100 livelli");
		uchar l, v;
		l = atoi(msg_open.Extract_valori(0)) - 100;
		v = atoi(msg_open.Extract_valori(1));
		stato_noto = true ;
		temp_nota = false ;
		qDebug("Livello = %d, velocita` = %d", l, v);
		if(l) {
		    if(!isActive()) {
			impostaAttivo(1);
			aggiorna = 1;
			// Valore iniziale = il valore impostato
			//val = h * 3600 + m * 60 + s;
			// Senno` come fa a conoscere la temporizzazione ?
			inizializza();
			// e programma un aggiornamento
			//myTimer->start((1000 * val) / NTIMEICONS );
			value = 0;
			aggiorna = 1;
		    }
		} else {
		    if(isActive()) {
			impostaAttivo(0);
			stato_noto = true ;
			aggiorna = 1;
			myTimer->stop();
		    }
		}
	    } else {
		qDebug("Frame misura con cosa = %d. Ignoro !!", c);
		return;
	    }
	} else if(msg_open.IsNormalFrame()) {
	    int c = atoi(msg_open.Extract_cosa());
	    qDebug("Comando con cosa = %d", c);
	    if ((c >= 1) && (c <= 30))  {
		if (isActive()!=1) {
		    impostaAttivo(1);
		    aggiorna=1;
		    val = 0;
		    // Chiede subito la temporizzazione
		    if(!temp_nota)
			inizializza();
		    // e programma un aggiornamento
		    // &myTimer->start((1000 * val) / NTIMEICONS );
		}
	    } else if(c == 0) {
		if (isActive()!=0) {
		    impostaAttivo(0);
		    aggiorna=1;
		    stato_noto = true;
		    temp_nota = true;
		    myTimer->stop();
		}
	    }
	}
    }
    if (aggiorna) {
	qDebug("invoco Draw con value = %d", value);
        Draw();
    }
}

void attuatAutomTempNuovoF::update()
{
    openwebnet msg_open;
    char frame[100];
    sprintf(frame, "*#1*%s*2##", getAddress());
    msg_open.CreateNullMsgOpen();     
    msg_open.CreateMsgOpen(frame, strlen(frame));
    emit sendFrame(msg_open.frame_open);
}

void attuatAutomTempNuovoF::Attiva()
{
    openwebnet msg_open;
    char frame[100];
    char *t ; leggi_tempo(t);
    sprintf(frame, "*#1*%s*#2*%s##", getAddress(), t);
    msg_open.CreateNullMsgOpen();     
    msg_open.CreateMsgOpen(frame, strlen(frame));
    emit sendFrame(msg_open.frame_open);
    // Chiede subito la temporizzazione
    update();
    // Valore iniziale = il valore impostato
    int v = h * 3600 + m * 60 + s;
    // e programma un aggiornamento
    myTimer->start((1000 * v) / NTIMEICONS );
    Draw();
}

// *#1*dove*2## 
void attuatAutomTempNuovoF::inizializza()
{ 
    openwebnet msg_open;
    char    pippo[50];
    
    memset(pippo,'\000',sizeof(pippo));
    strcat(pippo,"*#1*");
    strcat(pippo,getAddress());
    strcat(pippo,"*2##");
    //  qDebug("mando frame attuat autom Temp %s",pippo);
    msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
    emit sendFrame(msg_open.frame_open);    
}

// Chiede lo stato dell'attuatore con una frame vecchia
void attuatAutomTempNuovoF::chiedi_stato()
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

void attuatAutomTempNuovoF::leggi_tempo(char *&out)
{
    out = tempo;
}

void attuatAutomTempNuovoF::SetIcons(char *i1, char *i2, char *i3)
{
    qDebug("attuatAutomTempNuovoF::SetIcons");
    char tmp[MAX_PATH], tmp1[MAX_PATH] ;
    char *ptr;
    char pressIconName[MAX_PATH];
    Icon[0] = new QPixmap();
    strcpy(tmp1, i2);
    ptr = strtok(tmp1, ".");
    sprintf(tmp, "%soff.png", ptr);
    Icon[0]->load(tmp);
    qDebug("Icon[0] <- %s", tmp);
    Icon[1] = new QPixmap();
    strcpy(tmp1, i2);
    ptr = strtok(tmp1, ".");
    sprintf(tmp, "%son.png", ptr);
    Icon[1]->load(tmp);
    qDebug("Icon[1] <- %s", tmp);
    Icon[2] = new QPixmap();
    Icon[2]->load(i3);
    qDebug("Icon[2] <- %s", i3);
    getPressName(i3, pressIconName, sizeof(pressIconName));
    if (QFile::exists(pressIconName)) {
	pressIcon[2] = new QPixmap();
	pressIcon[2]->load(pressIconName);
	qDebug("pressIcon[2] <- %s", pressIconName);
    }
    for(int i = 0; i < NTIMEICONS ; i++) {
	Icon[3 + i] = new QPixmap();
	strcpy(tmp1, i1);
	ptr = strtok(tmp1, ".");
	sprintf(tmp, "%s%d.png", ptr, i);
	Icon[3 + i]->load(tmp);	
	qDebug("Icon[%d] <- %s", 3+i, tmp);
    }
}

void attuatAutomTempNuovoF::Draw()
{
    qDebug("attuatAutomTempNuovoF::Draw(), attivo = %d, value = %d", 
	   attivo, val);
    if (attivo == 1) {
	int index = ((10 * val * (NTIMEICONS-1))/((h * 3600) + (m * 60) + s)) ;
	index = (index % 10) >= 5 ? index/10 + 1 : index/10;
	if(index >= NTIMEICONS)
	    index = NTIMEICONS - 1;
	qDebug("index = %d", index);
	if (Icon[3 + index] && BannerIcon2) {
	    BannerIcon->setPixmap(*Icon[3 + index]);
	    qDebug("* Icon[%d]", 3 + index);
	}
	if (Icon[1] && BannerIcon) {
	    BannerIcon2->setPixmap(*Icon[1]);
	    qDebug("** Icon[%d]", 1);
	}
    } else {
	if (Icon[0] && BannerIcon) {
	    BannerIcon->setPixmap(*Icon[3]);
	    qDebug("*** Icon[3]");
	}
	if (Icon[3] && BannerIcon2) {
	    BannerIcon2->setPixmap(*Icon[0]);
	    qDebug("**** Icon[0]");
	}
    }
    if ( (dxButton) && (Icon[2]) ) {
	dxButton->setPixmap(*Icon[2]);
	if (pressIcon[2])
	    dxButton->setPressedPixmap(*pressIcon[2]);
    }
    if (BannerText) {
	BannerText->setAlignment(AlignHCenter|AlignVCenter);//AlignTop);
	BannerText->setFont( QFont( "helvetica", 14, QFont::Bold ) );
	BannerText->setText(testo);
    }
    if (SecondaryText) {	
	SecondaryText->setAlignment(AlignHCenter|AlignVCenter);
	SecondaryText->setFont( QFont( "helvetica", 18, QFont::Bold ) );
	SecondaryText->setText(testoSecondario);
    } 
}


/*****************************************************************
**gruppo di attuatInt
****************************************************************/

grAttuatInt::grAttuatInt( QWidget *parent,const char *name,void *indirizzi,char* IconaSx,char* IconaDx, char *icon ,int period,int number )
        : bann3But( parent, name )
        {     
    SetIcons(  IconaDx,IconaSx ,NULL,icon,period ,number );
    setAddress(indirizzi);
    connect(this,SIGNAL(dxClick()),this,SLOT(Alza()));
    connect(this,SIGNAL(sxClick()),this,SLOT(Abbassa()));
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
    //qDebug("%s - %s - %s - %s", IconaSx, IconaDx, icon, inactiveIcon);
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
 //   qDebug("mando frame ampli %s",pippo);
    msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
    emit sendFrame(msg_open.frame_open);    
    
    memset(pippo,'\000',sizeof(pippo));
    strcat(pippo,"*#16*");
    strcat(pippo,getAddress());
    strcat(pippo,"*5##");
 //   qDebug("mando frame ampli %s",pippo);
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
                    myRadio->setRDS("");
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
   // double f;
    
    memset(pippo,'\000',sizeof(pippo));
    memset(pippa,'\000',sizeof(pippa));
    strcat(pippo,"*#16*");
    strcat(pippo,getAddress());
    strcat(pippo,"*#10*");
    memset(pippa,'\000',sizeof(pippa));
    //     f=myRadio->getFreq()*1000;
    //     sprintf(pippa,"%.0f",f);
    //     qDebug("pippa2  :%s",&pippa[0]);
    //     strcat(pippo,pippa);
    //     strcat(pippo,"*");
    ic=(unsigned int)st;
    sprintf(pippa,"%01hu",st);
 //   qDebug("pippa1  :%01hu",&pippa[0]);
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
termoPage::termoPage ( QWidget *parent, const char *name ,char*indirizzo,char* IconaMeno,char* IconaPiu,char*IconaMan,char*IconaAuto, char* IconaAntigelo, char* IconaOff, QColor SecondForeground)
        : bannTermo( parent, name, SecondForeground)
        {       
    //SetIcons( IconaMeno, IconaPiu , ICON_SONDAOFF,ICON_SONDAANTI);/*IconaMan, IconaAuto );*/
    SetIcons( IconaMeno, IconaPiu, IconaOff, IconaAntigelo);
    setAddress(indirizzo);
    strncpy(&manIco[0],IconaMan,sizeof(manIco));
    strncpy(&autoIco[0],IconaAuto, sizeof(autoIco));
    connect(this,SIGNAL(dxClick()),this,SLOT(aumSetpoint()));
    connect(this,SIGNAL(sxClick()),this,SLOT(decSetpoint()));    
    setChi("4");
    stato=S_MAN;
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
                    if  (stato!=S_MAN) 
                    {
                        
                        stato=S_MAN;
                        mostra(BUT1);
                        mostra(BUT2);
                        nascondi(ICON);
                        tempImp->show();
                        aggiorna=1;
                        if (isShown())
                        {
                            ((sottoMenu*)parentWidget())->setNavBarMode(4,&autoIco[0]);
                            ((sottoMenu*)parentWidget())->forceDraw();
                        }
                    }
                }
                else if  ( (!strcmp(msg_open.Extract_cosa(),"111")) || (!strcmp(msg_open.Extract_cosa(),"211")) || (!strcmp(msg_open.Extract_cosa(),"311")) )
                {
                    if  (stato!=S_AUTO) 
                    {                 
                        nascondi(BUT1);
                        nascondi(BUT2);
                        nascondi(ICON);
                        tempImp->show();
                        stato=S_AUTO;
                        aggiorna=1;
                        if(isShown())
                        {
                            ((sottoMenu*)parentWidget())->setNavBarMode(4,&manIco[0]);
                            ((sottoMenu*)parentWidget())->forceDraw();
                        }
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
                    stato=S_ANTIGELO;
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
                    stato=S_OFF;
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
                    qDebug("temperatura misurata: %d",(int)icx);
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
                    
                    icx=atoi(msg_open.Extract_valori(0));
                    qDebug("temperatura setpoint: %d",(int)icx);                    
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

char* termoPage::getAutoIcon()
{
    return &autoIco[0];
}

char* termoPage::getManIcon()
{
    return &manIco[0];
}

void termoPage::autoMan()
{
    openwebnet msg_open;
    char    pippo[50];
    
    memset(pippo,'\000',sizeof(pippo));
    
    
    if    (stato==S_MAN)
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
                    nascondi(BUT1);
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
    tasti->setMode(tastiera::HIDDEN);
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
    tasti->setMode(tastiera::HIDDEN);
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
                    
                    in_progr=0;
                }
                else
                {
                    //   nascondi(BUT2);
                    in_progr=1;
                    exitInfo();
                }
                aggiorna=1;
            }
            else if  (!strncmp(msg_open.Extract_cosa(),"43",2))
            {
                bloccato=1;
                exitInfo();
                aggiorna=1;
            }
            else if  (!strncmp(msg_open.Extract_cosa(),"45",2))
            {
                in_progr=1;
                exitInfo();
                aggiorna=1;
            }
            else if  (!strncmp(msg_open.Extract_cosa(),"41",2))  
            {
                SetIcons(uchar(0),&iconOn[0]);       
                disconnect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
                connect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
                disconnect(this,SIGNAL(sxClick()),this,SLOT(stopProgScen()));
                //mostra(BUT2);
                aggiorna=1;
                in_progr=0;
            }
            else if  (!strncmp(msg_open.Extract_cosa(),"44",2))
            {
                SetIcons(uchar(0),&iconOn[0]);       
                disconnect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
                connect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
                disconnect(this,SIGNAL(sxClick()),this,SLOT(stopProgScen()));
                //mostra(BUT2);
                aggiorna=1;
                bloccato=0;
            } 
            else if (!strcmp(msg_open.Extract_cosa(),"0")) 
            {
            }
        }
    }    
    /*  if (!strcmp(msg_open.Extract_chi(),"1001"))
    {
        if ( (! strcmp(msg_open.Extract_dove(),&dove[0])) )
        {	
            if ( (! strcmp(msg_open.Extract_grandezza(),"1")) )
            {
                if ( (! strcmp(msg_open.Extract_valori(0),"55")) )
                    mostra(BUT2);	
            }
        }
    }*/
    
    if (aggiorna)
    {
//        qDebug("bloccato= %d * in_progr= %d",bloccato,in_progr);
        if (bloccato || in_progr)
            nascondi(BUT2);
        else
            mostra(BUT2);
        Draw();
    }
}

void gesModScen::inizializza()
{   
    openwebnet msg_open;
    char    pippo[50];
    
    memset(pippo,'\000',sizeof(pippo));
    strcat(pippo,"*#0*");
    strcat(pippo,&dove[0]);
    strcat(pippo,"##");
    msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
    emit sendFrame(msg_open.frame_open);    	
    nascondi(BUT2);     
}

/*****************************************************************
** Scenario evoluto
****************************************************************/	

scenEvo::scenEvo( QWidget *parent, const char *name, 
		  QPtrList<scenEvo_cond> *c, 
		  char *i1, char *i2, char *i3, char *i4, char *i5, 
		  char *i6, char *i7, QString act)
        : bann3But( parent, name )
{
    if(c)
	condList = new QPtrList<scenEvo_cond>(*c);
    cond_iterator = new QPtrListIterator<scenEvo_cond>(*condList);
    scenEvo_cond *co;
    QPtrListIterator<scenEvo_cond> *ci = 
	new QPtrListIterator<scenEvo_cond>(*condList);
    ci->toFirst();
    while( ( co = ci->current() ) != 0) {
      qDebug(co->getDescription());
      connect(co, SIGNAL(verificata()), this, SLOT(trig()));
      ++(*ci);
    }
    delete ci;
    action = act;
    qDebug("#### action = %s ####", action.ascii());
    SetIcons(i1, i2 , i3, i4);
    impostaAttivo(0);
    connect(this,SIGNAL(sxClick()),this,SLOT(toggleAttivaScev()));
    connect(this,SIGNAL(dxClick()),this,SLOT(configScev()));
    connect(this,SIGNAL(centerClick()),this,SLOT(forzaScev()));
    connect(parent, SIGNAL(frez(bool)), this, SLOT(freezed(bool)));
}

void scenEvo::toggleAttivaScev(void)
{
    qDebug("scenEvo::toggleAttivaScev");
    impostaAttivo(!isActive());
    Draw();
}


void scenEvo::configScev(void)
{
    qDebug("scenEvo::configScev");
    hide();
    scenEvo_cond *co = cond_iterator->current();
    qDebug("Invoco %p->mostra()", co);
    co->setBGColor(backgroundColor());
    co->setFGColor(foregroundColor());
    connect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
    connect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
    co->mostra();
}

void scenEvo::forzaScev(void)
{
    qDebug("scenEvo::forzaScev");
    trig();
    Draw();
}

void scenEvo::nextCond(void)
{
    qDebug("scenEvo::nextCond()");
    scenEvo_cond *co = cond_iterator->current();
    disconnect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
    disconnect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
    co->hide();
    if(!cond_iterator->atLast()) {
	++(*cond_iterator);
	co = cond_iterator->current();
	qDebug("co = %p", co);
	if(co) {
	    co->setBGColor(backgroundColor());
	    co->setFGColor(foregroundColor());
	    connect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	    connect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	    co->mostra();
	}
    } else {
	cond_iterator->toFirst();
	Draw();
	show();
    }
}

void scenEvo::prevCond(void)
{
    qDebug("scenEvo::prevCond()");
    scenEvo_cond *co = cond_iterator->current();
    disconnect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
    disconnect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
    co->hide();
    if(!cond_iterator->atFirst()) {
	--(*cond_iterator);
	co = cond_iterator->current();
	qDebug("co = %p", co);
	if(co) {
	    co->setBGColor(backgroundColor());
	    co->setFGColor(foregroundColor());
	    connect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	    connect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	    co->mostra();
	}
    } else {
	Draw();
	show();
    }
}

void scenEvo::Draw()
{
    // Icon[0] => left button (inactive)
    // pressIcon[0] => pressed left button (inactive)
    // Icon[1] => left button (active)
    // pressIcon[1] => pressed left button (active)
    // Icon[3] => center button
    // pressIcon[3] => pressed center button
    // Icon[2] => right button
    // pressIcon[2] => pressed right button
    qDebug("scenEvo::Draw()");
    cond_iterator->toFirst();
    if ( (sxButton) && (Icon[0]) && (Icon[1])) {
	int sxb_index = isActive() ? 0 : 1;
	sxButton->setPixmap(*Icon[sxb_index]);
	if (pressIcon[sxb_index])
	    sxButton->setPressedPixmap(*pressIcon[sxb_index]);
    }
    if ( (dxButton) && (Icon[2]) ) {
	dxButton->setPixmap(*Icon[2]);
	if(pressIcon[2])
	    dxButton->setPressedPixmap(*pressIcon[2]);
    }
    if ( (csxButton) && (Icon[3]) ) {
	csxButton->setPixmap(*Icon[3]);
	if(pressIcon[3])
	    csxButton->setPressedPixmap(*pressIcon[3]);
    }
    if (BannerText) {
	BannerText->setAlignment(AlignHCenter|AlignVCenter);//AlignTop);
	BannerText->setFont( QFont( "helvetica", 14, QFont::Bold ) );
	BannerText->setText(testo);
	//     qDebug("TESTO: %s", testo);
    }
    if (SecondaryText) {	
	SecondaryText->setAlignment(AlignHCenter|AlignVCenter);
	SecondaryText->setFont( QFont( "helvetica", 18, QFont::Bold ) );
	SecondaryText->setText(testoSecondario);
    }
}

const char *scenEvo::getAction() 
{
    return action.ascii() ; 
}

void scenEvo::setAction(const char *a)
{
    action = a ;
}


void scenEvo::trig()
{
    if(!isActive()) {
	qDebug("scenEvo::trig(), non abilitato, non faccio niente");
	return;
    }
    if(!action) {
	qDebug("scenEvo::trig(), act = NULL, non faccio niente");
	return;
    }
    qDebug("scenEvo::trig(), act = %s", action.ascii());
    openwebnet msg_open;
    msg_open.CreateMsgOpen((char *)action.ascii(), action.length());
    emit sendFrame(msg_open.frame_open);    	
}

void scenEvo::freezed(bool f)
{
    qDebug("scenEvo::freezed(bool f)");
    QPtrListIterator<scenEvo_cond> *ci = 
	new QPtrListIterator<scenEvo_cond>(*condList);
    ci->toFirst();
    scenEvo_cond *co ;
    while( ( co = ci->current() ) != 0) {
	co->setEnabled(!f);
	++(*ci);
    }
}
