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

 
tastiera::tastiera( QWidget *parent, const char *name, int line )
        : QWidget( parent, name )
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
#endif
    setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);      
    setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

    
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


    unoBut->setGeometry(POSX1,line*0,BUT_DIM,BUT_DIM);
    dueBut->setGeometry(POSX2,line*0,BUT_DIM,BUT_DIM);
    treBut->setGeometry(POSX3,line*0,BUT_DIM,BUT_DIM);
    quatBut->setGeometry(POSX1,line*1,BUT_DIM,BUT_DIM);
    cinBut->setGeometry(POSX2,line*1,BUT_DIM,BUT_DIM);
    seiBut->setGeometry(POSX3,line*1,BUT_DIM,BUT_DIM);
    setBut->setGeometry(POSX1,line*2,BUT_DIM,BUT_DIM);
    ottBut->setGeometry(POSX2,line*2,BUT_DIM,BUT_DIM);
    novBut->setGeometry(POSX3,line*2,BUT_DIM,BUT_DIM);
    zeroBut->setGeometry(POSX2,line*3,BUT_DIM,BUT_DIM);
    okBut->setGeometry(POSX3,line*3,BUT_DIM,BUT_DIM);        
    cancBut->setGeometry(POSX1, line*3,BUT_DIM,BUT_DIM);    
    scrittaLabel->setGeometry(0,line*4,MAX_WIDTH/2,line);	       
    digitLabel->setGeometry(MAX_WIDTH/2,line*4,MAX_WIDTH/2,line);    

 
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
    
    Icon->load(ICON_OK);
    getPressName((char*)ICON_OK, &pressIconName[0],sizeof(pressIconName));    
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
    qDebug("tastiera::setBGColor");
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
    qDebug("tastiera::setFGColor");
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
     scrittaLabel->setFont( QFont( DEFAULT_FONT, 14, QFont::Bold ) );
     scrittaLabel->setText("PASSWORD:"); // FIXME tradurre????
     digitLabel->setAlignment(AlignLeft|AlignVCenter);
     digitLabel->setFont( QFont( DEFAULT_FONT, 20, QFont::Bold ) );
     qDebug("tastiera::draw(), mode = %d", mode);
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


// tastiera_con_stati implementation
tastiera_con_stati::tastiera_con_stati(int s[8], 
				       QWidget *parent, const char *name) : 
    tastiera(parent, name, MAX_HEIGHT/6)
{
    int i, x;
    char tmp[2] = "1";
    for(i=0, x=POSX1_SMALL; i<8; i++, x+=BUT_SMALL_DIM) {
	// Create button
	stati[i] = new BtButton(this, "bottone stato");	
	stati[i]->setEnabled(0);
	stati[i]->setFont( QFont( DEFAULT_FONT, 16, QFont::Bold ) );
	if(s[i] == -1)
	{
	    stati[i]->setText("-");
	    st[i] = false;
	}
	else
	{
	    stati[i]->setText(tmp);
	    st[i] = s[i];
	}
	//stati[i]->setAlignment(AlignHCenter|AlignVCenter);
	tmp[0]++;
	stati[i]->setGeometry(x, (MAX_HEIGHT/6)*4 + MAX_HEIGHT/12, 
			      BUT_SMALL_DIM,  
			      BUT_SMALL_DIM);
	stati[i]->show();
    }

    scrittaLabel->setGeometry(0,(MAX_HEIGHT/6)*5,MAX_WIDTH/2,(MAX_HEIGHT/6));	       
    digitLabel->setGeometry(MAX_WIDTH/2,(MAX_HEIGHT/6)*5,MAX_WIDTH/2,
			    MAX_HEIGHT/6); 
    scrittaLabel->show();       
    digitLabel->show();   
 }

void tastiera_con_stati::setBGColor(QColor c)
{
    int i;
    qDebug("tastiera_con_stati::setBGColor");
    for(i=0; i<8; i++)
	stati[i]->setPaletteBackgroundColor(c);
    tastiera::setBGColor(c);
}

void tastiera_con_stati::setFGColor(QColor c)
{
    int i;
    qDebug("tastiera_con_stati::setFGColor");
    for(i=0; i<8; i++)
	stati[i]->setPaletteForegroundColor(c);
    tastiera::setFGColor(c);
}

void tastiera_con_stati::show()
{
    for(int i=0; i<8; i++) {
	if(st[i]) {
	    QColor fc = stati[i]->paletteForegroundColor();
	    stati[i]->setPaletteForegroundColor
		(
		    stati[i]->paletteBackgroundColor()
		    );
	    stati[i]->setPaletteBackgroundColor(fc);
	}
    }
    QWidget::show();
}
