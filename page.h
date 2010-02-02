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

class Client;
class QStackedWidget;
class QVBoxLayout;
class TransitionWidget;
class BannerContentBase;
class NavigationBar;


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
class Page : public QWidget
{
friend class BtMain;
Q_OBJECT
public:
	// Normally, the page is a fullscreen window, but sometimes is a part of
	// another page (see Antintrusion or SoundDiffusion)
	Page(QWidget *parent=0);
	virtual void inizializza();
	// TODO: needed for sound diffusion in AlarmClock. To be removed
	virtual void forceDraw() { }

	static void setClients(Client *command, Client *request);

	// A global way to send frames/init requests. Do not use these directly, prefer using
	// devices specific methods, unless you have to send frames without reading responses.
	void sendFrame(QString frame) const;
	void sendInit(QString frame) const;

	static void setMainWindow(QStackedWidget *window);
	static void installTransitionWidget(TransitionWidget *tr);

	// block the current transition if present and the future transitions until the "unlock"
	static void blockTransitions(bool);

public slots:
	/// An handle to allow customization of the page showed. Default implementation
	/// only show the page in fullscreen mode.
	virtual void showPage();

protected:
	// Init the transition widget with the current page
	void initTransition();
	// Let's start the transition
	void startTransition();

	static Page *currentPage();

private:
	static Client *client_richieste;
	static Client *client_comandi;
	static QStackedWidget *main_window;
	static TransitionWidget *transition_widget;
	static bool block_transitions;
	void forceClosed();

signals:
	/// Emitted when the page is closed.
	void Closed();
};


/**
 * \class BannerPage
 *
 * A page containing a list of banners.
 */
class BannerPage : public Page
{
Q_OBJECT
public:
	// the type returned by page_content
	typedef BannerContentBase ContentType;

	BannerPage(QWidget *parent=0);

	virtual void activateLayout();

signals:
	void forwardClick();

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

	void buildPage(BannerContentBase *content, NavigationBar *nav_bar, QWidget *top_widget=0);
	void buildPage(QWidget *top_widget=0);
	void buildPage(QString title) { buildPage(); } // for compatibility
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
