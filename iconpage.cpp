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


#include "iconpage.h"
#include "state_button.h"
#include "navigation_bar.h"
#include "skinmanager.h"
#include "fontmanager.h" // bt_global::font
#include "btmain.h" // bt_global::btmain
#include "labels.h" // TextOnImageLabel
#include "generic_functions.h" // setCfgValue

#include <QDomNode>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#define CONTENT_MARGIN  25


void IconPage::buildPage(IconContent *content, NavigationBar *nav_bar, const QString &title)
{
	PageTitleWidget *title_widget = 0;
	if (!title.isNull())
	{
		title_widget = new PageTitleWidget(title, SMALL_TITLE_HEIGHT);
	}
	ScrollablePage::buildPage(content, content, nav_bar, 0, title_widget);
}

BtButton *IconPage::addButton(const QString &label, const QString& icon_path, int x, int y)
{
	BtButton *b = new BtButton(icon_path, this);
#ifdef LAYOUT_TS_3_5
	b->setGeometry(x, y, DIM_BUT, DIM_BUT);
#else
	Q_ASSERT_X(page_content, "IconPage::addButton", "The IconPage must have the page_content!");
	page_content->addButton(b, label);
#endif

	return b;
}

void IconPage::addPage(Page *page, const QString &label, const QString &iconName, int x, int y)
{
	BtButton *b = addButton(label, iconName, x, y);
	connect(b, SIGNAL(clicked()), page, SLOT(showPage()));
	connect(page, SIGNAL(Closed()), this, SLOT(showPage()));
}


IconContent::IconContent(QWidget *parent) : ScrollableContent(parent)
{
	QGridLayout *l = static_cast<QGridLayout *>(layout());
	l->setContentsMargins(CONTENT_MARGIN, 0, CONTENT_MARGIN, 0);
	l->setSpacing(0);
	l->setColumnStretch(4, 1);
}

void IconContent::addButton(QWidget *button, const QString &label)
{
	// always create the container widget; this way the container is set to the
	// correct fixed size during layout and the button keeps its (smaller) fixed size
	QWidget *w = new QWidget;
	QVBoxLayout *l = new QVBoxLayout(w);
	l->setContentsMargins(0, 0, 0, 0);
	l->addWidget(button, 0, Qt::AlignHCenter);
	l->setSpacing(5);

	if (!label.isEmpty())
	{
		ScrollingLabel *lbl = new ScrollingLabel(label);

		lbl->setAlignment(Qt::AlignHCenter);
		lbl->setFont(bt_global::font->get(FontManager::BANNERDESCRIPTION));
		l->addWidget(lbl);
	}

	addWidget(w);
}

void IconContent::addWidget(QWidget *widget)
{
	need_update = true;
	items.append(widget);
	widget->hide();
}

void IconContent::drawContent()
{
	QGridLayout *l = qobject_cast<QGridLayout*>(layout());

	if (pages.size() == 0)
	{
		// compute the page list
		prepareLayout(items, 4);

		// add icons to the layout
		for (int i = 0; i < pages.size() - 1; ++i)
		{
			int base = pages[i];
			for (int j = 0; base + j < pages[i + 1]; ++j)
			{
				items.at(base + j)->setFixedWidth((width() - 2 * CONTENT_MARGIN) / 4);
				l->addWidget(items.at(base + j), j / 4, j % 4);
			}
		}

		l->setRowStretch(l->rowCount(), 1);
	}

	updateLayout(items);
}


IconPageButton::IconPageButton(const QString &label)
{
	ScrollingLabel *lbl = new ScrollingLabel(label);
	lbl->setAlignment(Qt::AlignHCenter);
	lbl->setFont(bt_global::font->get(FontManager::BANNERDESCRIPTION));

	button = new StateButton;

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(5);
	l->addWidget(button, 0, Qt::AlignHCenter);
	l->addWidget(lbl);
}

