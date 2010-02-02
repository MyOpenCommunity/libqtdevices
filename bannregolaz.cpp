/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannRegolaz.cpp
**
**Riga con tasto ON OFF, icona centrale divisa in due per regolazione
**
****************************************************************/

#include "bannregolaz.h"
#include "btbutton.h"
#include "fontmanager.h" // fontmanager

#include <QLabel>
#include <QHBoxLayout>

#define TIME_RIP_REGOLAZ 500

BannLevel::BannLevel(QWidget *parent) :
	BannerNew(parent)
{
	left_button = new BtButton;
	center_left_button = new BtButton;
	center_right_button = new BtButton;
	right_button = new BtButton;

	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(0);
	hbox->addWidget(left_button);
	hbox->addWidget(center_left_button);
	hbox->addWidget(center_right_button);
	hbox->addWidget(right_button);

	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));

	QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->setContentsMargins(0, 0, 0, 0);
	vbox->setSpacing(0);
	vbox->addLayout(hbox);
	vbox->addWidget(text);

	timer.setInterval(TIME_RIP_REGOLAZ);
	// start rate controlling timer
	connect(center_left_button, SIGNAL(pressed()), SLOT(startLeftTimer()));
	connect(center_right_button, SIGNAL(pressed()), SLOT(startRightTimer()));

	connect(center_left_button, SIGNAL(released()), &timer, SLOT(stop()));
	connect(center_right_button, SIGNAL(released()), &timer, SLOT(stop()));

	// we also want the user to be able to bash the button until the screen is broken
	connect(center_left_button, SIGNAL(clicked()), SIGNAL(center_left_clicked()));
	connect(center_right_button, SIGNAL(clicked()), SIGNAL(center_right_clicked()));
}

void BannLevel::initBanner(const QString &banner_text)
{
	text->setText(banner_text);
}

void BannLevel::initBanner(const QString &left, const QString &center_left, const QString &center_right,
		const QString &right, const QString &banner_text)
{
	left_button->setImage(left);
	center_left_button->setImage(center_left);
	center_right_button->setImage(center_right);
	right_button->setImage(right);
	text->setText(banner_text);
}

void BannLevel::setCenterLeftIcon(const QString &image)
{
	center_left_button->setImage(image);
}

void BannLevel::setCenterRightIcon(const QString &image)
{
	center_right_button->setImage(image);
}

void BannLevel::startLeftTimer()
{
	if (!timer.isActive())
	{
		timer.disconnect(SIGNAL(timeout()));
		connect(&timer, SIGNAL(timeout()), SIGNAL(center_left_clicked()));
		timer.start();
	}
}

void BannLevel::startRightTimer()
{
	if (!timer.isActive())
	{
		timer.disconnect(SIGNAL(timeout()));
		connect(&timer, SIGNAL(timeout()), SIGNAL(center_right_clicked()));
		timer.start();
	}
}



bannRegolaz::bannRegolaz(QWidget *parent) : BannerOld(parent)
{
	addItem(BUT2, 0, 0, BANREGOL_BUT_DIM , BANREGOL_BUT_DIM);
	addItem(BUT1, banner_width - BANREGOL_BUT_DIM, 0, BANREGOL_BUT_DIM , BANREGOL_BUT_DIM);
	addItem(BUT3, (banner_width - BUTREGOL_ICON_DIM_X)/2, 0, BUTREGOL_ICON_DIM_X/2, BUTREGOL_ICON_DIM_Y);
	addItem(BUT4, banner_width/2, 0, BUTREGOL_ICON_DIM_X/2, BUTREGOL_ICON_DIM_Y);
	addItem(TEXT, 0, BANREGOL_BUT_DIM, banner_width, banner_height - BANREGOL_BUT_DIM);

	timRip = NULL;

	connect(this,SIGNAL(cdxPressed()),this,SLOT(armTimRipdx()));
	connect(this,SIGNAL(cdxReleased()),this,SLOT(killTimRipdx()));
	connect(this,SIGNAL(csxPressed()),this,SLOT(armTimRipsx()));
	connect(this,SIGNAL(csxReleased()),this,SLOT(killTimRipsx()));
}

void bannRegolaz::armTimRipdx()
{
	if  (!timRip)
		timRip = new QTimer(this);
	if (!(timRip->isActive()))
	{
		timRip->start(TIME_RIP_REGOLAZ);
		disconnect(timRip,SIGNAL(timeout()),this,SIGNAL(csxClick()));
		connect(timRip,SIGNAL(timeout()),this,SIGNAL(cdxClick()));
	}
}

void bannRegolaz::armTimRipsx()
{
	if (!timRip)
		timRip = new QTimer(this);

	if (!(timRip->isActive()))
	{
		timRip->start(TIME_RIP_REGOLAZ);
		disconnect(timRip,SIGNAL(timeout()),this,SIGNAL(cdxClick()));
		connect(timRip,SIGNAL(timeout()),this,SIGNAL(csxClick()));
	}
}

void bannRegolaz::killTimRipdx()
{
	if (timRip)
		timRip->stop();
}

void bannRegolaz::killTimRipsx()
{
	if (timRip)
		timRip->stop();
}
