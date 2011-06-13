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
#include "media_device.h"
#include "fontmanager.h" // bt_global::font
#include "navigation_bar.h"

#include <QLabel>
#include <QGridLayout>


AuxPage::AuxPage(SourceDevice *dev, const QString &description)
{
	QWidget *content = new QWidget;
	QGridLayout *main_layout = new QGridLayout(content);
	main_layout->setSpacing(10);
	main_layout->setContentsMargins(0, 0, 0, 0);

	QLabel *title_label = new QLabel(description);
	title_label->setFont(bt_global::font->get(FontManager::SUBTITLE));
	main_layout->addWidget(title_label, 0, 0, 1, 1, Qt::AlignHCenter);

	QLabel *ambient_label = new QLabel;
	ambient_label->setFont(bt_global::font->get(FontManager::SUBTITLE));
	main_layout->addWidget(ambient_label, 1, 0, 1, 1, Qt::AlignHCenter);
	// The label for the ambient has a text only in the sounddiffusion
	// multichannel. Because we want a fixed layout for both cases, we set
	// the min height for the row as the sizeHint height of the label (that is
	// equal to the height of the label with some text inside).
	main_layout->setRowMinimumHeight(1, ambient_label->sizeHint().height());

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	buildPage(content, nav_bar);
}


