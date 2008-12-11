/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** multisounddiff.cpp
**
** Pagina sottomenu diffusione multicanale
**
****************************************************************/

#include "multisounddiff.h"
#include "ambdiffson.h"
#include "device_cache.h"
#include "device.h"
#include "device_status.h"
#include "sveglia.h"
#include "sounddiffusion.h"
#include "scenevocond.h"
#include "btmain.h"
#include "main.h" // BTouch
#include "xml_functions.h" // getChildren, getTextChild

#include <QTimer>
#include <QPoint>
#include <QDebug>

#include <assert.h>


MultiSoundDiff::MultiSoundDiff(QWidget *parent, QDomNode config_node) : sottoMenu(parent, 3, MAX_WIDTH, MAX_HEIGHT, NUM_RIGHE-1)
{
	sorgenti = new AudioSources(this, config_node);
	connect(this, SIGNAL(gesFrame(char *)), sorgenti, SIGNAL(gestFrame(char *)));
	connect(sorgenti, SIGNAL(actSrcChanged(int, int)), this, SIGNAL(actSrcChanged(int, int)));

	matr = btouch_device_cache.get_sound_matr_device();
	// Get status changed events back
	connect(matr, SIGNAL(status_changed(QList<device_status*>)),
		this, SLOT(status_changed(QList<device_status*>)));

	loadAmbienti(config_node);
}

MultiSoundDiff::~MultiSoundDiff()
{
	while (!dslist.isEmpty())
		delete dslist.takeFirst();
}

void MultiSoundDiff::loadAmbienti(QDomNode config_node)
{
	QDomNode item;
	foreach (item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		QString descr = getTextChild(item, "descr");
		QString img1 = IMG_PATH + getTextChild(item, "cimg1");
		QString img2 = IMG_PATH + getTextChild(item, "cimg2");
		QString img3 = IMG_PATH + getTextChild(item, "cimg3");

		SoundDiffusion *ds;
		if (id == INSIEME_AMBIENTI || id == AMBIENTE)
		{
			// Do not create "sorgenti" submenu
			ds = new SoundDiffusion(sorgenti, item);
			connect(ds, SIGNAL(closed(SoundDiffusion*)), this, SLOT(ds_closed(SoundDiffusion*)));
			connect(ds, SIGNAL(closed(SoundDiffusion*)), this, SIGNAL(dsClosed()));
			ds->draw();
			dslist.append(ds);
		}

		banner *b = 0;
		switch (id)
		{
		case INSIEME_AMBIENTI:
			b = new insAmbDiffSon(this, descr, img1, img2, ds, sorgenti, this);
			break;
		case AMBIENTE:
		{
			QString where = getTextChild(item, "where");
			b = new ambDiffSon(this, descr, where, img1, img2, img3, ds, sorgenti, this);
			sorgenti->addAmb(where);
			break;
		}
		default: // Nothing to do, the other items (source audio) is managed by AudioSources class
			break;
		}
		if (b)
		{
			connect(this, SIGNAL(gestFrame(char*)), b, SLOT(gestFrame(char*)));
			connect(this, SIGNAL(actSrcChanged(int, int)), b, SLOT(actSrcChanged(int, int)));
			connect(b, SIGNAL(ambChanged(const QString &, bool, QString)), sorgenti, SIGNAL(ambChanged(const QString &, bool, QString)));
			b->setText(descr);
			b->setId(id);
			elencoBanner.append(b);
		}
	}
	draw();
}

void MultiSoundDiff::setNavBarMode(uchar a, QString i)
{
	sottoMenu::setNavBarMode(a, i);
	for (int i = 0; i < dslist.size(); ++i)
		dslist.at(i)->setNavBarMode(a);
}

void MultiSoundDiff::setNumRighe(uchar n)
{
	sottoMenu::setNumRighe(n);
	for (int i = 0; i < dslist.size(); ++i)
		dslist.at(i)->setNumRighe(n);
}

void MultiSoundDiff::reparent(QWidget *parent, Qt::WindowFlags f, const QPoint & p, bool showIt)
{
	sottoMenu::reparent(parent, f, p, showIt);
	for (int i = 0; i < dslist.size(); ++i)
	{
		SoundDiffusion *ds = dslist.at(i);
		ds->setParent(!parent ? parent : this);
		ds->setWindowFlags(f);
		ds->move(p);
	}
}

void MultiSoundDiff::inizializza()
{
	qDebug("MultiSoundDiff::inizializza()");
	sorgenti->inizializza();
	matr->init();
}

void MultiSoundDiff::ds_closed(SoundDiffusion *ds)
{
	qDebug("MultiSoundDiff::ds_closed()");
	ds->hide();

	for (int i = 0; i < elencoBanner.size(); ++i)
	{
		elencoBanner.at(i)->Draw();
		elencoBanner.at(i)->show();
	}
	forceDraw();
	showFullScreen();
}

void MultiSoundDiff::hideEvent(QHideEvent *event)
{
	for (int i = 0; i < dslist.size(); ++i)
		dslist.at(i)->hide();
}

void MultiSoundDiff::resizewindows(int x, int y, int w, int h)
{
	for (int i = 0; i < dslist.size(); ++i)
	{
		SoundDiffusion *ds = dslist.at(i);
		ds->setGeom(x, y, w, h);
		ds->forceDraw();
	}
}

void MultiSoundDiff::ripristinaRighe(void)
{
	sottoMenu::setNumRighe(3);

	for (int i = 0; i < dslist.size(); ++i)
		dslist.at(i)->setNumRighe(4);
}

void MultiSoundDiff::status_changed(QList<device_status*> sl)
{
	stat_var curr_act(stat_var::ACTIVE_SOURCE);
	qDebug("MultiSoundDiff::status_changed()");

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

void MultiSoundDiff::gestFrame(char*frame)
{
	emit gesFrame(frame);
}

// contdiff implementation
contdiff::contdiff(SoundDiffusion *_ds, MultiSoundDiff *_dm) : QObject()
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
