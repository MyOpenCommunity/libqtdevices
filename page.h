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
class BannerContent;
class NavigationBar;
class BannerContent;


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


// This typedef is needed by slots status_changed(StatusList). In order to avoid
// duplication the typedef is put here, so all pages can be use freely
typedef QHash<int, QVariant> StatusList;

// Allows type-safe access to __content from page subclasses; by default
// it returns a QWidget; to return a different QWidget subtype, add a
//
// typedef TYPE ContentType;
//
// inside the page subclass (see BannerPage for an example)
#define page_content (content(this))

/**
 * Base class for application pages
 *
 * A Page is a widget that will be shown in the page container. In general, pages are not meant to be
 * full screen widgets; they just occupy the main part of the screen space, like on TouchX. However, they
 * can be full screen on low resolution hardware, like the BTouch. For full screen widgets, look at Window.
 *
 * A page is composed of a navigation bar, a content and optionally a top widget. Use the buildPage() method to create
 * a page with the correct layout for the hardware. BTouch pages are created with content at the top and navigation
 * bar at the bottom, while TouchX pages are created with navigation bar on the left and content on the right.
 * The top widget is a widget that is always shown at the top of the page, ie. it will not scroll up/down like the
 * rest of the content. An example is the plant widget on anti intrusion section.
 *
 * You can access the page content using the page_content macro. Page content can be any widget you like; as such,
 * you need to define its type into the public part of your class.
 *
 * Pages are shown with the showPage() method and must emit the Closed() signal when they are closed. Generally
 * the Closed() signal will be emitted when the user presses the back button on the navigation bar.
 */
class Page : public StyledWidget
{
friend class BtMain;
Q_OBJECT
public:
	/// Default title height
	static const int TITLE_HEIGHT = 70;

	// Indicates subsystem pagetype. For now it is used in touchx top_nav_bar
	// TODO: is this the same as pagSecLiv in main.h?
	enum PageType
	{
		NONE = 0,
		HOMEPAGE,
	};

	/**
	 * Type definition for content.
	 *
	 * If you need a different content type, you need to use a similar typedef in your page.
	 */
	typedef QWidget ContentType;

	// Normally, the page is a fullscreen window, but sometimes is a part of
	// another page (see Antintrusion or SoundDiffusion)
	Page(QWidget *parent=0);
	/// Obsolete: don't use it in new code
	virtual void inizializza();

	// TODO: needed for sound diffusion in AlarmClock. To be removed
	/// Obsolete: don't use it in new code
	virtual void forceDraw() { }

	// Defaults to NONE, reimplement to change page type.
	// TODO: This should really be pure virtual
	virtual PageType pageType();

	/**
	 * Page id of the first page of a section.
	 *
	 * This function identifies a section; on TouchX it's used to highlight the current section on the
	 * section bar at the top of the page.
	 * Internal pages of a section can return NO_SECTION.
	 */
	virtual int sectionId();

	/// Needed by sendFrame() and sendInit(). Will be removed.
	static void setClients(Client *command, Client *request);

	// A global way to send frames/init requests. Do not use these directly, prefer using
	// devices specific methods, unless you have to send frames without reading responses.
	/// Obsolete: don't use it in new code
	/// If you need to send frames, use a device.
	void sendFrame(QString frame) const;
	/// Obsolete: don't use it in new code
	void sendInit(QString frame) const;

	static void setPageContainer(PageContainer *window);
	/**
	 * Set up the layout for the page before starting a page transition
	 */
	virtual void activateLayout();

public slots:
	/// An handle to allow customization of the page showed. Default implementation
	/// show the page in fullscreen mode and call the transition effect if present.
	virtual void showPage();

	// The the page as the current page on the main window
	void setCurrentPage();

protected:
	// used by page_content
	// see the comment about page_content
	template<class P>
	typename P::ContentType* content(P*)
	{
		return (typename P::ContentType*)__content;
	}

	// WARNING: do not use this directly, use page_content #defined above
	QWidget *__content;
	Page *currentPage();
	void prepareTransition();
	void startTransition();

	/**
	 * Create a page with given content, navigation bar, top widget and title widget.
	 *
	 * \param content Page content. Can be any widget.
	 * \param nav_bar Navigation bar
	 * \param top_widget Top widget for the page
	 * \param title_widget Page title. It's invalid to use a title_widget on BTouch
	 */
	void buildPage(QWidget *content, QWidget *nav_bar, QWidget *top_widget=0, QWidget *title_widget=0);

	/**
	 * Convenience function to create a page with given content, navigation bar and title.
	 *
	 * \param content Content of the page
	 * \param nav_bar Navigation bar for the page
	 * \param label Page title (available on TouchX only)
	 * \param label_height Title height
	 * \param top_widget Top widget for the page
	 */
	void buildPage(QWidget *content, QWidget *nav_bar, const QString& label,
		       int label_height=TITLE_HEIGHT, QWidget *top_widget=0);

private:
	static PageContainer *page_container;
	static Client *client_richieste;
	static Client *client_comandi;
	void forceClosed();

signals:
	/// Emitted when the page is closed.
	void Closed();
	void forwardClick();
};


/**
 * \class BannerPage
 *
 * A page containing a list of banners.
 */
class BannerPage : public Page
{
public:
	// the type returned by page_content
	typedef BannerContent ContentType;

	BannerPage(QWidget *parent=0);

	virtual void activateLayout();
	virtual void inizializza();

protected:
	void buildPage(BannerContent *content, NavigationBar *nav_bar, const QString &title = QString(), QWidget *top_widget=0);
	/**
	 * Utility function to build a standard banner page.
	 *
	 * Connections are created between content and navigation bar, and between content and page.
	 */
	void buildPage(const QString &title = QString(), int title_height = TITLE_HEIGHT, QWidget *top_widget=0);

	/**
	 * Create a page with a custom content that contains a BannerContent.
	 *
	 * Some pages need to display some widgets around the content. Use this overload for such cases. Only the
	 * BannerContent will scroll using the navigation bar, the other widgets will not move.
	 * Connections are created between content and navigation bar, and between content and page.
	 */
	void buildPage(QWidget *content, BannerContent *banners, NavigationBar *nav_bar,
		       const QString &title = QString(), int title_height = TITLE_HEIGHT, QWidget *top_widget = 0);
};


// NOTE: this class is temporary, to avoid problems with pages without layout
class PageLayout : public Page
{
Q_OBJECT
public:
	PageLayout(QWidget *parent=0);

	/// Add the back buttons at the bottom of Page, using the layout. The buttons
	/// is connected with signal Closed.
	virtual void addBackButton();

protected:
	/// The vertical layout set in the costructor of Page and preformatted
	/// according to other pages (like sottoMenu pages..). No items are put in
	/// layout, so you can simply ignore it if you don't use layouts.
	QVBoxLayout *main_layout;
};

#endif
