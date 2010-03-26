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


#ifndef HEADERWIDGET_H
#define HEADERWIDGET_H

#include "page.h" // StyledWidget
#include "device.h" // StatusList
#include "main.h" // TemperatureScale

#include <QLabel>

class QDomNode;
class QHBoxLayout;
class QSignalMapper;
class HeaderNavigationBar;
class BtButton;
class TrayBar;
class FeedParser;
class FeedItemList;
class AudioPlayerPage;


// helper widgets, to display the temperature

class TemperatureDisplay : public QLabel
{
Q_OBJECT
public:
	TemperatureDisplay(device *probe);

private slots:
	void status_changed(const StatusList &sl);

protected:
	QString label;
	TemperatureScale temp_scale;
};

class HomepageTemperatureDisplay : public TemperatureDisplay
{
Q_OBJECT
public:
	HomepageTemperatureDisplay(device *probe);

protected:
	void paintEvent(QPaintEvent *e);
};

class InnerPageTemperatureDisplay : public TemperatureDisplay
{
Q_OBJECT
public:
	InnerPageTemperatureDisplay(device *probe);

protected:
	void paintEvent(QPaintEvent *e);
};


// Homepage link

class HomepageLink : public QWidget
{
Q_OBJECT
public:
	HomepageLink(const QString &description, const QString &icon);

signals:
	void pageClosed();
	void clicked();
};


// link to RSS feed

class HomepageFeedLink : public HomepageLink
{
Q_OBJECT
public:
	HomepageFeedLink(const QString &description, const QString &feed);

protected:
	void hideEvent(QHideEvent *e);

private slots:
	void displayFeed();
	void feedReady();

private:
	FeedParser *parser;
	FeedItemList *feed_items;
	QString url;
};


// link to IP radio

class HomepageIPRadioLink : public HomepageLink
{
Q_OBJECT
public:
	HomepageIPRadioLink(const QString &description, const QString &url);

private slots:
	void playRadio();

private:
	AudioPlayerPage *player;
	QString url;
};


// topmost header, contains the BTicino logo and (on internal pages) the
// clock and temperature display
class HeaderLogo : public StyledWidget
{
Q_OBJECT
public:
	HeaderLogo(TrayBar *tray);

	void loadItems(const QDomNode &config_node);
	void setControlsVisible(bool visible);

private:
	QWidget *time_display, *temperature_display, *date_display;
	TrayBar *tray_bar;
};


// only displayed on the home page, contains date, time and temperature
// display, a link to the settings page and links to multimedia contents
// (RSS feeds, web radio, web tv, web cams)
class HeaderInfo : public StyledWidget
{
Q_OBJECT
public:
	HeaderInfo();

	void loadItems(const QDomNode &config_node, Page *settings);
	void createSettingsPage();

signals:
	void showHomePage();
};


// helper class for HeaderNavigationBar, contains the scrollable list of links to
// internal pages
class HeaderNavigationWidget : public QWidget
{
Q_OBJECT
public:
	HeaderNavigationWidget();

	void addButton(int section_id, int page_id, const QString &icon);
	void addButton(Page *page, const QString &icon);
	void setCurrentSection(int section_id);

public slots:
	void scrollLeft();
	void scrollRight();

signals:
	void pageSelected(int page_id);

protected:
	void showEvent(QShowEvent *e);

private:
	void drawContent();
	BtButton *createButton(int section_id, const QString &icon);

private:
	int current_index, selected_section_id, visible_buttons;
	bool need_update;
	QList<int> section_ids;
	QList<QWidget *> buttons, selected;
	QHBoxLayout *button_layout;
	BtButton *left, *right;
	QSignalMapper *mapper;
};


// navigation bar shown in internal pages; contains a link to the home page
// and a scrollable list of links to section pages
class HeaderNavigationBar : public StyledWidget
{
Q_OBJECT
public:
	HeaderNavigationBar();

	void loadItems(const QDomNode &config_node, Page *settings);
	void setCurrentSection(int section_id);

signals:
	void showHomePage();
	void showSectionPage(int page_id);

private:
	HeaderNavigationWidget *navigation;
};


// contains all the header widgets and shows/hides them as the central page changes
class HeaderWidget : public QWidget
{
Q_OBJECT
public:
	HeaderWidget(TrayBar *tray_bar);
	void centralPageChanged(int section_id, Page::PageType);

	void loadConfiguration(const QDomNode &homepage_node, const QDomNode &infobar_node);

signals:
	void showHomePage();
	void showSectionPage(int page_id);

private:
	QVBoxLayout *main_layout;
	HeaderLogo *header_logo;
	HeaderNavigationBar *top_nav_bar;
	HeaderInfo *header_info;
};

#endif // HEADERWIDGET_H
