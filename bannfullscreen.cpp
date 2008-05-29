/*!
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 * \brief A base class for all banners that are fullscreen
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "bannfullscreen.h"
#include "fontmanager.h"
#include "main.h"
#include "sottomenu.h"

BannFullScreen::BannFullScreen(QWidget *parent, const char *name, QColor bg, QColor fg, QColor second_fg)
	: banner(parent, name)
{
	int h = (MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE - BUTMENPLUS_DIM_Y - DESCR_DIM_Y - TEMPMIS_Y - OFFSET_Y) / 5;
	temp_label = new QLabel(this, 0);
	temp_label->setGeometry(0, h + DESCR_DIM_Y, MAX_WIDTH, TEMPMIS_Y);
	descr_label = new QLabel(this, 0);
	descr_label->setGeometry(0, DESCR_DIM_Y + 50, MAX_WIDTH, TEMPMIS_Y);
	temp = "-23.5\272C";
	descr = "Zone";
}

void BannFullScreen::Draw()
{
	QFont aFont;
	FontManager::instance()->getFont(font_banTermo_tempMis, aFont);
	temp_label -> setFont(aFont);
	temp_label -> setAlignment(AlignHCenter|AlignVCenter);
	temp_label -> setText(temp);
	FontManager::instance()->getFont(font_banTermo_testo, aFont);
	descr_label -> setFont(aFont);
	descr_label -> setAlignment(AlignHCenter|AlignVCenter);
	descr_label -> setText(temp);

	banner::Draw();
}

void BannFullScreen::postDisplay()
{
	sottoMenu *parent = static_cast<sottoMenu *> (parentWidget());
	parent->setNavBarMode(3, "");
	qDebug("[TERMO] bfs::postDisplay() done");
}
