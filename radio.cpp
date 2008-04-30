/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 **radio.cpp
 **
 **finestra di dati sulla radio
 **
 ****************************************************************/

#include <qlabel.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qframe.h>
#include <qdatetime.h>
#include <qlcdnumber.h>
#include <qcursor.h>
#include <qfile.h>

#include "radio.h"
#include "banner.h"
#include "bannondx.h"
#include "bannfrecce.h"
#include "main.h"
#include "btlabel.h"
#include "genericfunz.h"
#include "btbutton.h"
#include "fontmanager.h"

radio::radio( QWidget *parent, const char *name, const QString & amb )
: QWidget( parent, name )
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	setCursor (QCursor (blankCursor));
	//   showFullScreen();
#endif
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

	bannNavigazione  = new bannFrecce(this,"bannerfrecce",1);
	bannNavigazione  ->setGeometry( 0 ,MAX_HEIGHT- MAX_HEIGHT/NUM_RIGHE ,MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE );

	memoBut = new BtButton(this,"Bottone di memorizzazione");
	decBut = new BtButton(this,"Bottone di decFreq");
	aumBut = new BtButton(this,"Bottone di aumFreq");
	autoBut = new BtButton(this,"Bottone di atoSearch");
	manBut = new BtButton(this,"Bottone di manSearch");
	cicBut = new BtButton(this,"Bottone di cicStaz");
	unoBut = new BtButton(this,"Bottone uno");
	dueBut = new BtButton(this,"Bottone due");
	treBut = new BtButton(this,"Bottone tre");
	quatBut = new BtButton(this,"Bottone quattro");
	cinBut = new BtButton(this,"Bottone cinque");
	cancBut = new BtButton(this,"Bottone di canc");

	rdsLabel = new BtLabel(this,"Bottone di sinistra");
	radioName = new BtLabel(this,"Bottone di sinistra");
	ambDescr = new BtLabel(this, "descrizione ambiente");
	ambDescr->setAlignment(AlignHCenter|AlignTop);
 	QFont aFont;
	FontManager::instance()->getFont( font_radio_descrizione_ambiente, aFont );
	ambDescr->setFont( aFont );
	ambDescr->setText( amb );
	freq = new QLCDNumber(this,"pippo");
	progrText = new BtLabel(this,"progressivo stazione");
	freq->setSegmentStyle(QLCDNumber::Flat); 
	freq->setSmallDecimalPoint(TRUE);
	freq->setNumDigits(6);
	freq->setGeometry(60,40,180,60);    
	freq->setLineWidth(0);
	memoBut->setGeometry(60,190,120,60);
	decBut->setGeometry(0,130,60,60);
	aumBut->setGeometry(180,130,60,60);
	autoBut->setGeometry(60,130,60,60);
	manBut->setGeometry(120,130,60,60);
	cicBut->setGeometry(0,40,60,60);
	rdsLabel->setGeometry(0,100,240,30);
	unoBut->setGeometry(15,130,60,60);
	dueBut->setGeometry(90,130,60,60);
	treBut->setGeometry(165,130,60,60);
	quatBut->setGeometry(15,190,60,60);
	cinBut->setGeometry(90,190,60,60);
	cancBut->setGeometry(165,190,60,60);
	radioName->setGeometry(0,0,240,20);
	ambDescr->setGeometry(0,20,240,20);
	progrText->setGeometry(65,55,35,30);

	unoBut->hide();
	dueBut->hide();
	treBut->hide();
	quatBut->hide();
	cinBut->hide();
	cancBut->hide();

	QPixmap* Icon = new QPixmap();
	QPixmap* pressIcon = new QPixmap();
	char pressIconName[MAX_PATH];

	Icon->load(ICON_CICLA);
	getPressName((char*)ICON_CICLA, &pressIconName[0],sizeof(pressIconName));
	cicBut->setPixmap(*Icon);

	if (QFile::exists(pressIconName))
	{
		pressIcon->load(pressIconName);
		cicBut->setPressedPixmap(*pressIcon);
	}

	Icon->load(ICON_PIU);
	getPressName((char*)ICON_PIU, &pressIconName[0],sizeof(pressIconName));
	aumBut->setPixmap(*Icon);
	if (QFile::exists(pressIconName))
	{
		pressIcon->load(pressIconName);
		aumBut->setPressedPixmap(*pressIcon);
	}

	Icon->load(ICON_MENO);
	getPressName((char*)ICON_MENO, &pressIconName[0],sizeof(pressIconName));
	decBut->setPixmap(*Icon);
	if (QFile::exists(pressIconName))
	{
		pressIcon->load(pressIconName);
		decBut->setPressedPixmap(*pressIcon);
	}

	Icon->load(ICON_MEM);
	getPressName((char*)ICON_MEM, &pressIconName[0],sizeof(pressIconName));
	memoBut->setPixmap(*Icon);
	if (QFile::exists(pressIconName))
	{
		pressIcon->load(pressIconName);
		memoBut->setPressedPixmap(*pressIcon);
	}

	Icon->load(ICON_MANUAL_ON);
	getPressName((char*)ICON_MANUAL_ON, &pressIconName[0],sizeof(pressIconName));
	manBut->setPixmap(*Icon);
	if (QFile::exists(pressIconName))
	{
		pressIcon->load(pressIconName);
		manBut->setPressedPixmap(*pressIcon);
	}

	Icon->load(ICON_AUTO_ON);
	getPressName((char*)ICON_AUTO_ON, &pressIconName[0],sizeof(pressIconName));
	autoBut->setPixmap(*Icon);
	if (QFile::exists(pressIconName))
	{
		pressIcon->load(pressIconName);
		autoBut->setPressedPixmap(*pressIcon);
	}

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

	Icon->load(ICON_CANC);
	getPressName((char*)ICON_CANC, &pressIconName[0],sizeof(pressIconName));    
	cancBut->setPixmap(*Icon);

	if (QFile::exists(pressIconName))
	{
		pressIcon->load(pressIconName);
		cancBut->setPressedPixmap(*pressIcon); 
	}


	manual=FALSE;
	wasManual=TRUE;



	connect(bannNavigazione  ,SIGNAL(backClick()),this,SIGNAL(Closed()));
	connect(decBut,SIGNAL(clicked()),this,SIGNAL(decFreqAuto()));
	connect(aumBut,SIGNAL(clicked()),this,SIGNAL(aumFreqAuto()));
	connect(cicBut,SIGNAL(clicked()),this,SIGNAL(changeStaz()));

	connect(autoBut,SIGNAL(clicked()),this,SLOT(setAuto()));
	connect(manBut,SIGNAL(clicked()),this,SLOT(setMan()));	
	connect(memoBut,SIGNAL(clicked()),this,SLOT(cambiaContesto()));
	connect(cancBut,SIGNAL(clicked()),this,SLOT(ripristinaContesto()));
	connect(unoBut,SIGNAL(clicked()),this,SLOT(memo1()));
	connect(dueBut,SIGNAL(clicked()),this,SLOT(memo2()));
	connect(treBut,SIGNAL(clicked()),this,SLOT(memo3()));
	connect(quatBut,SIGNAL(clicked()),this,SLOT(memo4()));
	connect(cinBut,SIGNAL(clicked()),this,SLOT(memo5()));
}
void radio::showRadio()
{    
	draw();
	showFullScreen();
}

void radio::setBGColor(int r, int g, int b)
{	
	setBGColor( QColor :: QColor(r,g,b));    
}
void radio::setFGColor(int r, int g, int b)
{
	setFGColor( QColor :: QColor(r,g,b));
}

void radio::setBGColor(QColor c)
{	
	setPaletteBackgroundColor( c );    
	memoBut->setPaletteBackgroundColor(c);
	decBut->setPaletteBackgroundColor(c);
	aumBut->setPaletteBackgroundColor(c);
	autoBut->setPaletteBackgroundColor(c);
	manBut->setPaletteBackgroundColor(c);
	cicBut->setPaletteBackgroundColor(c);
	rdsLabel->setPaletteBackgroundColor(c);
	radioName->setPaletteBackgroundColor(c);
	ambDescr->setPaletteBackgroundColor(c);
	unoBut->setPaletteBackgroundColor(c);
	dueBut->setPaletteBackgroundColor(c);
	treBut->setPaletteBackgroundColor(c);
	quatBut->setPaletteBackgroundColor(c);
	cinBut->setPaletteBackgroundColor(c);    
	cancBut->setPaletteBackgroundColor(c);    
	bannNavigazione->setBGColor(c);    
}
void radio::setFGColor(QColor c)
{
	setPaletteForegroundColor( c );
	memoBut->setPaletteForegroundColor(c);
	decBut->setPaletteForegroundColor(c);
	aumBut->setPaletteForegroundColor(c);
	autoBut->setPaletteForegroundColor(c);
	manBut->setPaletteForegroundColor(c);
	cicBut->setPaletteForegroundColor(c);
	rdsLabel->setPaletteForegroundColor(c);
	radioName->setPaletteForegroundColor(c);
	ambDescr->setPaletteForegroundColor(c);
	unoBut->setPaletteForegroundColor(c);
	dueBut->setPaletteForegroundColor(c);
	treBut->setPaletteForegroundColor(c);
	quatBut->setPaletteForegroundColor(c);
	cinBut->setPaletteForegroundColor(c);    
	cancBut->setPaletteForegroundColor(c);    
	bannNavigazione->setFGColor(c);  
}

int radio::setBGPixmap(char* backImage)
{
	QPixmap Back; 
	if(Back.load(backImage))
	{
		setPaletteBackgroundPixmap(Back);      
		return (0);
	}    
	return (1);
}


void radio::setFreq(float f)
{
	frequenza=f;
}

float radio::getFreq()
{
	return(frequenza);
}
void radio::setRDS( const QString & s)
{
	qrds = s;
	qrds.truncate( 8 );
}
QString * radio::getRDS()
{
	return & qrds;
}
void radio::setStaz(uchar st)
{
	stazione=st;
}
uchar radio::getStaz()
{
	return	(stazione);
}

bool radio::isManual()
{
	return (manual);
}

void radio::setAmbDescr(const QString & d)
{
	ambDescr->setText( d );
}

void radio::draw()
{
	QFont aFont;
	rdsLabel->setAlignment(AlignHCenter|AlignVCenter);
	FontManager::instance()->getFont( font_radio_rdsLabel, aFont );
	rdsLabel->setFont( aFont );
	radioName->setAlignment(AlignHCenter|AlignTop);
	FontManager::instance()->getFont( font_radio_radioName, aFont );
	radioName->setFont( aFont );
	radioName->setText( qnome );
	rdsLabel->setText( qrds );
	char fr[10];
	sprintf(fr,"%.2f",frequenza);
	freq->display(&fr[0]);

	QPixmap* Icon = new QPixmap();
	if (manual!=wasManual)
	{
		if (manual)
			Icon->load(ICON_MANUAL_ON);
		else
			Icon->load(ICON_MANUAL_OFF);
		manBut->setPixmap(*Icon);
		if (manual)
			Icon->load(ICON_AUTO_OFF);
		else
			Icon->load(ICON_AUTO_ON);
		autoBut->setPixmap(*Icon);
	}
	wasManual=manual;

	progrText ->setAlignment(AlignHCenter|AlignVCenter);
	FontManager::instance()->getFont( font_radio_progrText, aFont );
	progrText ->setFont( aFont );
	progrText -> setText(QString::number((int)stazione/*,'g',2*/)+":");
}

void radio::setNameU( const QString & s)
{
	qnome = s;
}

QString * radio::getNameU()
{
	return &qnome;
}

void radio::setAuto()
{
	connect(decBut,SIGNAL(clicked()),this,SIGNAL(decFreqAuto()));
	connect(aumBut,SIGNAL(clicked()),this,SIGNAL(aumFreqAuto()));
	disconnect(decBut,SIGNAL(clicked()),this,SIGNAL(decFreqMan()));
	disconnect(aumBut,SIGNAL(clicked()),this,SIGNAL(aumFreqMan()));
	disconnect(aumBut,SIGNAL(clicked()),this,SLOT(verTas()));
	disconnect(decBut,SIGNAL(clicked()),this,SLOT(verTas()));
	aumBut->setAutoRepeat (FALSE);
	decBut->setAutoRepeat (FALSE);
	manual=FALSE;
	draw();
}
void radio::setMan()
{
	disconnect(decBut,SIGNAL(clicked()),this,SIGNAL(decFreqAuto()));
	disconnect(aumBut,SIGNAL(clicked()),this,SIGNAL(aumFreqAuto()));
	connect(decBut,SIGNAL(clicked()),this,SIGNAL(decFreqMan()));
	connect(aumBut,SIGNAL(clicked()),this,SIGNAL(aumFreqMan()));
	aumBut->setAutoRepeat (TRUE);
	decBut->setAutoRepeat (TRUE);
	connect(aumBut,SIGNAL(clicked()),this,SLOT(verTas()));
	connect(decBut,SIGNAL(clicked()),this,SLOT(verTas()));
	manual=TRUE;
	draw();
}


void radio::memo1()
{
	emit(memoFreq(uchar(1)));
	ripristinaContesto();
}
void radio::memo2()
{
	emit(memoFreq(uchar(2)));    
	ripristinaContesto();
}
void radio::memo3()
{
	emit(memoFreq(uchar(3)));   
	ripristinaContesto();
}
void radio::memo4()
{
	emit(memoFreq(uchar(4)));   
	ripristinaContesto();
}
void radio::memo5()
{
	emit(memoFreq(uchar(5)));   
	ripristinaContesto(); 
}

void radio::cambiaContesto()
{
	unoBut->show();
	dueBut->show();
	treBut->show();
	quatBut->show();
	cinBut->show();
	cancBut->show();   
	memoBut->hide();
	decBut->hide();
	aumBut->hide();
	autoBut->hide();
	manBut->hide();
	cicBut->hide();
	disconnect(bannNavigazione  ,SIGNAL(backClick()),this,SIGNAL(Closed()));
	connect(bannNavigazione  ,SIGNAL(backClick()),this,SLOT(ripristinaContesto()));
}
void radio::ripristinaContesto()
{
	unoBut->hide();
	dueBut->hide();
	treBut->hide();
	quatBut->hide();
	cinBut->hide();
	cancBut->hide();   
	memoBut->show();
	decBut->show();
	aumBut->show();
	autoBut->show();
	manBut->show();
	cicBut->show();
	connect(bannNavigazione  ,SIGNAL(backClick()),this,SIGNAL(Closed()));
	disconnect(bannNavigazione  ,SIGNAL(backClick()),this,SLOT(ripristinaContesto()));    
}
void radio::verTas()
{
	if ( (!aumBut->isDown()) && (!decBut->isDown()) )
		emit (richFreq());
}

void radio::freezed(bool f)
{
	qDebug("radio::freezed()");
	// Disable radio and all of its children
	setDisabled(f);
}
