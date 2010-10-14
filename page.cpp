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
Client *Page::client_comandi = 0;
Client *Page::client_richieste = 0;
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


/*!
	\class Page
	\brief Base class for application pages

	A Page is a widget that will be shown in the PageContainer. In general, pages
	are not meant to be full screen widgets; they just occupy the main part of
	the screen space, like on the touchcreen 10. However, they can be full screen
	on low resolution hardware, like the touchscreen 3.5. For full screen widgets,
	look at Window.

	A page is composed of:
	\li a navigation bar, used to navigate the content of the page, close it and
		sometimes to execute special functions.
	\li a main widget, a QWidget displayed in the central area of the page.
		Often is the same object than the content.
	\li a content, a QWidget container for \c items, that can be banners, icons
		or whatever you want which contains also the logic to paginate the items.
	\li a top widget (optional), an header of the page that is always shown at
		the top of the page, ie. it will not scroll up/down like the rest of the
		content.

	All the logic about the \c life of the page should be placed in the page itself
	and \b not in the content.

	Use the buildPage() methods to create a page with the correct layout for the
	hardware.

	You can access the page content using the page_content macro. Page content
	can be any widget you like; as such, you need to define its type into the
	public part of your class. In this way, you can handle the pointer without
	doing any cast.

	Pages are shown with the showPage() method and must emit the Closed() signal
	when they are closed. Generally the Closed() signal will be emitted when the
	user presses the back button on the navigation bar.
 */

/*!
	\typedef QWidget Page::ContentType
	\brief Defines the type of the content.

	Allows type-safe access to the content from page subclasses using the
	page_content macro; to return a different QWidget subtype add a:

	\code
	typedef TYPE ContentType;
	\endcode

	inside the page subclass (see BannerPage for an example).
*/

/*!
	\fn Page::Closed()
	\brief Emitted when the page is closed
*/

/*!
	\fn Page::forwardClick()
	\brief Emitted when the forward button of the navigation bar is clicked.
*/

/*!
	\var Page::__content
	\warning Do not use this member, use the type safe macro page_content instead.
	\sa Page::ContentType
*/

/*!
	\fn Page::content(P *)
	\warning Do not use this method, use the type safe macro page_content instead.
	\sa Page::ContentType
*/

/*!
	\var Page::TITLE_HEIGHT
	\brief The normal title height used in standard pages.
*/

/*!
	\var Page::SMALL_TITLE_HEIGHT
	\brief The small title height used in standard pages.
*/

/*!
	\var Page::TINY_TITLE_HEIGHT
	\brief The tiny title height used in standard pages.
*/

/*!
	\brief Constuctor

	Build the page and add it to the PageContainer if the parent widget is not
	specified.
*/
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

/*!
	\brief Build the correct page layout for the hardware type.

	Pages for the touchscreen 3.5 are created with the main widget at the top
	and the navigation bar at the bottom, while pages for the touchscreen 10''
	are created with navigation bar on the left a main widget on the right.
	In the latter case, an optional top_widget can be used to have an header
	that is always shown at the top of the page and a title_widget to have
	a page title.
*/
void Page::buildPage(QWidget *main_widget, QWidget *content, AbstractNavigationBar *nav_bar, QWidget *top_widget, QWidget *title_widget)
{
	QBoxLayout *l;
#ifdef LAYOUT_TS_10
	l = new QHBoxLayout(this);
#else
	l = new QVBoxLayout(this);
#endif

	// the top_widget (if present) is a widget that must be at the top of the page,
	// limiting the height (so even the navigation) of the main_widget; for TS 10''
	// it must be between the title and the central_widget

#ifdef LAYOUT_TS_10
	// TODO add an API to set the page title and to set the page count
	//      and current page number
	if (nav_bar)
		l->addWidget(nav_bar, 0);

	QVBoxLayout *pl = new QVBoxLayout;
	pl->setContentsMargins(0, 0, 0, 0);
	pl->setSpacing(0);
	if (title_widget)
		pl->addWidget(title_widget);
	if (top_widget)
		pl->addWidget(top_widget);
	pl->addWidget(main_widget, 1);

	l->addLayout(pl, 1);

#else
	if (top_widget)
		l->addWidget(top_widget);

	Q_ASSERT_X(title_widget == NULL, "Page::buildPage", "TS 3.5'' pages can't have a title");
	l->addWidget(main_widget, 1);
	l->addWidget(nav_bar);
#endif
	l->setContentsMargins(0, 5, 0, 10);
	l->setSpacing(0);

	__content = content;
}

/*!
	\brief Build the correct page layout for the hardware type.

	This is an overloaded member function, provided for convenience, which uses
	the content also as the main widget.
*/
void Page::buildPage(QWidget *content, AbstractNavigationBar *nav_bar, QWidget *top_widget, QWidget *title_widget)
{
	buildPage(content, content, nav_bar, top_widget, title_widget);
}

/*!
	\brief Build the correct page layout for the hardware type.

	This is an overloaded member function, provided for convenience, which uses
	the content also as the main widget and for the touchscreen 10 define a page
	title with label as  \a text and \a label_height as height.
*/
void Page::buildPage(QWidget *content, AbstractNavigationBar *nav_bar, const QString& label, int label_height, QWidget *top_widget)
{
	QLabel *page_title = 0;

#ifdef LAYOUT_TS_10
	page_title = new QLabel(label);
	page_title->setFont(bt_global::font->get(FontManager::TITLE));
	page_title->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
	// the 10 here must match the margins in the PageTitleWidget above
	page_title->setFixedHeight(label_height + 10);
#endif

	buildPage(content, content, nav_bar, top_widget, page_title);
}

/*!
	\brief Insert a widget at the bottom of the page.

	\deprecated Don't use it, build the page using a main widget that contains a content
	and the desired bottom widget.
*/
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

/*!
	\brief Set up the layout for the page before starting a page transition
*/
void Page::activateLayout()
{
	QLayout *main_layout = layout();
	if (main_layout)
	{
		main_layout->activate();
		main_layout->update();
	}
}

/*!
	\brief Reimplement this to send frames for the page during the initialization.

	\deprecated Don't use it in new code.

	\sa sendFrame()
*/
void Page::inizializza()
{
}

/*!
	\brief Set the page container that contains all the pages.
*/
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

/*!
	\brief Reimplement this to perform some operations that should be done when 
	we exit from the page.

	\internal We cannot use the hideEvent method because that event trigger even
	if we go in a next page.
*/
void Page::cleanUp()
{
}

/*!
	\brief Make the page the current page showed in the PageContainer.
*/
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

/*!
	\brief send frames using the Client::COMMAND channel.

	\deprecated Don't use it in new code. If you need to send frames, use a device.
*/
void Page::sendFrame(QString frame) const
{
	Q_ASSERT_X(client_comandi, "Page::sendFrame", "Client comandi not set!");
	client_comandi->sendFrameOpen(frame);
}

/*!
	\brief send frames using the Client::REQUEST channel.

	\deprecated Don't use it in new code. If you need to send frames, use a device.
*/
void Page::sendInit(QString frame) const
{
	Q_ASSERT_X(client_richieste, "Page::sendInit", "Client richieste not set!");
	client_richieste->sendFrameOpen(frame);
}

/*!
	\brief Se the clients used to send frames for all pages.

	\deprecated This method is required by sendFrame() and sendInit(), will be removed.
*/
void Page::setClients(Client *command, Client *request)
{
	client_comandi = command;
	client_richieste = request;
}

/*!
	\brief Close the page.
*/
void Page::forceClosed()
{
	emit Closed();
}

void Page::prepareTransition()
{
	page_container->prepareTransition();
}

void Page::startTransition()
{
	page_container->startTransition(this);
}

/*!
	\brief An hook called right before the page is hidden by showPage() or by showWindow().
*/
void Page::aboutToHideEvent()
{
}

/*!
	\brief Return the section id of the page.

	This function identifies a section; on TS 10'' it's used to highlight the
	current section on thesection bar at the top of the page.
	Internal pages of a section can return NO_SECTION.
*/
int Page::sectionId() const
{
	return NO_SECTION;
}

