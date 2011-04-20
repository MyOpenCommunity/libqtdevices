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
	BtButton *btn = new BtButton(bt_global::skin->getImage(img));
	btn->setAutoRepeat(autorepeat);
	return btn;
}

Contrast::Contrast()
{
	// create buttons
	BtButton *dec_but = getButton("minus", true);
	BtButton *inc_but = getButton("plus", true);
	BtButton *ok_but = getButton("ok", false);

	connect(dec_but, SIGNAL(clicked()), SLOT(decContrast()));
	connect(inc_but, SIGNAL(clicked()), SLOT(incContrast()));
	connect(ok_but,  SIGNAL(clicked()), SIGNAL(Closed()));

	// create images
	QLabel *paint_label = new QLabel;
	QLabel *color_bar = new QLabel;

	paint_label->setFrameStyle(QFrame::Panel | QFrame::Raised);
	paint_label->setPixmap(QPixmap(bt_global::skin->getImage("logo")));

	color_bar->setFrameStyle(QFrame::Panel | QFrame::Raised);
	color_bar->setPixmap(QPixmap(bt_global::skin->getImage("colorbar")));

	// layout
	QHBoxLayout *b = new QHBoxLayout;
	b->setContentsMargins(0, 0, 0, 0);
	b->setSpacing(0);

	b->addWidget(dec_but, 0, Qt::AlignLeft);
	b->addWidget(ok_but);
	b->addWidget(inc_but, 0, Qt::AlignRight);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(5, 5, 5, 5);
	l->setSpacing(15);

	l->addWidget(paint_label, 0, Qt::AlignCenter);
	l->addWidget(color_bar, 0, Qt::AlignCenter);
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
