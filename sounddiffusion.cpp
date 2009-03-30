/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** sounddiffusion.cpp
 **
 **Sottomenù diffusione sonora
 **
 ****************************************************************/

#include "sounddiffusion.h"
#include "sottomenu.h"
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


AudioSources::AudioSources(QWidget *parent, const QDomNode &config_node) : sottoMenu(parent, 0, MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE, 1)
{
	loadItems(config_node);
}

void AudioSources::addAmb(QString a)
{
	for (int idx = elencoBanner.count() - 1; idx >= 0; idx--)
		elencoBanner.at(idx)->addAmb(a);
}

void AudioSources::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
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
			b = new BannerSorgenteMultimedia(this, item, where, QString(where.at(2)).toInt(), 4);
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
			b = new BannerSorgenteMultimediaMC(this, item, where, where.toInt(), img1, img2, img3);
			multi_channel = true;
			break;
		default:
			// Nothing to do, the other items is not managed by AudioSources.
			break;
		}
		if (b)
		{
			connect(b, SIGNAL(pageClosed()), SLOT(showSoundDiff()));
			if (multi_channel)
			{
				connect(b, SIGNAL(csxClick()), this, SLOT(goUp()));
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

void AudioSources::showSoundDiff()
{
	// The parent is always the SoundDiffusion instance (in MultiSoundDiff
	// AudioSources is reparented in "configura()")
	Page *parent = static_cast<Page*>(parentWidget());
	parent->showPage();
}

void AudioSources::setIndex(QString addr)
{
	for (int i = 0; i < elencoBanner.size(); ++i)
	{
		if (elencoBanner.at(i)->getAddress() == addr)
		{
			elencoBanner.at(i)->mostra(banner::BUT2);
			indice = i;
		}
		else
			elencoBanner.at(i)->nascondi(banner::BUT2);
	}
}

void AudioSources::mostra_all(char but)
{
	for (int i = 0; i < elencoBanner.size(); ++i)
		elencoBanner.at(i)->mostra(but);
}


AmpliContainer::AmpliContainer(QWidget *parent, const QDomNode &config_node) :
	sottoMenu(parent, 3, MAX_WIDTH, MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE + 3, 2)
{
	loadAmplifiers(config_node);
}

void AmpliContainer::showSoundDiff()
{
	// The parent is always the SoundDiffusion instance
	Page *parent = static_cast<Page*>(parentWidget());
	parent->showPage();
}

void AmpliContainer::loadAmplifiers(const QDomNode &config_node)
{
	// Amplifiers are items in SoundDiffusion and devices in MultiSoundDiff
	foreach (const QDomNode &node, getChildren(config_node, "item") + getChildren(config_node, "device"))
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
			b = new BannPowerAmplifier(this, node, where);
			connect(b, SIGNAL(pageClosed()), SLOT(showSoundDiff()));
			break;
		case GR_AMPLIFICATORI:
		{
			// TODO: In MultiSoundDiff ci sono piu' descr.. prendo la prima.. verificare che vada bene!
			QList<QDomNode> l = getChildren(node, "descr");
			if (!l.isEmpty())
				descr = l.at(0).toElement().text();

			// in MultiSoundDiff the list of where is in the form:
			// <element1><where>..</where></element1><element2>..</element2>..
			// in SoundDiffusion in the form:
			// <where1>..</where1><where2>..</where2>..
			QList<QString> addresses;
			foreach (const QDomNode &a, getChildren(node, "element"))
				addresses.append(getTextChild(a, "where"));

			if (addresses.isEmpty())
				foreach (const QDomNode &a, getChildren(node, "where"))
				{
					QStringList l = a.toElement().text().split(',');
					for (int i = 0; i < l.size(); ++i)
						addresses.append(l.at(i));
				}

			b = new grAmplificatori(this, addresses, img1, img2, img3, img4);
			break;
		}
		default: // Nothing to do, in SoundDiffusion there are other items managed by AudioSources class.
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

void AmpliContainer::setIndice(char c)
{
	if (c <= elencoBanner.count())
		indice = c;
}


SoundDiffusion::SoundDiffusion(AudioSources *s, const QDomNode &config_node)
{
	init(config_node);
	setSorgenti(s);
	shared_audiosources = true;
}

SoundDiffusion::SoundDiffusion(const QDomNode &config_node)
{
	init(config_node);
	setSorgenti(new AudioSources(this, config_node));
	shared_audiosources = false;
}

void SoundDiffusion::init(const QDomNode &config_node)
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

void SoundDiffusion::setSorgenti(AudioSources *s)
{
	sorgenti = s;
	setGeom(0, 0, MAX_WIDTH, MAX_HEIGHT);
	connect(this, SIGNAL(gesFrame(char *)), sorgenti, SIGNAL(gestFrame(char *)));
}

void SoundDiffusion::setNumRighe(uchar n)
{
	numRighe = n;
	qDebug("Invoking amplificatori->setNumRighe(%d)", n-2);
	amplificatori->setNumRighe(n-2);
	qDebug("sorgenti = %p", sorgenti);
	sorgenti->setNumRighe(1);
	sorgenti->draw();
	amplificatori->draw();
}

void SoundDiffusion::inizializza()
{
	amplificatori->inizializza();
	// If the audio sources are shared as in the MultiSoundDiff the initialization is
	// done by the object that instantiate the audio sources.
	if (!shared_audiosources)
		sorgenti->inizializza();
	sendInit("*16*53*100##");
}

void SoundDiffusion::gestFrame(char*frame)
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
void SoundDiffusion::freezed_handler(bool f)
{
	qDebug("SoundDiffusion::freezed(%d)", f);
	isVisual = f ? false : true;
	if (isHidden())
		isVisual = false;
}
*/

void SoundDiffusion::showEvent(QShowEvent *event)
{
	qDebug("SoundDiffusion::showEvent()");
	sorgenti->forceDraw();
	amplificatori->forceDraw();
	isVisual = true;

	sorgenti->show();
}

void SoundDiffusion::draw()
{
	sorgenti->draw();
	amplificatori->draw();
}

void SoundDiffusion::forceDraw()
{
	qDebug("SoundDiffusion::forceDraw()");
	sorgenti->forceDraw();
	amplificatori->forceDraw();
}

void SoundDiffusion::hideEvent(QHideEvent *event)
{
	qDebug("SoundDiffusion::hideEvent()");
	amplificatori->setIndice(0);
	isVisual = false;
}

void SoundDiffusion::setGeom(int x,int y,int w,int h)
{
	qDebug("SoundDiffusion::setGeom(%d, %d, %d, %d, numRighe = %d)",x, y, w, h, numRighe);
	QWidget::setGeometry(x, y, w, h);
	qDebug("sorgenti->setGeometry(%d, %d, %d, %d)", x, 0, w, h/numRighe);
	sorgenti->setGeometry(x,0,w,h/numRighe);
	qDebug("amplificatori->setGeometry(%d, %d, %d, %d)", x, h/numRighe,w,h/numRighe*(numRighe-1));
	amplificatori->setGeometry(x,h/numRighe,w,h/numRighe*(numRighe-1));
}

void SoundDiffusion::setNavBarMode(uchar c)
{
	amplificatori->setNavBarMode(c);
}

void SoundDiffusion::fineVis()
{
	isVisual = false;
	emit Closed();
	emit closed(this);
}

void SoundDiffusion::setFirstSource(int addr)
{
	qDebug("SoundDiffusion::setFirstSource(%d)", addr);
	sorgenti->setIndex(QString::number(addr));
}
