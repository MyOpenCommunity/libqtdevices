/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**sveglia.cpp
**
**Sottomenù imposta sveglia
**
****************************************************************/


#include <qfont.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <stdlib.h>
#include <qwidget.h>
#include <qcursor.h>
#include <qdatetime.h>
#include <qprocess.h>

#include <qfile.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "sveglia.h"
#include "genericfunz.h"


sveglia::sveglia( QWidget *parent, const char *name ,uchar t, uchar freq, diffSonora* diso, char* f,char* descr1,char* descr2,char* descr3,char* descr4, char*h, char*m)
        : QFrame( parent, name )
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
 //  showFullScreen();
#endif    
 qDebug("-----10-----");
    if(descr1)
	strncpy(&text1[0], descr1, sizeof(text1));
    if(descr2)
	strncpy(&text2[0], descr2, sizeof(text2));
    if(descr3)    
	strncpy(&text3[0], descr3, sizeof(text3));
    if(descr4)	
	strncpy(&text4[0], descr4, sizeof(text4));  
 qDebug("-----11-----");
   bannNavigazione  = new bannFrecce(this,"bannerfrecce",9);
   bannNavigazione  ->setGeometry( 0 , MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE ,MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE );
 qDebug("-----12-----");
   aumVolTimer=NULL;
   char iconName[MAX_PATH];
   QPixmap* Icon1 = new QPixmap();
   QPixmap* Icon2 = NULL;
	 
    setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT); 
    memset(iconName,'\000',sizeof(iconName));
    strcpy(iconName,ICON_FRECCIA_SU);
 qDebug("-----13-----");
    Icon1->load(iconName);	
    getPressName((char*)ICON_FRECCIA_SU, &iconName[0],sizeof(iconName));
            if (QFile::exists(iconName))
    {  	 
	Icon2 = new QPixmap();
	Icon2->load(iconName);
    }

    for (uchar idx=0;idx<2;idx++)
    {
	but[idx] = new BtButton(this,"freccia"/*+QString::number(idx)*/);

	but[idx] -> setGeometry(idx*80+50,80,60,60);
	but[idx] -> setAutoRepeat(true);
	but[idx] -> setPixmap(*Icon1);
	if (Icon2)
	{
	    but[idx] -> setPressedPixmap(*Icon2);
	}
    }
      qDebug("-----14-----");
    delete (Icon1);
    delete (Icon2);
    Icon2=NULL;
    Icon1 = new QPixmap();
    Icon1->load(ICON_FRECCIA_GIU);	
     getPressName((char*)ICON_FRECCIA_GIU, &iconName[0],sizeof(iconName));
        if (QFile::exists(iconName))
    {  
	Icon2 = new QPixmap();
	Icon2->load(iconName);
    }
    for (uchar idx=2;idx<4;idx++)
    {
	but[idx] = new BtButton(this,"freccia"+QString::number(idx));
	but[idx] -> setGeometry((idx-2)*80+50,190,60,60);	
	but[idx] -> setAutoRepeat(true);
	but[idx] -> setPixmap(*Icon1);
	if (Icon2)
	    but[idx] -> setPressedPixmap(*Icon2);
    }	
     qDebug("-----15-----");
     delete(Icon1);
     Icon1 = new QPixmap();
     Icon1->load(ICON_SVEGLIA_ON);    
       
     Immagine = new BtLabel(this, "immaginetta superiore");
    
     if (Icon1)
	 Immagine -> setPixmap(*Icon1); 
     qDebug("-----16-----");
    Immagine->setGeometry(90,0,80,80);
    delete(Icon1);
    delete(Icon2);
     Icon1 = new QPixmap();
     Icon1->load( ICON_VUOTO);    
     getPressName((char*)ICON_VUOTO, &iconName[0],sizeof(iconName));
     if (QFile::exists(iconName))
     {  
	Icon2 = new QPixmap();
	Icon2->load(iconName);
    }
    for (uchar idx=0;idx<4;idx++)
    {
	choice[idx] = new BtButton (this,"choice"+QString::number(idx));
	choice[idx]  -> setGeometry( 10,idx*60,60,60);
	choice[idx]  -> setToggleButton (TRUE);
	if (!(Icon1->isNull()))
	{
	    choice[idx]  -> setPixmap(*Icon1);
	}
	if (Icon2)
	    choice[idx]  -> setPressedPixmap(*Icon2);
	choice[idx]  -> hide();
	testiChoice[idx] = new BtLabel(this,"choiceLabel"+QString::number(idx));
	testiChoice[idx] -> setGeometry( 80,idx*60,120,60);
	testiChoice[idx] -> setAlignment(AlignHCenter|AlignVCenter);
	testiChoice[idx] -> setFont( QFont( "helvetica", 14, QFont::Bold ) );
	testiChoice[idx] -> hide();
    }	
    dataOra=NULL;
     qDebug("-----17-----");
    testiChoice[0]  -> setText(&text1[0]);
    testiChoice[1]  -> setText(&text2[0]);
    testiChoice[2]  -> setText(&text3[0]);
    testiChoice[3]  -> setText(&text4[0]);        
   // OroTemp = QDateTime(QDateTime::currentDateTime());
     oraSveglia =  new QDateTime();//QDate(),QTime(12,0));
     oraSveglia->setTime(QTime(atoi(h),atoi(m)));
     oraSveglia->setDate(QDate::currentDate(Qt::LocalTime));     
      qDebug("-----18-----");
     dataOra = new timeScript(this,"scrittaHomePage",2,oraSveglia);
     dataOra->setGeometry(40,140,160,50);
     dataOra->setFrameStyle( QFrame::Plain );
     dataOra->setLineWidth(0);    
     difson=diso;
    if (difson)
    {
	 difson->hide();
    }
    for (uchar idx=0;idx<AMPLI_NUM;idx++)
	volSveglia[idx]=0;
    minuTimer=NULL;
    tipoSveglia=freq ;
    tipo=t;
    qDebug("tipoSveglia= %d - tipo= %d",tipoSveglia,tipo);
    if (tipo==FRAME)
    {
	frame = new char[50];
	memset(frame,'\000',sizeof(frame));
	sprintf(frame,"%s",f);
/*	qDebug("f= %s",f);
	qDebug("frame= %s",frame);*/
    }
//    buzzer=1;
}

void sveglia::setBGColor(int r, int g, int b)
{
    setBGColor( QColor :: QColor(r,g,b)); 
}
void sveglia::setFGColor(int r, int g, int b)
{
    setFGColor( QColor :: QColor(r,g,b)); 
}
void sveglia::setBGColor(QColor c)
{	
    setPaletteBackgroundColor( c);    
    if (dataOra)
	dataOra->setPaletteBackgroundColor(c);
     for (uchar idx=0;idx<6;idx++)
    {
	    if (but[idx])
		but[idx]->setPaletteBackgroundColor(c);
    }
    for (uchar idx=0;idx<4;idx++)
    {
	    if (choice[idx])
		choice[idx]->setPaletteBackgroundColor(c);
	    testiChoice[idx]->setPaletteBackgroundColor(c);
    }
    if ( bannNavigazione)
	bannNavigazione->setBGColor(c);
}
void sveglia::setFGColor(QColor c)
{
    setPaletteForegroundColor(c);
    if (dataOra)
        dataOra->setPaletteForegroundColor(c);
     for (uchar idx=0;idx<6;idx++)
   {
	 if (but[idx])   
	     but[idx]->setPaletteForegroundColor(c);
    }
    for (uchar idx=0;idx<4;idx++)
    {
	    if (choice[idx])
		 choice[idx]->setPaletteForegroundColor(c);
	    testiChoice[idx]->setPaletteForegroundColor(c);
    }
        if ( bannNavigazione)
	bannNavigazione->setFGColor(c);
}

int sveglia::setBGPixmap(char* backImage)
{
    QPixmap Back; 
     if(Back.load(backImage))
    {
	 setPaletteBackgroundPixmap(Back);      
	 return (0);
    }    
     return (1);
}

void sveglia::okTime()
{
    disconnect( but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumOra()));
    disconnect( but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumMin()));
    disconnect( but[2] ,SIGNAL(clicked()),dataOra,SLOT(diminOra()));
    disconnect( but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminMin()));
    disconnect(bannNavigazione  ,SIGNAL(forwardClick()),this,SLOT(okTime()));
    connect( bannNavigazione ,SIGNAL(forwardClick()),this,SLOT(okTipo()));

    
    for (uchar idx=0;idx<4;idx++)
	but[idx]  -> hide();   
    for (uchar idx=0;idx<4;idx++)
    {
	choice[idx]  -> show();   
	testiChoice[idx] -> show();
	if (tipoSveglia==idx)
	    choice[idx]  ->setOn(TRUE);
	else
	    choice[idx]  ->setOn(FALSE);
    }
    dataOra->hide();
    Immagine->hide();
    
/*    delete(oraSveglia);
    oraSveglia = new QDateTime(dataOra->getDataOra());*/
	
}


void sveglia::mostra()
{
    for (uchar idx=0;idx<4;idx++)
	but[idx]  -> show();   
    dataOra->show();
    Immagine->show();
    
    bannNavigazione->show();
      for (uchar idx=0;idx<4;idx++)
    {
	choice[idx]  -> hide();
	testiChoice[idx] -> hide();
    }	
    show();     
    disconnect( but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumOra()));
    disconnect( but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumMin()));
    disconnect( but[2] ,SIGNAL(clicked()),dataOra,SLOT(diminOra()));
    disconnect( but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminMin()));
    
    connect( but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumOra()));
    connect( but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumMin()));
    connect( but[2] ,SIGNAL(clicked()),dataOra,SLOT(diminOra()));
    connect( but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminMin()));
    connect(bannNavigazione  ,SIGNAL(forwardClick()),this,SLOT(okTime()));
    disconnect( bannNavigazione ,SIGNAL(forwardClick()),this,SLOT(okTipo()));    
    disconnect( bannNavigazione ,SIGNAL(forwardClick()),this,SLOT(Closed()));    
    
    disconnect(bannNavigazione  ,SIGNAL(backClick()),this,SLOT(Closed()));
    connect(bannNavigazione  ,SIGNAL(backClick()),this,SLOT(Closed()));
    
 //   connect(bannNavigazione  ,SIGNAL(backClick()),this,SLOT(okTime()));
    if (difson)
    {
	disconnect(difson,SIGNAL(Closed()),this,SLOT(Closed()));
	connect(difson,SIGNAL(Closed()),this,SLOT(Closed()));
    }
    connect(choice[0],SIGNAL(toggled(bool)),this,SLOT(sel1(bool)));
    connect(choice[1],SIGNAL(toggled(bool)),this,SLOT(sel2(bool)));
    connect(choice[2],SIGNAL(toggled(bool)),this,SLOT(sel3(bool)));
    connect(choice[3],SIGNAL(toggled(bool)),this,SLOT(sel4(bool)));   
    aggiornaDatiEEprom=0;
}


void sveglia::drawSelectPage()
{
      for (uchar idx=0;idx<4;idx++)
    {
	if (idx!=tipoSveglia)
   	    choice[idx]->setOn(FALSE);
    }
}

void sveglia::sel1(bool isOn)
{
    if (isOn)
	tipoSveglia=ONCE;
    drawSelectPage();
}
void sveglia::sel2(bool isOn)
{
    if (isOn)
	tipoSveglia=SEMPRE;
    drawSelectPage();
}
void sveglia::sel3(bool isOn)
{
    if (isOn)
	tipoSveglia=FERIALI;
    drawSelectPage();
}
void sveglia::sel4(bool isOn)
{
    if (isOn)
	tipoSveglia=FESTIVI;
    drawSelectPage();
}
void sveglia::Closed()
{
    qDebug("Sveglia Closed");
    //imposta la sveglia in 
    if (difson)
    {
	qDebug("Sveglia Closed DIFSON");
	if(aggiornaDatiEEprom)
	{
	     difson->hide();
	     difson->setNumRighe((uchar)4);	     
	     difson->setGeom(0,0,240,320);
	     difson->setNavBarMode(3);
	    //	difson->/*amplificatori->*/showFullScreen();
	     difson->reparent((QWidget*)NULL,0,QPoint(0,0),(bool)FALSE);
	     disconnect(difson,SIGNAL(Closed()),this,SLOT(Closed()));
#if defined (BTWEB) || defined (BT_EMBEDDED)
	    
	    qDebug("Sveglia Closed AGGIORNA EEPROM");
	    int eeprom;
	    eeprom = open("/dev/nvram", O_RDWR | O_SYNC, 0666);
	    lseek(eeprom,BASE_EEPROM+(serNum-1)*(AMPLI_NUM+KEY_LENGTH+SORG_PAR)+KEY_LENGTH, SEEK_SET);
	    qDebug("%d",(BASE_EEPROM+(serNum-1)*(AMPLI_NUM+KEY_LENGTH+SORG_PAR)+KEY_LENGTH));
	    for(unsigned int idx=0; idx<AMPLI_NUM;idx++)
	    {
		write(eeprom,&volSveglia[idx],1 );
	//	qDebug("%d : %d", idx, volSveglia[idx]);
	    }
	    write(eeprom,&sorgente,1 );
	    write(eeprom,&stazione,1 );
	    ::close(eeprom);
	    qDebug("Sveglia Closed FINE AGGIORNA EEPROM");
#endif
	}	
    }
    hide();
    
    gesFrameAbil=FALSE;
    /*    delete(oraSveglia);
    oraSveglia=new QDateTime();*/
    activateSveglia(TRUE);
    //    qDebug("sveglia::Closed()");
    
    delete(oraSveglia);
    oraSveglia = new QDateTime(dataOra->getDataOra());
    
    copyFile("cfg/conf.xml","cfg/conf1.lmx");
    setCfgValue("cfg/conf1.lmx",SET_SVEGLIA, "hour",oraSveglia->time().toString("hh"),serNum);
    setCfgValue("cfg/conf1.lmx",SET_SVEGLIA, "minute",oraSveglia->time().toString("mm"),serNum);
    char t[2];
    sprintf(&t[0],"%d",tipoSveglia);
    setCfgValue("cfg/conf1.lmx",SET_SVEGLIA, "alarmset",&t[0],serNum);
    QDir::current().rename("cfg/conf1.lmx","cfg/conf.xml",FALSE);
    
    emit(ImClosed());
}

void sveglia::okTipo()
{
    disconnect( bannNavigazione ,SIGNAL(forwardClick()),this,SLOT(okTipo()));
    connect( bannNavigazione ,SIGNAL(forwardClick()),this,SLOT(Closed()));
    for (uchar idx=0;idx<4;idx++)
    {
	choice[idx]  -> hide();   
	testiChoice[idx] -> hide();
    }
    Immagine->show();
    if (tipo!=DI_SON)
    {
	Closed();
    }
    else if (difson)
    {	
	difson->setNumRighe((uchar)3);	
	difson->setGeom(0,0,240,240);	
	difson->setNavBarMode(6);
	difson->reparent((QWidget*)this,(int)0,QPoint(0,80),(bool)TRUE);	
//	difson->show();	

	this->bannNavigazione->hide();
	aggiornaDatiEEprom=1;
	gesFrameAbil=TRUE;
	sorgente=101;
	stazione=0;	    
	for(unsigned int idx=0; idx<AMPLI_NUM;idx++)
	{
	    volSveglia[idx]=0;
	}
    }
}

void sveglia::activateSveglia(bool a)
{
    svegliaAbil=a;
    if (a)
    {
	if (!minuTimer)
	{	
	    minuTimer=new QTimer(this,"tick");
	    minuTimer->start(200);
                    connect(minuTimer,SIGNAL(timeout()), this,SLOT(verificaSveglia()));
	    setCfgValue(SET_SVEGLIA, "enabled","1",serNum);
	}
    }
    else
    {
	if (minuTimer)
	{
	    minuTimer->stop();
	    disconnect(minuTimer,SIGNAL(timeout()), this,SLOT(verificaSveglia()));
	    delete(minuTimer);
	    minuTimer=NULL;
    	    setCfgValue(SET_SVEGLIA, "enabled","0",serNum);
	}
    }
}
   
void sveglia::gestFrame(char* frame)
 {
    if (gesFrameAbil==FALSE)
	return;
    
    openwebnet msg_open;
    int	deviceAddr;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
   
    if (!strcmp(msg_open.Extract_chi(),"16"))
    {
	deviceAddr=atoi(msg_open.Extract_dove());
	if ( ( deviceAddr>=0 ) && ( deviceAddr<=AMPLI_NUM ) )
	{
	    if (!msg_open.IsMeasureFrame())
	    {
		if (!strcmp(msg_open.Extract_cosa(),"13")) 		
		{
		    volSveglia[deviceAddr]=0;				
		    if (deviceAddr==0)
		    for (uchar idx=0;idx<AMPLI_NUM;idx++)
			volSveglia[idx]=0;
		    if (deviceAddr<10)
		    for (uchar idx=0;idx<10;idx++)
			volSveglia[deviceAddr*10+idx]=0;		    
		}
		if (!strcmp(msg_open.Extract_cosa(),"3")) 		
		{
		    qDebug("ho visto un ampli acceso!");
		    openwebnet msg_open;
		    char    pippo[50];
		    memset(pippo,'\000',sizeof(pippo));
		    strcat(pippo,"*#16*");
		    sprintf(&pippo[5],"%d",deviceAddr);
		    strcat(pippo,"*1##");
		    msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
		    emit sendFrame(msg_open.frame_open);    
		}
	    }
	    else
	    {
		if ( !strcmp(msg_open.Extract_grandezza(),"1")) 
		{
		    int vol;
		    
		    vol=atoi(msg_open.Extract_valori(0))&0x1F;
		 //   vol=trasformaVol(vol);
		    volSveglia[deviceAddr]=vol;
		    qDebug("o visto un volume di %d",deviceAddr);
		}
	    }
	}
	else
	{
	    if (!strcmp(msg_open.Extract_cosa(),"3")) 
	    {
		sorgente=deviceAddr;
	    }
	    if (msg_open.IsMeasureFrame() && ( !strcmp(msg_open.Extract_grandezza(),"7")) )
	    {
		stazione=atoi(msg_open.Extract_valori(1))&0x1F;
		qDebug("Stazione %d",stazione);
	    }
	}
    }    
 }

void sveglia::verificaSveglia()
{
    if (!svegliaAbil)
	return;
    
    QDateTime actualDateTime = QDateTime(QDateTime::currentDateTime(Qt::LocalTime));
    
  
    if ( (tipoSveglia==SEMPRE) || \
	 ((tipoSveglia==ONCE) && (onceToGest)) ||\
	 ((tipoSveglia==FERIALI) && (actualDateTime.date().dayOfWeek()<6)) ||\
	 ((tipoSveglia==FESTIVI) && (actualDateTime.date().dayOfWeek()>5)) )
    {
	qDebug("secsTo: %d",oraSveglia->time().secsTo(actualDateTime.time()));
	if  ((actualDateTime.time()>=(oraSveglia->time()))&&((oraSveglia->time().secsTo(actualDateTime.time())<60)))
	{
	    if (tipo==BUZZER)
	    {
		aumVolTimer = new QTimer (this,"timer sveglia Buzzer");
		aumVolTimer -> start (100,FALSE);
		connect(aumVolTimer,SIGNAL(timeout()),this,SLOT(buzzerAlarm()));
		contaBuzzer=0;
		conta2min=0;
		emit( freeze(TRUE));
	    }
	    else if(tipo==DI_SON)
	    {
		aumVolTimer = new QTimer (this,"timer aumenta volume sveglia");
		aumVolTimer -> start (3000,FALSE);
		connect(aumVolTimer,SIGNAL(timeout()),this,SLOT(aumVol()));
		conta2min=0;
		emit( freeze(TRUE));
	    }
	    else if (tipo==FRAME)
	    {
		qDebug("mando la frame: %s", frame);
		emit(sendFrame(frame));		
	    }

	    qDebug("PARTE LA SVEGLIA");
	    onceToGest=FALSE;

	}
    }    
    minuTimer->start((60-actualDateTime.time().second())*1000);
}


bool sveglia::getActivation()
{
//    qDebug("getActivation. svegliaAbil= %d",svegliaAbil);
    return(svegliaAbil);
}

void sveglia::aumVol()
{
    if (conta2min==0)
    {
	openwebnet msg_open;
	char    pippo[50];
	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*16*3*");
	sprintf(&pippo[6],"%d",sorgente);
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);    
	
	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#16*");
	sprintf(&pippo[strlen(pippo)],"%d",sorgente);
	strcat(pippo,"*#7*");
	sprintf(&pippo[strlen(pippo)],"%d",stazione);
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);
	    
	for (uchar idx=0;idx<AMPLI_NUM;idx++)
	{
	    if (volSveglia[idx]>0)
	    {		
		memset(pippo,'\000',sizeof(pippo));
		strcat(pippo,"*16*3*");
		sprintf(&pippo[6],"%02d",idx);
		strcat(pippo,"##");
		msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
		emit sendFrame(msg_open.frame_open);    
	    }
	}
	
    }
    
    conta2min++;
	
    if (conta2min<32)
    {
	for (uchar idx=0;idx<AMPLI_NUM;idx++)
	{
	    if (volSveglia[idx]>=conta2min)
	    {
		openwebnet msg_open;
		char    pippo[50];
		
		memset(pippo,'\000',sizeof(pippo));
		strcat(pippo,"*#16*");
		sprintf(&pippo[5],"%02d",idx);
		strcat(pippo,"*#1*");
		sprintf(&pippo[11],"%d",conta2min);
		strcat(pippo,"##");
		msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
		emit sendFrame(msg_open.frame_open);    
	    }
	}
    }
    else if (conta2min>40)
    {
	 qDebug("SPENGO LA SVEGLIA per timeout");
	aumVolTimer->stop();
	delete (aumVolTimer);
	
	// manda general OFF
	openwebnet msg_open;
	char    pippo[50];
     
	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*16*13*0##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);    
    }
    
}

void sveglia::buzzerAlarm()
{
    if (contaBuzzer==0)
    {
	buzAbilOld=getBeep();
	setBeep(TRUE,FALSE);
    }	
    if  (contaBuzzer%2==0) 
    {
	if ( ((contaBuzzer+2)%12) && (contaBuzzer%12) )
	{
	    beep(10);
	}
    }
    contaBuzzer++;
    if (contaBuzzer>=10*60*2)
    {
	qDebug("SPENGO LA SVEGLIA");
	aumVolTimer->stop();
	setBeep(buzAbilOld,FALSE);
	delete (aumVolTimer);
	aumVolTimer=NULL;	
	emit(freeze(FALSE));
    }
}

void sveglia::spegniSveglia(bool b)
{
    if ( (!b) && (aumVolTimer) )
    {
	if (aumVolTimer->isActive()) 
	{
	    qDebug("SPENGO LA SVEGLIA");
	    aumVolTimer->stop();
	    setBeep(buzAbilOld,FALSE);
	    delete (aumVolTimer);
	    aumVolTimer=NULL;	   
	}
    }
}

void sveglia::setSerNum(int s){serNum=s;}




void sveglia::inizializza()
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    if (tipo==DI_SON)
    {
	int eeprom;
	char chiave[6];
	
	memset(&chiave[0],'\000',sizeof(chiave));
	eeprom = open("/dev/nvram", O_RDWR | O_SYNC, 0666);
	lseek(eeprom, BASE_EEPROM+(serNum-1)*(AMPLI_NUM+KEY_LENGTH+SORG_PAR),SEEK_SET );
	read(eeprom, &chiave[0], 5);
	
	if (strcmp(&chiave[0],AL_KEY))
	{
	    lseek(eeprom, BASE_EEPROM+(serNum-1)*(AMPLI_NUM+KEY_LENGTH+SORG_PAR), SEEK_SET);
	    write(eeprom,AL_KEY,5);
	    for(unsigned int idx=0; idx<AMPLI_NUM;idx++)
	    {
		write(eeprom,"\000",1 );
		volSveglia[idx]=0;
	    }
	}
	else
	{
	    int ploffete=BASE_EEPROM+(serNum-1)*(AMPLI_NUM+KEY_LENGTH+SORG_PAR)+KEY_LENGTH;
	    lseek(eeprom,ploffete, SEEK_SET);
	    for(unsigned int idx=0; idx<AMPLI_NUM;idx++)
	    {
		read(eeprom,&volSveglia[idx],1 );
		volSveglia[idx]&=0x1F;
//		qDebug("%d : %d", idx, volSveglia[idx]);
	    }
	    read(eeprom,&sorgente,1 );
	    read(eeprom,&stazione,1 );
	}
	::close(eeprom);    // servono i :: se no fa la close() di QWidget
    }
#endif
}
