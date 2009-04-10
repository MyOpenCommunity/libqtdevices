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
#include "device.h"
#include "device_status.h"
#include "alarmclock.h"
#include "sounddiffusion.h"
#include "scenevocond.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "xml_functions.h" // getChildren, getTextChild

#include <QTimer>
#include <QPoint>
#include <QDebug>

#include <assert.h>

AudioSources *MultiSoundDiffInterface::sorgenti = 0;

MultiSoundDiffInterface::~MultiSoundDiffInterface()
{
	while (!dslist.isEmpty())
		delete dslist.takeFirst();
}

void MultiSoundDiffInterface::loadAmbienti(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
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
			ds = createSoundDiffusion(sorgenti, item);

			connect(ds, SIGNAL(closed(SoundDiffusion*)), this, SLOT(ds_closed(SoundDiffusion*)));
			connect(ds, SIGNAL(closed(SoundDiffusion*)), SLOT(showPage()));
			ds->draw();
			dslist.append(ds);
		}

		banner *b = 0;
		switch (id)
		{
		case INSIEME_AMBIENTI:
			b = new insAmbDiffSon(this, descr, img1, img2, ds, sorgenti);
			break;
		case AMBIENTE:
		{
			QString where = getTextChild(item, "where");
			b = new ambDiffSon(this, descr, where, img1, img2, img3, ds, sorgenti);
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


MultiSoundDiff::MultiSoundDiff(const QDomNode &config_node)
{
	if (!sorgenti)
		sorgenti = new AudioSources(this, config_node);
	sorgenti->hide();
	connect(this, SIGNAL(gesFrame(char *)), sorgenti, SIGNAL(gestFrame(char *)));
	connect(sorgenti, SIGNAL(actSrcChanged(int, int)), this, SIGNAL(actSrcChanged(int, int)));

	matr = bt_global::devices_cache.get_sound_matr_device();
	// Get status changed events back
	connect(matr, SIGNAL(status_changed(QList<device_status*>)),
		this, SLOT(status_changed(QList<device_status*>)));

	loadAmbienti(config_node);
}

SoundDiffusion *MultiSoundDiff::createSoundDiffusion(AudioSources *sorgenti, const QDomNode &conf)
{
	return new SoundDiffusion(sorgenti, conf);
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

void MultiSoundDiff::inizializza()
{
	qDebug("MultiSoundDiff::inizializza()");
	sorgenti->inizializza();
	matr->init();
	for (int i = 0; i < dslist.size(); ++i)
		dslist.at(i)->inizializza();
}

void MultiSoundDiff::ds_closed(SoundDiffusion *ds)
{
	qDebug("MultiSoundDiff::ds_closed()");

	for (int i = 0; i < elencoBanner.size(); ++i)
	{
		elencoBanner.at(i)->Draw();
		elencoBanner.at(i)->show();
	}
	forceDraw();
	showPage();
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


MultiSoundDiffAlarm::MultiSoundDiffAlarm(const QDomNode &config_node)
{
	if (!sorgenti)
		sorgenti = new AudioSources(this, config_node);

	loadAmbienti(config_node);

	setGeometry(0, 80, 240, 240);
	move(0, 80);
	setNavBarMode(6);
	setNumRighe(3);
}

void MultiSoundDiffAlarm::showPage()
{
	show();
}

SoundDiffusion *MultiSoundDiffAlarm::createSoundDiffusion(AudioSources *sorgenti, const QDomNode &conf)
{
	SoundDiffusion *sd = new SoundDiffusionAlarm(sorgenti, conf);
	connect(sd, SIGNAL(Closed()), SIGNAL(Closed()));
	return sd;
}


// contdiff implementation
contdiff::contdiff(SoundDiffusion *_ds, MultiSoundDiffAlarm *_dm) : QObject()
{
	ds = _ds;
	dm = _dm;
	if (ds)
		connect(ds, SIGNAL(Closed()), this, SIGNAL(Closed()));
	if (dm)
		connect(dm, SIGNAL(Closed()), this, SIGNAL(Closed()));
}

void contdiff::setParent(QWidget *parent)
{
	if (dm)
		dm->setParent(parent);
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
		ds->showPage();
	if (dm)
		dm->show();
}

void contdiff::resizewindows()
{
	if (dm)
		dm->setGeometry(0, 80, 240, 240);
}

