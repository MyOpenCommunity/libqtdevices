/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**antintrusione.cpp
**
**Sottomenù antiintrusione
**
****************************************************************/

#include "antintrusione.h"
extern unsigned char tipoData;

antintrusione::antintrusione( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
//    showFullScreen();
#endif     
   numRighe=NUM_RIGHE;  
   zone = new sottoMenu(this,"Zone",3,MAX_WIDTH, MAX_HEIGHT-MAX_HEIGHT/numRighe,/*numRighe-*/2); 
   impianto = new sottoMenu(this,"impianto",0,MAX_WIDTH, MAX_HEIGHT/numRighe,1); 
   allarmi.clear();
   allarmi.setAutoDelete(true);
    setGeom(0,0,MAX_WIDTH,MAX_HEIGHT);
    connect(zone  ,SIGNAL(Closed()),this,SIGNAL(Closed()));
    
    connect(this,SIGNAL(gestFrame(char *)),zone,SIGNAL(gestFrame(char *)));
    connect(this,SIGNAL(gestFrame(char *)),impianto,SIGNAL(gestFrame(char *)));
    connect(zone,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*))); 
    connect(impianto,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*)));
#if 0
    connect(impianto,SIGNAL(sendFramew(char*)), this, SIGNAL(sendFramew(char*)));
#endif
    connect(this, SIGNAL(openAckRx()), impianto, SIGNAL(openAckRx()));
    connect(this, SIGNAL(openNakRx()), impianto, SIGNAL(openNakRx()));
//    connect(allarmi,SIGNAL(Closed()),this , SLOT(show())); 	 
    
#if 0
    connect(allarmi,SIGNAL(Closed()),allarmi , SLOT(hide())); 	     
    connect(allarmi,SIGNAL(Closed()),zone, SLOT(show())); 	     
    connect(allarmi,SIGNAL(Closed()),impianto , SLOT(show())); 	         
    connect(impianto, SIGNAL(goDx()), allarmi,SLOT(show())); 	 
#else
    connect(impianto, SIGNAL(goDx()), this, SLOT(showAlarms()));
#endif
//    connect(impianto, SIGNAL(goDx()), this,SLOT(hide())); 	     
    
    connect(this,SIGNAL(freezed(bool)),zone,SLOT(freezed(bool)));
    connect(this,SIGNAL(freezed(bool)),impianto,SLOT(freezed(bool)));        
#if 0
    connect(this,SIGNAL(freezed(bool)),allarmi,SLOT(freezed(bool)));           
    connect(allarmi,SIGNAL(itemKilled()),this,SLOT(testranpo()));
#endif

}

void antintrusione::testranpo()
{
    QTimer *t=new  QTimer(this,"T");
    connect(t,SIGNAL(timeout()),this,SLOT( ctrlAllarm()));
    t->start(150,TRUE);
}

void antintrusione:: ctrlAllarm()
{
    qDebug("ctrlAllarm %d", allarmi.count());
    if (!allarmi.isEmpty())
	impianto->getLast()->mostra(banner::BUT1);
    else
        impianto->getLast()->nascondi(banner::BUT1);
    impianto->show();
    zone->show();
}

void antintrusione::setBGColor(int r, int g, int b)
{	
    setPaletteBackgroundColor( QColor :: QColor(r,g,b));    
    if (impianto)
	impianto-> setBGColor(backgroundColor() );
    if (zone)
	zone-> setBGColor(backgroundColor() );
}

void antintrusione::setFGColor(int r, int g, int b)
{
    setPaletteForegroundColor( QColor :: QColor(r,g,b));
    if (impianto)
	impianto-> setFGColor(foregroundColor() );
    if (zone)    
	zone-> setFGColor(foregroundColor() );
}


void antintrusione::draw()
{	
    ctrlAllarm();
    if (impianto)
	impianto-> draw();
    if (zone)
	zone-> draw();
    if(allarmi.isEmpty()) return;
    QPtrListIterator<allarme> *ai = new QPtrListIterator<allarme>(allarmi);
    ai->toFirst();
    allarme *a;
    while((a = ai->current())) {
	a->draw();
	++(*ai);
    }
}


int antintrusione::setBGPixmap(char* backImage)
{
    QPixmap Back; 
     if(Back.load(backImage))
    {
	 setPaletteBackgroundPixmap(Back);      
	 return (0);
    }    
     return (1);
}


int antintrusione::addItem(char tipo, char* descrizione, void* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *pressedIcon,int periodo, int numFrame, char* txt_tecnico, char* txt_intrusione, char* txt_manomissione, char* txt_panic)
 {        
    if (tipo== IMPIANTINTRUS)
    {
	impianto -> addItem(tipo, descrizione,indirizzo,  IconaSx, IconaDx, icon, pressedIcon);
#if 0
	connect(impianto->getLast(),SIGNAL(impiantoInserito()),allarmi,SLOT(svuota(bool)));
#else
	// FIXME: SVUOTA LISTA ALLARMI
#endif
	connect(impianto->getLast(), SIGNAL(abilitaParz(bool)),
		this, SIGNAL(abilitaParz(bool)));
	connect(impianto->getLast(), SIGNAL(clearChanged()),
		this, SIGNAL(clearChanged()));
	connect(this, SIGNAL(partChanged(zonaAnti*)), impianto->getLast(),
		SLOT(partChanged(zonaAnti*)));
	connect(impianto, SIGNAL(openAckRx()), impianto->getLast(),
		SLOT(openAckRx()));
	connect(impianto, SIGNAL(openNakRx()), impianto->getLast(),
		SLOT(openNakRx()));
	memset(&testoManom[0],'\000',MAX_PATH);
	memset(&testoTecnico[0],'\000',MAX_PATH);
	memset(&testoIntrusione[0],'\000',MAX_PATH);
	memset(&testoPanic[0],'\000',MAX_PATH);  
	if (txt_manomissione)
	    strncpy(&testoManom[0],txt_manomissione,MAX_PATH);
	if (txt_tecnico)
	    strncpy(&testoTecnico[0],txt_tecnico,MAX_PATH);
	if (txt_intrusione)
	    strncpy(&testoIntrusione[0],txt_intrusione,MAX_PATH);
	if (txt_panic)
	    strncpy(&testoPanic[0],txt_panic,MAX_PATH);
    }
    else if (tipo== ZONANTINTRUS) {
	zone->addItem(tipo , descrizione , indirizzo ,IconaSx,IconaDx, icon, 
		      pressedIcon);
	connect(this, SIGNAL(abilitaParz(bool)), zone->getLast(), 
		SLOT(abilitaParz(bool)));
	connect(this, SIGNAL(clearChanged()), zone->getLast(),
		SLOT(clearChanged()));
	connect(zone->getLast(), SIGNAL(partChanged(zonaAnti*)),
		this, SIGNAL(partChanged(zonaAnti*)));
	// Alhtough looking at the source one would say that more than 
	// one "impianto" could be configured, in real life only one 
	// impianto can exist
	((impAnti *)impianto->getLast())->setZona((zonaAnti *)zone->getLast());
    }
    return(1);    
 }

void antintrusione::setNumRighe(uchar n)
{
    numRighe=n;
    zone->setNumRighe(n-1);
    impianto->setNumRighe(1);
    zone->draw();
    impianto->draw();
}


void antintrusione::inizializza()
{     
    zone-> inizializza();
    impianto->inizializza();
//     emit sendFrame("*16*53*100##");      frame per richiesta stato impianto
}


void antintrusione::gesFrame(char*frame)
{	
    emit gestFrame(frame);
    openwebnet msg_open;
    char aggiorna;
    
    aggiorna=0;
      
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
   
    if (!strcmp(msg_open.Extract_chi(),"5"))
    {
	if ( (! strncmp(msg_open.Extract_cosa(),"12",2) ) || (! strncmp(msg_open.Extract_cosa(),"15",2) ) || \
	     (! strncmp(msg_open.Extract_cosa(),"16",2) ) || (! strncmp(msg_open.Extract_cosa(),"17",2) ) )     
	{
            
	    char descr[2*MAX_PATH],time[MAX_PATH];
	    char zona[3];       
	    char *tipo = "Z";
	    allarme::altype t;
        
        memset(&descr[0],'\000',2*MAX_PATH);
        
            if  ( (! strncmp(msg_open.Extract_cosa(),"12",2)) && 
		  (testoTecnico[0]) ) {
                strncpy(&descr[0],&testoTecnico[0],MAX_PATH);
		t = allarme::TECNICO;
		tipo = "AUX";
	    }
	    if  ( (! strncmp(msg_open.Extract_cosa(),"15",2)) && 
		  (testoIntrusione[0]) ) {
                strncpy(&descr[0],&testoIntrusione[0],MAX_PATH);
		t = allarme::INTRUSIONE;
	    }
	    if  ( (! strncmp(msg_open.Extract_cosa(),"16",2)) && 
		  (testoManom[0]) ) {
                strncpy(&descr[0],&testoManom[0],MAX_PATH);
		t = allarme::MANOMISSIONE;
	    }
	    if  ( (! strncmp(msg_open.Extract_cosa(),"17",2)) && 
		  (testoPanic[0]) ) {
                strncpy(&descr[0],&testoPanic[0],MAX_PATH);
		t = allarme::PANIC;
	    }
	    strcpy(&zona[0],msg_open.Extract_dove());
	    sprintf(&time[0], "\n%s   %s    %s %s", 
		    QDateTime::currentDateTime().toString("hh:mm").ascii(),
		    QDateTime::currentDateTime().toString("dd.MM").ascii(), 
		    tipo, &zona[1]);	
        strncat(&descr[0],&time[0],MAX_PATH);
#if 0
        allarmi->addItem(ALLARME, &descr[0], NULL, ICON_DEL);
#else
	allarmi.append(new allarme(NULL, descr, NULL, ICON_DEL, t));
	curr_alarm = allarmi.getLast();
	curr_alarm->setFGColor(foregroundColor());
	curr_alarm->setBGColor(backgroundColor());
	connect(curr_alarm, SIGNAL(Back()), this, SLOT(closeAlarms()));
	connect(curr_alarm, SIGNAL(Next()), this, SLOT(nextAlarm()));
	connect(curr_alarm, SIGNAL(Prev()), this, SLOT(prevAlarm()));
	connect(curr_alarm, SIGNAL(Delete()), this, SLOT(deleteAlarm()));
	connect(this, SIGNAL(freezed(bool)), curr_alarm, SLOT(freezed(bool)));
#endif
        aggiorna=1;
	}
	}
    if (aggiorna)
    {
    qDebug("ARRIVATO ALLARME!!!!");
	allarmi.getLast()->show();
        impianto->hide();
        zone->hide();
        //this->showFullScreen();
	hide();
        ctrlAllarm();
    }
}




void antintrusione::setGeom(int x,int y,int w,int h)
{
      QWidget::setGeometry(x,y,w,h);
      if (impianto)
	impianto->setGeometry(x,y,w,h/numRighe);
    if(zone)
	zone->setGeometry(x,h/numRighe,w,h/numRighe*(numRighe-1));
#if 0
    if (allarmi)
	allarmi->setGeometry(x,y,w,h);
#else
    // Nothing here
#endif
}

void antintrusione::setNavBarMode(uchar c)
{
    zone->setNavBarMode(c);
}

void antintrusione::nextAlarm()
{
    qDebug("antiintrusione::nextAlarm()");
    curr_alarm->hide();
    curr_alarm = allarmi.next();
    if(!curr_alarm) {
	qDebug("fine");
	curr_alarm = allarmi.first();
    }
    curr_alarm->show();
}

void antintrusione::prevAlarm()
{
    qDebug("antiintrusione::prevAlarm()");
    curr_alarm->hide();
    curr_alarm = allarmi.prev();
    if(!curr_alarm)
	curr_alarm = allarmi.last();
    curr_alarm->show();
}

void antintrusione::deleteAlarm()
{
    qDebug("antiintrusione::deleteAlarm()");
    // Enough, autodelete is set
    curr_alarm->hide();
    allarmi.remove(curr_alarm);
    if(allarmi.isEmpty()) {
	curr_alarm = NULL;
	//closeAlarms();
	testranpo();
	return;
    }
    curr_alarm = allarmi.getLast();
    curr_alarm->show();
}

void antintrusione::closeAlarms()
{
    qDebug("antiintrusione::closeAlarms()");
    if(curr_alarm)
	curr_alarm->hide();
    impianto->show();
    zone->show();
}

void antintrusione::showAlarms()
{
    qDebug("antiintrusione::showAlarms()");
    curr_alarm = allarmi.getLast();
    if(!curr_alarm) return;
    impianto->hide();
    zone->hide();
    curr_alarm->show();
}
