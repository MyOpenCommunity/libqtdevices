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


#ifndef PAGE_H
#define PAGE_H

#include <QWidget>

#include "styledwidget.h"

class Client;
class PageContainer;
class QVBoxLayout;
class TransitionWidget;
class AbstractNavigationBar;


// helper widget containing a centered label and a label on the right
// displaying the current page
class PageTitleWidget : public QWidget
{
Q_OBJECT
public:
	PageTitleWidget(const QString &label, int height);

public slots:
	void setCurrentPage(int current, int total);
	void setTitle(QString title);
};


// This typedef is needed by slots valueReceived(DeviceValues). In order to avoid
// duplication the typedef is put here, so all pages can be use freely
typedef QHash<int, QVariant> DeviceValues;


#define page_content (content(this))

/*!
	\ingroup Core
	\brief Base class for application pages.

	A Page is a widget that will be shown in the PageContainer. In general, pages
	are not meant to be full screen widgets; they just occupy the main part of
	the screen space, like on the touchcreen 10. However, they can be full screen
	on low resolution hardware, like the touchscreen 3.5. For full screen widgets,
	look at Window.

	A page is composed of:
	\li an AbstractNavigationBar, usually used to navigate the content of the page,
		close it and sometimes to execute special functions.
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
	user presses the back button on the NavigationBar.
*/
class Page : public StyledWidget
{
friend class BtMain;
Q_OBJECT
public:
	/*!
		\brief The normal title height used in standard pages.
	*/
	static const int TITLE_HEIGHT = 70;

	/*!
		\brief The small title height used in standard pages.
	*/
	static const int SMALL_TITLE_HEIGHT = 35;

	/*!
		\brief The tiny title height used in standard pages.
	*/
	static const int TINY_TITLE_HEIGHT = 20;

	/*!
		\brief Defines the type of the content.

		Allows type-safe access to the content from page subclasses using the
		page_content macro; to return a different QWidget subtype add a:

		\code
		typedef TYPE ContentType;
		\endcode

		inside the page subclass (see BannerPage for an example).
	*/
	typedef QWidget ContentType;

	/*!
		\brief Constuctor

		Build the page and add it to the PageContainer if the parent widget is not
		specified.
	*/
	Page(QWidget *parent = 0);
	~Page();

	/*!
		\brief Send frames for the page during the initialization.

		\warning Don't use it in new code.

		\sa sendFrame()
	*/
	virtual void inizializza() {}

	/*!
		\brief Return the section id of the page.

		This function identifies a section; on TS 10'' it's used to highlight the
		current section on thesection bar at the top of the page.
		Internal pages of a section can return NO_SECTION.
	*/
	virtual int sectionId() const;

	/*!
		\brief Se the clients used to send frames for all pages.

		\warning This method is required by sendFrame() and sendInit(), will be removed.
		\sa Client
	*/
	static void setClients(Client *command, Client *request);

	/*!
		\brief send a \a frame using the Client::COMMAND channel.

		\warning Don't use it in new code. If you need to send frames, use a device.
	*/
	void sendFrame(QString frame) const;

	/*!
		\brief send \a frame using the Client::REQUEST channel.

		\warning Don't use it in new code. If you need to send frames, use a device.
	*/
	void sendInit(QString frame) const;

	/*!
		\brief Set the PageContainer that contains all the pages.
	*/
	static void setPageContainer(PageContainer *container);

	/*!
		\brief Set up the layout for the page before starting a page transition
	*/
	virtual void activateLayout();

	/*!
		\brief Insert a widget at the bottom of the page.

		\warning Don't use it, build the page using a main widget that contains a content
		and the desired bottom widget.
	*/
	void addBottomWidget(QWidget *bottom);

	/*!
		\brief Closes the page.
	*/
	void forceClosed();

	/*!
		\brief An hook called right before the page is hidden by showPage() or by showWindow().
	*/
	virtual void aboutToHideEvent() {}

public slots:
	/*!
		\brief Make the page the current page showed in the PageContainer.

		Note that also the Window that contains the Page is shown too.
	*/
	virtual void showPage();

	/*!
		\brief Perform some operations that should be done when the user exits from
		the page.

		In the default implementation nothing is done.
		\internal We cannot use the hideEvent method because that event trigger even
		if we go in a next page.
	*/
	virtual void cleanUp() {}

signals:
	/*!
		\brief Emitted when the page is closed
	*/
	void Closed();

	/*!
		\brief Emitted when the forward button of the NavigationBar is clicked.
	*/
	void forwardClick();

protected:
	/*!
		\warning Do not use this method, use the type safe macro page_content instead.
		\sa Page::ContentType
	*/
	template<class P> typename P::ContentType* content(P*)
	{
		return (typename P::ContentType*)__content;
	}

	/*!
		\warning Do not use this member, use the type safe macro page_content instead.
		\sa Page::ContentType
	*/
	QWidget *__content;

	void prepareTransition();
	void startTransition();

	/*!
		\brief Build the correct page layout for the hardware type.

		Pages for the touchscreen 3.5 are created with the \a main_widget at the
		top and the \a nav_bar at the bottom, while pages for the touchscreen 10''
		are created with the \a nav_bar on the left and the \a main_widget on the
		right. In the latter case, an optional \a top_widget can be used to have
		an header always shown at the top of the page and a \a title_widget to
		have a page title.
	*/
	void buildPage(QWidget *main_widget, QWidget *content, AbstractNavigationBar *nav_bar,
		QWidget *top_widget = 0, QWidget *title_widget = 0);

	/*!
		\brief Build the correct page layout for the hardware type.

		This is an overloaded member function, provided for convenience, which uses
		the \a content also as the \a main_widget.
	*/
	void buildPage(QWidget *content, AbstractNavigationBar *nav_bar, QWidget *top_widget = 0,
		QWidget *title_widget = 0);

	/*!
		\brief Build the correct page layout for the hardware type.

		This is an overloaded member function, provided for convenience, which uses
		the \a content also as the \a main_widget and for the touchscreen 10 defines
		a page title with label as \a text and \a label_height as height.
	*/
	void buildPage(QWidget *content, AbstractNavigationBar *nav_bar, const QString& label,
		int label_height = TITLE_HEIGHT, QWidget *top_widget = 0);

private:
	static PageContainer *page_container;
	static Client *client_richieste;
	static Client *client_comandi;

	void setCurrentPage();
	Page *currentPage();
};

#endif
