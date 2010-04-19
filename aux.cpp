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
#include "btbutton.h"
#include "main.h" // ICON_FFWD
#include "fontmanager.h" // bt_global::font
#include "navigation_bar.h"

#include <QVBoxLayout>
#include <QLabel>


Aux::Aux(const QString &name, const QString &amb)
{
	QWidget *content = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(content);

	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);

	main_layout->addSpacing(30);
	QLabel *aux_name = new QLabel;
	aux_name->setFont(bt_global::font->get(FontManager::SUBTITLE));
	aux_name->setText(name);
	main_layout->addWidget(aux_name, 0, Qt::AlignHCenter);

	main_layout->addSpacing(30);
	amb_descr = new QLabel;
	amb_descr->setFont(bt_global::font->get(FontManager::SUBTITLE));
	amb_descr->setText(amb);
	main_layout->addWidget(amb_descr, 0, Qt::AlignHCenter);

	main_layout->addSpacing(20);
	BtButton *fwd_but = new BtButton;
	fwd_but->setImage(ICON_FFWD);
	main_layout->addWidget(fwd_but, 0, Qt::AlignHCenter);
	main_layout->addStretch(1);

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	buildPage(content, nav_bar);

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(fwd_but, SIGNAL(clicked()), SIGNAL(Btnfwd()));
}

void Aux::setAmbDescr(const QString &d)
{
	amb_descr->setText(d);
}

