/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**tastiera.cpp
**
**finestra di tastiera numerica
**
****************************************************************/


#include <qfont.h>
#include <qlayout.h>
#include <stdlib.h>
#include <qwidget.h>
#include <qframe.h>
#include <qprocess.h>
#include <qstring.h>


#include "tastiera.h"
#include "banner.h"
#include "main.h"

 
tastiera::tastiera( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
#if defined(BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
    showFullScreen();
#endif
    setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);      


    
    unoBut = new BtButton(this,"Bottone uno");
    dueBut = new BtButton(this,"Bottone due");
    treBut = new BtButton(this,"Bottone tre");
    quatBut = new BtButton(this,"Bottone quattro");
    cinBut = new BtButton(this,"Bottone cinque");
    seiBut = new BtButton(this,"Bottone uno");
    setBut = new BtButton(this,"Bottone due");
    ottBut = new BtButton(this,"Bottone tre");
    novBut = new BtButton(this,"Bottone quattro");
    zeroBut = new BtButton(this,"Bottone cinque");
    okBut = new BtButton(this,"Bottone di canc");
    cancBut = new BtButton(this,"Bottone di canc");
    
    digitLabel = new BtLabel(this,"");
    scrittaLabel = new BtLabel(this,"PASSWORD:");


    unoBut->setGeometry(POSX1,LINE*0,BUT_DIM,BUT_DIM);
    dueBut->setGeometry(POSX2,LINE*0,BUT_DIM,BUT_DIM);
    treBut->setGeometry(POSX3,LINE*0,BUT_DIM,BUT_DIM);
    quatBut->setGeometry(POSX1,LINE*1,BUT_DIM,BUT_DIM);
    cinBut->setGeometry(POSX2,LINE*1,BUT_DIM,BUT_DIM);
    seiBut->setGeometry(POSX3,LINE*1,BUT_DIM,BUT_DIM);
    setBut->setGeometry(POSX1,LINE*2,BUT_DIM,BUT_DIM);
    ottBut->setGeometry(POSX2,LINE*2,BUT_DIM,BUT_DIM);
    novBut->setGeometry(POSX3,LINE*2,BUT_DIM,BUT_DIM);
    zeroBut->setGeometry(POSX2,LINE*3,BUT_DIM,BUT_DIM);
    okBut->setGeometry(POSX3,LINE*3,BUT_DIM,BUT_DIM);        
    cancBut->setGeometry(POSX1, LINE*3,BUT_DIM,BUT_DIM);    
    scrittaLabel->setGeometry(0,LINE*4,MAX_WIDTH/2,LINE);	       
    digitLabel->setGeometry(MAX_WIDTH/2,LINE*4,MAX_WIDTH/2,LINE);    

 
    QPixmap *Icon, *pressIcon;
    char   pressIconName[MAX_PATH];  
    
    Icon = new QPixmap();
    pressIcon = new QPixmap();
    
    Icon->load(ICON_UNO);
    getPressName((char*)ICON_UNO, &pressIconName[0],sizeof(pressIconName));    
    unoBut->setPixmap(*Icon);
    
    if (QFile::exists(pressIconName))
    {
	pressIcon->load(pressIconName);
	unoBut->setPressedPixmap(*pressIcon); 
    }
    
    Icon->load(ICON_DUE);
    getPressName((char*)ICON_DUE, &pressIconName[0],sizeof(pressIconName));    
    dueBut->setPixmap(*Icon);
    
    if (QFile::exists(pressIconName))
    {
	pressIcon->load(pressIconName);
	dueBut->setPressedPixmap(*pressIcon); 
    }
    
    Icon->load(ICON_TRE);
    getPressName((char*)ICON_TRE, &pressIconName[0],sizeof(pressIconName));    
    treBut->setPixmap(*Icon);
    
    if (QFile::exists(pressIconName))
    {
	pressIcon->load(pressIconName);
	treBut->setPressedPixmap(*pressIcon); 
    }
    
    Icon->load(ICON_QUATTRO);
    getPressName((char*)ICON_QUATTRO, &pressIconName[0],sizeof(pressIconName));    
    quatBut->setPixmap(*Icon);
    
    if (QFile::exists(pressIconName))
    {
	pressIcon->load(pressIconName);
	quatBut->setPressedPixmap(*pressIcon); 
    }
    
    Icon->load(ICON_CINQUE);
    getPressName((char*)ICON_CINQUE, &pressIconName[0],sizeof(pressIconName));    
    cinBut->setPixmap(*Icon);
    
    if (QFile::exists(pressIconName))
    {
	pressIcon->load(pressIconName);
	cinBut->setPressedPixmap(*pressIcon); 
    }
    
    Icon->load(ICON_SEI);
    getPressName((char*)ICON_SEI, &pressIconName[0],sizeof(pressIconName));    
    seiBut->setPixmap(*Icon);
    
    if (QFile::exists(pressIconName))
    {
	pressIcon->load(pressIconName);
	seiBut->setPressedPixmap(*pressIcon); 
    }
    
    Icon->load(ICON_SETTE);
    getPressName((char*)ICON_SETTE, &pressIconName[0],sizeof(pressIconName));    
    setBut->setPixmap(*Icon);
    
    if (QFile::exists(pressIconName))
    {
	pressIcon->load(pressIconName);
	setBut->setPressedPixmap(*pressIcon); 
    }
    
    Icon->load(ICON_OTTO);
    getPressName((char*)ICON_OTTO, &pressIconName[0],sizeof(pressIconName));    
    ottBut->setPixmap(*Icon);
    
    if (QFile::exists(pressIconName))
    {
	pressIcon->load(pressIconName);
	ottBut->setPressedPixmap(*pressIcon); 
    }

    Icon->load(ICON_NOVE);
    getPressName((char*)ICON_NOVE, &pressIconName[0],sizeof(pressIconName));    
    novBut->setPixmap(*Icon);    
    if (QFile::exists(pressIconName))
    {
	pressIcon->load(pressIconName);
	novBut->setPressedPixmap(*pressIcon); 
    }
    
    Icon->load(ICON_ZERO);
    getPressName((char*)ICON_ZERO, &pressIconName[0],sizeof(pressIconName));    
    zeroBut->setPixmap(*Icon);    
    if (QFile::exists(pressIconName))
    {
	pressIcon->load(pressIconName);
	zeroBut->setPressedPixmap(*pressIcon); 
    }    
    
    Icon->load(ICON_OK_60);
    getPressName((char*)ICON_OK_60, &pressIconName[0],sizeof(pressIconName));    
    okBut->setPixmap(*Icon);    
    if (QFile::exists(pressIconName))
    {
	pressIcon->load(pressIconName);
	okBut->setPressedPixmap(*pressIcon); 
    }
    
     Icon->load(ICON_CANC);
    getPressName((char*)ICON_CANC, &pressIconName[0],sizeof(pressIconName));    
    cancBut->setPixmap(*Icon);    
    if (QFile::exists(pressIconName))
    {
	pressIcon->load(pressIconName);
	cancBut->setPressedPixmap(*pressIcon); 
    }
    memset(pwd,'\000', sizeof(pwd));
    mode=CLEAN;

    connect(unoBut,SIGNAL(clicked()),this,SLOT(press1()));
    connect(dueBut,SIGNAL(clicked()),this,SLOT(press2()));
    connect(treBut,SIGNAL(clicked()),this,SLOT(press3()));
    connect(quatBut,SIGNAL(clicked()),this,SLOT(press4()));
    connect(cinBut,SIGNAL(clicked()),this,SLOT(press5()));      
    connect(seiBut,SIGNAL(clicked()),this,SLOT(press6()));
    connect(setBut,SIGNAL(clicked()),this,SLOT(press7()));
    connect(ottBut,SIGNAL(clicked()),this,SLOT(press8()));
    connect(novBut,SIGNAL(clicked()),this,SLOT(press9()));
    connect(zeroBut,SIGNAL(clicked()),this,SLOT(press0()));          
    connect(cancBut,SIGNAL(clicked()),this,SLOT(canc()));
    connect(okBut,SIGNAL(clicked()),this,SLOT(ok()));    
    
     unoBut->show();
    dueBut->show();
    treBut->show();
    quatBut->show();
    cinBut->show();
    seiBut->show();
    setBut->show();
    ottBut->show();
    novBut->show();
    zeroBut->show();
    okBut->show();       
    cancBut->show();   
    scrittaLabel->show();       
    digitLabel->show();   
 }
void tastiera::showTastiera()
{
    memset(&pwd[0],'\000',sizeof(&pwd[0]));
    draw();    
    show();
}

void tastiera::setBGColor(int r, int g, int b)
{	
    setBGColor( QColor :: QColor(r,g,b));    
}
void tastiera::setFGColor(int r, int g, int b)
{
    setFGColor( QColor :: QColor(r,g,b));
}

void tastiera::setBGColor(QColor c)
{	
    setPaletteBackgroundColor( c );    
 
    unoBut->setPaletteBackgroundColor(c);
    dueBut->setPaletteBackgroundColor(c);
    treBut->setPaletteBackgroundColor(c);
    quatBut->setPaletteBackgroundColor(c);
    cinBut->setPaletteBackgroundColor(c);        
    seiBut->setPaletteBackgroundColor(c);
    setBut->setPaletteBackgroundColor(c);
    ottBut->setPaletteBackgroundColor(c);
    novBut->setPaletteBackgroundColor(c);
    zeroBut->setPaletteBackgroundColor(c);    
    okBut->setPaletteBackgroundColor(c);   
    cancBut->setPaletteBackgroundColor(c);    
    digitLabel->setPaletteBackgroundColor( c );   
    scrittaLabel->setPaletteBackgroundColor( c );   
}
void tastiera::setFGColor(QColor c)
{
    setPaletteForegroundColor( c );

    unoBut->setPaletteForegroundColor(c);
    dueBut->setPaletteForegroundColor(c);
    treBut->setPaletteForegroundColor(c);
    quatBut->setPaletteForegroundColor(c);
    cinBut->setPaletteForegroundColor(c);    
    seiBut->setPaletteForegroundColor(c);
    setBut->setPaletteForegroundColor(c);
    ottBut->setPaletteForegroundColor(c);
    novBut->setPaletteForegroundColor(c);
    zeroBut->setPaletteForegroundColor(c);    
    okBut->setPaletteForegroundColor(c);        
    cancBut->setPaletteForegroundColor(c);    
    digitLabel->setPaletteForegroundColor( c );
    scrittaLabel->setPaletteForegroundColor( c );
}

int tastiera::setBGPixmap(char* backImage)
{
    QPixmap Back; 
     if(Back.load(backImage))
    {
	 setPaletteBackgroundPixmap(Back);      
	 return (0);
    }    
     return (1);
}





void tastiera::draw()
{
     scrittaLabel->setAlignment(AlignHCenter|AlignVCenter);
     scrittaLabel->setFont( QFont( "helvetica", 14, QFont::Bold ) );
     scrittaLabel->setText("PASSWORD:");
     digitLabel->setAlignment(AlignLeft|AlignVCenter);
     digitLabel->setFont( QFont( "helvetica", 20, QFont::Bold ) );
     if (mode==CLEAN)
	 digitLabel->setText(&pwd[0]);
     else
     {
	     char pw[10];
	     memset(pw,'\000',sizeof(pw));
	     memset(pw,'*',strlen(&pwd[0]));
	     digitLabel->setText(&pw[0]);
      }
}



void tastiera::press1()
{
    if (strlen(&pwd[0])<5)
	strcat(&pwd[0],"1");
    draw();
}
void tastiera::press2()
{
    if (strlen(&pwd[0])<5)
	strcat(&pwd[0],"2");
    draw();
}
void tastiera::press3()
{
    if (strlen(&pwd[0])<5)
	strcat(&pwd[0],"3");
    draw();
}
void tastiera::press4()
{
    if (strlen(&pwd[0])<5)
	strcat(&pwd[0],"4");
    draw();
}
void tastiera::press5()
{
    if (strlen(&pwd[0])<5)
	strcat(&pwd[0],"5");
    draw();
}
void tastiera::press6()
{
    if (strlen(&pwd[0])<5)
	strcat(&pwd[0],"6");
    draw();
}
void tastiera::press7()
{
    if (strlen(&pwd[0])<5)
	strcat(&pwd[0],"7");
    draw();
}
void tastiera::press8()
{
    if (strlen(&pwd[0])<5)
	strcat(&pwd[0],"8");
    draw();
}
void tastiera::press9()
{
    if (strlen(&pwd[0])<5)
	strcat(&pwd[0],"9");
    draw();
}
void tastiera::press0()
{
    if (strlen(&pwd[0])<5)
	strcat(&pwd[0],"0");
    draw();
}    
void tastiera::canc()
{
    if (strlen(&pwd[0])>0)
	pwd[strlen(&pwd[0])-1]='\000';
    else
    {
    	hide();
	emit (Closed(NULL));
    }
    draw();
}
void tastiera::ok()
{
    hide();
    emit(Closed(&pwd[0]));
}    

void tastiera::setMode(char m)	
{
    mode=m;
//    memset(&pwd[0],'\000',sizeof(pwd));
}


