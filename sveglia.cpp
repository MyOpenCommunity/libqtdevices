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
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <stdlib.h>
#include <qwidget.h>
#include <qcursor.h>
#include <qdatetime.h>
#include <qprocess.h>

#include <qfile.h>

#include "sveglia.h"
#include "genericfunz.h"
 
sveglia::sveglia( QWidget *parent, const char *name ,uchar freq, uchar t, diffSonora* diso, char* f)
        : QFrame( parent, name )
{
#if defined(BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
 //  showFullScreen();
#endif    
   bannNavigazione  = new bannFrecce(this,"bannerfrecce",9);
   bannNavigazione  ->setGeometry( 0 , MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE ,MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE );
   
   char iconName[MAX_PATH];
   QPixmap* Icon1 = new QPixmap();
   QPixmap* Icon2 = NULL;
	 
    setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT); 
    memset(iconName,'\000',sizeof(iconName));
    strcpy(iconName,ICON_FRECCIA_SU);
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
    
     delete(Icon1);
     Icon1 = new QPixmap();
     Icon1->load(ICON_SVEGLIA_ON);    
       
     Immagine = new QLabel(this, "immaginetta superiore");
    
     if (Icon1)
	 Immagine -> setPixmap(*Icon1); 
    
     
    Immagine->setGeometry(80,0,80,80);
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
	choice[idx]  -> setPixmap(*Icon1);
	if (Icon2)
	    choice[idx]  -> setPressedPixmap(*Icon2);
	choice[idx]  -> hide();
	
	testiChoice[idx] = new QLabel(this,"choiceLabel"+QString::number(idx));
	testiChoice[idx] -> setGeometry( 80,idx*60,120,60);
	testiChoice[idx] -> setAlignment(AlignHCenter|AlignVCenter);
	testiChoice[idx] -> setFont( QFont( "helvetica", 14, QFont::Bold ) );
	testiChoice[idx] -> hide();
    }	

    dataOra=NULL;
    
    testiChoice[0]  -> setText("UNA VOLTA");
    testiChoice[1]  -> setText("SEMPRE");
    testiChoice[2]  -> setText("LUN-VEN");
    testiChoice[3]  -> setText("SAB-DOM");        
    
   // OroTemp = QDateTime(QDateTime::currentDateTime());
     oraSveglia =  new QDateTime(QDate(),QTime(12,0));
    
     dataOra = new timeScript(this,"scrittaHomePage",2,oraSveglia);
     
     dataOra->setGeometry(40,140,160,50);
     dataOra->setFrameStyle( QFrame::Plain );
     dataOra->setLineWidth(0);    
     
     difson=diso;
    if (difson)
	difson->hide();
    
  /*  connect( but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumOra()));
    connect( but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumMin()));  
    connect( but[2] ,SIGNAL(clicked()),dataOra,SLOT(diminOra()));
    connect( but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminMin()));
  
    connect(bannNavigazione  ,SIGNAL(backClick()),this,SLOT(Closed()));
    connect(bannNavigazione  ,SIGNAL(backClick()),this,SLOT(okTime()));
    connect(bannNavigazione  ,SIGNAL(forwardClick()),this,SLOT(okTime()));
    connect(difson,SIGNAL(Closed()),this,SLOT(Closed()));
    connect(choice[0],SIGNAL(toggled(bool)),this,SLOT(sel1(bool)));
    connect(choice[1],SIGNAL(toggled(bool)),this,SLOT(sel2(bool)));
    connect(choice[2],SIGNAL(toggled(bool)),this,SLOT(sel3(bool)));
    connect(choice[3],SIGNAL(toggled(bool)),this,SLOT(sel4(bool)));   
*/
    for (uchar idx=0;idx<AMPLI_NUM;idx++)
	volSveglia[idx]=0;
    minuTimer=NULL;
    tipoSveglia=freq ;
    tipo=t;
    if (tipo==FRAME)
    {
	frame = new char[50];
	memset(frame,'\000',sizeof(frame));
	sprintf(frame,"%s",f);
	qDebug("f= %s",f);
	qDebug("frame= %s",frame);
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
 /*  if (buzzer)
    {
	delete (bannNavigazione);
	bannNavigazione  = new bannFrecce(this,"bannerfrecce",10);
	bannNavigazione  ->setGeometry( 0 , MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE ,MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE );  
	bannNavigazione->setFGColor(foregroundColor());
	bannNavigazione->setBGColor(backgroundColor());
    }*/
    
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
    
    delete(oraSveglia);
    oraSveglia = new QDateTime(dataOra->getDataOra());
	
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
    connect( but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumOra()));
    connect( but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumMin()));
    connect( but[2] ,SIGNAL(clicked()),dataOra,SLOT(diminOra()));
    connect( but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminMin()));
    connect(bannNavigazione  ,SIGNAL(forwardClick()),this,SLOT(okTime()));
    disconnect( bannNavigazione ,SIGNAL(forwardClick()),this,SLOT(okTipo()));    
    disconnect( bannNavigazione ,SIGNAL(forwardClick()),this,SLOT(Closed()));    

    connect(bannNavigazione  ,SIGNAL(backClick()),this,SLOT(Closed()));
    connect(bannNavigazione  ,SIGNAL(backClick()),this,SLOT(okTime()));
    connect(difson,SIGNAL(Closed()),this,SLOT(Closed()));
    connect(choice[0],SIGNAL(toggled(bool)),this,SLOT(sel1(bool)));
    connect(choice[1],SIGNAL(toggled(bool)),this,SLOT(sel2(bool)));
    connect(choice[2],SIGNAL(toggled(bool)),this,SLOT(sel3(bool)));
    connect(choice[3],SIGNAL(toggled(bool)),this,SLOT(sel4(bool)));   
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
    //imposta la sveglia in 
    if (difson)
    {
	difson->setNumRighe((uchar)4);
	difson->setGeom(0,0,240,320);
	difson->setNavBarMode(3);
	difson->reparent((QWidget*)NULL,0,QPoint(0,0),(bool)FALSE);
	difson->hide();
    }
    hide();
 /*   delete (bannNavigazione);
   bannNavigazione  = new bannFrecce(this,"bannerfrecce",9);
   bannNavigazione  ->setGeometry( 0 , MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE ,MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE );*/
   
    gesFrameAbil=FALSE;
/*    delete(oraSveglia);
    oraSveglia=new QDateTime();*/
    activateSveglia(TRUE);
//    qDebug("sveglia::Closed()");
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
    else
    {
	if (difson)
	{
	    difson->setNumRighe((uchar)3);
	    difson->setGeom(0,0,240,240);	
	    difson->setNavBarMode(6);
	    difson->reparent((QWidget*)this,(int)0,QPoint(0,80),(bool)FALSE);
	    difson->show();
	    
	    bannNavigazione->hide();
	}
	gesFrameAbil=TRUE;
	sorgente=101;
	stazione=0;	    
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
    
    QDateTime actualDateTime = QDateTime(QDateTime::currentDateTime());
    
  
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
    qDebug("getActivation. svegliaAbil= %d",svegliaAbil);
    return(svegliaAbil);
}

void sveglia::aumVol()
{
    if (conta2min==0)
    {
	openwebnet msg_open;
	char    pippo[50];
	for (uchar idx=0;idx<AMPLI_NUM;idx++)
	{
	    if (volSveglia[idx]>0)
	    {
		
		memset(pippo,'\000',sizeof(pippo));
		strcat(pippo,"*16*3*");
		sprintf(&pippo[6],"%d",idx);
		strcat(pippo,"##");
		msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
		emit sendFrame(msg_open.frame_open);    
	    }
	}
	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*16*3*");
	sprintf(&pippo[6],"%d",sorgente);
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);    
	
	if (stazione)
	{
	    memset(pippo,'\000',sizeof(pippo));
	    strcat(pippo,"*#16*");
	    sprintf(&pippo[strlen(pippo)],"%d",sorgente);
	    strcat(pippo,"*#7*");
	    sprintf(&pippo[strlen(pippo)],"%d",stazione);
	    strcat(pippo,"##");
	    msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	    emit sendFrame(msg_open.frame_open);
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
		sprintf(&pippo[5],"%d",idx);
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
	 qDebug("SPENGO LA SVEGLIA");
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
//    qDebug("buzzerAlarm");
    if  (contaBuzzer%2==0) 
    {
 //   qDebug("DIV x 2");
	if (contaBuzzer%10) 
	{
	    qApp->beep();
	//    qDebug("BEEP");
	}
    }
    contaBuzzer++;
    if (contaBuzzer>=10*60*2)
    {
	qDebug("SPENGO LA SVEGLIA");
	aumVolTimer->stop();
	delete (aumVolTimer);
    }
}

void sveglia::spegniSveglia()
{
	aumVolTimer->stop();
	delete (aumVolTimer);
}

