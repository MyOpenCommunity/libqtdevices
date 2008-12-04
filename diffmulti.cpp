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
#include "btmain.h"
#include "main.h" // BTouch

#include <QTimer>
#include <QPoint>
#include <QDebug>

#include <assert.h>

dati_ampli_multi::dati_ampli_multi(char t, QString d, char *ind,
		int p1, QString _I1, QString _I2, QString _I3, QString _I4, QString _I5)
{
	init(t, d, p1, _I1, _I2, _I3, _I4, _I5);

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

dati_ampli_multi::dati_ampli_multi(char t, QString d, QList<QString *> *ind,
		int p1, QString _I1, QString _I2, QString _I3, QString _I4, QString _I5)
{
	init(t, d, p1, _I1, _I2, _I3, _I4, _I5);
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

void dati_ampli_multi::init(char t, QString d, int p1,
		QString _I1, QString _I2, QString _I3, QString _I4, QString _I5)
{
	tipo = t;
	descr = d;
	I1 = _I1;
	I2 = _I2;
	I3 = _I3;
	I4 = _I4;
	I5 = _I5;
	modo = p1;
	qDebug() << "dati_ampli_multi: descr = " << descr;
}

dati_ampli_multi::~dati_ampli_multi()
{
	// We should delete indirizzo but is problematic because it is a void*
}


diffmulti::diffmulti(QWidget *parent, QDomNode config_node) : sottoMenu(parent, 3, MAX_WIDTH, MAX_HEIGHT, NUM_RIGHE-1)
{
	sorgenti = new AudioSources(this, config_node);
	connect(this, SIGNAL(gesFrame(char *)), sorgenti, SIGNAL(gestFrame(char *)));
	connect(sorgenti, SIGNAL(actSrcChanged(int, int)), this, SIGNAL(actSrcChanged(int, int)));

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

int diffmulti::addItem(char tipo,  QString descrizione, char* indirizzo, QList<QString*> &icon_names,int modo, int numFrame)
{
	switch (tipo)
	{
	case SORGENTE_MULTIM_MC:
	case SORG_RADIO:
	case SORG_AUX:
		break;

	// TODO: codice duplicato da sotto, da eliminare quanto prima
	case INSIEME_AMBIENTI:
	case AMBIENTE:
		{
			// Do not create "sorgenti" submenu
			diffSonora *ds = new diffSonora(0, sorgenti);
			connect(ds, SIGNAL(closed(diffSonora*)), this, SLOT(ds_closed(diffSonora*)));
			connect(ds, SIGNAL(closed(diffSonora*)), this, SIGNAL(dsClosed()));
			ds->draw();
			banner *b;
			if (tipo == AMBIENTE)
			{
				b = new ambDiffSon(this, descrizione, indirizzo,
						*safeAt(icon_names, 0), *safeAt(icon_names, 1), *safeAt(icon_names, 2),
						&datimmulti, ds, sorgenti, this);
			}
			else
			{
				b = new insAmbDiffSon(this, descrizione, *safeAt(icon_names, 0),
						*safeAt(icon_names, 1), &datimmulti, ds,  sorgenti, this);
			}
			elencoBanner.append(b);
			dslist.append(ds);
			banner *last = elencoBanner.last();
			last->setText(descrizione);
			last->setId(tipo);
			connect(this, SIGNAL(gestFrame(char*)), last, SLOT(gestFrame(char*)));
			connect(this, SIGNAL(actSrcChanged(int, int)), last, SLOT(actSrcChanged(int, int)));
			connect(last, SIGNAL(ambChanged(const QString &, bool, QString)), sorgenti, SIGNAL(ambChanged(const QString &, bool, QString)));
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
		datimmulti.append(new dati_ampli_multi(tipo, descrizione, indirizzo, modo,
					*safeAt(icon_names, 0), *safeAt(icon_names, 1),
					*safeAt(icon_names, 2), *safeAt(icon_names, 3)));
		break;

	case POWER_AMPLIFIER:
		qDebug() << "Icone Power Multi = " << *safeAt(icon_names, 0) << " - " << *safeAt(icon_names, 1)
			<< " - "<< *safeAt(icon_names, 2) << " - " << *safeAt(icon_names, 3) << " - "
			<< *safeAt(icon_names, 4);

		datimmulti.append(new dati_ampli_multi(tipo, descrizione, indirizzo, modo,
					*safeAt(icon_names, 0), *safeAt(icon_names, 1),
					*safeAt(icon_names, 2), *safeAt(icon_names, 3),
					*safeAt(icon_names, 4)));
		break;

	default:
		addItemU(tipo, descrizione,indirizzo, icon_names, modo, numFrame);
		break;
	}
	return 1;
}

int diffmulti::addItem(char tipo,  QString descrizione, QList<QString *> *indirizzo, QList<QString*> &icon_names,int modo, int numFrame)
{
	// we can never be here with tipo == AMBIENTE (see also xmlconfhandler.cpp and conf.xml spec)
	assert(tipo != AMBIENTE);
	qDebug("diffmulti::addItem (%d)", tipo);
	qDebug("Amplificatore (%p)", indirizzo);

	switch (tipo)
	{
	case INSIEME_AMBIENTI:
		{
			// Do not create "sorgenti" submenu
			diffSonora *ds = new diffSonora(0, sorgenti);
			connect(ds, SIGNAL(closed(diffSonora*)), this, SLOT(ds_closed(diffSonora*)));
			connect(ds, SIGNAL(closed(diffSonora*)), this, SIGNAL(dsClosed()));
			ds->draw();
			banner *b;
			b = new insAmbDiffSon(this, descrizione, *safeAt(icon_names, 0),
					*safeAt(icon_names, 1), &datimmulti, ds,  sorgenti, this);
			elencoBanner.append(b);
			dslist.append(ds);
			banner *last = elencoBanner.last();
			last->setText(descrizione);
			last->setId(tipo);
			connect(this, SIGNAL(gestFrame(char*)), last, SLOT(gestFrame(char*)));
			connect(this, SIGNAL(actSrcChanged(int, int)), last, SLOT(actSrcChanged(int, int)));
			connect(last, SIGNAL(ambChanged(const QString &, bool, QString)), sorgenti, SIGNAL(ambChanged(const QString &, bool, QString)));

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
			datimmulti.append(new dati_ampli_multi(tipo, descrizione, indirizzo, modo,
					*safeAt(icon_names, 0), *safeAt(icon_names, 1),
					*safeAt(icon_names, 2), *safeAt(icon_names, 3)));
			break;
		}

	default:
		assert(!"Unknown item type on diffmulti::addItem");
	}
	return 1;
}

void diffmulti::setNavBarMode(uchar a, QString i)
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
		dm->setGeometry(x, y, w, h);
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
		dm->setGeometry(0, 80, 240, 240);
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
