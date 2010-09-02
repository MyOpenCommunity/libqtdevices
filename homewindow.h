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

#include <QWidget>
#include <QHash>

class PageContainer;
class HeaderWidget;
class FavoritesWidget;
class Page;
class BtButton;


/**
 * This class manages the buttons placed on the top of the window, near the
 * logo. Buttons of these types have a global role, no matter what is the
 * current page/window.
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
	};

	TrayBar();
	void addButton(BtButton *b, ButtonId id);
	BtButton *getButton(ButtonId id);

private:
	QHash<int, BtButton*> buttons;
};


// controls the layout of "normal" pages; contains the stack of pages and the
// header and favorites widgets
class HomeWindow : public Window
{
Q_OBJECT
public:
	HomeWindow();
	PageContainer *centralLayout();
	Page *currentPage();

	void loadConfiguration();
	TrayBar *tray_bar;

	virtual void aboutToHideEvent();

signals:
	void showHomePage();
	void showSectionPage(int page_id);

public slots:
	void centralWidgetChanged(int index);
	void currentSectionChanged(int section_id);

private slots:
	void aboutToChangePage();

private:
	PageContainer *central_widget;
	HeaderWidget *header_widget;
	FavoritesWidget *favorites_widget;
};


#endif // HOMEWINDOW_H
