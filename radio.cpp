
/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 **radio.cpp
 **
 **finestra di dati sulla radio
 **
 ****************************************************************/

#include "radio.h"
#include "banner.h"
#include "bannfrecce.h"
#include "main.h"
#include "btbutton.h"
#include "fontmanager.h" // bt_global::font

#include <QLabel>
#include <QPixmap>
#include <QFrame>
#include <QDateTime>
#include <QLCDNumber>
#include <QFile>


radio::radio(QWidget *parent, const QString & amb)
	: QWidget(parent)
{
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

	bannNavigazione = new bannFrecce(this,1);
	bannNavigazione->setGeometry(0 ,MAX_HEIGHT- MAX_HEIGHT/NUM_RIGHE ,MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE);

	memoBut = new BtButton(this);
	decBut = new BtButton(this);
	aumBut = new BtButton(this);
	autoBut = new BtButton(this);
	manBut = new BtButton(this);
	cicBut = new BtButton(this);
	unoBut = new BtButton(this);
	dueBut = new BtButton(this);
	treBut = new BtButton(this);
	quatBut = new BtButton(this);
	cinBut = new BtButton(this);
	cancBut = new BtButton(this);

	rdsLabel = new QLabel(this);
	radioName = new QLabel(this);
	ambDescr = new QLabel(this);
	ambDescr->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
	ambDescr->setFont(bt_global::font.get(FontManager::SMALLTEXT));
	ambDescr->setText(amb);
	freq = new QLCDNumber(this);
	progrText = new QLabel(this);
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

	cicBut->setImage(ICON_CICLA);

	aumBut->setImage(ICON_PIU);

	decBut->setImage(ICON_MENO);

	memoBut->setImage(ICON_MEM);

	manBut->setImage(ICON_MANUAL_ON);

	autoBut->setImage(ICON_AUTO_ON);

	unoBut->setImage(ICON_UNO);

	dueBut->setImage(ICON_DUE);

	treBut->setImage(ICON_TRE);

	quatBut->setImage(ICON_QUATTRO);

	cinBut->setImage(ICON_CINQUE);

	cancBut->setImage(ICON_CANC);

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

void radio::setFreq(float f)
{
	frequenza=f;
}

float radio::getFreq()
{
	return(frequenza);
}

void radio::setRDS(const QString & s)
{
	qrds = s;
	qrds.truncate(8);
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
	ambDescr->setText(d);
}

void radio::draw()
{
	rdsLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	rdsLabel->setFont(bt_global::font.get(FontManager::SUBTITLE));

	radioName->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
	radioName->setFont(bt_global::font.get(FontManager::SMALLTEXT));
	radioName->setText(qnome);
	rdsLabel->setText(qrds);
	char fr[10];
	sprintf(fr,"%.2f",frequenza);
	freq->display(fr);

	if (manual!=wasManual)
	{
		if (manual)
		{
			manBut->setImage(ICON_MANUAL_ON, BtButton::NO_FLAG);
			autoBut->setImage(ICON_AUTO_OFF, BtButton::NO_FLAG);
		}
		else
		{
			manBut->setImage(ICON_MANUAL_OFF, BtButton::NO_FLAG);
			autoBut->setImage(ICON_AUTO_ON, BtButton::NO_FLAG);
		}
	}
	wasManual=manual;

	progrText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	progrText->setFont(bt_global::font.get(FontManager::TEXT));
	progrText->setText(QString::number((int)stazione)+":");
}

void radio::setName(const QString & s)
{
	qnome = s;
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
	if ((!aumBut->isDown()) && (!decBut->isDown()))
		emit (richFreq());
}
