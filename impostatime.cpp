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
#include "openclient.h"

#include <QPixmap>
#include <QWidget>
#include <QCursor>
#include <QDateTime>
#include <QLabel>
#include <QFile>

extern unsigned char tipoData;


impostaTime::impostaTime(QWidget *parent, const char *name) : QFrame(parent)
{
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

	QDateTime OroTemp = QDateTime::currentDateTime();
	dataOra = new timeScript(this,"impostazioni",1,&OroTemp);
	dataOra->setGeometry(10,120,220,80);
	dataOra->setFrameStyle(QFrame::Plain);
	dataOra->setLineWidth(0);
	dataOra->hide();

	for (uchar idx = 0; idx < 3; idx++)
	{
		but[idx] = new BtButton(this);
		but[idx]->setGeometry(idx*80+10,60,60,60);
		but[idx]->setAutoRepeat(true);
		but[idx]->setImage(ICON_FRECCIA_SU);
	}

	for (uchar idx=3;idx<6;idx++)
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
	disconnect(but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumOra()));
	disconnect(but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumMin()));
	disconnect(but[2] ,SIGNAL(clicked()),dataOra,SLOT(aumSec()));
	disconnect(but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminOra()));
	disconnect(but[4] ,SIGNAL(clicked()),dataOra,SLOT(diminMin()));
	disconnect(but[5] ,SIGNAL(clicked()),dataOra,SLOT(diminSec()));
	disconnect(but[6] ,SIGNAL(clicked()),this,SLOT(OKTime()));

	dataOra->showDate();

	QPixmap* Icon1 = new QPixmap();
	Icon1->load(ICON_CALENDARIO);

	if (Icon1)
		Immagine->setPixmap(*Icon1);

	if (tipoData == 1)
	{
		connect(but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumDay()));
		connect(but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumMonth()));
		connect(but[2] ,SIGNAL(clicked()),dataOra,SLOT(aumYear()));
		connect(but[4] ,SIGNAL(clicked()),dataOra,SLOT(diminDay()));
		connect(but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminMonth()));
		connect(but[5] ,SIGNAL(clicked()),dataOra,SLOT(diminYear()));
		connect(but[6] ,SIGNAL(clicked()),this,SLOT(OKDate()));
	}
	else
	{
		connect(but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumDay()));
		connect(but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumMonth()));
		connect(but[2] ,SIGNAL(clicked()),dataOra,SLOT(aumYear()));
		connect(but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminDay()));
		connect(but[4] ,SIGNAL(clicked()),dataOra,SLOT(diminMonth()));
		connect(but[5] ,SIGNAL(clicked()),dataOra,SLOT(diminYear()));
		connect(but[6] ,SIGNAL(clicked()),this,SLOT(OKDate()));
	}

#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	openwebnet msg_open;
	char pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#13**#0*");

	QByteArray buf = dataOra->getDataOra().toString("hh*mm*ss").toAscii();
	strcat(pippo, buf.constData());
	strcat(pippo,"**##");

	msg_open.CreateMsgOpen((char*)pippo,strlen((char*)pippo));
	emit sendFrame(msg_open.frame_open);
#endif
}

void impostaTime::OKDate()
{
	if (tipoData == 1)
	{
		disconnect(but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumDay()));
		disconnect(but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumMonth()));
		disconnect(but[2] ,SIGNAL(clicked()),dataOra,SLOT(aumYear()));
		disconnect(but[4] ,SIGNAL(clicked()),dataOra,SLOT(diminDay()));
		disconnect(but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminMonth()));
		disconnect(but[5] ,SIGNAL(clicked()),dataOra,SLOT(diminYear()));
		disconnect(but[6] ,SIGNAL(clicked()),this,SLOT(OKDate()));
	}
	else
	{
		disconnect(but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumDay()));
		disconnect(but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumMonth()));
		disconnect(but[2] ,SIGNAL(clicked()),dataOra,SLOT(aumYear()));
		disconnect(but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminDay()));
		disconnect(but[4] ,SIGNAL(clicked()),dataOra,SLOT(diminMonth()));
		disconnect(but[5] ,SIGNAL(clicked()),dataOra,SLOT(diminYear()));
		disconnect(but[6] ,SIGNAL(clicked()),this,SLOT(OKDate()));
	}

#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	openwebnet msg_open;
	char    pippo[50];
	QString s;

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#13**#1*00*");

	QByteArray buf = dataOra->getDataOra().toString("dd*MM*yyyy").toAscii();
	strcat(pippo, buf.constData());
	strcat(pippo,"##");

	msg_open.CreateMsgOpen((char*)pippo,strlen((char*)pippo));
	emit sendFrame(msg_open.frame_open);
#endif
	hide();
}

void impostaTime::mostra()
{
	qDebug("impostaTime::mostra()");
	dataOra->reset();
	dataOra->showTime();
	disconnect(but[0], 0, dataOra, 0);
	disconnect(but[1], 0, dataOra, 0);
	disconnect(but[2], 0, dataOra, 0);
	disconnect(but[3], 0, dataOra, 0);
	disconnect(but[4], 0, dataOra, 0);
	disconnect(but[5], 0, dataOra, 0);
	disconnect(but[6], 0, dataOra, 0);

	QPixmap *Icon1 = new QPixmap();
	Icon1->load(ICON_OROLOGIO);

	if (Icon1)
		Immagine->setPixmap(*Icon1);

	connect(but[0] ,SIGNAL(clicked()),dataOra,SLOT(aumOra()));
	connect(but[1] ,SIGNAL(clicked()),dataOra,SLOT(aumMin()));
	connect(but[2] ,SIGNAL(clicked()),dataOra,SLOT(aumSec()));
	connect(but[3] ,SIGNAL(clicked()),dataOra,SLOT(diminOra()));
	connect(but[4] ,SIGNAL(clicked()),dataOra,SLOT(diminMin()));
	connect(but[5] ,SIGNAL(clicked()),dataOra,SLOT(diminSec()));
	connect(but[6] ,SIGNAL(clicked()),this,SLOT(OKTime()));

#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	showFullScreen();
#else
	show();
#endif

	dataOra->show();
}

void impostaTime::hideEvent(QHideEvent *event)
{
	qDebug("impostaTime::hideEvent()");
	if (dataOra)
		dataOra->hide();
}
