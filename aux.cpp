/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


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
