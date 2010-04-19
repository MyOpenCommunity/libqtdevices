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
#include "skinmanager.h"
#include "hardware_functions.h" // getContrast, setContrast
#include "btbutton.h"

#include <QFrame>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>


static BtButton *getButton(QString img, bool autorepeat)
{
	BtButton *btn = new BtButton;
	btn->setImage(bt_global::skin->getImage(img));
	btn->setAutoRepeat(autorepeat);
	return btn;
}

Contrast::Contrast()
{
	// create buttons
	BtButton *decBut = getButton("minus", true);
	BtButton *incBut = getButton("plus", true);
	BtButton *okBut = getButton("ok", false);

	connect(decBut, SIGNAL(clicked()), SLOT(decContrast()));
	connect(incBut, SIGNAL(clicked()), SLOT(incContrast()));
	connect(okBut,  SIGNAL(clicked()), SIGNAL(Closed()));

	// create images
	QLabel *paintLabel = new QLabel;
	QLabel *colorBar = new QLabel;

	paintLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
	paintLabel->setPixmap(QPixmap(bt_global::skin->getImage("logo")));

	colorBar ->setFrameStyle(QFrame::Panel | QFrame::Raised);
	colorBar ->setPixmap(QPixmap(bt_global::skin->getImage("colorbar")));

	// layout
	QHBoxLayout *b = new QHBoxLayout;
	b->setContentsMargins(0, 0, 0, 0);
	b->setSpacing(0);

	b->addWidget(decBut, 0, Qt::AlignLeft);
	b->addWidget(okBut);
	b->addWidget(incBut, 0, Qt::AlignRight);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);

	l->addWidget(paintLabel, 0, Qt::AlignCenter);
	l->addWidget(colorBar, 0, Qt::AlignCenter);
	l->addLayout(b);
}

void Contrast::incContrast()
{
	int c = getContrast();
	if (c < 140)
		setContrast(c+10);
}

void Contrast::decContrast()
{
	int c = getContrast();
	if (c >= 10)
		setContrast(c-10);
}
