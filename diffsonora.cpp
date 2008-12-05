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
#include "amplificatori.h" // grAmplificatori
#include "xml_functions.h" // getChildren, getTextChild
#include "sorgentiaux.h"
#include "sorgentimedia.h"
#include "sorgentiradio.h"
#include "poweramplifier.h"

#include <openwebnet.h> // class openwebnet

#include <QVariant> // setProperty
#include <QDebug>
#include <QLabel>

#include <assert.h>


AudioSources::AudioSources(QWidget *parent, QDomNode config_node) : sottoMenu(parent, 0, MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE, 1)
{
	loadItems(config_node);
}

void AudioSources::addAmb(QString a)
{
	for (int idx = elencoBanner.count() - 1; idx >= 0; idx--)
		elencoBanner.at(idx)->addAmb(a);
}

void AudioSources::loadItems(QDomNode config_node)
{
	QDomNode item;
	foreach (item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		QString descr = getTextChild(item, "descr");
		QString where = getTextChild(item, "where");
		QString img1 = IMG_PATH + getTextChild(item, "cimg1");
		QString img2 = IMG_PATH + getTextChild(item, "cimg2");
		QString img3 = IMG_PATH + getTextChild(item, "cimg3");
		banner *b = 0;
		bool multi_channel = false;

		switch (id)
		{
		case SORGENTE_AUX:
			b = new sorgente_aux(this, descr, where);
			break;
		case SORGENTE_RADIO:
			b = new banradio(this, where);
			break;
		case SORGENTE_MULTIM:
			b = new BannerSorgenteMultimedia(this, where, QString(where.at(2)).toInt(), 4);
			break;
		case SORG_RADIO:
			b = new sorgenteMultiRadio(this, where, img1, img2, img3);
			multi_channel = true;
			break;
		case SORG_AUX:
			b = new sorgenteMultiAux(this, descr, where, img1, img2, img3);
			multi_channel = true;
			break;
		case SORGENTE_MULTIM_MC:
			b = new BannerSorgenteMultimediaMC(this, where, where.toInt(), img1, img2, img3);
			multi_channel = true;
			break;
		default:
			// Nothing to do, the other items is not managed by AudioSources.
			break;
		}
		if (b)
		{
			if (multi_channel)
			{
				connect(b, SIGNAL(csxClick()), this, SLOT(goDown()));
				connect(this, SIGNAL(ambChanged(const QString &, bool, QString)),
					b, SLOT(ambChanged(const QString &, bool, QString)));
				connect(b, SIGNAL(active(int, int)), this, SIGNAL(actSrcChanged(int, int)));
			}
			b->setText(descr);
			b->setId(id);
			appendBanner(b);
		}
	}
}


AmpliContainer::AmpliContainer(QWidget *parent, QDomNode config_node) :
	sottoMenu(parent, 3, MAX_WIDTH, MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE + 3, 2)
{
	loadAmplifiers(config_node);
}

void AmpliContainer::loadAmplifiers(QDomNode config_node)
{
	// Amplifiers are items in diffSonora and devices in diffmulti
	QDomNode node;
	foreach (node, getChildren(config_node, "item") + getChildren(config_node, "device"))
	{
		int id = getTextChild(node, "id").toInt();
		QString descr = getTextChild(node, "descr");
		QString where = getTextChild(node, "where");
		QString img1 = IMG_PATH + getTextChild(node, "cimg1");
		QString img2 = IMG_PATH + getTextChild(node, "cimg2");
		QString img3 = IMG_PATH + getTextChild(node, "cimg3");
		QString img4 = IMG_PATH + getTextChild(node, "cimg4");
		QString img5 = IMG_PATH + getTextChild(node, "cimg5");
		banner *b = 0;

		switch (id)
		{
		case AMPLIFICATORE:
			b = new amplificatore(this, where, img1, img2, img3, img4);
			break;
		case POWER_AMPLIFIER:
			b = new PowerAmplifier(this, where, img1, img2, img3, img4, img5);
			break;
		case GR_AMPLIFICATORI:
		{
			// TODO: In diffmulti ci sono piu' descr.. prendo la prima.. verificare che vada bene!
			QList<QDomNode> l = getChildren(node, "descr");
			if (!l.isEmpty())
				descr = l.at(0).toElement().text();

			// in diffmulti the list of where is in the form:
			// <element1><where>..</where></element1><element2>..</element2>..
			// in diffsonora in the form:
			// <where1>..</where1><where2>..</where2>..
			QList<QString> addresses;
			QDomNode a;
			foreach (a, getChildren(node, "element"))
				addresses.append(getTextChild(a, "where"));

			if (addresses.isEmpty())
				foreach (a, getChildren(node, "where"))
				{
					QStringList l = a.toElement().text().split(',');
					for (int i = 0; i < l.size(); ++i)
						addresses.append(l.at(i));
				}

			b = new grAmplificatori(this, addresses, img1, img2, img3, img4);
			break;
		}
		default: // Nothing to do, in diffSonora there are other items managed by AudioSources class.
			break;
		}

		if (b)
		{
			b->setText(descr);
			b->setId(id);
			appendBanner(b);
		}
	}
}


diffSonora::diffSonora(QWidget *parent, AudioSources *s, QDomNode config_node) : QWidget(parent)
{
	init(config_node);
	setSorgenti(s);
}

diffSonora::diffSonora(QWidget *parent, QDomNode config_node) : QWidget(parent)
{
	init(config_node);
	AudioSources *s = new AudioSources(this, config_node);
	connect(s, SIGNAL(Closed()), SLOT(fineVis()));
	setSorgenti(s);
}

void diffSonora::init(QDomNode config_node)
{
	numRighe = NUM_RIGHE;
	// TODO: verificare questo parametro, che prima non era inizializzato, a che valore
	// deve essere inizializzato
	isVisual = false;

	amplificatori = new AmpliContainer(this, config_node);
	connect(amplificatori, SIGNAL(Closed()), SLOT(fineVis()));
	connect(this, SIGNAL(gesFrame(char *)), amplificatori, SIGNAL(gestFrame(char *)));

	QLabel *linea = new QLabel(this);
	linea->setGeometry(0, MAX_HEIGHT/NUM_RIGHE, MAX_WIDTH, 3);
	linea->setProperty("noStyle", true);
}

void diffSonora::setSorgenti(AudioSources *s)
{
	sorgenti = s;
	setGeom(0, 0, MAX_WIDTH, MAX_HEIGHT);
	connect(this, SIGNAL(gesFrame(char *)), sorgenti, SIGNAL(gestFrame(char *)));
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
