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

#include <qmap.h>
#include <QList>

class FeedItemWidget;
class ListBrowser;


struct FeedPath
{
	QString path;
	QString desc;
	FeedPath(QString p="", QString d="")
	{
		path = p;
		desc = d;
	}
};


class FeedManager : public Page
{
Q_OBJECT
public:
	enum Status
	{
		SELECTION = 0,
		BROWSING,
		READING
	};

	FeedManager();

public slots:
	virtual void showPage();

private:
	FeedParser parser;
	FeedData data;
	ListBrowser *list_browser;
	QList<FeedPath> feed_list;
	Status status;
	FeedItemWidget *feed_widget;
	QMap<QString, unsigned> page_indexes;

	/// Load the feed list from the configuration file.
	void loadFeedList();
	void setupPage();

private slots:
	void itemIsClicked(int item);
	void feedReady();

	void backClick();
	void downClick();
	void upClick();
};

#endif
