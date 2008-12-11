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
#include "btbutton.h"
#include "generic_functions.h"
#include "xmlconfhandler.h"
#include "bannfrecce.h"
#include "actuators.h"
#include "amplificatori.h"
#include "poweramplifier.h"
#include "sorgentiradio.h"
#include "sorgentiaux.h"
#include "sorgentimedia.h"
#include "carico.h"
#include "bannfullscreen.h"

#include <QByteArray>
#include <QPixmap>
#include <QTimer>
#include <QRegExp>
#include <QDebug>

#include <algorithm>

#include <assert.h>

#define IMG_OK IMG_PATH "btnok.png"

sottoMenu::sottoMenu(QWidget *parent, uchar navBarMode,int wi,int hei, uchar n) : Page(parent)
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

void sottoMenu::appendBanner(banner *b)
{
	elencoBanner.append(b);
	connectLastBanner();

	for (int idx = elencoBanner.size() - 2; idx >= 0; idx--)
	{
		if (elencoBanner.at(idx)->getId() == b->getId())
		{
			b->setSerNum(elencoBanner.at(idx)->getSerNum() + 1);
			idx = -1;
		}
	}
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
					b->setGeometry(0, i*  Page::height() /numRighe, Page::width(), Page::height()/numRighe);
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

uchar sottoMenu::getNumRig()
{
	return numRighe;
}

void sottoMenu::setGeometry(int x, int y, int w, int h)
{
	//purtroppo in QTE se da un figlio faccio height() o width() mi da le dimensioni del padre...
	qDebug("sottoMenu::setGeometry(%d, %d, %d, %d)", x, y, w, h);
	height = h;
	width = w;
	Page::setGeometry(x, y, w, h);
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

void sottoMenu::showEvent(QShowEvent *event)
{
	qDebug() << "sottoMenu::showEvent()";
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

// Specialized submenus function definition
//
ProgramMenu::ProgramMenu(QWidget *parent, QDomNode conf) : sottoMenu(parent)
{
	conf_root = conf;
}

void ProgramMenu::status_changed(QList<device_status*> sl)
{
	// TODO: vedere se da bannfullscreen puo' arrivare un segnale season_changed
	// con il quale il programMenu possa aggiornarsi graficamente. In questo modo
	// si evitano dipendenze del sottomenu con i device_status.
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
	// TODO: riscrivere con le nuove funzioni di xml_functions!!
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
