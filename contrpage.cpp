/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**tastiera.cpp
**
**finestra di tastiera numerica
**
****************************************************************/

#include "contrpage.h"
#include "banner.h"
#include "main.h"
#include "genericfunz.h"
#include "btlabel.h"
#include "btbutton.h"

#include <QWidget>
#include <QFrame>
#include <QString>
#include <QFile>


contrPage::contrPage(QWidget *parent, const char *name) : QWidget(parent)
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	setCursor(QCursor(Qt::BlankCursor));
	showFullScreen();
#endif
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH,MAX_HEIGHT));

	aumBut = new BtButton(this);
	decBut = new BtButton(this);
	okBut = new BtButton(this);

	paintLabel = new BtLabel(this,"PL");
	colorBar = new BtLabel(this,"CB");

	decBut->setGeometry(0,MAX_HEIGHT-BUT_DIM, BUT_DIM, BUT_DIM);
	aumBut->setGeometry(MAX_WIDTH-BUT_DIM, MAX_HEIGHT-BUT_DIM, BUT_DIM, BUT_DIM);
	okBut->setGeometry((MAX_WIDTH-BUT_DIM)/2,MAX_HEIGHT-BUT_DIM,BUT_DIM,BUT_DIM);

	paintLabel->setGeometry((MAX_WIDTH-IMG_X)/2, (MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE-2*IMG_Y)/2, IMG_X, IMG_Y);
	paintLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
	// TODO: risistemare il layout affinche' non ci sia bisogno del resize!!
	//paintLabel->setAutoResize(TRUE);
	paintLabel->setPixmap(QPixmap(IMG_PATH "my_home.png"));

	colorBar ->setGeometry((MAX_WIDTH-IMG_X)/2, (MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE-2*IMG_Y)/2+IMG_Y, IMG_X, IMG_Y);
	colorBar ->setFrameStyle(QFrame::Panel | QFrame::Raised);
	// TODO: vedi sopra!!
	//colorBar ->setAutoResize(TRUE);
	colorBar ->setPixmap(QPixmap(IMG_PATH "colorbar.png"));

	QPixmap *Icon, *pressIcon;
	char   pressIconName[MAX_PATH];

	Icon = new QPixmap();
	pressIcon = new QPixmap();

	Icon->load(ICON_MENO);
	getPressName((char*)ICON_MENO, &pressIconName[0],sizeof(pressIconName));
	decBut->setPixmap(*Icon);

	if (QFile::exists(pressIconName))
	{
		pressIcon->load(pressIconName);
		decBut->setPressedPixmap(*pressIcon);
	}

	Icon->load(ICON_PIU);
	getPressName((char*)ICON_PIU, &pressIconName[0],sizeof(pressIconName));
	aumBut->setPixmap(*Icon);

	if (QFile::exists(pressIconName))
	{
		pressIcon->load(pressIconName);
		aumBut->setPressedPixmap(*pressIcon);
	}

	Icon->load(ICON_OK);
	getPressName((char*)ICON_OK, &pressIconName[0],sizeof(pressIconName));
	okBut->setPixmap(*Icon);

	if (QFile::exists(pressIconName))
	{
		pressIcon->load(pressIconName);
		okBut->setPressedPixmap(*pressIcon);
	}
	aumBut->show();
	decBut->show();
	okBut->show();
	paintLabel->show();
	colorBar->show();
	aumBut->setAutoRepeat(TRUE);
	decBut->setAutoRepeat(TRUE);
	connect(decBut,SIGNAL(clicked()),this,SLOT(decContr()));
	connect(aumBut,SIGNAL(clicked()),this,SLOT(aumContr()));
	connect(okBut,SIGNAL(clicked()),this,SIGNAL(Close()));
}

void contrPage::setBGColor(int r, int g, int b)
{
	setBGColor(QColor::QColor(r,g,b));
}

void contrPage::setFGColor(int r, int g, int b)
{
	setFGColor(QColor::QColor(r,g,b));
}

void contrPage::setBGColor(QColor c)
{
	setPaletteBackgroundColor(c);
	decBut->setPaletteBackgroundColor(c);
	aumBut->setPaletteBackgroundColor(c);
	okBut->setPaletteBackgroundColor(c);
	paintLabel->setPaletteBackgroundColor(c);
}

void contrPage::setFGColor(QColor c)
{
	setPaletteForegroundColor(c);
	decBut->setPaletteForegroundColor(c);
	aumBut->setPaletteForegroundColor(c);
	okBut->setPaletteForegroundColor(c);
	paintLabel->setPaletteForegroundColor(c);
}

int contrPage::setBGPixmap(char* backImage)
{
	QPixmap Back;
	if(Back.load(backImage))
	{
		setPaletteBackgroundPixmap(Back);
		return 0;
	}
	return 1;
}

void contrPage::aumContr()
{
	uchar c;

	c = getContrast();
	if (c < 140)
		setContrast(c+10, FALSE);
}

void contrPage::decContr()
{
	uchar c;

	c = getContrast();
	if (c >= 10)
		setContrast(c-10,FALSE);
}

// TODO: funzioni di compatibilita' qt3.. da rimuovere!!
void contrPage::setPaletteBackgroundColor(const QColor &c)
{
	QPalette palette;
	palette.setColor(backgroundRole(), c);
	setPalette(palette);
}

void contrPage::setPaletteForegroundColor(const QColor &c)
{
	QPalette palette;
	palette.setColor(foregroundRole(), c);
	setPalette(palette);
}

void contrPage::setPaletteBackgroundPixmap(const QPixmap &pixmap)
{
	QPalette palette;
	palette.setBrush(backgroundRole(), QBrush(pixmap));
	setPalette(palette);
}
