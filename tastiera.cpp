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

	dueBut->setImage(ICON_DUE);

	treBut->setImage(ICON_TRE);

	quatBut->setImage(ICON_QUATTRO);

	cinBut->setImage(ICON_CINQUE);

	seiBut->setImage(ICON_SEI);

	setBut->setImage(ICON_SETTE);

	ottBut->setImage(ICON_OTTO);

	novBut->setImage(ICON_NOVE);

	zeroBut->setImage(ICON_ZERO);

	okBut->setImage(ICON_OK);

	cancBut->setImage(ICON_CANC);

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
	memset(pwd,'\000',sizeof(pwd));
	draw();
	show();
}

void tastiera::draw()
{
	scrittaLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	QFont aFont;
	FontManager::instance()->getFont(font_tastiera_scritta_label, aFont);
	scrittaLabel->setFont(aFont);
	scrittaLabel->setText(tr("PASSWORD:"));
	digitLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

	FontManager::instance()->getFont(font_tastiera_digit_label, aFont);
	digitLabel->setFont(aFont);
	qDebug("tastiera::draw(), mode = %d", mode);
	if (mode == CLEAN)
		digitLabel->setText(pwd);
	else
	{
		char pw[10];
		memset(pw,'\000',sizeof(pw));
		memset(pw,'*',strlen(pwd));
		digitLabel->setText(pw);
	}
}

void tastiera::press1()
{
	if (strlen(pwd) < 5)
		strcat(pwd,"1");
	draw();
}

void tastiera::press2()
{
	if (strlen(pwd) < 5)
		strcat(pwd,"2");
	draw();
}

void tastiera::press3()
{
	if (strlen(pwd) < 5)
		strcat(pwd,"3");
	draw();
}

void tastiera::press4()
{
	if (strlen(pwd) < 5)
		strcat(pwd,"4");
	draw();
}

void tastiera::press5()
{
	if (strlen(pwd) < 5)
		strcat(pwd,"5");
	draw();
}

void tastiera::press6()
{
	if (strlen(pwd) < 5)
		strcat(pwd,"6");
	draw();
}

void tastiera::press7()
{
	if (strlen(pwd) < 5)
		strcat(pwd,"7");
	draw();
}

void tastiera::press8()
{
	if (strlen(pwd) < 5)
		strcat(pwd,"8");
	draw();
}

void tastiera::press9()
{
	if (strlen(pwd) < 5)
		strcat(pwd,"9");
	draw();
}

void tastiera::press0()
{
	if (strlen(pwd) < 5)
		strcat(pwd,"0");
	draw();
}

void tastiera::canc()
{
	if (strlen(pwd) > 0)
		pwd[strlen(pwd) - 1] = '\000';
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
	emit(Closed(pwd));
}

void tastiera::setMode(char m)	
{
	mode = m;
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
