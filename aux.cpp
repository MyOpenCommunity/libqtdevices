/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** aux.cpp
 **
 **finestra di dati sulla sorgente aux
 **
 ****************************************************************/


#include <qfont.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <stdlib.h>
#include <qwidget.h>
#include <qframe.h>
#include <qdatetime.h>
#include <qprocess.h>
#include <qstring.h>

#include <qfile.h>

#include "aux.h"
#include "banner.h"
#include "bannondx.h"
#include "bannfrecce.h"
#include "main.h"

//#include "ban.h"

aux::aux( QWidget *parent, const char *name, const char *amb )
: QWidget( parent, name )
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	setCursor (QCursor (blankCursor));
	//   showFullScreen();
#endif
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

	bannNavigazione = new bannFrecce(this,"bannerfrecce",1);
	bannNavigazione->setGeometry( 0 ,MAX_HEIGHT- MAX_HEIGHT/NUM_RIGHE ,MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE );

	auxName = new BtLabel(this,"Nome della sorgente");
	ambDescr = new BtLabel(this, "descrizione ambiente");
	ambDescr->setAlignment(AlignHCenter|AlignTop);
	ambDescr->setFont( QFont( "Helvetica", 24, QFont::Bold ) );
	ambDescr->setText(amb);
	auxName->setGeometry(0,30,240,40);
	auxName->setAlignment(AlignHCenter|AlignTop);
	auxName->setFont( QFont( "Helvetica", 24, QFont::Bold ) );
	auxName->setText(name);
	ambDescr->setGeometry(0,100,240,40);
	fwdBut = new BtButton(this, "bottone fwd");
	fwdBut->setGeometry(MAX_WIDTH/2-30, 160, 60, 60);
	QPixmap* Icon = new QPixmap();
	QPixmap* pressIcon = new QPixmap();
	char pressIconName[MAX_PATH];
	Icon->load(ICON_FFWD);
	getPressName((char*)ICON_FFWD, &pressIconName[0],sizeof(pressIconName));
	fwdBut->setPixmap(*Icon);
	if (QFile::exists(pressIconName))
	{
		pressIcon->load(pressIconName);
		fwdBut->setPressedPixmap(*pressIcon);
	}
	connect(fwdBut, SIGNAL(clicked()), this, SIGNAL(Btnfwd()));
	connect(bannNavigazione  ,SIGNAL(backClick()),this,SIGNAL(Closed()));
}


void aux::showAux()
{
	draw();
	showFullScreen();
}


void aux::setBGColor(int r, int g, int b)
{	
	setBGColor( QColor :: QColor(r,g,b));
}


void aux::setFGColor(int r, int g, int b)
{
	setFGColor( QColor :: QColor(r,g,b));
}


void aux::setBGColor(QColor c)
{	
	setPaletteBackgroundColor( c );
	auxName->setPaletteBackgroundColor(c);
	ambDescr->setPaletteBackgroundColor(c);
	bannNavigazione->setBGColor(c);
	fwdBut->setPaletteBackgroundColor(c);
}


void aux::setFGColor(QColor c)
{
	setPaletteForegroundColor( c );
	auxName->setPaletteForegroundColor(c);
	ambDescr->setPaletteForegroundColor(c);
	bannNavigazione->setFGColor(c);
	fwdBut->setPaletteForegroundColor(c);
}


int aux::setBGPixmap(char* backImage)
{
	QPixmap Back; 
	if(Back.load(backImage))
	{
		setPaletteBackgroundPixmap(Back);
		return (0);
	}
	return (1);
}


void aux::setAmbDescr(char *d)
{
	ambDescr->setText(d);
}


void aux::draw()
{
}


void aux::freezed(bool f)
{
	qDebug("aux::freezed()");
	// Disable aux and all of its children
	setDisabled(f);
}
