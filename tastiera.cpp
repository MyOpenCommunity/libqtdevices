/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**tastiera.cpp
**
**finestra di tastiera numerica
**
****************************************************************/


#include "tastiera.h"
#include "banner.h"
#include "fontmanager.h"
#include "btbutton.h"

#include <QFrame>
#include <QLabel>
#include <QString>
#include <QCursor>
#include <QFile>
#include <QDebug>
#include <QButtonGroup>

#define BUT_DIM 60
#define POSX1 (MAX_WIDTH-BUT_DIM*3)/6
#define POSX2 POSX1*3+BUT_DIM
#define POSX3 POSX2+POSX1*2+BUT_DIM

#define BUT_SMALL_DIM (MAX_WIDTH-POSX1*2)/8
#define POSX1_SMALL POSX1

tastiera::tastiera(QWidget *parent, const char *name, int line) : QWidget(parent)
{
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

	buttons_group = new QButtonGroup(this);
	unoBut = new BtButton(this);
	dueBut = new BtButton(this);
	treBut = new BtButton(this);
	quatBut = new BtButton(this);
	cinBut = new BtButton(this);
	seiBut = new BtButton(this);
	setBut = new BtButton(this);
	ottBut = new BtButton(this);
	novBut = new BtButton(this);
	zeroBut = new BtButton(this);
	okBut = new BtButton(this);
	cancBut = new BtButton(this);

	digitLabel = new QLabel(this);
	scrittaLabel = new QLabel(this);

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

	unoBut->setImage(ICON_UNO);
	buttons_group->addButton(unoBut, 1);
	dueBut->setImage(ICON_DUE);
	buttons_group->addButton(dueBut, 2);
	treBut->setImage(ICON_TRE);
	buttons_group->addButton(treBut, 3);
	quatBut->setImage(ICON_QUATTRO);
	buttons_group->addButton(quatBut, 4);
	cinBut->setImage(ICON_CINQUE);
	buttons_group->addButton(cinBut, 5);
	seiBut->setImage(ICON_SEI);
	buttons_group->addButton(seiBut, 6);
	setBut->setImage(ICON_SETTE);
	buttons_group->addButton(setBut, 7);
	ottBut->setImage(ICON_OTTO);
	buttons_group->addButton(ottBut, 8);
	novBut->setImage(ICON_NOVE);
	buttons_group->addButton(novBut, 9);
	zeroBut->setImage(ICON_ZERO);
	buttons_group->addButton(zeroBut, 0);

	okBut->setImage(ICON_OK);
	cancBut->setImage(ICON_CANC);

	mode = CLEAN;

	scrittaLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	QFont aFont;
	FontManager::instance()->getFont(font_tastiera_scritta_label, aFont);
	scrittaLabel->setFont(aFont);
	scrittaLabel->setText(tr("PASSWORD:"));
	digitLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

	FontManager::instance()->getFont(font_tastiera_digit_label, aFont);
	digitLabel->setFont(aFont);

	connect(buttons_group, SIGNAL(buttonClicked(int)), SLOT(buttonClicked(int)));
	connect(cancBut,SIGNAL(clicked()),this,SLOT(canc()));
	connect(okBut,SIGNAL(clicked()),this,SLOT(ok()));
}

void tastiera::showTastiera()
{
	draw();
	show();
}

void tastiera::draw()
{
	qDebug("tastiera::draw(), mode = %d", mode);
	if (mode == CLEAN)
		digitLabel->setText(text);
	else
		digitLabel->setText(QString(text.length(),'*'));
}

void tastiera::buttonClicked(int number)
{
	qDebug() << "button clicked " << number;
	if (text.length() < 5)
		text += QString::number(number);
	draw();
}

void tastiera::canc()
{
	if (text.length() > 0)
		text.chop(1);
	else
	{
		hide();
		emit Closed(NULL);
	}
	draw();
}

void tastiera::ok()
{
	// TODO: l'hide deve essere la classe "esterna" a gestirlo sul segnale Closed()
	// che NON deve avere parametri. Il testo/password devono essere specificati
	// attraverso un'altro segnale (o estratti attraverso un getter)
	hide();
	emit Closed(text.toAscii().data());
}

void tastiera::setMode(tastiType t)
{
	mode = t;
}


// tastiera_con_stati implementation
tastiera_con_stati::tastiera_con_stati(int s[8], QWidget *parent, const char *name)
	: tastiera(parent, name, MAX_HEIGHT/6)
{
	int i, x;
	char tmp[2] = "1";
	QFont aFont;
	FontManager::instance()->getFont(font_tastiera_bottoni_stati, aFont);
	
	for (i = 0, x = POSX1_SMALL; i < 8; i++, x += BUT_SMALL_DIM)
	{
		// Create button
		stati[i] = new BtButton(this);
		stati[i]->setEnabled(0);

		stati[i]->setFont(aFont);
		if (s[i] == -1)
		{
			stati[i]->setText("-");
			st[i] = false;
		}
		else
		{
			stati[i]->setText(tmp);
			st[i] = s[i];
		}
		tmp[0]++;
		stati[i]->setGeometry(x, (MAX_HEIGHT/6)*4 + MAX_HEIGHT/12, BUT_SMALL_DIM, BUT_SMALL_DIM);
		stati[i]->show();
	}

	scrittaLabel->setGeometry(0,(MAX_HEIGHT/6)*5,MAX_WIDTH/2,(MAX_HEIGHT/6));
	digitLabel->setGeometry(MAX_WIDTH/2,(MAX_HEIGHT/6)*5,MAX_WIDTH/2, MAX_HEIGHT/6);
	scrittaLabel->show();
	digitLabel->show();
}

void tastiera_con_stati::show()
{
	for (int i = 0; i < 8; i++)
	{
		if (st[i])
		{
			// TODO: e' necessario trovare un modo per invertire colore di testo e sfondo
			// con gli stylesheet, che forse e' parsare la stringa ottenuta con stylesheet()!!
			//QColor fc = stati[i]->paletteForegroundColor();
			//stati[i]->setPaletteForegroundColor(stati[i]->paletteBackgroundColor());
			//stati[i]->setPaletteBackgroundColor(fc);
		}
	}
	QWidget::show();
}
