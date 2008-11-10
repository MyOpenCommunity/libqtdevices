/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** diffmulti.cpp
**
** Pagina sottomenu diffusione multicanale
**
****************************************************************/

#include "diffmulti.h"
#include "ambdiffson.h"
#include "device_cache.h"
#include "device.h"
#include "generic_functions.h" // safeAt
#include "sveglia.h"
#include "diffsonora.h"
#include "scenevocond.h"

#include <QTimer>
#include <QPoint>
#include <QDebug>

#include <assert.h>

dati_ampli_multi::dati_ampli_multi(char t, QList<QString*> *d, char *ind,
		int p1, QString _I1, QString _I2, QString _I3, QString _I4, QString _I5)
{
	init(t, d, p1, _I1, _I2, _I3, _I4, _I5);
	qDebug() << "dati_ampli_multi: descr = " << *descr->at(0);

	if (t == AMPLIFICATORE || t == POWER_AMPLIFIER)
	{
		if (t == AMPLIFICATORE)
			qDebug("Amplificatore (%s)", (char *)ind);
		else
			qDebug("Power amplifier (%s)", (char *)ind);
		indirizzo = new(char[20]);
		memcpy(indirizzo, ind, 20);
	}
	else
		assert(!"Unknown amplifier type!!");
}

dati_ampli_multi::dati_ampli_multi(char t, QList<QString*> *d, QList<QString *> *ind,
		int p1, QString _I1, QString _I2, QString _I3, QString _I4, QString _I5)
{
	init(t, d, p1, _I1, _I2, _I3, _I4, _I5);
	qDebug() << "dati_ampli_multi: descr = " << *descr->at(0);
	if (t == GR_AMPLIFICATORI)
	{
		qDebug("gruppo AMPLIFICATORI !!");
		QList<QString*>* tmp_indirizzo = new QList<QString*>(*ind);
		indirizzo = tmp_indirizzo;
		qDebug("indirizzo = %p", indirizzo);
		for (int i = 0; i < tmp_indirizzo->size(); ++i)
			qDebug() << "INDIRIZZO = " << *tmp_indirizzo->at(i);
	}
	else
		assert(!"Unknown amplifier type!!");
}

void dati_ampli_multi::init(char t, QList<QString *> *d, int p1,
		QString _I1, QString _I2, QString _I3, QString _I4, QString _I5)
{
	tipo = t;
	// TODO: allocare nello stack!
	descr = new QList<QString*>(*d);
	I1 = _I1;
	I2 = _I2;
	I3 = _I3;
	I4 = _I4;
	I5 = _I5;
	modo = p1;
}

dati_ampli_multi::~dati_ampli_multi()
{
	delete descr;
	// We should delete indirizzo but is problematic because it is a void*
}


diffmulti::diffmulti(QWidget *parent, const char *name, uchar navBarMode,int wi,int hei, uchar n)
	: sottoMenu(parent, name, navBarMode, wi, hei, n)
{
	sorgenti = new sottoMenu(this,"Sorgenti",0,MAX_WIDTH, MAX_HEIGHT/4 - 3,1);
	matr = btouch_device_cache.get_sound_matr_device();

	// Get status changed events back
	connect(matr, SIGNAL(status_changed(QList<device_status*>)),
		this, SLOT(status_changed(QList<device_status*>)));
}

diffmulti::~diffmulti()
{
	while (!datimmulti.isEmpty())
		delete datimmulti.takeFirst();

	while (!dslist.isEmpty())
		delete dslist.takeFirst();
}

int diffmulti::addItem(char tipo,  QList<QString*> *descrizioni, char* indirizzo, QList<QString*> &icon_names,int modo, int numFrame)
{
	banner *b;
	switch (tipo)
	{
		/** WARNING SORG_RADIO and SORG_AUX are multichannel sources.
		 *  the not-multichannel are called SORGENTE_RADIO and SORGENTE_AUX (!)
		 *
		 *  So we added here the case for our new source that is SORGENTE_MULTIM_MC
		 */
	case SORGENTE_MULTIM_MC:
		qDebug("diffmulti::additem -> Entering SORGENTE_MULTIM_MC case...");
		// Kemosh FIX: store indirizzo to emit the proper init frame
		_where_address = numFrame;
	case SORG_RADIO:
		qDebug("diffmulti::additem -> Entering SORG_RADIO case...");
	case SORG_AUX:
		qDebug("diffmulti::additem -> Entering SORG_AUX case...");
		qDebug() << "Source (" << (int)tipo << ", " << *descrizioni->at(0) << "): appending to source list";
		qDebug("sorgenti->addItem (%p)", sorgenti);
		/*
		 * NOTE: numFrame parametere name is not significative: it's the (cut down) where address.
		 */
		sorgenti->addItemU(tipo, *descrizioni->at(0), (void *)indirizzo, icon_names, 0, numFrame);
		b = sorgenti->getLast();
		connect(b, SIGNAL(csxClick()), sorgenti, SLOT(goUp()));
		connect(sorgenti, SIGNAL(ambChanged(const QString &, bool, char *)),b, SLOT(ambChanged(const QString &, bool, char *)));
		connect(b, SIGNAL(active(int, int)), this, SIGNAL(actSrcChanged(int, int)));
		connect(this,SIGNAL(gesFrame(char *)),b,SLOT(gestFrame(char *)));
		connect(b,SIGNAL(sendInit(char*)),this, SIGNAL(sendInit(char*)));
		break;

	// TODO: codice duplicato da sotto, da eliminare quanto prima
	case INSIEME_AMBIENTI:
	case AMBIENTE:
		{
			// Do not create "sorgenti" submenu
			diffSonora *ds = new diffSonora(NULL, sorgenti);
			connect(ds, SIGNAL(closed(diffSonora*)), this, SLOT(ds_closed(diffSonora*)));
			connect(ds, SIGNAL(closed(diffSonora*)), this, SIGNAL(dsClosed()));
			ds->draw();
			banner *b;
			if (tipo == AMBIENTE)
			{
				b = new ambDiffSon(this, *descrizioni->at(0), (char *)indirizzo,
						*safeAt(icon_names, 0), *safeAt(icon_names, 1), *safeAt(icon_names, 2),
						&datimmulti, ds, sorgenti, this);
			}
			else
			{
				b = new insAmbDiffSon(this, descrizioni, indirizzo, *safeAt(icon_names, 0),
						*safeAt(icon_names, 1), &datimmulti, ds,  sorgenti, this);
			}
			elencoBanner.append(b);
			dslist.append(ds);
			banner *last = elencoBanner.last();
			last->SetTextU(*(descrizioni->at(0)));
			last->setId(tipo);
			connect(this, SIGNAL(gestFrame(char*)), last, SLOT(gestFrame(char*)));
			connect(this, SIGNAL(actSrcChanged(int, int)), last, SLOT(actSrcChanged(int, int)));
			connect(last, SIGNAL(svegl(bool)), this , SIGNAL(svegl(bool)));
			connect(last, SIGNAL(killMe(banner*)), this, SLOT(killBanner(banner*)));
			connect(last, SIGNAL(ambChanged(const QString &, bool, char *)), sorgenti, SIGNAL(ambChanged(const QString &, bool, char *)));
			if (tipo == AMBIENTE)
				sorgenti->addAmb((char *)indirizzo);

			while (!datimmulti.isEmpty())
				delete datimmulti.takeFirst();
			draw();
			break;
		}

	case AMPLIFICATORE:
		qDebug() << "Icone = " << *safeAt(icon_names, 0) << " - " << *safeAt(icon_names, 1)
			<< " - "<< *safeAt(icon_names, 2) << " - " << *safeAt(icon_names, 3);
		datimmulti.append(new dati_ampli_multi(tipo, descrizioni, indirizzo, modo,
					*safeAt(icon_names, 0), *safeAt(icon_names, 1),
					*safeAt(icon_names, 2), *safeAt(icon_names, 3)));
		break;

	case POWER_AMPLIFIER:
		qDebug() << "Icone Power Multi = " << *safeAt(icon_names, 0) << " - " << *safeAt(icon_names, 1)
			<< " - "<< *safeAt(icon_names, 2) << " - " << *safeAt(icon_names, 3) << " - "
			<< *safeAt(icon_names, 4);

		datimmulti.append(new dati_ampli_multi(tipo, descrizioni, indirizzo, modo,
					*safeAt(icon_names, 0), *safeAt(icon_names, 1),
					*safeAt(icon_names, 2), *safeAt(icon_names, 3),
					*safeAt(icon_names, 4)));
		break;

	default:
		addItemU(tipo, *descrizioni->at(0),indirizzo, icon_names, modo, numFrame);
		break;
	}
	return 1;
}

int diffmulti::addItem(char tipo,  QList<QString*> *descrizioni, QList<QString *> *indirizzo, QList<QString*> &icon_names,int modo, int numFrame)
{
	qDebug("diffmulti::addItem (%d)", tipo);
	qDebug("Amplificatore (%s)", (char *)indirizzo);
	qDebug("%d elementi in descrizioni", descrizioni->count());

	switch (tipo)
	{
	case INSIEME_AMBIENTI:
	case AMBIENTE:
		{
			// Do not create "sorgenti" submenu
			diffSonora *ds = new diffSonora(NULL, sorgenti);
			connect(ds, SIGNAL(closed(diffSonora*)), this, SLOT(ds_closed(diffSonora*)));
			connect(ds, SIGNAL(closed(diffSonora*)), this, SIGNAL(dsClosed()));
			ds->draw();
			banner *b;
			if (tipo == AMBIENTE)
			{
				b = new ambDiffSon(this, *descrizioni->at(0), (char *)indirizzo,
						*safeAt(icon_names, 0), *safeAt(icon_names, 1), *safeAt(icon_names, 2),
						&datimmulti, ds, sorgenti, this);
			}
			else
			{
				b = new insAmbDiffSon(this, descrizioni, indirizzo, *safeAt(icon_names, 0),
						*safeAt(icon_names, 1), &datimmulti, ds,  sorgenti, this);
			}
			elencoBanner.append(b);
			dslist.append(ds);
			banner *last = elencoBanner.last();
			last->SetTextU(*(descrizioni->at(0)));
			last->setId(tipo);
			connect(this, SIGNAL(gestFrame(char*)), last, SLOT(gestFrame(char*)));
			connect(this, SIGNAL(actSrcChanged(int, int)), last, SLOT(actSrcChanged(int, int)));
			connect(last, SIGNAL(svegl(bool)), this , SIGNAL(svegl(bool)));
			connect(last, SIGNAL(killMe(banner*)), this, SLOT(killBanner(banner*)));
			connect(last, SIGNAL(ambChanged(const QString &, bool, char *)), sorgenti, SIGNAL(ambChanged(const QString &, bool, char *)));
			if (tipo == AMBIENTE)
				sorgenti->addAmb((char *)indirizzo);

			while (!datimmulti.isEmpty())
				delete datimmulti.takeFirst();
			draw();
			break;
		}
	case GR_AMPLIFICATORI:
		{
			qDebug("Gruppo amplificatori");
			QList<QString*> *indirizzi = (QList<QString*> *)indirizzo;
			for (int i = 0; i < indirizzi->size(); ++i)
			{
				qDebug() << "ADDRESS =" << *indirizzi->at(i);
			}
			datimmulti.append(new dati_ampli_multi(tipo, descrizioni, indirizzo, modo,
					*safeAt(icon_names, 0), *safeAt(icon_names, 1),
					*safeAt(icon_names, 2), *safeAt(icon_names, 3)));
			break;
		}

	default:
		addItemU(tipo, *descrizioni->at(0),indirizzo, icon_names, modo, numFrame);
		break;
	}
	return 1;
}

void diffmulti::setNavBarMode(uchar a, char* i)
{
	sottoMenu::setNavBarMode(a, i);
	for (int i = 0; i < dslist.size(); ++i)
		dslist.at(i)->setNavBarMode(a);
}

void diffmulti::setNumRighe(uchar n)
{
	sottoMenu::setNumRighe(n);
	for (int i = 0; i < dslist.size(); ++i)
		dslist.at(i)->setNumRighe(n);
}

void diffmulti::reparent(QWidget *parent, Qt::WindowFlags f, const QPoint & p, bool showIt)
{
	sottoMenu::reparent(parent, f, p, showIt);
	for (int i = 0; i < dslist.size(); ++i)
	{
		diffSonora *ds = dslist.at(i);
		ds->setParent(!parent ? parent : this);
		ds->setWindowFlags(f);
		ds->move(p);
	}
}

void diffmulti::inizializza()
{
	qDebug("diffmulti::inizializza()");
	sorgenti->inizializza();
	matr->init();
}

void diffmulti::ds_closed(diffSonora *ds)
{
	qDebug("diffmulti::ds_closed()");
	ds->hide();

	for (int i = 0; i < elencoBanner.size(); ++i)
	{
		elencoBanner.at(i)->Draw();
		elencoBanner.at(i)->show();
	}
	forceDraw();
	showFullScreen();
}

void diffmulti::hideEvent(QHideEvent *event)
{
	for (int i = 0; i < dslist.size(); ++i)
		dslist.at(i)->hide();
}

void diffmulti::setGeom(int x, int y, int w,int h)
{
	qDebug("diffmulti::setGeom(%d, %d, %d, %d)", x, y, w, h);
	setGeometry(x, y, w, h);
}

void diffmulti::forceDraw()
{
	qDebug("diffmulti::forceDraw()");
	sottoMenu::forceDraw();
}

void diffmulti::resizewindows(int x, int y, int w, int h)
{
	for (int i = 0; i < dslist.size(); ++i)
	{
		diffSonora *ds = dslist.at(i);
		ds->setGeom(x, y, w, h);
		ds->forceDraw();
	}
}

void diffmulti::ripristinaRighe(void)
{
	sottoMenu::setNumRighe(3);

	for (int i = 0; i < dslist.size(); ++i)
		dslist.at(i)->setNumRighe(4);
}

void diffmulti::status_changed(QList<device_status*> sl)
{
	stat_var curr_act(stat_var::ACTIVE_SOURCE);
	qDebug("diffmulti::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::SOUNDMATR:
			for (int i=device_status_sound_matr::AMB1_INDEX;
					i<=device_status_sound_matr::AMB8_INDEX; i++)
			{
				ds->read(i, curr_act);
				qDebug("Curr active source for amb %d is now %d", i+1, curr_act.get_val());
				emit actSrcChanged(i+1, curr_act.get_val());
			}
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}
}

void diffmulti::gestFrame(char*frame)
{
	emit gesFrame(frame);
}

// contdiff implementation
contdiff::contdiff(diffSonora *_ds, diffmulti *_dm) : QObject()
{
	qDebug("contdiff::contdiff(ds = %p, dm = %p)", _ds, _dm);
	ds = _ds;
	dm = _dm;
	if (ds)
		connect(ds, SIGNAL(Closed()), this, SIGNAL(Closed()));
	if (dm)
		connect(dm, SIGNAL(Closed()), this, SIGNAL(Closed()));
}

void contdiff::reparent(QWidget *parent, unsigned int f, QPoint point, bool showIt)
{
	qDebug("contdiff::reparent()");
	
	if (ds)
	{
		ds->setParent(parent);
		ds->setWindowFlags((Qt::WindowFlags)f);
		ds->move(point);
		if (showIt)
			ds->show();
	}
	if (dm)
		dm->reparent(parent, (Qt::WindowFlags)f, point, showIt);
}

void contdiff::setNavBarMode(uchar m)
{
	if (ds)
		ds->setNavBarMode(m);
	if (dm)
		dm->setNavBarMode(m);
}

void contdiff::setNumRighe(uchar n)
{
	if (ds)
		ds->setNumRighe(n);
	if (dm)
		dm->setNumRighe(n);
}

void contdiff::setGeom(int x, int y, int w, int h)
{
	qDebug("contdiff::setGeom(%d, %d, %d, %d)", x, y, w, h);
	if (ds)
		ds->setGeom(x, y, w, h);
	if (dm)
		dm->setGeom(x, y, w, h);
}

void contdiff::forceDraw()
{
	if (ds)
		ds->forceDraw();
	if (dm)
		dm->forceDraw();
}

void contdiff::hide()
{
	if (ds)
		ds->hide();
	if (dm)
		dm->hide();
}

void contdiff::show()
{
	if (ds)
		ds->show();
	if (dm)
		dm->show();
}

void contdiff::ripristinaRighe()
{
	if (ds)
		ds->setNumRighe((uchar)4);
	if (dm)
		dm->ripristinaRighe();
}

void contdiff::resizewindows()
{
	if (ds)
		ds->setGeom(0, 80, 240, 240);
	if (dm)
	{
		dm->setGeom(0, 80, 240, 240);
		dm->resizewindows(0, 0, 240, 240);
	}
}

void contdiff::restorewindows()
{
	if (dm)
		dm->resizewindows(0, 0, 240, 320);
}

void contdiff::connectClosed(sveglia *s)
{
	disconnect(this, SIGNAL(Closed()), s, SLOT(Closed()));
	connect(this, SIGNAL(Closed()), s, SLOT(Closed()));
	if (dm)
		connect(dm, SIGNAL(dsClosed()), s, SLOT(Closed()));
}

void contdiff::disconnectClosed(sveglia *s)
{
	if (dm)
		disconnect(dm, SIGNAL(dsClosed()), s, SLOT(Closed()));
}
