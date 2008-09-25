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
#include "genericfunz.h"
#include "sveglia.h"
#include "diffsonora.h"
#include "scenevocond.h"

#include <qwidget.h>
#include <qtimer.h>
#include <qpoint.h>


dati_ampli_multi::dati_ampli_multi(char t, QPtrList<QString> *d, void *ind,
		int p1, char *_I1, char *_I2, char *_I3, char *_I4, char *_I5)
{
	tipo = t;
	descr = new QPtrList<QString>(*((QPtrList<QString> *)d));
	qDebug("dati_ampli_multi: descr = %s", descr->at(0)->ascii());
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
	{
		qDebug("gruppo AMPLIFICATORI !!");
		indirizzo = new QPtrList<QString>(*((QPtrList<QString> *)ind));
		qDebug("indirizzo = %p", indirizzo);	
		QPtrListIterator<QString> *lii = new QPtrListIterator<QString>(*(QPtrList<QString> *)(indirizzo));
		QString *bah;
		lii->toFirst();
		while ((bah = lii->current()))
		{
			qDebug("INDIRIZZO = %s", bah->ascii());
			++(*lii);
		}
	}
	I1 = QString(_I1);
	I2 = QString(_I2);
	I3 = QString(_I3);
	I4 = QString(_I4);
	I5 = QString(_I5);
	modo = p1;
}

dati_ampli_multi::~dati_ampli_multi()
{
	// We should delete indirizzo but is problematic because it is a void*
}


diffmulti::diffmulti(QWidget *parent, const char *name, uchar navBarMode,int wi,int hei, uchar n)
	: sottoMenu(parent, name, navBarMode, wi, hei, n)
{
	sorgenti = new sottoMenu(this,"Sorgenti",0,MAX_WIDTH, MAX_HEIGHT/4 - 3,1);
	datimmulti = new QPtrList<dati_ampli_multi>;
	datimmulti->setAutoDelete(true);
	dslist = new QPtrList<diffSonora>;
	dslist->setAutoDelete(true);
	matr = btouch_device_cache.get_sound_matr_device();
	connect(sorgenti, SIGNAL(freeze(bool)), this, SIGNAL(freeze(bool)));
	connect(this, SIGNAL(frez(bool)), sorgenti, SLOT(freezed(bool)));

	// Get status changed events back
	connect(matr, SIGNAL(status_changed(QPtrList<device_status>)), this, SLOT(status_changed(QPtrList<device_status>)));
}

int diffmulti::addItem(char tipo, QPtrList<QString> *descrizioni, void* indirizzo, QPtrList<QString> &icon_names,int modo, int numFrame,
		QColor SecondForeground, char* descr1, char* descr2,char* descr3, char* descr4, int par3,int par4, QPtrList<QString> *lt,
		QPtrList<scenEvo_cond> *lc, QString action, QString light, QString key, QString unknown,QValueList<int>sstart, QValueList<int>sstop)
{
	qDebug("diffmulti::addItem (%d)", tipo);
	qDebug("Amplificatore (%s)", (char *)indirizzo);
	qDebug("%d elementi in descrizioni", descrizioni->count());

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
		qDebug("Source (%d, %s): appending to source list", tipo, descrizioni->at(0)->ascii());
		qDebug("sorgenti->addItem (%p)", sorgenti);
		/*
			* NOTE: numFrame parametere name is not significative: it's the (cut down) where address.
			*/
		sorgenti->addItemU(tipo, *descrizioni->at(0), (char *)indirizzo, icon_names, 0, numFrame);
		b = sorgenti->getLast();
		connect(b, SIGNAL(csxClick()), sorgenti, SLOT(goUp()));
		connect(sorgenti, SIGNAL(ambChanged(const QString &, bool, char *)),b, SLOT(ambChanged(const QString &, bool, char *)));
		connect(b, SIGNAL(active(int, int)), this, SIGNAL(actSrcChanged(int, int)));
		connect(this,SIGNAL(gesFrame(char *)),b,SLOT(gestFrame(char *)));
		connect(b,SIGNAL(sendInit(char*)),this, SIGNAL(sendInit(char*)));
		break;

	case INSIEME_AMBIENTI:
	case AMBIENTE:
		{
			diffSonora *ds;
			// Do not create "sorgenti" submenu
			ds = new diffSonora(NULL, "Diff sonora ambiente", false);
			ds->setSorgenti(sorgenti);
			ds->setBGColor(backgroundColor().red(), backgroundColor().green(), backgroundColor().blue());
			ds->setFGColor(foregroundColor().red(), foregroundColor().green(), foregroundColor().blue());
			connect(ds, SIGNAL(closed(diffSonora*)), this, SLOT(ds_closed(diffSonora*)));
			connect(ds, SIGNAL(closed(diffSonora*)), this, SIGNAL(dsClosed()));
			QObject::connect(ds,SIGNAL(freeze(bool)), this, SIGNAL(freeze(bool)));
			QObject::connect(this,SIGNAL(frez(bool)), ds, SLOT(freezed_handler(bool)));
			QObject::connect(this,SIGNAL(frez(bool)), ds, SIGNAL(freezed(bool)));
			ds->draw();
			banner *b;
			if (tipo == AMBIENTE)
			{
				b = new ambDiffSon(this, descrizioni->at(0)->ascii(), (char *)indirizzo,
					(char *)safeAt(icon_names, 0)->ascii(), (char *)safeAt(icon_names, 1)->ascii(),
					(char *)safeAt(icon_names, 2)->ascii(),
					datimmulti, ds, sorgenti, this);
			}
			else
			{
				b = new insAmbDiffSon(this, descrizioni, indirizzo,
					(char *)safeAt(icon_names, 0)->ascii(), (char *)safeAt(icon_names, 1)->ascii(),
					datimmulti, ds,  sorgenti, this);
			}
			elencoBanner.append(b);
			dslist->append(ds);
			elencoBanner.getLast()->SetTextU(*(descrizioni->at(0)));
			elencoBanner.getLast()->setBGColor(backgroundColor());
			elencoBanner.getLast()->setFGColor(foregroundColor());
			elencoBanner.getLast()->setId(tipo);
			connect(this, SIGNAL(gestFrame(char*)), elencoBanner.getLast(), SLOT(gestFrame(char*)));
			connect(this, SIGNAL(actSrcChanged(int, int)), elencoBanner.getLast(), SLOT(actSrcChanged(int, int)));
			connect(elencoBanner.getLast(), SIGNAL(freeze(bool)), this, SIGNAL(freeze(bool)));
			connect(elencoBanner.getLast(), SIGNAL(svegl(bool)), this , SIGNAL(svegl(bool)));
			connect(this , SIGNAL(frez(bool)), elencoBanner.getLast(), SIGNAL(freezed(bool)));
			connect(elencoBanner.getLast(), SIGNAL(killMe(banner*)), this, SLOT(killBanner(banner*)));
			connect(elencoBanner.getLast(), SIGNAL(ambChanged(const QString &, bool, char *)), sorgenti, SIGNAL(ambChanged(const QString &, bool, char *)));
			if (tipo == AMBIENTE)
				sorgenti->addAmb((char *)indirizzo);
			datimmulti->clear();
			draw();
			break;
		}
	case GR_AMPLIFICATORI:
		{
			qDebug("Gruppo amplificatori");
			QPtrListIterator<QString> *lsi = new QPtrListIterator<QString>(*(QPtrList<QString> *)indirizzo);
			QString *s;
			while ((s = lsi->current()))
			{
				qDebug("ADDRESS = %s", s->ascii());
				++(*lsi);
			}
			datimmulti->append(new dati_ampli_multi(tipo, descrizioni, indirizzo, modo,
					(char *)safeAt(icon_names, 0)->ascii(), (char *)safeAt(icon_names, 1)->ascii(),
					(char *)safeAt(icon_names, 2)->ascii(), (char *)safeAt(icon_names, 3)->ascii()));
			delete lsi;
			break;
		}

	case AMPLIFICATORE:
		qDebug("Icone = %s - %s - %s - %s",
					(char *)safeAt(icon_names, 0)->ascii(), (char *)safeAt(icon_names, 1)->ascii(),
					(char *)safeAt(icon_names, 2)->ascii(), (char *)safeAt(icon_names, 3)->ascii());
		datimmulti->append(new dati_ampli_multi(tipo, descrizioni, indirizzo, modo,
					(char *)safeAt(icon_names, 0)->ascii(), (char *)safeAt(icon_names, 1)->ascii(),
					(char *)safeAt(icon_names, 2)->ascii(), (char *)safeAt(icon_names, 3)->ascii()));
		break;

	case POWER_AMPLIFIER:
		qDebug("Icone Power Multi = %s - %s - %s - %s - %s",
					(char *)safeAt(icon_names, 0)->ascii(), (char *)safeAt(icon_names, 1)->ascii(),
					(char *)safeAt(icon_names, 2)->ascii(), (char *)safeAt(icon_names, 3)->ascii(),
					(char *)safeAt(icon_names, 4)->ascii());
		datimmulti->append(new dati_ampli_multi(tipo, descrizioni, indirizzo, modo,
					(char *)safeAt(icon_names, 0)->ascii(), (char *)safeAt(icon_names, 1)->ascii(),
					(char *)safeAt(icon_names, 2)->ascii(), (char *)safeAt(icon_names, 3)->ascii(),
					(char *)safeAt(icon_names, 4)->ascii()));
		break;

	default:
		sottoMenu::addItemU(tipo, *descrizioni->at(0),
				indirizzo, icon_names, modo, numFrame,
				SecondForeground, descr1, descr2,
				descr3, descr4, par3,
				par4, lt, lc, action, light, key, unknown,
				sstart, sstop);
		break;
	}
	return 1;
}

void diffmulti::setNavBarMode(uchar a, char* i)
{
	sottoMenu::setNavBarMode(a, i);
	QPtrListIterator<diffSonora> *dsi = new QPtrListIterator<diffSonora>(*dslist);
	dsi->toFirst();
	diffSonora *ds;
	while ((ds = dsi->current()) != 0)
	{
		ds->setNavBarMode(a);
		++(*dsi);
	}
	delete dsi;
}

void diffmulti::setNumRighe(uchar n)
{
	sottoMenu::setNumRighe(n);
	QPtrListIterator<diffSonora> *dsi = new QPtrListIterator<diffSonora>(*dslist);
	dsi->toFirst();
	diffSonora *ds;
	while ((ds = dsi->current()) != 0)
	{
		ds->setNumRighe(n);
		++(*dsi);
	}
	delete dsi;
}

void diffmulti::reparent(QWidget *par, unsigned int f, QPoint p, bool showIt)
{
	sottoMenu::reparent(par, f, p, showIt);
	QPtrListIterator<diffSonora> *dsi = new QPtrListIterator<diffSonora>(*dslist);
	dsi->toFirst();
	diffSonora *ds;
	while ((ds = dsi->current()) != 0)
	{
		ds->reparent(!par ? par : this, f, QPoint(0, 0), 0);
		++(*dsi);
	}
	delete dsi;
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
	QPtrListIterator<banner> *lbi = new QPtrListIterator<banner>(elencoBanner);
	lbi->toFirst();
	banner *b;
	while ((b = lbi->current()) != 0)
	{
		b->Draw();
		b->show();
		++(*lbi);
	}
	forceDraw();
	showFullScreen();
	delete lbi;
}

void diffmulti::hide()
{
	qDebug("diffmulti::hide()");
	sottoMenu::hide();
	QPtrListIterator<diffSonora> *dsi = new QPtrListIterator<diffSonora>(*dslist);
	dsi->toFirst();
	diffSonora *ds;
	while ((ds = dsi->current()) != 0)
	{
		ds->hide();
		++(*dsi);
	}
	delete dsi;
}

void diffmulti::show()
{
	QWidget::show();
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
	QPtrListIterator<diffSonora> *dsi = new QPtrListIterator<diffSonora>(*dslist);
	dsi->toFirst();
	diffSonora *ds;
	while ((ds = dsi->current()) != 0)
	{
		ds->setGeom(x, y, w, h);
		ds->forceDraw();
		++(*dsi);
	}
	delete dsi;
}

void diffmulti::ripristinaRighe(void)
{
	sottoMenu::setNumRighe(3);
	QPtrListIterator<diffSonora> *dsi = new QPtrListIterator<diffSonora>(*dslist);
	dsi->toFirst();
	diffSonora *ds;
	while ((ds = dsi->current()) != 0)
	{
		ds->setNumRighe(4);
		++(*dsi);
	}
	delete dsi;
}

void diffmulti::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_act(stat_var::ACTIVE_SOURCE);
	qDebug("diffmulti::status_changed()");
	QPtrListIterator<device_status> *dsi = new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	while ((ds = dsi->current()) != 0)
	{
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
		++(*dsi);
	}
	delete dsi;
}

void diffmulti::freezed_handler(bool f)
{
	qDebug("diffmulti::freezed(%d)", f);
	sottoMenu::freezed(f);
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
		ds->reparent(parent, f, point, showIt);
	if (dm)
		dm->reparent(parent, f, point, showIt);
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
