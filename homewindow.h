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


#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H

#include "window.h"
#include "state_button.h" // StateButton::Status

#include <QWidget>
#include <QHash>

class PageContainer;
class HeaderWidget;
class FavoritesWidget;
class Page;
class BtButton;


/*!
	\ingroup Core
	\brief Manages the icons placed in the HeaderLogo, at the top of the window.
*/
class TrayBar : public QWidget
{
Q_OBJECT
public:
	enum ButtonId
	{
		RING_EXCLUSION,
		HANDS_FREE,
		PROF_STUDIO,
		AUDIO_PLAYER,
		TELE_LOOP
	};

	TrayBar();
	void addButton(BtButton *b, ButtonId id);
	BtButton *getButton(ButtonId id);

private:
	QHash<int, BtButton*> buttons;
};


/*!
	\ingroup Core
	\brief Contains and controls the stack of the pages, favourites and header widgets.

	Besides containing the widgets, it also handles the communications between the
	header navigation/info widgets and the page container.

	\see PageContainer
	\see FavoritesWidget
	\see HeaderWidget
*/
class HomeWindow : public Window
{
Q_OBJECT
public:
	HomeWindow();
	PageContainer *centralLayout();

	void loadConfiguration();
	TrayBar *tray_bar;

	virtual void aboutToHideEvent();

signals:
	/*!
		\brief Emitted to show the home page
	 */
	void showHomePage();

	/*!
		\brief Emitted to show the specified section page
	 */
	void showSectionPage(int page_id);

public slots:
	/*!
		\brief Changes header layout for home page or inner pages.
	 */
	void centralWidgetChanged(Page *p);

	/*!
		\brief Higlights the current section in the navigation bar.
	 */
	void currentSectionChanged(int section_id);

	/*!
		\brief Keeps the status of buttons in home page in sync to the state of buttons in the navigation bar.

		Used by the \ref Messages section link.
	 */
	void iconStateChanged(int page_id, StateButton::Status st);

private slots:
	void aboutToChangePage();

private:
	PageContainer *central_widget;
	HeaderWidget *header_widget;
	FavoritesWidget *favorites_widget;
};


#endif // HOMEWINDOW_H
