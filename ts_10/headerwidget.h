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
#include "device.h" // DeviceValues
#include "main.h" // TemperatureScale
#include "state_button.h" // StateButton::Status

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
class WebcamPage;


/*!
	\ingroup Core
	\brief Base class for date/time display widgets displayed in the page header.
 */
class PollingDisplayWidget : public QLabel
{
public:
	PollingDisplayWidget();
	~PollingDisplayWidget();

protected:
	void timerEvent(QTimerEvent *e);
	void paintEvent(QPaintEvent *e);

	virtual void paintLabel(QPainter &painter) = 0;

private:
	int timer_id;
};


/*!
	\ingroup Core
	\brief Display current time for internal pages.

	\see HeaderLogo
	\see HomepageTimeDisplay
 */
class InnerPageTimeDisplay : public PollingDisplayWidget
{
public:
	InnerPageTimeDisplay();

protected:
	virtual void paintLabel(QPainter &painter);
};


/*!
	\ingroup Core
	\brief Display current date for internal pages.

	\see HeaderLogo
	\see HomepageDateDisplay
 */
class InnerPageDateDisplay : public PollingDisplayWidget
{
public:
	InnerPageDateDisplay();

protected:
	virtual void paintLabel(QPainter &painter);
};


/*!
	\ingroup Core
	\brief Display current time for the home page.

	\see HeaderInfo
	\see InnerPageTimeDisplay
 */
class HomepageTimeDisplay : public PollingDisplayWidget
{
public:
	HomepageTimeDisplay();

protected:
	virtual void paintLabel(QPainter &painter);
};


/*!
	\ingroup Core
	\brief Display current date for the home page.

	\see HeaderInfo
	\see InnerPageDateDisplay
 */
class HomepageDateDisplay : public PollingDisplayWidget
{
public:
	HomepageDateDisplay();

protected:
	virtual void paintLabel(QPainter &painter);
};


/*!
	\ingroup Core
	\brief Base class to display the temperature from an external probe or non-controlled zone.
 */
class TemperatureDisplay : public QLabel
{
Q_OBJECT
public:
	TemperatureDisplay(device *probe);

private slots:
	void valueReceived(const DeviceValues &values_list);

protected:
	QString label;
	TemperatureScale temp_scale;
};


/*!
	\ingroup Core
	\brief Display the temperature from an external probe or non-controlled zone.

	Similar to InnerPageTemperatureDisplay, but used only for the home page.
 */
class HomepageTemperatureDisplay : public TemperatureDisplay
{
Q_OBJECT
public:
	HomepageTemperatureDisplay(device *probe);

protected:
	void paintEvent(QPaintEvent *e);
};


/*!
	\ingroup Core
	\brief Display the temperature from an external probe or non-controlled zone.

	Similar to HomepageTemperatureDisplay, but used only in internal pages.
 */
class InnerPageTemperatureDisplay : public TemperatureDisplay
{
Q_OBJECT
public:
	InnerPageTemperatureDisplay(device *probe);

protected:
	void paintEvent(QPaintEvent *e);
};


/*!
	\ingroup Core
	\brief Base class for feed/web radio/webcam links in home page.

	Currently displayed in the HeaderInfo bar in the home page.
 */
class HomepageLink : public QWidget
{
Q_OBJECT
public:
	HomepageLink(const QString &description, const QString &icon);

signals:
	void pageClosed();
	void clicked();
	void aboutToClick();

private slots:
	void buttonClicked();
};


/*!
	\ingroup Core
	\brief Link to an RSS feed.

	Displayed in the homepage HeaderInfo.  When clicked, parses the feed and
	displays feed items.
 */
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


/*!
	\ingroup Core
	\brief Link to a web radio

	Displayed in the homepage HeaderInfo.  When clicked, starts playing the
	configured web radio.
 */
class HomepageIPRadioLink : public HomepageLink
{
Q_OBJECT
public:
	HomepageIPRadioLink(const QString &description, const QString &url);

private slots:
	void playRadio();
	void radioClosed();

private:
	AudioPlayerPage *player;
	QString url;
};


/*!
	\ingroup Core
	\brief Link to an webcam.

	Displayed in the homepage HeaderInfo.  When clicked, displays the webcam page.
 */
class HomepageWebcamLink : public HomepageLink
{
Q_OBJECT
public:
	HomepageWebcamLink(const QString &description, const QString &url);

private slots:
	void showWebcam();
	void reloadWebcam();

private:
	WebcamPage *webcam;
	QString url, title;
};


/*!
	\ingroup Core
	\brief Topmost page header.

	Displays the logo and TrayBar and (in inner pages) date, time and temperature.

	\see InnerPageTimeDisplay
	\see InnerPageDateDisplay
	\see InnerPageTemperatureDisplay
	\see TrayBar
 */
class HeaderLogo : public StyledWidget
{
Q_OBJECT
public:
	HeaderLogo(TrayBar *tray);

	void loadItems(const QDomNode &config_node);

	/*!
		\brief Show date, time and temperature display.
	 */
	void setControlsVisible(bool visible);

private:
	QWidget *time_display, *temperature_display, *date_display;
	TrayBar *tray_bar;
};


/*!
	\ingroup Core
	\brief Informative bar displayed in the home page.

	Always contains a link to the settings page; depending on the configuration
	could display date, time and the temperature from a probe, and contain links
	to various multimedia items.

	\see HomepageTemperatureDisplay
	\see HomepageDateDisplay
	\see HomepageTimeDisplay
	\see HomepageFeedLink
	\see HomepageIPRadioLink
	\see HomepageWebcamLink
 */
class HeaderInfo : public StyledWidget
{
Q_OBJECT
public:
	HeaderInfo();

	void loadItems(const QDomNode &config_node, Page *settings);

signals:
	/*!
		\brief Emitted when the page linked to one of the home page links is closed.

		Handled by HeaderWidget.
	 */
	void showHomePage();

	/*!
		\brief Emitted when the user clicks on one of the links (settings or multimedia items).

		Handled by HeaderWidget.
	 */
	void aboutToChangePage();

private slots:
	void showSettings();

private:
	Page *settings;
};


/*!
	\ingroup Core
	\brief Helper class for HeaderNavigationBar

	Contains a scrollable list of links to internal pages (all the configured
	sections plus the settings).
 */
class HeaderNavigationWidget : public QWidget
{
Q_OBJECT
public:
	HeaderNavigationWidget();

	void addButton(int section_id, int page_id, const QString &icon, const QString &icon_on = QString());
	StateButton *createButton(int section_id, const QString &icon, const QString &icon_on = QString());
	void setCurrentSection(int section_id);
	void changeIconState(int page_id, StateButton::Status st);

signals:
	void pageSelected(int page_id);

protected:
	void showEvent(QShowEvent *e);

private:
	void drawContent();
	QWidget *itemForIndex(int i);

private slots:
	void scrollLeft();
	void scrollRight();

private:
	int current_index, selected_section_id, visible_buttons;
	bool need_update;
	QList<int> section_ids;
	QList<QWidget*> buttons, selected;
	QHBoxLayout *button_layout;
	BtButton *left, *right;
	QSignalMapper *mapper;
};


/*!
	\ingroup Core
	\brief Navigation bar shown in internal pages.

	Contains a link to the home page and a scrollable list of links to section
	pages plus the settings page.

	\see HeaderNavigationWidget
 */
class HeaderNavigationBar : public StyledWidget
{
Q_OBJECT
public:
	HeaderNavigationBar();

	void loadItems(const QDomNode &config_node, Page *settings);
	void setCurrentSection(int section_id);
	HeaderNavigationWidget *navigation;

signals:
	/*!
		\brief Emitted when the user clicks the home page link.

		Handled by HeaderWidget.
	 */
	void showHomePage();

	/*!
		\brief Emitted when the user clicks one of the section icons.

		Handled by HeaderWidget.
	 */
	void showSectionPage(int page_id);

	/*!
		\brief Emitted when the user clicks on one of the links (home page or section page).

		Handled by HeaderWidget.
	 */
	void aboutToChangePage();

private slots:
	void goHome();
	void showSettings();
	void pageSelected(int page_id);

private:
	Page *settings;
};


/*!
	\ingroup Core
	\brief Container for all header widgets

	Contains HeaderLogo, HeaderInfo and HeaderNavigationBar and shows/hides them
	depending on the displayed page (home page or internal page).
 */
class HeaderWidget : public QWidget
{
Q_OBJECT
public:
	HeaderWidget(TrayBar *tray_bar);

	/*!
		\brief Called to change header layout for home/inner pages.
	 */
	void centralPageChanged(int section_id, bool is_homepage);

	/*!
		\brief Called to change the active section in the navigation bar.
	 */
	void sectionChanged(int section_id);

	/*!
		\brief Called to propagate state changes to the icons in the navigation bar.
	 */
	void iconStateChanged(int page_id, StateButton::Status st);

	void loadConfiguration(const QDomNode &homepage_node, const QDomNode &infobar_node);

signals:
	/*!
		\brief Display the home page
	 */
	void showHomePage();

	/*!
		\brief Display one of the section pages
	 */
	void showSectionPage(int page_id);

	/*!
		\brief Emitted before showSectionPage() and showHomePage()
	 */
	void aboutToChangePage();

private:
	QVBoxLayout *main_layout;
	HeaderLogo *header_logo;
	HeaderNavigationBar *top_nav_bar;
	HeaderInfo *header_info;
};

#endif // HEADERWIDGET_H
