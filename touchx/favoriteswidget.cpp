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


#include "favoriteswidget.h"
#include "xml_functions.h"
#include "fontmanager.h"
#include "bannerfactory.h"
#include "banner.h"

#include <QVBoxLayout>
#include <QLabel>


FavoritesWidget::FavoritesWidget()
{
	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

void FavoritesWidget::loadItems(const QDomNode &config_node)
{
	QVBoxLayout *l = new QVBoxLayout(this);
	QLabel *title = new QLabel(getTextChild(config_node, "descr"));

	title->setFont(bt_global::font->get(FontManager::TITLE));
	title->setAlignment(Qt::AlignHCenter);

	l->addWidget(title);

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		if (banner *b = getBanner(item))
			l->addWidget(b);
		else
		{
			int id = getTextChild(item, "id").toInt();
			qFatal("Type of item %d not handled on favorites page!", id);
		}
	}

	l->addStretch(1);
}

QSize FavoritesWidget::minimumSizeHint() const
{
	return QSize(237, 335);
}

QSize FavoritesWidget::sizeHint() const
{
	return QSize(237, 335);
}
