/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** aux.cpp
 **
 **finestra di dati sulla sorgente aux
 **
 ****************************************************************/


#include <qpixmap.h>
#include <qcursor.h>
#include <qfile.h>

#include "aux.h"
#include "bannfrecce.h"
#include "btbutton.h"
#include "btlabel.h"
#include "genericfunz.h"
#include "fontmanager.h"


aux::aux(QWidget *parent, const QString & name, const QString & amb) : QWidget(parent)
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	setCursor(QCursor(Qt::BlankCursor));
#endif
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

	bannNavigazione = new bannFrecce(this,"bannerfrecce",1);
	bannNavigazione->setGeometry(0 ,MAX_HEIGHT- MAX_HEIGHT/NUM_RIGHE ,MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE);

	auxName = new BtLabel(this,"Nome della sorgente");
	ambDescr = new BtLabel(this, "descrizione ambiente");
	ambDescr->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
	QFont aFont;
	FontManager::instance()->getFont(font_aux_descr_ambiente, aFont);
	ambDescr->setFont(aFont);
	ambDescr->setText(amb);
	auxName->setGeometry(0,30,240,40);
	auxName->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

	FontManager::instance()->getFont(font_aux_nome_sorgente, aFont);
	auxName->setFont(aFont);
	auxName->setText(name);
	ambDescr->setGeometry(0,100,240,40);
	fwdBut = new BtButton(this);
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
	connect(bannNavigazione, SIGNAL(backClick()),this,SIGNAL(Closed()));
}

void aux::showAux()
{
	draw();
	showFullScreen();
}

void aux::setBGColor(int r, int g, int b)
{
	setBGColor(QColor::QColor(r,g,b));
}

void aux::setFGColor(int r, int g, int b)
{
	setFGColor(QColor::QColor(r,g,b));
}

void aux::setBGColor(QColor c)
{
	setPaletteBackgroundColor(c);
	auxName->setPaletteBackgroundColor(c);
	ambDescr->setPaletteBackgroundColor(c);
	bannNavigazione->setBGColor(c);
	fwdBut->setPaletteBackgroundColor(c);
}

void aux::setFGColor(QColor c)
{
	setPaletteForegroundColor(c);
	auxName->setPaletteForegroundColor(c);
	ambDescr->setPaletteForegroundColor(c);
	bannNavigazione->setFGColor(c);
	fwdBut->setPaletteForegroundColor(c);
}

void aux::setAmbDescr(const QString & d)
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

void aux::setPaletteBackgroundColor(const QColor &c)
{
	QPalette palette;
	palette.setColor(backgroundRole(), c);
	setPalette(palette);
}

void aux::setPaletteForegroundColor(const QColor &c)
{
	QPalette palette;
	palette.setColor(foregroundRole(), c);
	setPalette(palette);
}

void aux::setPaletteBackgroundPixmap(const QPixmap &pixmap)
{
	QPalette palette;
	palette.setBrush(backgroundRole(), QBrush(pixmap));
	setPalette(palette);
}

