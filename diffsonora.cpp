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

#include <QCursor>

#include <stdlib.h>


diffSonora::diffSonora(QWidget *parent, sottoMenu *_sorgenti) : QWidget(parent), sorgenti(NULL)
{
	numRighe = NUM_RIGHE;
	// TODO: verificare questo parametro, che prima non era inizializzato, a che valore
	// deve essere inizializzato
	isVisual = false;

	amplificatori = new sottoMenu(this, "Amplificatori", 3, MAX_WIDTH, MAX_HEIGHT-MAX_HEIGHT/numRighe, 2);

	if (!_sorgenti)
	{
		_sorgenti = new sottoMenu(this, "Sorgenti", 0, MAX_WIDTH, MAX_HEIGHT/numRighe, 1);
		connect(_sorgenti, SIGNAL(Closed()), SLOT(fineVis()));
	}

	setSorgenti(_sorgenti);
	connect(amplificatori, SIGNAL(Closed()), SLOT(fineVis()));
	BtLabel *linea = new BtLabel(this);

	linea->setGeometry(0,77,240,3);
	linea->setStyleSheet("QLabel {background-color:#FFFFFF;}");

	connect(this,SIGNAL(gesFrame(char *)),amplificatori,SIGNAL(gestFrame(char *)));
	connect(amplificatori,SIGNAL(sendFrame(char*)),this , SIGNAL(sendFrame(char*)));
	connect(amplificatori,SIGNAL(sendInit(char*)),this , SIGNAL(sendInit(char*)));
	connect(this,SIGNAL(freezed(bool)),amplificatori,SLOT(freezed(bool)));
	qDebug("diffSonora::diffSonora() END");
}

void diffSonora::setSorgenti(sottoMenu *s)
{
	sorgenti = s;
	setGeom(0, 0, MAX_WIDTH, MAX_HEIGHT);
	connect(this, SIGNAL(gesFrame(char *)), sorgenti, SIGNAL(gestFrame(char *)));
	connect(sorgenti, SIGNAL(sendFrame(char*)), this, SIGNAL(sendFrame(char*)));
	connect(sorgenti, SIGNAL(sendInit(char*)), this, SIGNAL(sendInit(char*)));
	connect(this, SIGNAL(freezed(bool)), sorgenti, SLOT(freezed(bool)));
}

// TODO: sarebbe meglio che icon_names diventasse una QList<QString>.. al momento
// e' l'xmlconfhandler che "detiene" l'ownership delle stringhe contenute nella
// lista.. anche se poi a tutti gli effetti non le cancella.
// In ogni caso sarebbe da allocare staticamente le stringhe che fanno parte di
// icon_names.

int diffSonora::addItemU(char tipo, const QString & qdescrizione, void* indirizzo,
	QList<QString*> &icon_names, int modo, int where, char *ambdescr)
{
	if  (tipo == SORGENTE_AUX || tipo == SORGENTE_RADIO || tipo == SORGENTE_MULTIM)
	{
		sorgenti->addItemU(tipo, qdescrizione, indirizzo, icon_names, modo, where);
	}
	else if (tipo == SORG_RADIO || tipo == SORG_AUX || tipo == SORGENTE_MULTIM_MC)
	{
		sorgenti->addItemU(tipo, qdescrizione, indirizzo, icon_names, modo, 0, QColor(0,0,0), ambdescr);
		banner *b = sorgenti->getLast();
		connect(b, SIGNAL(csxClick()), sorgenti, SLOT(goDown()));
	}
	else
		amplificatori->addItemU(tipo, qdescrizione, indirizzo, icon_names);
	return 1;
}

void diffSonora::setNumRighe(uchar n)
{
	numRighe = n;
	qDebug("Invoking amplificatori->setNumRighe(%d)", n-2);
	amplificatori->setNumRighe(n-2);
	qDebug("sorgenti = %p", sorgenti);
	sorgenti->setNumRighe(1);
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
				QByteArray buf = QString::number(w+100, 10).toAscii();
				sorgenti->setIndex(buf.data());
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
			QByteArray buf = QString::number(w+100, 10).toAscii();
			sorgenti->setIndex(buf.data());
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
	isVisual = f ? false : true;
	if (isHidden())
		isVisual = false;
}

void diffSonora::show()
{
	qDebug("diffSonora::show()");
	openwebnet msg_open;
	sorgenti->forceDraw();
	amplificatori->forceDraw();
	isVisual = true;

	sorgenti->show();
	if (amplificatori)
		amplificatori->show();

	QWidget::show();
}

void diffSonora::draw()
{
	sorgenti->draw();
	if (amplificatori)
		amplificatori->draw();
}

void diffSonora::forceDraw()
{
	qDebug("diffSonora::forceDraw()");
	sorgenti->forceDraw();
	if (amplificatori)
		amplificatori->forceDraw();
}

void diffSonora::hide()
{
	qDebug("diffSonora::hide()");

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
	qDebug("sorgenti->setGeometry(%d, %d, %d, %d)", x, 0, w, h/numRighe);
	sorgenti->setGeometry(x,0,w,h/numRighe);
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
	isVisual = false;
	emit Closed();
	emit closed(this);
}

void diffSonora::setFirstSource(int addr)
{
	qDebug("diffSonora::setFirstSource(%d)", addr);
	QByteArray buf = QString::number(addr, 10).toAscii();
	sorgenti->setIndex(buf.data());
}
