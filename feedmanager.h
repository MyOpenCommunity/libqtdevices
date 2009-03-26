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
class bannFrecce;


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


class FeedManager : public PageLayout
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

private:
	FeedParser parser;
	FeedData data;
	ListBrowser *list_browser;
	QList<FeedPath> feed_list;
	Status status;
	FeedItemWidget *feed_widget;
	QMap<QString, unsigned> page_indexes;

	bannFrecce *bannNavigazione;

	/// Load the feed list from the configuration file.
	void loadFeedList();
	void showEvent(QShowEvent *e);
	void setupPage();

private slots:
	void itemIsClicked(int item);
	void feedReady();

	void backClick();
	void downClick();
	void upClick();

signals:
	void Closed();
};

#endif
