/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** ambdiffson.cpp
 **
 **definizione degli ambienti di diffusione sonora implementati
 **
 ****************************************************************/

#include "ambdiffson.h"
#include "generic_functions.h" // void getAmbName(...)
#include "btbutton.h"
#include "sounddiffusion.h"
#include "multisounddiff.h"
#include "fontmanager.h" // bt_global::font

#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QFont>

#include <assert.h>

/*****************************************************************
 ** Ambiente diffusione sonora multicanale
 ****************************************************************/


ambDiffSon::ambDiffSon(QWidget *parent, QString d, QString indirizzo, QString IconaSx, QString IconaDx, QString icon,
	SoundDiffusion *ds, AudioSources *sorg, MultiSoundDiff *dm) : bannBut2Icon(parent)
{
	qDebug() << "ambDiffSon::ambDiffSon()";
	descr = d;
	SetIcons(icon, getAmbName(IconaSx, indirizzo), IconaDx);
	Draw();
	setAddress(indirizzo);
	connect(this, SIGNAL(sxClick()), this, SLOT(configura()));

	diffson = ds;
	sorgenti = sorg;
	diffmul = dm;
	setDraw(false);
}

void ambDiffSon::Draw()
{
	qDebug("ambDiffSon::Draw()");
	sxButton->setPixmap(*Icon[0]);
	if (pressIcon[0])
		sxButton->setPressedPixmap(*pressIcon[0]);
	BannerIcon->repaint();
	BannerIcon->setPixmap(*(Icon[1]));
	BannerIcon->repaint();
	BannerIcon2->repaint();
	BannerIcon2->setPixmap(*(Icon[3]));
	BannerIcon2->repaint();
	BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	BannerText->setFont(bt_global::font->get(FontManager::TEXT));
	BannerText->setText(qtesto);
}

void ambDiffSon::hideEvent(QHideEvent *event)
{
	qDebug("ambDiffSon::hideEvent()");
	setDraw(false);
}

void ambDiffSon::configura()
{
	qDebug("ambDiffSon::configura()");
	emit ambChanged(descr, false, getAddress());
	sorgenti->reparent(diffson, 0, diffson->geometry().topLeft());
	qDebug("connecting diffson(%p) to diffmul(%p)", diffson, diffmul);
	diffson->setFirstSource(actSrc);
	diffson->forceDraw();
	diffson->showFullScreen();
	setDraw(true);
}

void ambDiffSon::actSrcChanged(int a, int s)
{
	qDebug("ambDiffSon::actSrcChanged(%d, %d)", a, s);
	if (a != getAddress().toInt())
	{
		qDebug("not my address, discarding event");
		return;
	}
	qDebug("First source's where is %d", actSrc);
	if (actSrc != (100 + a*10 + s))
	{
		actSrc = 100 + a*10 + s;
		if (isDraw())
		{
			diffson->setFirstSource(actSrc);
			sorgenti->draw();
		}
	}
}

void ambDiffSon::setDraw(bool d)
{
	is_draw = d;
}

bool ambDiffSon::isDraw()
{
	return is_draw;
}

/*****************************************************************
 ** Insieme ambienti diffusione sonora multicanale
 ****************************************************************/

insAmbDiffSon::insAmbDiffSon(QWidget *parent, QString d, QString Icona1, QString Icona2,
	SoundDiffusion *ds, AudioSources *sorg, MultiSoundDiff *dm) : bannPuls(parent)
{
	qDebug() << "insAmbDiffSon::insAmbDiffSon() : " << Icona1 << " " << Icona2;
	descr = d;
	SetIcons(Icona1, Icona2);
	Draw();
	connect(this, SIGNAL(sxClick()), this, SLOT(configura()));
	diffson = ds;
	sorgenti = sorg;
	diffmul = dm;
}

void insAmbDiffSon::Draw()
{
	qDebug("insAmbDiffSon::Draw()");
	sxButton->setPixmap(*Icon[1]);
	if (pressIcon[1])
		sxButton->setPressedPixmap(*pressIcon[1]);
	BannerIcon->repaint();
	BannerIcon->setPixmap(*(Icon[0]));
	BannerIcon->repaint();
	BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	BannerText->setFont(bt_global::font->get(FontManager::TEXT));
	BannerText->setText(qtesto);
}

void insAmbDiffSon::configura()
{
	qDebug("insAmbDiffSon::configura()");
	emit ambChanged(descr, true, QString());
	qDebug("sorgenti->parent() = %p", sorgenti->parent());
	sorgenti->reparent(diffson, 0, diffson->geometry().topLeft());
	qDebug("connecting diffson(%p) to diffmul(%p)", diffson, diffmul);
	sorgenti->mostra_all(banner::BUT2);
	diffson->forceDraw();
	diffson->showFullScreen();
}


void insAmbDiffSon::actSrcChanged(int a, int s)
{
	qDebug("ambDiffSon::actSrcChanged(%d, %d), ignored", a, s);
}
