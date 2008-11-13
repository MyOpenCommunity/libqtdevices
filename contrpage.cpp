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
#include "generic_functions.h" // getContrast, setContrast
#include "btbutton.h"

#include <QWidget>
#include <QFrame>
#include <QString>
#include <QLabel>
#include <QFile>


contrPage::contrPage(QWidget *parent, const char *name) : QWidget(parent)
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	showFullScreen();
#endif
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH,MAX_HEIGHT));

	aumBut = new BtButton(this);
	decBut = new BtButton(this);
	okBut = new BtButton(this);

	paintLabel = new QLabel(this);
	colorBar = new QLabel(this);

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

	decBut->setImage(ICON_MENO);
	aumBut->setImage(ICON_PIU);
	okBut->setImage(ICON_OK);

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
