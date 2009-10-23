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
class MainWindow;
class QVBoxLayout;
class TransitionWidget;
class ContentWidget;
class NavigationBar;


// This typedef is needed by slots status_changed(StatusList). In order to avoid
// duplication the typedef is put here, so all pages can be use freely
typedef QHash<int, QVariant> StatusList;


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

	static void setMainWindow(MainWindow *window);
	void activateLayout();

public slots:
	/// An handle to allow customization of the page showed. Default implementation
	/// show the page in fullscreen mode and call the transition effect if present.
	virtual void showPage();

	// The the page as the current page on the main window
	void setCurrentPage();

protected:
	ContentWidget *content_widget;
	Page *currentPage();
	void startTransition(const QPixmap &prev_image);
	void buildPage(ContentWidget *content, NavigationBar *nav_bar, QWidget *top_widget=0);
	void buildPage(QWidget *top_widget=0);

private:
	static MainWindow *main_window;
	static Client *client_richieste;
	static Client *client_comandi;
	void forceClosed();

signals:
	/// Emitted when the page is closed.
	void Closed();
	void forwardClick();
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
