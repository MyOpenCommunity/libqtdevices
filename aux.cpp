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
#include "btlabel.h"
#include "genericfunz.h"
#include "fontmanager.h"

#include <QPixmap>
#include <QCursor>
#include <QFile>


aux::aux(QWidget *parent, const QString & name, const QString & amb) : QWidget(parent)
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	setCursor(QCursor(Qt::BlankCursor));
#endif
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

	bannNavigazione = new bannFrecce(this,"bannerfrecce",1);
	bannNavigazione->setGeometry(0 ,MAX_HEIGHT- MAX_HEIGHT/NUM_RIGHE ,MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE);

	auxName = new BtLabel(this);
	ambDescr = new BtLabel(this);
	ambDescr->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
	QFont aFont;
	FontManager::instance()->getFont(font_aux_descr_ambiente, aFont);
	ambDescr->setFont(aFont);
	ambDescr->setText(amb);
	auxName->setGeometry(0,30,240,40);
	auxName->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

	FontManager::instance()->getFont(font_aux_nome_sorgente, aFont);
	auxName->setFont(aFont);
	auxName->setText(name);
	ambDescr->setGeometry(0,100,240,40);
	fwdBut = new BtButton(this);
	fwdBut->setGeometry(MAX_WIDTH/2-30, 160, 60, 60);
	QPixmap* Icon = new QPixmap();
	QPixmap* pressIcon = new QPixmap();
	char pressIconName[MAX_PATH];
	Icon->load(ICON_FFWD);
	getPressName((char*)ICON_FFWD, &pressIconName[0],sizeof(pressIconName));
	fwdBut->setPixmap(*Icon);
	if (QFile::exists(pressIconName))
	{
		pressIcon->load(pressIconName);
		fwdBut->setPressedPixmap(*pressIcon);
	}
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

void aux::freezed(bool f)
{
	qDebug("aux::freezed()");
	// Disable aux and all of its children
	setDisabled(f);
}
