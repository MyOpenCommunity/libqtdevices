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
class QVBoxLayout;


/**
 * \class Page
 *
 * This class should be the base class for all the fullscreen pages of application.
 * It offer at its children some facilities which can improve productivity.
 */
class Page : public QWidget
{
Q_OBJECT
public:
	// Normally, the page is a fullscreen window, but sometimes is a part of
	// another page (see Antintrusion or SoundDiffusion)
	Page(QWidget *parent=0);
	virtual void inizializza();
	static void setClients(Client *command, Client *request);

	// A global way to send frames/init requests. Do not use these directly, prefer using
	// devices specific methods, unless you have to send frames without reading responses.
	void sendFrame(QString frame);
	void sendInit(QString frame);

	/// Add the back buttons at the bottom of Page, using the layout. The buttons
	/// is connected with signal Closed.
	virtual void addBackButton();

public slots:
	/// An handle to allow customization of the page showed. Default implementation
	/// only show the page in fullscreen mode.
	virtual void showPage();

protected:
	/// The vertical layout set in the costructor of Page and preformatted
	/// according to other pages (like sottoMenu pages..). No items are put in
	/// layout, so you can simply ignore it if you don't use layouts.
	QVBoxLayout *main_layout;

private:
	static Client *client_richieste;
	static Client *client_comandi;

signals:
	/// Emitted when the page is closed.
	void Closed();
};

#endif
