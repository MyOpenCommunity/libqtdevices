/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 **diffSonora.cpp
 **
 **Sottomenù diffusione sonora
 **
 ****************************************************************/

#include "diffsonora.h"
#include "sottomenu.h"
#include "btlabel.h"

#include <openwebnet.h> // class openwebnet

#include <qcursor.h>

#include <stdlib.h>

diffSonora::diffSonora(QWidget *parent, const char *name, bool creasorgenti)
	: QWidget(parent, name)
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	setCursor(QCursor(blankCursor));
#endif
	numRighe = NUM_RIGHE;

	if (creasorgenti)
		sorgenti = new sottoMenu(this, "Sorgenti", 0, MAX_WIDTH, MAX_HEIGHT/numRighe, 1);
	else
		sorgenti = NULL;
	
	amplificatori = new sottoMenu(this, "Amplificatori", 3, MAX_WIDTH, MAX_HEIGHT-MAX_HEIGHT/numRighe,/*numRighe-*/2); 

	if (creasorgenti)
		setGeom(0, 0, MAX_WIDTH, MAX_HEIGHT);

	if (sorgenti)
		connect(sorgenti, SIGNAL(Closed()), this, SLOT(fineVis()));
	connect(amplificatori  ,SIGNAL(Closed()),this,SLOT(fineVis()));
	BtLabel* linea = new BtLabel(this,NULL,0);
	linea->setGeometry(0,77,240,3);
	linea->setPaletteForegroundColor(QColor::QColor(0,0,0));

	if (creasorgenti)
	{
		connect(this,SIGNAL(gesFrame(char *)),sorgenti,SIGNAL(gestFrame(char *)));
		connect(sorgenti,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*)));
		connect(sorgenti,SIGNAL(sendInit(char*)),this , SIGNAL(sendInit(char*)));
		connect(this,SIGNAL(freezed(bool)),sorgenti,SLOT(freezed(bool)));
	}

	connect(this,SIGNAL(gesFrame(char *)),amplificatori,SIGNAL(gestFrame(char *))); 
	connect(amplificatori,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*)));
	connect(amplificatori,SIGNAL(sendInit(char*)),this , SIGNAL(sendInit(char*)));
	connect(this,SIGNAL(freezed(bool)),amplificatori,SLOT(freezed(bool)));
	qDebug("diffSonora::diffSonora() END");
}

void diffSonora::setSorgenti(sottoMenu *s)
{
	sorgenti = s;
	setGeom(0,0,MAX_WIDTH,MAX_HEIGHT);
	connect(this,SIGNAL(gesFrame(char *)),sorgenti,SIGNAL(gestFrame(char *)));
	connect(sorgenti,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*)));
	connect(sorgenti,SIGNAL(sendInit(char*)),this , SIGNAL(sendInit(char*)));
	connect(this,SIGNAL(freezed(bool)),sorgenti,SLOT(freezed(bool)));
}

void diffSonora::setBGColor(int r, int g, int b)
{
	setPaletteBackgroundColor(QColor::QColor(r,g,b));
	if (sorgenti)
		sorgenti->setBGColor(backgroundColor());
	if (amplificatori)
		amplificatori->setBGColor(backgroundColor());
}

void diffSonora::setFGColor(int r, int g, int b)
{
	setPaletteForegroundColor(QColor::QColor(r,g,b));
	if (sorgenti)
		sorgenti->setFGColor(foregroundColor());
	if (amplificatori)
		amplificatori->setFGColor(foregroundColor());
}

int diffSonora::setBGPixmap(char *backImage)
{
	QPixmap Back;
	if (Back.load(backImage))
	{
		setPaletteBackgroundPixmap(Back);
		return 0;
	}
	return 1;
}

int diffSonora::addItemU(char tipo, const QString & qdescrizione, void* indirizzo,
	QPtrList<QString> &icon_names, int modo, int where, char *ambdescr)
{
	if  ((tipo==SORGENTE_AUX) || (tipo==SORGENTE_RADIO) || (tipo==SORGENTE_MULTIM))
	{
		sorgenti->setBGColor(backgroundColor());
		sorgenti->setFGColor(foregroundColor());
		sorgenti->addItemU(tipo, qdescrizione, indirizzo, icon_names, modo, where);
	}
	else if ((tipo == SORG_RADIO) || (tipo == SORG_AUX) || (tipo == SORGENTE_MULTIM_MC))
	{
		sorgenti->setBGColor(backgroundColor());
		sorgenti->setFGColor(foregroundColor());
		sorgenti->addItemU(tipo, qdescrizione, indirizzo, icon_names, modo, 0, QColor(0,0,0), ambdescr);
		banner *b = sorgenti->getLast();
		connect(b, SIGNAL(csxClick()), sorgenti, SLOT(goDown()));
	}
	else
	{
		amplificatori->addItemU(tipo, qdescrizione, indirizzo, icon_names);
	}
	return 1;
}

void diffSonora::setNumRighe(uchar n)
{
	numRighe=n;
	qDebug("Invoking amplificatori->setNumRighe(%d)", n-2);
	amplificatori->setNumRighe(n-2);
	qDebug("sorgenti = %p", sorgenti);
	if (sorgenti)
		sorgenti->setNumRighe(1);
	if (sorgenti)
		sorgenti->draw();
	amplificatori->draw();
}

void diffSonora::inizializza()
{
	amplificatori->inizializza();
	sorgenti->inizializza();
	emit sendInit("*16*53*100##");
}

void diffSonora::gestFrame(char*frame)
{
	emit gesFrame(frame);
	openwebnet msg_open;
	char aggiorna;
	int w;

	aggiorna = 0;

	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

	if (!strcmp(msg_open.Extract_chi(),"16"))
	{
		w = strtoul(msg_open.Extract_dove(), NULL, 10);
		if (w < 100)
			goto not_ours;
		while (w >= 100)
			w -= 100;
		while (w >= 10)
			w -= 10;
		{
			if ((!strcmp(msg_open.Extract_cosa(),"0")) || (!strcmp(msg_open.Extract_cosa(),"3")))
			{
				sorgenti->setIndex((char *)QString::number(w+100, 10).ascii());
				aggiorna = 1;
				qDebug("accesa sorg: %d", w);
			}
		}
	}
	else if (!strcmp(msg_open.Extract_chi(),"22"))
	{
		if (!strncmp(msg_open.Extract_cosa(),"2", 1) && (!strcmp(msg_open.Extract_dove(),"5") &&
			(!strcmp(msg_open.Extract_livello(),"2"))))
		{
			w = strtoul(msg_open.Extract_interfaccia(), NULL, 10);
			sorgenti->setIndex((char *)QString::number(w+100, 10).ascii());
			aggiorna = 1;
			qDebug("accesa sorg(WHO=22): %d", w);
		}
	}

not_ours:
	if (aggiorna)
	{
		sorgenti->draw();
		if (isVisual)
		{
			QWidget::show();
		}
	}
}

void diffSonora::freezed_handler(bool f)
{
	qDebug("diffSonora::freezed(%d)", f);
	isVisual = f ? 0 : 1;
	if (isHidden())
		isVisual = false;
}

void diffSonora::show()
{
	openwebnet msg_open;
	sorgenti->forceDraw();
	amplificatori->forceDraw();
	isVisual = 1;

	if (sorgenti)
		sorgenti->show();
	if (amplificatori)
		amplificatori->show();

	QWidget::show();
}

void diffSonora::draw()
{
	if (sorgenti)
		sorgenti->draw();
	if (amplificatori)
		amplificatori->draw();
}

void diffSonora::forceDraw()
{
	qDebug("diffSonora::forceDraw()");
	if (sorgenti)
		sorgenti->forceDraw();
	if (amplificatori)
		amplificatori->forceDraw();
}

void diffSonora::hide()
{
	qDebug("diffSonora::hide()");

	if (sorgenti)
		sorgenti->hide(false);
	if (amplificatori)
		amplificatori->hide();
	if (amplificatori)
		amplificatori->setIndice(0);
	isVisual = false;
	QWidget::hide();
}

void diffSonora::setGeom(int x,int y,int w,int h)
{
	qDebug("diffSonora::setGeom(%d, %d, %d, %d, numRighe = %d)",x, y, w, h, numRighe);
	QWidget::setGeometry(x, y, w, h);
	if (sorgenti)
	{
		qDebug("sorgenti->setGeometry(%d, %d, %d, %d)", x, 0, w, h/numRighe);
		sorgenti->setGeometry(x,0,w,h/numRighe);
	}
	if (amplificatori)
	{
		qDebug("amplificatori->setGeometry(%d, %d, %d, %d)", x, h/numRighe,w,h/numRighe*(numRighe-1));
		amplificatori->setGeometry(x,h/numRighe,w,h/numRighe*(numRighe-1));
	}
}

void diffSonora::setNavBarMode(uchar c)
{
	amplificatori->setNavBarMode(c);
}

void diffSonora::fineVis()
{
	isVisual=0;
	emit(Closed());
	emit(closed(this));
}

void diffSonora::setFirstSource(int addr)
{
	qDebug("diffSonora::setFirstSource(%d)", addr);
	sorgenti->setIndex((char *)QString::number(addr, 10).ascii());
}
