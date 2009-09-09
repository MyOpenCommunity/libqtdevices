#include "banner.h"
#include "allarme.h"
#include "bannfrecce.h"
#include "main.h"
#include "fontmanager.h" // bt_global::font
#include "icondispatcher.h" // bt_global::icons_cache

#include <QPixmap>
#include <QWidget>
#include <QDebug>
#include <QLabel>


/*****************************************************************
** Generic alarm
****************************************************************/	

allarme::allarme(const QString &name, char *indirizzo, QString IconaDx, altype t)
{
	qDebug("allarme::allarme()");
	qDebug() << "indirizzo =" << indirizzo << ", IconaDx =" << IconaDx << ", tipo = " << t;
	type = t;
	SetIcons(IconaDx);
	descr->setText(name);
	connect(bnav, SIGNAL(backClick()), this, SIGNAL(Closed()));
	connect(bnav, SIGNAL(upClick()), this, SIGNAL(Prev()));
	connect(bnav, SIGNAL(downClick()), this, SIGNAL(Next()));
	connect(bnav, SIGNAL(forwardClick()), this, SIGNAL(Delete()));
}

void allarme::SetIcons(QString icon)
{
	qDebug("allarme::SetIcons()");
	QString icon_name;
	switch (type)
	{
    case allarme::TECNICO:
		icon_name = IMG_PATH "imgalltec.png";
		break;
    default:
		icon_name = IMG_PATH "imgallintr.png";
		break;
	}
	Immagine = new QLabel(this);
	Immagine->setPixmap(*bt_global::icons_cache.getIcon(icon_name));
	Immagine->setGeometry(MAX_WIDTH/2 - ICON_DIM/2, MAX_HEIGHT/(4*NUM_RIGHE),
		ICON_DIM, MAX_HEIGHT/NUM_RIGHE);

	descr = new QLabel(this);
	descr->setFont(bt_global::font->get(FontManager::TEXT));
	descr->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	descr->setGeometry(0, MAX_HEIGHT/2 - (MAX_HEIGHT/NUM_RIGHE)/2,MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);

	bnav = new bannFrecce(this, 4, icon);
	bnav->setGeometry(0 , MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE, MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);

	bnav->show();
	Immagine->show();
	descr->show();
}

void allarme::draw()
{
    qDebug("allarme::draw()");
}

