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
#include "diffsonora.h"
#include "diffmulti.h" // struct data_ampli_multi
#include "sottomenu.h"
#include "fontmanager.h"
#include "btmain.h"
#include "main.h" // BTouch

#include <QDebug>
#include <QLabel>
#include <QFont>

#include <stdlib.h>
#include <assert.h>

/*****************************************************************
 ** Ambiente diffusione sonora multicanale
 ****************************************************************/


ambDiffSon::ambDiffSon(QWidget *parent, QString _name, char *indirizzo, QString IconaSx, QString IconaDx, QString icon,
	QList<dati_ampli_multi*> *la, diffSonora *ds, sottoMenu *sorg, diffmulti *dm) : bannBut2Icon(parent), name(_name)
{
	qDebug() << "ambDiffSon::ambDiffSon() : " << indirizzo << " " << IconaSx << " " << IconaDx << " " << icon;
	QString zoneIcon = getAmbName(IconaSx, indirizzo);
	qDebug() << "zoneIcon = " << zoneIcon;
	SetIcons(icon, zoneIcon, IconaDx);
	Draw();
	setAddress(indirizzo);
	connect(this, SIGNAL(sxClick()), this, SLOT(configura()));

	diffson = ds;
	sorgenti = sorg;
	diffmul = dm;

	for (int i = 0; i < la->size(); ++i)
	{
		dati_ampli_multi *am = la->at(i);

		// TODO: far diventare icons un QList<QString>!! Al momento l'unico
		// motivo per cui vengono evitati crash e' perche' i nomi delle icone
		// non vengono memorizzati ma utilizzati esclusivamente per ottenere
		// le corrispondenti pixmap (vedi banner::setIcons)
		QList<QString*> icons;
		QString qI1(am->I1);
		QString qI2(am->I2);
		QString qI3(am->I3);
		QString qI4(am->I4);
		QString qI5(am->I5);
		icons.append(&qI1);
		icons.append(&qI2);
		icons.append(&qI3);
		icons.append(&qI4);
		icons.append(&qI5);


		if (am->tipo == AMPLIFICATORE || am->tipo == POWER_AMPLIFIER)
		{
			qDebug() << "Adding amplifier (" << static_cast<int>(am->tipo) << ", "
				<< (char *)am->indirizzo << " " << am->descr << ")";
			diffson->addItemU(am->tipo, am->descr, (char *)am->indirizzo, icons, am->modo);
		}
		else if (am->tipo == GR_AMPLIFICATORI)
		{
			qDebug("Adding amplifier group");
			qDebug("indirizzo = %p", am->indirizzo);

			QList<QString*> *indirizzi = (QList<QString*> *)(am->indirizzo);
			for (int i = 0; i < indirizzi->size(); ++i)
			{
				QString s = *indirizzi->at(i);
				QStringList qsl = s.split(',');
				// TODO: passo un * a QList<QString*> che non verra' mai distrutto!
				// sistemare passando un QList<QString>!!
				QList<QString*> *li = new QList<QString*>();
				for (int j = 0; j < qsl.size(); ++j)
					li->append(new QString(qsl[j]));
				diffson->addItemU(am->tipo, am->descr, li, icons, am->modo);
			}
		}
		else
			assert(!"Unknown amplifier type!!");
	}
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
	QFont aFont;
	FontManager::instance()->getFont(font_items_bannertext, aFont);
	BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	BannerText->setFont(aFont);
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
	emit(ambChanged(name, false, getAddress()));
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
	if (a != atoi(getAddress()))
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

insAmbDiffSon::insAmbDiffSon(QWidget *parent, QString _name, QString Icona1, QString Icona2, QList<dati_ampli_multi*> *la,
	diffSonora *ds, sottoMenu *sorg, diffmulti *dm) : bannButIcon(parent), name(_name)
{
	qDebug() << "insAmbDiffSon::insAmbDiffSon() : " << Icona1 << " " << Icona2;
	// TODO: c'e' un sacco codice duplicato con ambdiffson!!!
	SetIcons(Icona1, Icona2);
	Draw();
	connect(this, SIGNAL(sxClick()), this, SLOT(configura()));
	diffson = ds;
	sorgenti = sorg;
	diffmul = dm;

	// TODO: vedi ambdiffson!!! questo pezzo di codice qua sotto e' duplicato!
	for (int i = 0; i < la->size(); ++i)
	{
		dati_ampli_multi *am = la->at(i);

		QList<QString*> icons;
		QString qI1(am->I1);
		QString qI2(am->I2);
		QString qI3(am->I3);
		QString qI4(am->I4);
		QString qI5(am->I5);
		icons.append(&qI1);
		icons.append(&qI2);
		icons.append(&qI3);
		icons.append(&qI4);
		icons.append(&qI5);

		if (am->tipo == AMPLIFICATORE || am->tipo == POWER_AMPLIFIER)
		{
			qDebug() << "Adding amplifier (" << static_cast<int>(am->tipo) << ", "
				<< (char *)am->indirizzo << " " << am->descr << ")";
			diffson->addItemU(am->tipo, am->descr, (char *)am->indirizzo, icons, am->modo);
		}
		else if (am->tipo == GR_AMPLIFICATORI)
		{
			qDebug("Adding amplifier group");
			qDebug("indirizzo = %p", am->indirizzo);

			QList<QString*> *indirizzi = (QList<QString*> *)(am->indirizzo);
			for (int i = 0; i < indirizzi->size(); ++i)
			{
				QString s = *indirizzi->at(i);
				QStringList qsl = s.split(',');
				QList<QString*> *li = new QList<QString*>();
				for (int j = 0; j < qsl.size(); ++j)
					li->append(new QString(qsl[j]));
				diffson->addItemU(am->tipo, am->descr, li, icons, am->modo);
			}
		}
		else
			assert(!"Unknown amplifier type!!");
	}
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
	QFont aFont;
	FontManager::instance()->getFont(font_items_bannertext, aFont);
	BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	BannerText->setFont(aFont);
	BannerText->setText(qtesto);
}

void insAmbDiffSon::configura()
{
	qDebug("insAmbDiffSon::configura()");
	emit(ambChanged(name, true, (char *)NULL));
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
