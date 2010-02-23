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

	bannFrecce *bannNavigazione;

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
