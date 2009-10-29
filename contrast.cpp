#include "contrast.h"
#include "main.h"
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
	btn->setImage(img);
	btn->setAutoRepeat(autorepeat);
	return btn;
}

Contrast::Contrast()
{
	// create buttons
	BtButton *decBut = getButton(ICON_MENO, true);
	BtButton *incBut = getButton(ICON_PIU, true);
	BtButton *okBut = getButton(ICON_OK, false);

	connect(decBut, SIGNAL(clicked()), SLOT(decContrast()));
	connect(incBut, SIGNAL(clicked()), SLOT(incContrast()));
	connect(okBut,  SIGNAL(clicked()), SIGNAL(Closed()));

	// create images
	QLabel *paintLabel = new QLabel;
	QLabel *colorBar = new QLabel;

	paintLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
	paintLabel->setPixmap(QPixmap(IMG_PATH "my_home.png"));

	colorBar ->setFrameStyle(QFrame::Panel | QFrame::Raised);
	colorBar ->setPixmap(QPixmap(IMG_PATH "colorbar.png"));

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
		setContrast(c+10, FALSE);
}

void Contrast::decContrast()
{
	int c = getContrast();
	if (c >= 10)
		setContrast(c-10,FALSE);
}
