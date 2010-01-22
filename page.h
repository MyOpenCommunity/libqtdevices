/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class represent a Page.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date December 2008
 */

#ifndef PAGE_H
#define PAGE_H

#include <QWidget>

#include "styledwidget.h"

class Client;
class PageContainer;
class QVBoxLayout;
class TransitionWidget;
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
 * \class Page
 *
 * This class should be the base class for all the fullscreen pages of application.
 * It offer at its children some facilities which can improve productivity.
 */
class Page : public StyledWidget
{
friend class BtMain;
Q_OBJECT
public:
	static const int TITLE_HEIGHT = 70;

	// Indicates subsystem pagetype. For now it is used in touchx top_nav_bar
	// TODO: is this the same as pagSecLiv in main.h?
	enum PageType
	{
		NONE = 0,
		HOMEPAGE,
	};

	// the type returned by page_content
	// see the comment about page_content above
	typedef QWidget ContentType;

	// Normally, the page is a fullscreen window, but sometimes is a part of
	// another page (see Antintrusion or SoundDiffusion)
	Page(QWidget *parent=0);
	virtual void inizializza();
	// TODO: needed for sound diffusion in AlarmClock. To be removed
	virtual void forceDraw() { }
	// Defaults to NONE, reimplement to change page type.
	// TODO: This should really be pure virtual
	virtual PageType pageType();
	// page id of the first page of a section; internal pages of a section
	// can return NO_SECTION
	virtual int sectionId();

	static void setClients(Client *command, Client *request);

	// A global way to send frames/init requests. Do not use these directly, prefer using
	// devices specific methods, unless you have to send frames without reading responses.
	void sendFrame(QString frame) const;
	void sendInit(QString frame) const;

	static void setPageContainer(PageContainer *window);
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
	void buildPage(QWidget *content, QWidget *nav_bar, QWidget *top_widget=0, QWidget *title_widget=0);
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
	void buildPage(const QString &title = QString(), int title_height = TITLE_HEIGHT, QWidget *top_widget=0);

	// allows creating a BannerPage where the content is a generic QWidget containing a BannerContent
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
