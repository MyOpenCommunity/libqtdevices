/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 **sottoMenu.cpp
 **
 **Sottomenù sottoMenu
 **
 ****************************************************************/

#include "sottomenu.h"
#include "btmain.h"
#include "main.h" // BTouch
#include "bannsettings.h"
#include "postoext.h"
#include "setitems.h"
#include "btbutton.h"
#include "diffsonora.h"
#include "diffmulti.h"
#include "versio.h"
#include "generic_functions.h"
#include "xmlconfhandler.h"
#include "bannfrecce.h"
#include "dimmers.h"
#include "actuators.h"
#include "scenari.h"
#include "amplificatori.h"
#include "poweramplifier.h"
#include "automatismi.h"
#include "sorgentiradio.h"
#include "sorgentiaux.h"
#include "sorgentimedia.h"
#include "carico.h"
#include "bann_antintrusione.h"
#include "bannfullscreen.h"
#include "lansettings.h"

#include <QByteArray>
#include <QPixmap>
#include <QCursor>
#include <QTimer>
#include <QRegExp>
#include <QDebug>

#include <algorithm>

#include <assert.h>

#define IMG_OK IMG_PATH "btnok.png"

sottoMenu::sottoMenu(QWidget *parent, uchar navBarMode,int wi,int hei, uchar n)
	:QWidget(parent)
{
	numRighe = n;
	scroll_step = 1;
	hasNavBar = navBarMode;
	width = wi;
	height = hei;

	setGeometry(0,0,width,height);
	setFixedSize(QSize(width, height));

	bannNavigazione = NULL;

	if (navBarMode)
	{
		bannNavigazione = new bannFrecce(this, navBarMode);

		bannNavigazione->setGeometry(0 , height-height/numRighe ,width , height/numRighe);
		connect(bannNavigazione,SIGNAL(backClick()),this,SIGNAL(Closed()));
		connect(bannNavigazione,SIGNAL(upClick()),this,SLOT(goUp()));
		connect(bannNavigazione,SIGNAL(downClick()),this,SLOT(goDown()));
		connect(bannNavigazione,SIGNAL(forwardClick()),this,SIGNAL(goDx()));
	}

	indice = 0;
	indicold = 100;
	// TODO: verificare se e' necessario!!
	if (!parentWidget())
		showFullScreen();
}

sottoMenu::~sottoMenu()
{
	while (!elencoBanner.isEmpty())
		delete elencoBanner.takeFirst();
}

void sottoMenu::setNavBarMode(uchar navBarMode, QString IconBut4)
{
	if (navBarMode != hasNavBar)
	{
		if (bannNavigazione)
		{
			bannNavigazione->hide();
			delete(bannNavigazione);
			bannNavigazione = NULL;
		}
		if (navBarMode)
		{
			bannNavigazione = new bannFrecce(this, navBarMode, IconBut4);

			bannNavigazione->setGeometry(0, height-height/NUM_RIGHE, width, height/NUM_RIGHE/*numRighe */);
			connect(bannNavigazione, SIGNAL(backClick()), this, SIGNAL(Closed()));
			connect(bannNavigazione, SIGNAL(upClick()), this, SLOT(goUp()));
			connect(bannNavigazione, SIGNAL(downClick()), this, SLOT(goDown()));
			connect(bannNavigazione, SIGNAL(forwardClick()), this, SIGNAL(goDx()));
		}
		hasNavBar = navBarMode;
		setModeIcon(IconBut4);
	}
	else if (IconBut4 != iconName)
		setModeIcon(IconBut4);
}

void sottoMenu::setModeIcon(QString iconBut4)
{
	iconName = iconBut4;
	bannNavigazione->SetIcons(1, iconName);
	bannNavigazione->Draw();
}

int sottoMenu::addItemU(char tipo, const QString & qdescrizione, QList<QString> indirizzo,
	QList<QString*> &icon_names, int periodo, int numFrame)
{
	qDebug("sottoMenu::addItem(lista)");
	switch (tipo)
	{
	case GR_AMPLIFICATORI:
		elencoBanner.append(new grAmplificatori(this, indirizzo, *safeAt(icon_names, 0), *safeAt(icon_names, 1),
			*safeAt(icon_names, 2), *safeAt(icon_names, 3)));
		break;
	default:
		assert(!"********** sottoMenu::addItem():unknown item type!!! ************");
	}
	connectLastBanner();

	banner *last = elencoBanner.last();
	last->setText(qdescrizione);
	last->setAnimationParams(periodo,numFrame);
	last->setId(tipo);

	for (int idx = elencoBanner.size() - 2; idx >= 0; idx--)
	{
		if (elencoBanner.at(idx)->getId() == tipo)
		{
			elencoBanner.last()->setSerNum(elencoBanner.at(idx)->getSerNum() + 1);
			idx = -1;
		}
	}

	return 1;
}

int sottoMenu::addItemU(char tipo, const QString & qdescrizione, void *indirizzo,
		QList<QString*> &icon_names, int periodo, int numFrame,
		char *descr1, char *descr2, char *descr3, char *descr4,
		int par3, int par4,
		QList<QString*> *lt, QList<scenEvo_cond*> *lc,
		QString action, QString light, QString key, QString unknown,
		QList<int> sstart, QList<int> sstop,
		QString txt1, QString txt2, QString txt3)
{
	QByteArray buf_descr = qdescrizione.toAscii();
	const char * descrizione = buf_descr.constData();

	QString IconaSx = *safeAt(icon_names, 0);
	QString IconaDx = *safeAt(icon_names, 1);
	QString icon = *safeAt(icon_names, 2);
	QString pressedIcon = *safeAt(icon_names, 3);
	QString icoEx1 = *safeAt(icon_names, 4);
	QString icoEx2 = *safeAt(icon_names, 5);
	QString icoEx3 = *safeAt(icon_names, 6);

	qDebug("sottoMenu::addItem (%p)", lt);
	switch (tipo)
	{
	case ATTUAT_AUTOM:
		elencoBanner.append(new attuatAutom(this, (char*)indirizzo, IconaSx, IconaDx, icon, pressedIcon, periodo, numFrame));
		break;
	case DIMMER:
		elencoBanner.append(new dimmer(this, (char*)indirizzo, IconaSx, IconaDx, icon, pressedIcon, icoEx1));
		break;
	case DIMMER_100:
		elencoBanner.append(new dimmer100(this, (char*)indirizzo, IconaSx, IconaDx ,icon, pressedIcon, icoEx1, par3, par4));
		break;
	case SCENARIO:
		elencoBanner.append(new scenario(this, (char*)indirizzo, IconaSx));
		break;
	case GR_ATTUAT_AUTOM:
		elencoBanner.append(new grAttuatAutom(this, indirizzo, IconaSx, IconaDx, icon, periodo, numFrame));
		break;
	case GR_DIMMER:
		elencoBanner.append(new grDimmer(this, indirizzo, IconaSx, IconaDx, icon, pressedIcon));
		break;
	case GR_DIMMER100:
		elencoBanner.append(new grDimmer100(this, indirizzo, IconaSx, IconaDx, icon, pressedIcon, sstart, sstop));
		break;
	case CARICO:
		elencoBanner.append(new carico(this, (char*)indirizzo, IconaSx));
		break;
	case ATTUAT_AUTOM_INT:
		elencoBanner.append(new attuatAutomInt(this, (char*)indirizzo, IconaSx, IconaDx, icon, pressedIcon));
		break;
	case ATTUAT_AUTOM_INT_SIC:
		elencoBanner.append(new attuatAutomIntSic(this, (char*)indirizzo, IconaSx, IconaDx, icon, pressedIcon));
		break;
	case ATTUAT_AUTOM_TEMP:
		elencoBanner.append(new attuatAutomTemp(this, (char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon,periodo,numFrame,lt));
		break;
	case ATTUAT_AUTOM_TEMP_NUOVO_N:
		elencoBanner.append(new attuatAutomTempNuovoN(this, (char*)indirizzo, IconaSx, IconaDx, icon, pressedIcon, periodo, numFrame, lt));
		break;
	case ATTUAT_AUTOM_TEMP_NUOVO_F:
		if (!lt->count())
			lt->append(new QString(""));
		elencoBanner.append(new attuatAutomTempNuovoF(this, (char*)indirizzo, IconaSx, IconaDx, icon, *lt->at(0)));
		break;
	case GR_ATTUAT_INT:
		elencoBanner.append(new grAttuatInt(this, indirizzo, IconaSx, IconaDx, icon, periodo, numFrame));
		break;
	case ATTUAT_AUTOM_PULS:
		elencoBanner.append(new attuatPuls(this, (char*)indirizzo, IconaSx, IconaDx, AUTOMAZ, periodo, numFrame));
		break;
	case ATTUAT_VCT_LS:
		elencoBanner.append(new attuatPuls(this, (char*)indirizzo, IconaSx, IconaDx, VCT_LS, periodo, numFrame));
		break;
	case ATTUAT_VCT_SERR:
		elencoBanner.append(new attuatPuls(this, (char*)indirizzo, IconaSx, IconaDx, VCT_SERR, periodo, numFrame));
		break;
	case AUTOM_CANC_ATTUAT_VC:
		elencoBanner.append(new automCancAttuatVC(this, (char*)indirizzo, IconaSx, IconaDx));
		break;
	case AUTOM_CANC_ATTUAT_ILL:
		elencoBanner.append(new automCancAttuatIll(this, (char*)indirizzo, IconaSx, IconaDx, *lt->at(0)));
		break;
	case SET_DATA_ORA:
		elencoBanner.append(new setDataOra(this));
		break;
	case POWER_AMPLIFIER:
		elencoBanner.append(new PowerAmplifier(this, (char*)indirizzo, IconaSx, IconaDx, icon, pressedIcon, icoEx1));
		break;
	case POWER_AMPLIFIER_PRESET:
		elencoBanner.append(new PowerAmplifierPreset(this));
		break;
	case POWER_AMPLIFIER_TREBLE:
		elencoBanner.append(new PowerAmplifierTreble(this));
		break;
	case POWER_AMPLIFIER_BASS:
		elencoBanner.append(new PowerAmplifierBass(this));
		break;
	case POWER_AMPLIFIER_BALANCE:
		elencoBanner.append(new PowerAmplifierBalance(this));
		break;
	case POWER_AMPLIFIER_LOUD:
		elencoBanner.append(new PowerAmplifierLoud(this));
		break;
	case AMPLIFICATORE:
		elencoBanner.append(new amplificatore(this, (char*)indirizzo, IconaSx, IconaDx, icon, pressedIcon));
		break;
	case SORGENTE_AUX:
		elencoBanner.append(new sorgente_aux(this, descrizione, (char*)indirizzo));
		break;
	case SORGENTE_MULTIM:/// New Multimedia Audio Source.  NOTE:numFrame parametere name is not significative:it's the (cut down) where address
		elencoBanner.append(new BannerSorgenteMultimedia(this, (char *)indirizzo, numFrame, 4));
		break;
	case SORGENTE_MULTIM_MC:
		elencoBanner.append(new BannerSorgenteMultimediaMC(this, (char *)indirizzo, numFrame, IconaSx, IconaDx, icon));
		break;
	case SORGENTE_RADIO:
		elencoBanner.append(new banradio(this, (char*)indirizzo));
		break;
	case SET_SVEGLIA:
		elencoBanner.append(new impostaSveglia(this, (contdiff*)indirizzo, IconaSx, IconaDx, icon, pressedIcon, periodo, numFrame, icoEx1, par3));
		break;
	case CALIBRAZIONE:
		{
			calibration *c = new calibration(this, IconaSx);
			elencoBanner.append(c);
			connect(c, SIGNAL(startCalib()), this, SIGNAL(startCalib()));
			connect(c, SIGNAL(endCalib()), this, SIGNAL(endCalib()));
			break;
		}
	/// New parameters for termoPage (that is in items.cpp)
	// WARNING! par3 is <type> tag of TERMO page, par4 is <ind_centrale>
	/* Ecco i nuovi Componenti
	TERMO_FANCOIL=52,           Thermoregulation zone with fan-coil control
	TERMO_4Z=53,                4 Zones Thermoregulation
	TERMO_4Z_FANCOIL=54,        4 Zones Thermoregulation with fan-coil control
	TERMO_EXTPROBE=55,          External not-controlled probe
	TERMO_PROBE=56,             Not-controlled probe
	*/
	case ZONANTINTRUS:
		elencoBanner.append(new zonaAnti(this, qdescrizione, (char*)indirizzo, IconaSx, IconaDx, icon));
		break;
	case IMPIANTINTRUS:
		elencoBanner.append(new impAnti(this, (char*)indirizzo, IconaSx, IconaDx, icon, pressedIcon));
		break;
	case SUONO: // iconaSx e' in realta' "value"
		elencoBanner.append(new impBeep(this, IconaSx, IconaDx, icon));
		break;
	case CONTRASTO: // iconaSx e' in realta' "value"
		elencoBanner.append(new impContr(this, IconaSx, IconaDx));
		break;
	case VERSIONE:
		elencoBanner.append(new machVers(this, (versio*)indirizzo, IconaSx));
		break;
	case PROTEZIONE:
		elencoBanner.append(new impPassword(this, IconaDx, icon, pressedIcon, IconaSx, periodo));
		break;
	case MOD_SCENARI:
		elencoBanner.append(new gesModScen(this, (char*)indirizzo, IconaSx, IconaDx, icon, pressedIcon, icoEx1, icoEx2, icoEx3));
		break;
	case SCENARIO_EVOLUTO:
		elencoBanner.append(new scenEvo(this, lc, IconaSx, IconaDx, icon, pressedIcon, action, par3));
		break;
	case SCENARIO_SCHEDULATO:
		elencoBanner.append(new scenSched(this, IconaSx, IconaDx, icon, pressedIcon, descr1, descr2, descr3, descr4));
		break;
	case POSTO_ESTERNO:
		elencoBanner.append(new postoExt(this, qdescrizione, IconaSx, IconaDx, icon, pressedIcon, (char *)indirizzo, light, key, unknown));
		break;
	case SORG_RADIO:
		elencoBanner.append(new sorgenteMultiRadio(this, (char *)indirizzo, IconaSx, IconaDx, icon, descr1));
		break;
	case SORG_AUX:
		elencoBanner.append(new sorgenteMultiAux(this, descrizione, (char *)indirizzo, IconaSx, IconaDx, icon, descr1));
		break;
	case BRIGHTNESS:
		elencoBanner.append(new BannBrightness(this));
		break;
	case CLEANSCREEN:
		elencoBanner.append(new BannCleanScreen(this));
		break;
	case LANSETTINGS:
	{
		bannOnDx *b = new bannOnDx(this);
		b->SetIcons(ICON_INFO, 1);
		elencoBanner.append(b);
		// create lansettings menu
		LanSettings *ls = new LanSettings(this);
		ls->hide();
		// TODO: rimuovere la hideChildren! Rimplementare la hideEvent al suo posto!
		connect(this, SIGNAL(hideChildren()), ls, SLOT(hide()));
		connect(b, SIGNAL(click()), ls, SLOT(showFullScreen()));
		break;
	}
	default:
		assert(!"********** sottoMenu::addItem():unknown item type!!! ************");
	}
	connectLastBanner();

	banner *last = elencoBanner.last();
	last->setText(qdescrizione);
	last->setAnimationParams(periodo,numFrame);
	last->setId(tipo);
	
	for (int idx = elencoBanner.size() - 2; idx >= 0; idx--)
	{
		if (elencoBanner.at(idx)->getId() == tipo)
		{
			elencoBanner.last()->setSerNum(elencoBanner.at(idx)->getSerNum() + 1);
			idx = -1;
		}
	}

	return 1;
}

void sottoMenu::appendBanner(banner *b)
{
	elencoBanner.append(b);
	connectLastBanner();
}

void sottoMenu::connectLastBanner()
{
	banner *last = elencoBanner.last();
	connect(this, SIGNAL(gestFrame(char*)), last, SLOT(gestFrame(char*)));
	connect(this, SIGNAL(parentChanged(QWidget *)), last, SLOT(grandadChanged(QWidget *)));
	connect(last, SIGNAL(richStato(QString)), this, SIGNAL(richStato(QString)));
}

void sottoMenu::showItem(int id)
{
	indice = id;
	forceDraw();
}

void sottoMenu::draw()
{
	qDebug() << "sottoMenu::draw()";
	assert(indice >= 0 && "index of elencoBanner (sottoMenu) less than 0!!");
	if (indicold != indice)
	{
		for (int i = 0; i < elencoBanner.size(); ++i)
			elencoBanner.at(i)->hide();

		if (hasNavBar)
		{
			if (!elencoBanner.isEmpty() && indice < elencoBanner.size())
				bannNavigazione->setCustomButton(elencoBanner.at(indice)->customButton());

			int end = numRighe;
			if (scroll_step != 1)
			{
				// next line takes care of the case when we have to draw 1 or 2 banners only
				// see also ListBrowser::showList()
				unsigned tmp = std::min(indice + numRighe, elencoBanner.size());
				end = tmp - indice;
			}

			for (int i = 0; i < end; ++i)
			{
				if (indice + i < elencoBanner.size() || elencoBanner.size() > numRighe)
				{
					int tmp = (indice + i) % elencoBanner.size();
					int y = i * (height - MAX_HEIGHT / NUM_RIGHE) / numRighe;
					int h = (height - MAX_HEIGHT / NUM_RIGHE) / numRighe;
					qDebug("elencoBanner.at(%d)->setGeometry(%d, %d, %d, %d)", tmp, 0, y, width, h);
					elencoBanner.at(tmp)->setGeometry(0, y, width, h);
					elencoBanner.at(tmp)->Draw();
					elencoBanner.at(tmp)->show();
				}
			}
			qDebug("Invoking bannNavigazione->setGeometry(%d, %d, %d, %d)",
					0, height - MAX_HEIGHT / NUM_RIGHE, width, MAX_HEIGHT / NUM_RIGHE);
			bannNavigazione->setGeometry(0, height - MAX_HEIGHT / NUM_RIGHE, width, MAX_HEIGHT / NUM_RIGHE);
			bannNavigazione->Draw();
			bannNavigazione->show();
		}
		else
		{
			for (int i = 0; i < numRighe; ++i)
			{
				if  (indice + i < elencoBanner.size() || elencoBanner.size() >= numRighe)
				{
					banner *b = elencoBanner.at((indice + i) % elencoBanner.size());
					b->setGeometry(0, i*  QWidget::height() /numRighe, QWidget::width(),QWidget::height()/numRighe);
					b->Draw();
					b->show();
				}
			}
		}
		indicold = indice;
	}
}

void sottoMenu::forceDraw()
{
	qDebug() << "sottoMenu::forceDraw()";
	indicold = indice + 1;
	draw();
}

void sottoMenu::goUp()
{
	if (elencoBanner.size() > numRighe)
	{
		indicold = indice;
		// This should work with both scroll_step = 1 (default) and scroll_step = 3
		// Suppose we have 5 banners, 3 banners per page, scroll_step = 3
		//  . first page, indice == 0, banners shown:0,1,2
		//  . second page, indice == 3, banners shown:3,4
		//  . when user presses arrow down again:(indice == 6) > (5 banners) => show first page
		// Suppose we have scroll_step = 1
		//  . when indice == 4, user presses arrow down:indice == 5 >= 5 banners => indice = 0 (same as previous code)
		indice = indice + scroll_step;
		if (indice >= elencoBanner.size())
			indice = 0;
		draw();
	}
}

void sottoMenu::goDown()
{
	qDebug("sottoMenu::goDown(), numRighe = %d", numRighe);
	if (elencoBanner.count() > numRighe)
	{
		indicold=indice;
		indice = indice - scroll_step;
		if (indice < 0)
		{
			// Suppose we have 5 banners, 3 banners per page, scroll_step = 3
			// when indice == 0 and the user presses arrow up:we must display banner 3,4 only
			unsigned remainder = elencoBanner.count() % scroll_step;
			// remainder == 0 if scroll_step == 1, so this should work with scroll_step default value
			if (remainder)
				indice = elencoBanner.count() - remainder;
			else
				// remember:indice is negative
				indice = elencoBanner.count() + indice;
		}
		qDebug("indice = %d\n", indice);
		draw();
	}
}

void sottoMenu::setNumRighe(uchar n)
{
	numRighe = n;
}

void sottoMenu::setScrollStep(unsigned step)
{
	scroll_step = step;
}

banner* sottoMenu::getLast()
{
	return elencoBanner.last();
}

void sottoMenu::inizializza()
{
	qDebug("sottoMenu::inizializza()");
	QTimer::singleShot(300, this, SLOT(init()));
}

void sottoMenu::init()
{
	qDebug("sottoMenu::init()");
	for (int i = 0; i < elencoBanner.size(); ++i)
		elencoBanner.at(i)->inizializza();
}

void sottoMenu::init_dimmer()
{
	qDebug("sottoMenu::init_dimmer()");
	for (int i = 0; i < elencoBanner.size(); ++i)
	{
		switch(elencoBanner.at(i)->getId())
		{
		case DIMMER:
		case DIMMER_100:
		case ATTUAT_AUTOM:
		case ATTUAT_AUTOM_TEMP:
		case ATTUAT_AUTOM_TEMP_NUOVO_N:
		case ATTUAT_AUTOM_TEMP_NUOVO_F:
			elencoBanner.at(i)->inizializza(true);
			break;
		default:
			break;
		}
	}
}

void sottoMenu::setIndex(QString addr)
{
	for (int i = 0; i < elencoBanner.size(); ++i)
	{
		if (elencoBanner.at(i)->getAddress() == addr)
		{
			qDebug() << "setindex trovato" << addr;
			elencoBanner.at(i)->mostra(banner::BUT2);
			indice = i;
		}
		else
			elencoBanner.at(i)->nascondi(banner::BUT2);
	}
}

void sottoMenu::mostra_all(char but)
{
	for (int i = 0; i < elencoBanner.size(); ++i)
		elencoBanner.at(i)->mostra(but);
}

void sottoMenu::setHeight(int h)
{
	height = h;
}

uchar sottoMenu::getNumRig()
{
	return numRighe;
}

int sottoMenu::getHeight()
{
	return height;
}

void sottoMenu::setGeometry(int x, int y, int w, int h)
{
	//purtroppo in QTE se da un figlio faccio height() o width() mi da le dimensioni del padre...
	qDebug("sottoMenu::setGeometry(%d, %d, %d, %d)", x, y, w, h);
	height = h;
	width = w;
	QWidget::setGeometry(x, y, w, h);
}

void sottoMenu::hideEvent(QHideEvent *event)
{
	qDebug() << "sottoMenu::hideEvent()";
	emit hideChildren();

	// TODO: diffsonora era l'unica a chiamare hide(false), evitando quindi che
	// fosse resettato l'indice.. capire come mai!
	indice = 0;
	/*
	if (index)
	{
		indice = 0;
		forceDraw();
	}
	*/
}

uint sottoMenu::getCount()
{
	return elencoBanner.count();
}

void  sottoMenu::setIndice(char c)
{
	if (c <= elencoBanner.count())
		indice = c;
}

void sottoMenu::showEvent(QShowEvent *event)
{
	qDebug() << "sottoMenu::showEvent()";

	// TODO: sottoMenu deve gestire troppi casi particolari, spezzare!
	if (objectName() == "ILLUMINO")
		init_dimmer();
	forceDraw();
}

void sottoMenu::reparent(QWidget *parent, Qt::WindowFlags f, const QPoint &p, bool showIt)
{
	qDebug("sottoMenu::reparent()");
	emit parentChanged(parent);
	setParent(parent);
	setWindowFlags(f);
	move(p);
}

void sottoMenu::addAmb(char *a)
{
	qDebug("sottoMenu::addAmb(%s)", a);
	for (int idx = elencoBanner.count() - 1; idx >= 0; idx--)
		elencoBanner.at(idx)->addAmb(a);
}

void sottoMenu::initBanner(banner *bann, QDomNode conf)
{
	QDomNode n = findNamedNode(conf, "descr");
	bann->setText(n.toElement().text());

	n = findNamedNode(conf, "id");
	bann->setId(n.toElement().text().toInt());

	bann->setAnimationParams(0, 0);
	// note:we are ignoring the serial number...
}

QDomNode sottoMenu::findNamedNode(QDomNode root, QString name)
{
	QList<QDomNode> nodes;
	nodes.append(root);
	while (!nodes.isEmpty())
	{
		QDomNode n = nodes.first();
		QDomNode item = n.namedItem(name);
		if (item.isNull())
		{
			QDomNodeList list = n.childNodes();
			for (unsigned i = 0; i < list.length(); ++i)
				nodes.append(list.item(i));
			nodes.pop_front();
		}
		else
		{
			return item;
		}
	}
	QDomNode null;
	return null;
}


// Specialized submenus function definition
//
ProgramMenu::ProgramMenu(QWidget *parent, QDomNode conf) : sottoMenu(parent)
{
	conf_root = conf;
}

void ProgramMenu::status_changed(QList<device_status*> sl)
{
	bool update = false;

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (ds->get_type() == device_status::THERMAL_REGULATOR_4Z || ds->get_type() == device_status::THERMAL_REGULATOR_99Z)
		{
			stat_var curr_season(stat_var::SEASON);
			ds->read(device_status_thermal_regulator::SEASON_INDEX, curr_season);
			switch (curr_season.get_val())
			{
			case thermal_regulator::SUMMER:
				if (season == thermal_regulator::WINTER)
				{
					season = thermal_regulator::SUMMER;
					createSummerBanners();
					update = true;
				}
				break;
			case thermal_regulator::WINTER:
				if (season == thermal_regulator::SUMMER)
				{
					season = thermal_regulator::WINTER;
					createWinterBanners();
					update = true;
				}
				break;
			}
		}
	}

	if (update)
		forceDraw();
}


WeeklyMenu::WeeklyMenu(QWidget *parent, QDomNode conf) : ProgramMenu(parent, conf)
{
	season = thermal_regulator::SUMMER;
	createSummerBanners();
}

void WeeklyMenu::createSummerBanners()
{
	elencoBanner.clear();
	const QString i_ok = QString("%1%2").arg(IMG_PATH).arg("btnok.png");
	const QString i_central = QString("%1%2").arg(IMG_PATH).arg("programma_estivo.png");

	if (conf_root.nodeName().contains(QRegExp("item(\\d\\d?)")) == 0)
	{
		qFatal("[TERMO] WeeklyMenu:wrong node in config file");
	}

	QDomNode summer = conf_root.namedItem("summer");
	if (!summer.isNull())
	{
		QDomNode programs = summer.namedItem("prog");
		QDomNode p;
		if (!programs.isNull())
			p = programs.firstChild();
		while (!p.isNull())
		{
			BannWeekly *bp = new BannWeekly(this);
			bp->SetIcons(i_ok, QString(), i_central);
			connect(bp, SIGNAL(programNumber(int)), this, SIGNAL(programClicked(int)));
			// set Text taken from conf.xml
			if (p.isElement())
			{
				bp->setText(p.toElement().text());
				QRegExp re("(\\d)");
				int index = re.indexIn(p.nodeName());
				if (index != -1)
					bp->setProgram(re.cap(1).toInt());
			}
			elencoBanner.append(bp);
			p = p.nextSibling();
		}
	}
}

void WeeklyMenu::createWinterBanners()
{
	elencoBanner.clear();
	const QString i_ok = QString("%1%2").arg(IMG_PATH).arg("btnok.png");
	const QString i_central = QString("%1%2").arg(IMG_PATH).arg("programma_invernale.png");

	if (conf_root.nodeName().contains(QRegExp("item(\\d\\d?)")) == 0)
	{
		qFatal("[TERMO] WeeklyMenu:wrong node in config file");
	}

	QDomNode winter = conf_root.namedItem("winter");
	if (!winter.isNull())
	{
		QDomNode programs = winter.namedItem("prog");
		QDomNode p;
		if (!programs.isNull())
			p = programs.firstChild();
		while (!p.isNull())
		{
			BannWeekly *bp = new BannWeekly(this);
			bp->SetIcons(i_ok, QString(), i_central);
			connect(bp, SIGNAL(programNumber(int)), this, SIGNAL(programClicked(int)));
			// set Text taken from conf.xml
			if (p.isElement())
			{
				bp->setText(p.toElement().text());
				QRegExp re("(\\d)");
				int index = re.indexIn(p.nodeName());
				if (index != -1)
					bp->setProgram(re.cap(1).toInt());
			}
			elencoBanner.append(bp);
			p = p.nextSibling();
		}
	}
}

ScenarioMenu::ScenarioMenu(QWidget *parent, QDomNode conf) : ProgramMenu(parent, conf)
{
	season = thermal_regulator::SUMMER;
	createSummerBanners();
}

void ScenarioMenu::createSummerBanners()
{
	elencoBanner.clear();
	const QString i_ok = QString("%1%2").arg(IMG_PATH).arg("btnok.png");
	const QString i_central = QString("%1%2").arg(IMG_PATH).arg("scenario_estivo.png");

	if (conf_root.nodeName().contains(QRegExp("item(\\d\\d?)")) == 0)
	{
		qFatal("[TERMO] ScenarioMenu:wrong node in config file");
	}

	QDomNode summer = conf_root.namedItem("summer");
	if (!summer.isNull())
	{
		QDomNode programs = summer.namedItem("scen");
		QDomNode p;
		if (!programs.isNull())
			p = programs.firstChild();
		while (!p.isNull())
		{
			BannWeekly *bp = new BannWeekly(this);
			bp->SetIcons(i_ok, QString(), i_central);
			connect(bp, SIGNAL(programNumber(int)), this, SIGNAL(programClicked(int)));
			// set Text taken from conf.xml
			if (p.isElement())
			{
				bp->setText(p.toElement().text());
				QRegExp re("(\\d+)");
				int index = re.indexIn(p.nodeName());
				if (index != -1)
					bp->setProgram(re.cap(1).toInt());
			}
			elencoBanner.append(bp);
			p = p.nextSibling();
		}
	}
}

void ScenarioMenu::createWinterBanners()
{
	elencoBanner.clear();
	const QString i_ok = QString("%1%2").arg(IMG_PATH).arg("btnok.png");
	const QString i_central = QString("%1%2").arg(IMG_PATH).arg("scenario_invernale.png");

	if (conf_root.nodeName().contains(QRegExp("item(\\d\\d?)")) == 0)
	{
		qFatal("[TERMO] ScenarioMenu:wrong node in config file");
	}

	QDomNode winter = conf_root.namedItem("winter");
	if (!winter.isNull())
	{
		QDomNode programs = winter.namedItem("scen");
		QDomNode p;
		if (!programs.isNull())
			p = programs.firstChild();
		while (!p.isNull())
		{
			BannWeekly *bp = new BannWeekly(this);
			bp->SetIcons(i_ok, QString(), i_central);
			connect(bp, SIGNAL(programNumber(int)), this, SIGNAL(programClicked(int)));
			// set Text taken from conf.xml
			if (p.isElement())
			{
				bp->setText(p.toElement().text());
				QRegExp re("(\\d+)");
				int index = re.indexIn(p.nodeName());
				if (index != -1)
					bp->setProgram(re.cap(1).toInt());
			}
			elencoBanner.append(bp);
			p = p.nextSibling();
		}
	}
}


TimeEditMenu::TimeEditMenu(QWidget *parent) : sottoMenu(parent, 10, MAX_WIDTH, MAX_HEIGHT, 1)
{
	setNavBarMode(10, IMG_OK);
	time_edit = new FSBannTime(this);
	elencoBanner.append(time_edit);
	connect(bannNavigazione, SIGNAL(forwardClick()), this, SLOT(performAction()));
}

void TimeEditMenu::performAction()
{
	emit timeSelected(time());
}

BtTime TimeEditMenu::time()
{
	return time_edit->time();
}

DateEditMenu::DateEditMenu(QWidget *parent) : sottoMenu(parent, 10, MAX_WIDTH, MAX_HEIGHT, 1)
{
	setNavBarMode(10, IMG_OK);
	date_edit = new FSBannDate(this);
	elencoBanner.append(date_edit);
	connect(bannNavigazione, SIGNAL(forwardClick()), this, SLOT(performAction()));
}

void DateEditMenu::performAction()
{
	emit dateSelected(date());
}

QDate DateEditMenu::date()
{
	return date_edit->date();
}
