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
#include "homewindow.h" // TrayBar

#include <QDomNode>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#define DIM_BUT 80
#define BACK_BUTTON_X    0
#define BACK_BUTTON_Y  250
#define BACK_BUTTON_DIM 60
#define CONTENT_MARGIN  25


void IconPage::buildPage(IconContent *content, NavigationBar *nav_bar, const QString &title)
{
	PageTitleWidget *title_widget = 0;
	if (!title.isNull())
	{
		title_widget = new PageTitleWidget(title, SMALL_TITLE_HEIGHT);
		connect(content, SIGNAL(contentScrolled(int, int)), title_widget, SLOT(setCurrentPage(int, int)));
	}
	Page::buildPage(content, nav_bar, 0, title_widget);

	// TODO duplicated in BannerPage

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(this, SIGNAL(Closed()), content, SLOT(resetIndex()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(forwardClick()));
	connect(nav_bar, SIGNAL(upClick()), content, SLOT(pgUp()));
	connect(nav_bar, SIGNAL(downClick()), content, SLOT(pgDown()));
	connect(content, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));
}

void IconPage::activateLayout()
{
	if (page_content)
		page_content->updateGeometry();

	Page::activateLayout();

	if (page_content)
		page_content->drawContent();
}

BtButton *IconPage::addButton(const QString &label, const QString& icon_path, int x, int y)
{
	BtButton *b = new BtButton(this);
	if (page_content == NULL)
		b->setGeometry(x, y, DIM_BUT, DIM_BUT);
	else
		page_content->addButton(b, label);
	b->setImage(icon_path);

	return b;
}

void IconPage::addPage(Page *page, const QString &label, const QString &iconName, int x, int y)
{
	BtButton *b = addButton(label, iconName, x, y);
	connect(b, SIGNAL(clicked()), page, SLOT(showPage()));
	connect(page, SIGNAL(Closed()), this, SLOT(showPage()));
}

void IconPage::addBackButton()
{
	BtButton *b = new BtButton(this);
	b->setGeometry(BACK_BUTTON_X, BACK_BUTTON_Y, BACK_BUTTON_DIM, BACK_BUTTON_DIM);
	b->setImage(bt_global::skin->getImage("back"));
	connect(b, SIGNAL(clicked()), SIGNAL(Closed()));
}


IconContent::IconContent(QWidget *parent) : GridContent(parent)
{
	QGridLayout *l = static_cast<QGridLayout *>(layout());
	l->setContentsMargins(CONTENT_MARGIN, 0, CONTENT_MARGIN, 0);
	l->setSpacing(0);
	l->setColumnStretch(4, 1);
}

void IconContent::addButton(QWidget *button, const QString &label)
{
	QWidget *w = button;

	if (!label.isEmpty())
	{
		w = new QWidget;
		QVBoxLayout *l = new QVBoxLayout(w);
		QLabel *lbl = new QLabel(label);

		lbl->setAlignment(Qt::AlignHCenter);
		lbl->setFont(bt_global::font->get(FontManager::BANNERDESCRIPTION));
		l->addWidget(button, 0, Qt::AlignHCenter);
		l->addWidget(lbl);
	}

	addWidget(w);
}

void IconContent::addWidget(QWidget *widget)
{
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
	QLabel *lbl = new QLabel(label);
	lbl->setText(label);
	lbl->setAlignment(Qt::AlignHCenter);
	lbl->setFont(bt_global::font->get(FontManager::BANNERDESCRIPTION));

	button = new StateButton;

	QVBoxLayout *l = new QVBoxLayout(this);
	l->addWidget(button, 0, Qt::AlignHCenter);
	l->addWidget(lbl);
}


IconButtonOnTray::IconButtonOnTray(const QString &label, const QString &icon_on, const QString &icon_off,
	const QString &tray_icon) : IconPageButton(label)
{
	button->setOffImage(bt_global::skin->getImage(icon_off));
	button->setOnImage(bt_global::skin->getImage(icon_on));
	connect(button, SIGNAL(clicked()), SLOT(toggleActivation()));
	tray_button = new BtButton(bt_global::skin->getImage(tray_icon));
	connect(tray_button, SIGNAL(clicked()), SLOT(turnOff()));
	bt_global::btmain->trayBar()->addButton(tray_button);
	updateStatus();
}

void IconButtonOnTray::toggleActivation()
{
	button->setStatus(!button->getStatus());
	updateStatus();
}

void IconButtonOnTray::updateStatus()
{
	tray_button->setVisible(button->getStatus() == StateButton::ON);
}

void IconButtonOnTray::turnOff()
{
	button->setStatus(false);
	updateStatus();
}
