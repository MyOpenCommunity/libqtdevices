/*!
 * \feedmanager.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief The feed manager, that display the feed list or the details of a news.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 */

#ifndef FEED_MANAGER_H
#define FEED_MANAGER_H

#include "feedparser.h"
#include "page.h"

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


// displays the list of RSS feeds
class FeedManager : public Page
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


// display the items of an RSS feed
class FeedItemList : public Page
{
Q_OBJECT
public:
	typedef ItemList ContentType;

	FeedItemList();

	void setFeedInfo(int page, const FeedData &feed_data);
	int currentPage();

private slots:
	void itemIsClicked(int item);

private:
	FeedData data;
	PageTitleWidget *title_widget;
	FeedItem *feed_item;
	QString forward_icon, feed_icon;
};


// display a single RSS news item
class FeedItem : public Page
{
Q_OBJECT
public:
	FeedItem();

	void setInfo(const QString &feed_title, const FeedItemInfo &feed_item);

private:
	FeedItemWidget *item_widget;
	PageTitleWidget *title_widget;
};

#endif
