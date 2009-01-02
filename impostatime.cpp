/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**impostaTime.cpp
**
**Sottomenù imposta data/ora
**
****************************************************************/

#include "impostatime.h"
#include "timescript.h"
#include "btbutton.h"
#include "global.h" // BTouch
#include "main.h" // getConfElement

#include <QPixmap>
#include <QDateTime>
#include <QLabel>
#include <QFile>



impostaTime::impostaTime()
{
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

	QDateTime OroTemp = QDateTime::currentDateTime();
	dataOra = new timeScript(this,1,&OroTemp);
	dataOra->setGeometry(10,120,220,80);
	dataOra->setFrameStyle(QFrame::Plain);
	dataOra->setLineWidth(0);

	for (int idx = 0; idx < 3; idx++)
	{
		but[idx] = new BtButton(this);
		but[idx]->setGeometry(idx*80+10,60,60,60);
		but[idx]->setAutoRepeat(true);
		but[idx]->setImage(ICON_FRECCIA_SU);
	}

	for (int idx = 3; idx < 6; idx++)
	{
		but[idx] = new BtButton(this);
		but[idx]->setGeometry((idx-3)*80+10,200,60,60);
		but[idx]->setAutoRepeat(true);
		but[idx]->setImage(ICON_FRECCIA_GIU);
	}

	but[6] = new BtButton(this);
	but[6]->setGeometry(90,260,60,60);
	but[6]->setImage(ICON_OK);

	Immagine = new QLabel(this);
	Immagine->setGeometry(90,0,120,60);
}

void impostaTime::OKTime()
{
	disconnectAllButton();
	dataOra->showDate();

	QPixmap *icon = BTouch->getIcon(ICON_CALENDARIO);
	if (icon)
		Immagine->setPixmap(*icon);

	QDomElement el = getConfElement("setup/generale/clock/dateformat");
	if (!el.isNull() && el.text().toInt() == 1)
	{
		connect(but[1], SIGNAL(clicked()),dataOra,SLOT(aumDay()));
		connect(but[0], SIGNAL(clicked()),dataOra,SLOT(aumMonth()));
		connect(but[2], SIGNAL(clicked()),dataOra,SLOT(aumYear()));
		connect(but[4], SIGNAL(clicked()),dataOra,SLOT(diminDay()));
		connect(but[3], SIGNAL(clicked()),dataOra,SLOT(diminMonth()));
		connect(but[5], SIGNAL(clicked()),dataOra,SLOT(diminYear()));
	}
	else
	{
		connect(but[0], SIGNAL(clicked()),dataOra,SLOT(aumDay()));
		connect(but[1], SIGNAL(clicked()),dataOra,SLOT(aumMonth()));
		connect(but[2], SIGNAL(clicked()),dataOra,SLOT(aumYear()));
		connect(but[3], SIGNAL(clicked()),dataOra,SLOT(diminDay()));
		connect(but[4], SIGNAL(clicked()),dataOra,SLOT(diminMonth()));
		connect(but[5], SIGNAL(clicked()),dataOra,SLOT(diminYear()));
	}

	connect(but[6] ,SIGNAL(clicked()), SLOT(OKDate()));

	QString f = "*#13**#0*" + dataOra->getDataOra().toString("hh*mm*ss") + "**##";
	BTouch->sendFrame(f);
}

void impostaTime::OKDate()
{
	disconnectAllButton();
	QString f = "*#13**#1*00*" + dataOra->getDataOra().toString("dd*MM*yyyy") + "##";
	BTouch->sendFrame(f);
	emit Closed();
}

void impostaTime::showEvent(QShowEvent *event)
{
	qDebug("impostaTime::showEvent()");
	dataOra->reset();
	dataOra->showTime();

	QPixmap *icon = BTouch->getIcon(ICON_OROLOGIO);
	if (icon)
		Immagine->setPixmap(*icon);

	connect(but[0], SIGNAL(clicked()),dataOra,SLOT(aumOra()));
	connect(but[1], SIGNAL(clicked()),dataOra,SLOT(aumMin()));
	connect(but[2], SIGNAL(clicked()),dataOra,SLOT(aumSec()));
	connect(but[3], SIGNAL(clicked()),dataOra,SLOT(diminOra()));
	connect(but[4], SIGNAL(clicked()),dataOra,SLOT(diminMin()));
	connect(but[5], SIGNAL(clicked()),dataOra,SLOT(diminSec()));
	connect(but[6], SIGNAL(clicked()),this,SLOT(OKTime()));
}

void impostaTime::disconnectAllButton()
{
	for (int i = 0; i <= 6; ++i)
		but[i]->disconnect();
}
