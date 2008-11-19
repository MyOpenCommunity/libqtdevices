#include "banner.h"
#include "allarme.h"
#include "bannfrecce.h"
#include "main.h"
#include "fontmanager.h"

#include <QPixmap>
#include <QWidget>
#include <QCursor>
#include <QLabel>


/*****************************************************************
** Generic alarm
****************************************************************/	

allarme::allarme(QWidget *parent, const QString & name, char *indirizzo, char *IconaDx, altype t): QFrame(parent)
{
    qDebug("allarme::allarme()");
    qDebug("indirizzo = %s, IconaDx = %s, tipo = %d", indirizzo, IconaDx, t);
    type = t;
    SetIcons(IconaDx);
    setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT);
    setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));
    descr->setText(name);
    connect(bnav, SIGNAL(backClick()), this, SIGNAL(Back()));
    connect(bnav, SIGNAL(upClick()), this, SIGNAL(Prev()));
    connect(bnav, SIGNAL(downClick()), this, SIGNAL(Next()));
    connect(bnav, SIGNAL(forwardClick()), this, SIGNAL(Delete()));
    // ??
    show();
}

void allarme::SetIcons(QString icon)
{
    qDebug("allarme::SetIcons()");
	QPixmap p;
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
	p.load(icon_name);
    Immagine = new QLabel(this);
    Immagine->setPixmap(p);
    Immagine->setGeometry(MAX_WIDTH/2 - ICON_DIM/2, MAX_HEIGHT/(4*NUM_RIGHE),
		ICON_DIM, MAX_HEIGHT/NUM_RIGHE);

    descr = new QLabel(this);
    QFont aFont;
    FontManager::instance()->getFont(font_allarme_descr, aFont);
    descr->setFont(aFont);
    descr->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    descr->setGeometry(0, MAX_HEIGHT/2 - (MAX_HEIGHT/NUM_RIGHE)/2,MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);

    bnav = new bannFrecce(this, 4, icon);
    bnav->setGeometry(0 , MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE, MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);
}

void allarme::draw()
{
    qDebug("allarme::draw()");
}

void allarme::showEvent(QShowEvent *event)
{
    qDebug("allarme::showEvent()");
    bnav->show();
    Immagine->show();
    descr->show();
}
