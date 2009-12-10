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
#include "main.h" // Season
#include "btbutton.h"
#include "generic_functions.h"
#include "bannfrecce.h"
#include "banner.h"

#include <QPixmap>
#include <QTimer>
#include <QRegExp>
#include <QDebug>


sottoMenu::sottoMenu(QWidget *parent, uchar navBarMode,int wi,int hei, uchar n) : Page(parent)
{
	numRighe = n;
	scroll_step = 1;
	hasNavBar = navBarMode;
	width = wi;
	height = hei;

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
	// reset the index only when we are closing
	// we can't rely on hideEvent() because QStackedWidget (ie main_window) hides and shows
	// the pages it is displaying
	connect(this, SIGNAL(Closed()), SLOT(resetIndex()));

	// TODO: verificare se il setGeometry serve davvero! (dipende da se width e height servono o no..)
	setGeometry(0,0,width,height);
	if (!parentWidget())
		showPage();
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
		if (elencoBanner.at(idx)->getId() == b->getId() && b->getId() != -1)
		{
			b->setSerNum(elencoBanner.at(idx)->getSerNum() + 1);
			idx = -1;
		}
	}
}

void sottoMenu::connectLastBanner()
{
	banner *last = elencoBanner.last();
	connect(last, SIGNAL(richStato(QString)), this, SIGNAL(richStato(QString)));
}

void sottoMenu::showItem(int id)
{
	indice = id;
	forceDraw();
}

void sottoMenu::draw()
{
	Q_ASSERT_X(indice >= 0, "sottoMenu::draw", "index of elencoBanner (sottoMenu) less than 0!!");
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
					elencoBanner.at(tmp)->setGeometry(0, y, width, h);
					elencoBanner.at(tmp)->Draw();
					elencoBanner.at(tmp)->show();
				}
			}
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
	QTimer::singleShot(300, this, SLOT(init()));
}

void sottoMenu::init()
{
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

void sottoMenu::resetIndex()
{
	indice = 0;
}

void sottoMenu::showEvent(QShowEvent *event)
{
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
