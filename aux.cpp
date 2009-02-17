/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** aux.cpp
 **
 **finestra di dati sulla sorgente aux
 **
 ****************************************************************/

#include "aux.h"
#include "bannfrecce.h"
#include "btbutton.h"
#include "generic_functions.h"
#include "fontmanager.h" // bt_global::font

#include <QPixmap>
#include <QLabel>
#include <QFile>


aux::aux(QWidget *parent, const QString & name, const QString & amb) : QWidget(parent)
{
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

	bannNavigazione = new bannFrecce(this, 1);
	bannNavigazione->setGeometry(0 ,MAX_HEIGHT- MAX_HEIGHT/NUM_RIGHE ,MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE);

	auxName = new QLabel(this);
	ambDescr = new QLabel(this);
	ambDescr->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
	ambDescr->setFont(bt_global::font->get(FontManager::SUBTITLE));
	ambDescr->setText(amb);
	auxName->setGeometry(0,30,240,40);
	auxName->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

	auxName->setFont(bt_global::font->get(FontManager::SUBTITLE));
	auxName->setText(name);
	ambDescr->setGeometry(0,100,240,40);

	fwdBut = new BtButton(this);
	fwdBut->setGeometry(MAX_WIDTH/2-30, 160, 60, 60);
	fwdBut->setImage(ICON_FFWD);

	connect(fwdBut, SIGNAL(clicked()), this, SIGNAL(Btnfwd()));
	connect(bannNavigazione, SIGNAL(backClick()),this,SIGNAL(Closed()));
}

void aux::showAux()
{
	draw();
	showFullScreen();
}

void aux::setAmbDescr(const QString & d)
{
	ambDescr->setText(d);
}

void aux::draw()
{
}
