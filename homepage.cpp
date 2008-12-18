/****************************************************************
***
* BTicino Touch scren Colori art. H4686
**
** homePage.cpp
**
** Finestra principale
**
****************************************************************/

#include "homepage.h"
#include "timescript.h"
#include "fontmanager.h"
#include "temperatureviewer.h"
#include "btbutton.h"

#include <QLabel>

#define DIM_BUT 80


homePage::homePage()
{
	temp_viewer = new TemperatureViewer(this);
}

void homePage::addButton(int x, int y, QString iconName, char function, QString chix, QString cosax,QString dovex, char tipo)
{
	BtButton *b = new BtButton(this);
	b->setGeometry(x, y, DIM_BUT, DIM_BUT);
	b->setImage(iconName);

	switch (function)
	{
	case AUTOMAZIONE:
		connect(b,SIGNAL(clicked()),this, SIGNAL(Automazione()));
		break;
	case ILLUMINAZIONE:
		connect(b,SIGNAL(clicked()),this, SIGNAL(Illuminazione()));
		break;
	case ANTIINTRUSIONE:
		connect(b,SIGNAL(clicked()),this, SIGNAL(Antiintrusione()));
		break;
	case CARICHI:
		connect(b,SIGNAL(clicked()),this, SIGNAL(Carichi()));
		break;
	case TERMOREGOLAZIONE:
		connect(b,SIGNAL(clicked()),this, SIGNAL(Termoregolazione()));
		break;
	case DIFSON:
		connect(b,SIGNAL(clicked()),this, SIGNAL(Difson()));
		break;
	case DIFSON_MULTI:
		connect(b,SIGNAL(clicked()),this, SIGNAL(Difmulti()));
		break;
	case SCENARI:
		connect(b,SIGNAL(clicked()),this, SIGNAL(Scenari()));
		break;
	case IMPOSTAZIONI:
		connect(b,SIGNAL(clicked()),this, SIGNAL(Settings()));
		break;
	case SCENARI_EVOLUTI:
		connect(b, SIGNAL(clicked()), this, SIGNAL(ScenariEvoluti()));
		break;
	case VIDEOCITOFONIA:
		connect(b,SIGNAL(clicked()),this, SIGNAL(Videocitofonia()));
		break;
	case SUPERVISIONE:
		connect(b, SIGNAL(clicked()), this, SIGNAL(Supervisione()));
		break;
	}
}

void homePage::addClock(int x, int y,int width,int height,int style, int line)
{
	timeScript *d = new timeScript(this, 1);
	d->setGeometry(x,y,width,height);
	d->setFrameStyle(style);
	d->setLineWidth(line);
}

void homePage::addDate(int x, int y,int width,int height, int style, int line)
{
	timeScript *d = new timeScript(this, 25);
	d->setGeometry(x,y,width,height);
	d->setFrameStyle(style);
	d->setLineWidth(line);
}

void homePage::addTemp(QString z, int x, int y, int width, int height, int style, int line, const QString & qtext, const char *Ext)
{
	temp_viewer->add(z, x, y, width, height, style, line, qtext, QString(Ext));
}

void homePage::inizializza()
{
	temp_viewer->inizializza();
}

void homePage::addDescr(const QString & qz, int x, int y, int width, int height, int style,int line)
{
	QFont aFont;
	FontManager::instance()->getFont(font_homepage_bottoni_descrizione, aFont);
	QLabel *descr = new QLabel(this);
	descr->setFont(aFont);
	descr->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	descr->setText(qz);
	descr->setGeometry(x,y,width,height);
	descr->setFrameStyle(style);
	descr->setLineWidth(line);
}

void homePage::gestFrame(char* frame)
{
	temp_viewer->gestFrame(frame);
}

