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
#include "genericfunz.h" // void getAmbName(...)
#include "btlabel.h"
#include "btbutton.h"
#include "diffsonora.h"
#include "diffmulti.h" // struct data_ampli_multi
#include "sottomenu.h"
#include "fontmanager.h"

/*****************************************************************
 ** Ambiente diffusione sonora multicanale
 ****************************************************************/

ambDiffSon::ambDiffSon( QWidget *parent, const char *name, void *indirizzo, char* IconaSx, char* IconaDx, char *icon, QPtrList<dati_ampli_multi> *la, diffSonora *ds, sottoMenu *sorg, diffmulti *dm)
: bannBut2Icon( parent, name )
{
	qDebug("ambDiffSon::ambDiffSon() : %s %s %s %s", (char *)indirizzo, IconaSx, IconaDx, icon);
	char zoneIcon[50];
	getAmbName(IconaSx, zoneIcon, (char *)indirizzo, sizeof(zoneIcon));
	qDebug("zoneIcon = %s", zoneIcon);
	SetIcons(icon, zoneIcon, IconaDx);
	Draw();
	setAddress((char *)indirizzo);
	connect(this, SIGNAL(sxClick()), this, SLOT(configura()));

	//diffson = new diffSonora(NULL, "Diff sonora ambiente");
	diffson = ds;
	sorgenti = sorg;
	diffmul = dm;
	QPtrListIterator<dati_ampli_multi> *lai = new QPtrListIterator<dati_ampli_multi>(*la);
	lai->toFirst();
	dati_ampli_multi *am;
	while( ( am = lai->current() ) != 0) 
	{
		QPtrList<QString> icons;
		QString qI1(am->I1);
		QString qI2(am->I2);
		QString qI3(am->I3);
		QString qI4(am->I4);
		icons.append(&qI1);
		icons.append(&qI2);
		icons.append(&qI3);
		icons.append(&qI4);

		if(am->tipo == AMPLIFICATORE)
		{
			qDebug("Adding amplifier (%d, %s %s)", am->tipo, (char *)am->indirizzo, am->descr->at(0)->ascii());
			diffson->addItemU(am->tipo, *am->descr->at(0), (char *)am->indirizzo, icons, am->modo);
		}
		else 
		{
			qDebug("Adding amplifier group");
			qDebug("indirizzo = %p", am->indirizzo);
			QPtrListIterator<QString> *lii = new QPtrListIterator<QString>(*(QPtrList<QString> *)(am->indirizzo));
			QString *i;
			lii->toFirst();
			while ( ( i = lii->current()) )
			{
				QStringList qsl = QStringList::split(QChar(','), *i);
				QPtrList<QString> *indirizzi = new QPtrList<QString>();
				indirizzi->setAutoDelete(true);
				for(unsigned int j=0; j<qsl.count(); j++)
					indirizzi->append(new QString(qsl[j]));
				diffson->addItemU(am->tipo, *am->descr->at(0), indirizzi, icons, am->modo);
				++(*lii);
			}
			delete lii;
		}
		++(*lai);
	}
	delete lai;
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
	FontManager::instance()->getFont( font_items_bannertext, aFont );
	BannerText->setAlignment(AlignHCenter|AlignVCenter);//AlignTop);
	BannerText->setFont( aFont );
	BannerText->setText( qtesto );
}

void ambDiffSon::hide()
{
	qDebug("ambDiffSon::hide()");
	setDraw(false);
	banner::hide();
}

void ambDiffSon::configura()
{
	qDebug("ambDiffSon::configura()");
	emit(ambChanged(QString(name()), false, getAddress()));
	qDebug("sorgenti->parent() = %p", sorgenti->parent());
	qDebug("disconnecting sorgenti->parent from diffmulti(%p)", diffmul);
	disconnect(sorgenti->parent(), SIGNAL(sendFrame(char *)), diffmul, SIGNAL(sendFrame(char *)));
	disconnect(sorgenti->parent(), SIGNAL(sendInit(char *)), diffmul, SIGNAL(sendInit(char *)));
	sorgenti->reparent(diffson, 0, diffson->geometry().topLeft());
	connect(diffson, SIGNAL(sendFrame(char *)), diffmul, SIGNAL(sendFrame(char *)));
	connect(diffson, SIGNAL(sendInit(char *)), diffmul, SIGNAL(sendInit(char *)));
	qDebug("connecting diffson(%p) to diffmul(%p)", diffson, diffmul);
	//connect(diffmul, SIGNAL(gestFrame(char *)), 
	//diffson, SIGNAL(gestFrame(char *)));
	//diffson->setGeom(0,0,MAX_WIDTH,MAX_HEIGHT);
	diffson->setFirstSource(actSrc);
	diffson->forceDraw();
	diffson->showFullScreen();
	setDraw(true);
}

void ambDiffSon::actSrcChanged(int a, int s)
{
	qDebug("ambDiffSon::actSrcChanged(%d, %d)", a, s);
	if(a != atoi(getAddress())) {
		qDebug("not my address, discarding event");
		return;
	}
	qDebug("First source's where is %d", actSrc);
	if(actSrc != (100 + a*10 + s))
	{
		actSrc = 100 + a*10 + s;
		if(isDraw())
		{
			diffson->setFirstSource(actSrc);
			sorgenti->draw();
			//diffson->forceDraw();
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

insAmbDiffSon::insAmbDiffSon( QWidget *parent, QPtrList<QString> *names, void *indirizzo,char* Icona1,char* Icona2, QPtrList<dati_ampli_multi> *la, diffSonora *ds, sottoMenu *sorg, diffmulti *dm)
: bannButIcon( parent, (char *)names->at(0)->ascii() )
{
	qDebug("insAmbDiffSon::insAmbDiffSon() : %s %s %s", (char *)indirizzo, Icona1, Icona2);
	SetIcons(Icona1, Icona2);
	Draw();
	//setAddress(indirizzo);
	connect(this, SIGNAL(sxClick()), this, SLOT(configura()));
	diffson = ds;
	sorgenti = sorg;
	diffmul = dm;

	QPtrListIterator<dati_ampli_multi> *lai = new QPtrListIterator<dati_ampli_multi>(*la);
	lai->toFirst();
	dati_ampli_multi *am;
	while( ( am = lai->current() ) != 0)
	{
		QPtrList<QString> icons;
		QString qI1(am->I1);
		QString qI2(am->I2);
		QString qI3(am->I3);
		QString qI4(am->I4);
		icons.append(&qI1);
		icons.append(&qI2);
		icons.append(&qI3);
		icons.append(&qI4);

		if(am->tipo == AMPLIFICATORE) {
			qDebug("Adding amplifier (%d, %s %s)", am->tipo, 
					(char *)am->indirizzo, (char *)am->descr->at(0)->ascii());
			diffson->addItemU(am->tipo, *am->descr->at(0), 
					(char *)am->indirizzo,
					icons, am->modo);
		} else {
			qDebug("Adding amplifier group(%d)", am->tipo);
			qDebug("indirizzo = %p", am->indirizzo);
			QPtrListIterator<QString> *lii =
				new QPtrListIterator<QString>(*(QPtrList<QString> *)
						(am->indirizzo));
			QString *i;
			lii->toFirst();
			while( ( i = lii->current()) ) {
				QStringList qsl = QStringList::split(QChar(','), *i);
				QPtrList<QString> *indirizzi = new QPtrList<QString>();
				indirizzi->setAutoDelete(true);
				for(unsigned int j=0; j<qsl.count(); j++)
					indirizzi->append(new QString(qsl[j]));
				diffson->addItemU(am->tipo, *am->descr->at(0), 
						indirizzi,
						icons, am->modo);
				++(*lii);
			}
			delete lii;
		}
		++(*lai);
	}
	delete lai;
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
	FontManager::instance()->getFont( font_items_bannertext, aFont );
	BannerText->setAlignment(AlignHCenter|AlignVCenter);//AlignTop);
	BannerText->setFont( aFont );
	BannerText->setText( qtesto );
}

void insAmbDiffSon::configura()
{
	qDebug("insAmbDiffSon::configura()");
	emit(ambChanged(QString(name()), true, (char *)NULL));
	qDebug("sorgenti->parent() = %p", sorgenti->parent());
	qDebug("disconnecting sorgenti->parent from diffmulti(%p)", diffmul);
	disconnect(sorgenti->parent(), SIGNAL(sendFrame(char *)), diffmul, SIGNAL(sendFrame(char *)));
	disconnect(sorgenti->parent(), SIGNAL(sendInit(char *)), diffmul, SIGNAL(sendInit(char *)));
	sorgenti->reparent(diffson, 0, diffson->geometry().topLeft());
	connect(diffson, SIGNAL(sendFrame(char *)), diffmul, SIGNAL(sendFrame(char *)));
	connect(diffson, SIGNAL(sendInit(char *)), diffmul, SIGNAL(sendInit(char *)));
	qDebug("connecting diffson(%p) to diffmul(%p)", diffson, diffmul);
	sorgenti->mostra_all(banner::BUT2);
	diffson->forceDraw();
	diffson->showFullScreen();
}


void insAmbDiffSon::actSrcChanged(int a, int s)
{
	qDebug("ambDiffSon::actSrcChanged(%d, %d), ignored", a, s);
}
