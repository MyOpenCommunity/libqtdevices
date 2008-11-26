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
#include "btmain.h"
#include "main.h" // BTouch

#include <openwebnet.h> // class openwebnet

#include <QCursor>
#include <QVariant> // setProperty
#include <QLabel>

#include <assert.h>

diffSonora::diffSonora(QWidget *parent, sottoMenu *_sorgenti) : QWidget(parent), sorgenti(NULL)
{
	numRighe = NUM_RIGHE;
	// TODO: verificare questo parametro, che prima non era inizializzato, a che valore
	// deve essere inizializzato
	isVisual = false;

	amplificatori = new sottoMenu(this, 3, MAX_WIDTH, MAX_HEIGHT-MAX_HEIGHT/numRighe, 2);

	if (!_sorgenti)
	{
		_sorgenti = new sottoMenu(this, 0, MAX_WIDTH, MAX_HEIGHT/numRighe, 1);
		connect(_sorgenti, SIGNAL(Closed()), SLOT(fineVis()));
	}

	setSorgenti(_sorgenti);
	connect(amplificatori, SIGNAL(Closed()), SLOT(fineVis()));
	QLabel *linea = new QLabel(this);

	linea->setGeometry(0,77,240,3);
	linea->setProperty("noStyle", true);

	connect(this,SIGNAL(gesFrame(char *)),amplificatori,SIGNAL(gestFrame(char *)));
}

void diffSonora::setSorgenti(sottoMenu *s)
{
	sorgenti = s;
	setGeom(0, 0, MAX_WIDTH, MAX_HEIGHT);
	connect(this, SIGNAL(gesFrame(char *)), sorgenti, SIGNAL(gestFrame(char *)));
}

// TODO: sarebbe meglio che icon_names diventasse una QList<QString>.. al momento
// e' l'xmlconfhandler che "detiene" l'ownership delle stringhe contenute nella
// lista.. anche se poi a tutti gli effetti non le cancella.
// In ogni caso sarebbe da allocare staticamente le stringhe che fanno parte di
// icon_names.
int diffSonora::addItemU(char tipo, const QString &description, QList<QString *> *indirizzo,
	QList<QString*> &icon_names)
{
	if (tipo == GR_AMPLIFICATORI)
	{
		// TODO: rimuovere questa conversione, modificando l'xmlconfhandler!
		QList <QString> ind;
		for (int i = 0; i < indirizzo->size(); ++i)
			ind.append(*indirizzo->at(i));

		amplificatori->addItemU(tipo, description, ind, icon_names);
		return 1;
	}
	else
		assert(!"difSonora::addItemU() called with type != GR_AMPLIFICATORI");
}

int diffSonora::addItemU(char tipo, const QString &description, char* indirizzo,
	QList<QString*> &icon_names, int modo, int where, const char *ambdescr)
{
	// TODO: trasformare ambdescr in qstring o almeno in un const char*!
	switch (tipo)
	{
	case SORGENTE_AUX:
	case SORGENTE_RADIO:
	case SORGENTE_MULTIM:
		sorgenti->addItemU(tipo, description, (void *)indirizzo, icon_names, modo, where, const_cast<char*>(ambdescr));
		break;
	case SORG_RADIO:
	case SORG_AUX:
	case SORGENTE_MULTIM_MC:
	{
		sorgenti->addItemU(tipo, description, (void *)indirizzo, icon_names, modo, 0, const_cast<char*>(ambdescr));
		banner *b = sorgenti->getLast();
		connect(b, SIGNAL(csxClick()), sorgenti, SLOT(goDown()));
		break;
	}
	case AMPLIFICATORE:
		amplificatori->addItemU(tipo, description, (void *)indirizzo, icon_names);
		break;
	default:
		qWarning("%s: Adding an unknown type to sottoMenu amplificatori", __FILE__);
		amplificatori->addItemU(tipo, description, (void *)indirizzo, icon_names);
	}
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
	BTouch->sendInit("*16*53*100##");
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
				sorgenti->setIndex(QString::number(w+100));
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
			sorgenti->setIndex(QString::number(w+100));
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

// TODO: capire il significato di questa gestione particolare del 'isVisual'!!
/*
void diffSonora::freezed_handler(bool f)
{
	qDebug("diffSonora::freezed(%d)", f);
	isVisual = f ? false : true;
	if (isHidden())
		isVisual = false;
}
*/

void diffSonora::showEvent(QShowEvent *event)
{
	qDebug("diffSonora::showEvent()");
	openwebnet msg_open;
	sorgenti->forceDraw();
	amplificatori->forceDraw();
	isVisual = true;

	sorgenti->show();
}

void diffSonora::draw()
{
	sorgenti->draw();
	amplificatori->draw();
}

void diffSonora::forceDraw()
{
	qDebug("diffSonora::forceDraw()");
	sorgenti->forceDraw();
	amplificatori->forceDraw();
}

void diffSonora::hideEvent(QHideEvent *event)
{
	qDebug("diffSonora::hideEvent()");

	sorgenti->hide();
	amplificatori->setIndice(0);
	isVisual = false;
}

void diffSonora::setGeom(int x,int y,int w,int h)
{
	qDebug("diffSonora::setGeom(%d, %d, %d, %d, numRighe = %d)",x, y, w, h, numRighe);
	QWidget::setGeometry(x, y, w, h);
	qDebug("sorgenti->setGeometry(%d, %d, %d, %d)", x, 0, w, h/numRighe);
	sorgenti->setGeometry(x,0,w,h/numRighe);
	qDebug("amplificatori->setGeometry(%d, %d, %d, %d)", x, h/numRighe,w,h/numRighe*(numRighe-1));
	amplificatori->setGeometry(x,h/numRighe,w,h/numRighe*(numRighe-1));
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
	sorgenti->setIndex(QString::number(addr));
}
