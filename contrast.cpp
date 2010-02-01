#include "contrast.h"
#include "skinmanager.h"
#include "hardware_functions.h" // getContrast, setContrast
#include "btbutton.h"

#include <QFrame>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>


static BtButton *getButton(QString img, bool autorepeat)
{
	BtButton *btn = new BtButton;
	btn->setImage(bt_global::skin->getImage(img));
	btn->setAutoRepeat(autorepeat);
	return btn;
}

Contrast::Contrast()
{
	// create buttons
	BtButton *decBut = getButton("minus", true);
	BtButton *incBut = getButton("plus", true);
	BtButton *okBut = getButton("ok", false);

	connect(decBut, SIGNAL(clicked()), SLOT(decContrast()));
	connect(incBut, SIGNAL(clicked()), SLOT(incContrast()));
	connect(okBut,  SIGNAL(clicked()), SIGNAL(Closed()));

	// create images
	QLabel *paintLabel = new QLabel;
	QLabel *colorBar = new QLabel;

	paintLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
	paintLabel->setPixmap(QPixmap(bt_global::skin->getImage("logo")));

	colorBar ->setFrameStyle(QFrame::Panel | QFrame::Raised);
	colorBar ->setPixmap(QPixmap(bt_global::skin->getImage("colorbar")));

	// layout
	QHBoxLayout *b = new QHBoxLayout;
	b->setContentsMargins(0, 0, 0, 0);
	b->setSpacing(0);

	b->addWidget(decBut, 0, Qt::AlignLeft);
	b->addWidget(okBut);
	b->addWidget(incBut, 0, Qt::AlignRight);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);

	l->addWidget(paintLabel, 0, Qt::AlignCenter);
	l->addWidget(colorBar, 0, Qt::AlignCenter);
	l->addLayout(b);
}

void Contrast::incContrast()
{
	int c = getContrast();
	if (c < 140)
		setContrast(c+10);
}

void Contrast::decContrast()
{
	int c = getContrast();
	if (c >= 10)
		setContrast(c-10);
}
