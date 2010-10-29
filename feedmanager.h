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


#ifndef FEED_MANAGER_H
#define FEED_MANAGER_H

#include "feedparser.h"
#include "scrollablepage.h"

#include <QList>

class FeedItemWidget;
class ItemList;
class QDomNode;
class FeedItem;
class FeedItemList;


struct FeedInfo
{
	QString path;
	QString desc;
	QString icon;
	int current_page;

	FeedInfo(QString p, QString d, QString i="")
	{
		path = p;
		desc = d;
		icon = i;
		current_page = 0;
	}
};


/*!
	\ingroup Multimedia
	\brief Shows the list of RSS feeds taken from the configuration file.
 */
class FeedManager : public ScrollablePage
{
Q_OBJECT
public:
	typedef ItemList ContentType;

	FeedManager(const QDomNode &conf_node);

protected:
	void hideEvent(QHideEvent *e);

private:
	/// Load the feed list from the configuration file.
	void loadFeedList(const QDomNode &conf_node);

private slots:
	void itemIsClicked(int item);
	void feedReady();
	void feedClosed();

private:
	FeedParser parser;
	FeedItemList *feed_items;
	QList<FeedInfo> feed_list;
	int current_feed;
};


/*!
	\ingroup Multimedia
	\brief Displays the items of a single RSS feed.
 */
class FeedItemList : public ScrollablePage
{
Q_OBJECT
public:
	typedef ItemList ContentType;

	FeedItemList();

	void setFeedInfo(int page, const FeedData &feed_data);
	int currentPage();

private slots:
	void itemIsClicked(int item);
	void showPrev();
	void showNext();

private:
	FeedData data;
	PageTitleWidget *title_widget;
	FeedItem *feed_item;
	QString forward_icon, feed_icon;
	int current_shown_item;
};


/*!
	\ingroup Multimedia
	\brief Display a single RSS news item.
*/
class FeedItem : public Page
{
Q_OBJECT
public:
	FeedItem();

	void setInfo(const QString &feed_title, const FeedItemInfo &feed_item);

signals:
	void showNext();
	void showPrev();

private:
	FeedItemWidget *item_widget;
	PageTitleWidget *title_widget;
};

#endif
