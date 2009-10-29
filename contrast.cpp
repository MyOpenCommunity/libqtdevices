#include "contrast.h"
#include "main.h"
#include "hardware_functions.h" // getContrast, setContrast
#include "btbutton.h"

#include <QFrame>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>


Contrast::Contrast()
{
	aumBut = new BtButton(this);
	decBut = new BtButton(this);
	okBut = new BtButton(this);

	paintLabel = new QLabel(this);
	colorBar = new QLabel(this);

	paintLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
	paintLabel->setPixmap(QPixmap(IMG_PATH "my_home.png"));

	colorBar ->setFrameStyle(QFrame::Panel | QFrame::Raised);
	colorBar ->setPixmap(QPixmap(IMG_PATH "colorbar.png"));

	decBut->setImage(ICON_MENO);
	aumBut->setImage(ICON_PIU);
	okBut->setImage(ICON_OK);

	aumBut->setAutoRepeat(true);
	decBut->setAutoRepeat(true);
	connect(decBut,SIGNAL(clicked()),this,SLOT(decContr()));
	connect(aumBut,SIGNAL(clicked()),this,SLOT(aumContr()));
	connect(okBut,SIGNAL(clicked()),this,SIGNAL(Closed()));

	// layout
	QHBoxLayout *b = new QHBoxLayout;
	b->setContentsMargins(0, 0, 0, 0);
	b->setSpacing(0);

	b->addWidget(decBut, 0, Qt::AlignLeft);
	b->addWidget(okBut);
	b->addWidget(aumBut, 0, Qt::AlignRight);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);

	l->addWidget(paintLabel, 0, Qt::AlignCenter);
	l->addWidget(colorBar, 0, Qt::AlignCenter);
	l->addLayout(b);
}

void Contrast::aumContr()
{
	uchar c;

	c = getContrast();
	if (c < 140)
		setContrast(c+10, FALSE);
}

void Contrast::decContr()
{
	uchar c;

	c = getContrast();
	if (c >= 10)
		setContrast(c-10,FALSE);
}
