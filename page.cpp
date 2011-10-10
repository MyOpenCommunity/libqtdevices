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


#include "page.h"
#include "openclient.h" // Client
#include "btbutton.h"
#include "transitionwidget.h"
#include "pagecontainer.h"
#include "navigation_bar.h"
#include "fontmanager.h"
#include "main.h" // NO_SECTION

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMetaObject> // className
#include <QTime>
#include <QDir>
#include <QApplication>
#include <QLabel>


// The defines to grab screenshot of pages
#define GRAB_PAGES 0
#define SCREENSHOT_DIR "/home/bticino/cfg/extra/0"


// Inizialization of static member
ClientWriter *Page::client_command = 0;
ClientWriter *Page::client_request = 0;
PageContainer *Page::page_container = 0;


PageTitleWidget::PageTitleWidget(const QString &label, int height)
{
	QLabel *title = new QLabel(label);
	title->setFont(bt_global::font->get(FontManager::TITLE));
	title->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
	title->setFixedHeight(height);

	QLabel *current_page = new QLabel;
	current_page->setFont(bt_global::font->get(FontManager::TEXT));
	current_page->setAlignment(Qt::AlignTop|Qt::AlignRight);

	QHBoxLayout *t = new QHBoxLayout(this);
	t->setContentsMargins(10, 0, 10, 10);
	t->setSpacing(0);
	t->addStretch(1);
	t->addWidget(title);
	t->addWidget(current_page, 1);
}

void PageTitleWidget::setTitle(QString title)
{
	QLabel *label = static_cast<QLabel*>(layout()->itemAt(1)->widget());

	label->setText(title);
}

void PageTitleWidget::setCurrentPage(int current, int total)
{
	QLabel *label = static_cast<QLabel*>(layout()->itemAt(2)->widget());

	if (total <= 1)
		label->setText(QString());
	else
		label->setText(QString("%1/%2").arg(current + 1).arg(total));
}


Page::Page(QWidget *parent) : StyledWidget(parent)
{
	Q_ASSERT_X(page_container, "Page::Page", "PageContainer not set!");
	__content = 0;

	// pages with parent have a special meaning (for example, sound diffusion)
	// so they must not handled here
	if (!parent)
		page_container->addPage(this);
}

Page::~Page()
{
	// This should not be necessary. However with Qt 4.5.0 when delete the
	// AlarmPage there was a 'race condition' or something like that which causes
	// a segfault fixed removing immediately the AlarmPage from the stacked widget.
	if (page_container->indexOf(this) != -1)
		page_container->removeWidget(this);
}

void Page::buildPage(QWidget *main_widget, QWidget *content, AbstractNavigationBar *nav_bar, QWidget *title_widget)
{
	QBoxLayout *l;

#ifdef LAYOUT_TS_10
	l = new QHBoxLayout(this);

	if (nav_bar)
		l->addWidget(nav_bar, 0);

	QVBoxLayout *pl = new QVBoxLayout;
	pl->setContentsMargins(0, 0, 0, 0);
	pl->setSpacing(0);
	if (title_widget)
		pl->addWidget(title_widget);
	pl->addWidget(main_widget, 1);

	l->addLayout(pl, 1);

	int left_margin = nav_bar ? 13 : 0;
	l->setContentsMargins(left_margin, 5, 0, 10);
	l->setSpacing(0);
#else
	l = new QVBoxLayout(this);

	Q_ASSERT_X(title_widget == NULL, "Page::buildPage", "TS 3.5'' pages can't have a title");
	l->addWidget(main_widget, 1);
	l->addStretch();

	if (nav_bar)
		l->addWidget(nav_bar);

	l->setContentsMargins(5, 5, 5, 5);
	l->setSpacing(0);
#endif

	__content = content;
}

void Page::buildPage(QWidget *content, AbstractNavigationBar *nav_bar, QWidget *title_widget)
{
	buildPage(content, content, nav_bar, title_widget);
}

void Page::buildPage(QWidget *content, AbstractNavigationBar *nav_bar, const QString& label, int label_height)
{
	QLabel *page_title = 0;

#ifdef LAYOUT_TS_10
	page_title = new QLabel(label);
	page_title->setFont(bt_global::font->get(FontManager::TITLE));
	page_title->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
	// the 10 here must match the margins in the PageTitleWidget above
	page_title->setFixedHeight(label_height + 10);
#endif

	buildPage(content, content, nav_bar, page_title);
}

void Page::addBottomWidget(QWidget *bottom)
{
#ifdef LAYOUT_TS_10
	QLayout *main = layout();
	QVBoxLayout *l = qobject_cast<QVBoxLayout *>(static_cast<QLayout *>(main->itemAt(1)));
	Q_ASSERT_X(l, "Page::addBottomWidget", "TS 10'' layout()->itemAt(1) is not a QVBoxLayout, fix buildPage()!");
	l->addWidget(bottom, 0, Qt::AlignCenter);
#else
	// take into account nav_bar
	// TODO: test it.
	//l->insertWidget(l->count() - 2, bottom);
	qFatal("Page::addBottomWidget is not implemented for TS 3.5''");
#endif
}

void Page::activateLayout()
{
	QLayout *main_layout = layout();
	if (main_layout)
	{
		main_layout->activate();
		main_layout->update();
	}
}

void Page::setPageContainer(PageContainer *container)
{
	page_container = container;
}

Page *Page::currentPage()
{
	return page_container->currentPage();
}

void Page::setCurrentPage()
{
	page_container->setCurrentPage(this);
}

void Page::showPage()
{
	page_container->showPage(this);

#if GRAB_PAGES
	Page *p = currentPage();
	QPixmap screenshot = QPixmap::grabWidget(p);

	QDir grab_dir(SCREENSHOT_DIR);
	if (!grab_dir.exists())
		grab_dir.mkpath(SCREENSHOT_DIR);

	QString filename(QString(p->metaObject()->className()) + "_" + QTime::currentTime().toString("hh.mm.ss.zzz") + ".png");
	screenshot.save(QString(SCREENSHOT_DIR) + "/" + filename);
#endif
}

void Page::sendFrame(QString frame) const
{
	Q_ASSERT_X(client_command, "Page::sendFrame", "Client command not set!");
	client_command->sendFrameOpen(frame);
}

void Page::sendInit(QString frame) const
{
	Q_ASSERT_X(client_request, "Page::sendInit", "Client request not set!");
	client_request->sendFrameOpen(frame);
}

void Page::setClients(ClientWriter *command, ClientWriter *request)
{
	client_command = command;
	client_request = request;
}


void Page::forceClosed()
{
	emit Closed();
}

void Page::prepareTransition(QWidget *w)
{
	page_container->prepareTransition(w);
}

void Page::startTransition(QWidget *w)
{
	page_container->startTransition(w);
}

int Page::sectionId() const
{
	return NO_SECTION;
}

