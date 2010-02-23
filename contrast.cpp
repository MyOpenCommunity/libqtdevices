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


#include "contrast.h"
#include "main.h"
#include "generic_functions.h" // getContrast, setContrast
#include "btbutton.h"

#include <QFrame>
#include <QString>
#include <QLabel>


Contrast::Contrast()
{
	aumBut = new BtButton(this);
	decBut = new BtButton(this);
	okBut = new BtButton(this);

	paintLabel = new QLabel(this);
	colorBar = new QLabel(this);

	decBut->setGeometry(0,MAX_HEIGHT-BUT_DIM, BUT_DIM, BUT_DIM);
	aumBut->setGeometry(MAX_WIDTH-BUT_DIM, MAX_HEIGHT-BUT_DIM, BUT_DIM, BUT_DIM);
	okBut->setGeometry((MAX_WIDTH-BUT_DIM)/2,MAX_HEIGHT-BUT_DIM,BUT_DIM,BUT_DIM);

	paintLabel->setGeometry((MAX_WIDTH-IMG_X)/2, (MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE-2*IMG_Y)/2, IMG_X, IMG_Y);
	paintLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
	paintLabel->setPixmap(QPixmap(IMG_PATH "my_home.png"));

	colorBar ->setGeometry((MAX_WIDTH-IMG_X)/2, (MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE-2*IMG_Y)/2+IMG_Y, IMG_X, IMG_Y);
	colorBar ->setFrameStyle(QFrame::Panel | QFrame::Raised);
	colorBar ->setPixmap(QPixmap(IMG_PATH "colorbar.png"));

	decBut->setImage(ICON_MENO);
	aumBut->setImage(ICON_PIU);
	okBut->setImage(ICON_OK);

	aumBut->show();
	decBut->show();
	okBut->show();
	paintLabel->show();
	colorBar->show();
	aumBut->setAutoRepeat(true);
	decBut->setAutoRepeat(true);
	connect(decBut,SIGNAL(clicked()),this,SLOT(decContr()));
	connect(aumBut,SIGNAL(clicked()),this,SLOT(aumContr()));
	connect(okBut,SIGNAL(clicked()),this,SIGNAL(Closed()));
}

void Contrast::aumContr()
{
	uchar c;

	c = getContrast();
	if (c < 140)
		setContrast(c+10, FALSE);
}

void Contrast::decContr()
{
	uchar c;

	c = getContrast();
	if (c >= 10)
		setContrast(c-10,FALSE);
}
