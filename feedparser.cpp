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


#include "feedparser.h"

#include <qurl.h>
#include <qdom.h>
#include <QDebug>


static void parseRSS20Item(QDomNode item, FeedData *feed_data);
static void parseRSS20Channel(QDomNode channel, FeedData *feed_data);
static void parseATOM10Entry(QDomNode entry, FeedData *data);


FeedParser::FeedParser()
{
	busy = false;
	connect(&connection, SIGNAL(done(bool)), SLOT(downloadFinished(bool)));
	connect(&connection, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)), SLOT(responseHeader(const QHttpResponseHeader &)));
}

void FeedParser::responseHeader(const QHttpResponseHeader &h)
{
	qDebug() << "response header code: " << h.statusCode();
	switch (h.statusCode())
	{
	// TODO: implement max number of redirections, as stated in http specs
	case 301:  // permanent move, update conf.xml
		// TODO: update conf.xml
		//fall below
	case 302: // temporary move, don't update conf.xml
	case 307: // temporary redirect
		appendRequest(h.value("location"));
		break;
	default:
		break;
	}
}

FeedData FeedParser::getFeedData()
{
	return feed_info;
}

void FeedParser::appendRequest(const QString &url)
{
	qDebug() << "fetching url: " << url << ", from host: " << QUrl(url).host();
	connection.setHost(QUrl(url).host());
	connection.get(url);
}

void FeedParser::parse(QString url)
{
	if (busy)
		return;

	busy = true;
	feed_info = FeedData();
	appendRequest(url);
}

void FeedParser::abort()
{
	if (!busy)
		return;

	connection.abort();
}

void FeedParser::downloadFinished(bool error)
{
	// check if we were redirected
	if (!connection.hasPendingRequests())
	{
		busy = false;
		if (error)
			qWarning("Connection error on download feed!");
		else
		{
			QByteArray xml_data = connection.readAll();
			Q_ASSERT(xml_data.size() != 0);
			extractFeedData(xml_data);
		}
	}
}

void FeedParser::extractFeedData(const QByteArray &xml_data)
{
	FeedType type = extractFeedType(xml_data);
	switch (type)
	{
	case RSS20:
		parseRSS20(xml_data);
		break;
	case ATOM10:
		parseAtom10(xml_data);
		break;
	default:
		qWarning("Wrong feed type, doing nothing");
		return;
	}
	emit feedReady();
}

FeedParser::FeedType FeedParser::extractFeedType(const QByteArray &xml_data)
{
	QDomDocument doc;
	doc.setContent(xml_data);
	QDomElement root = doc.documentElement();
	// check rss 2.0
	if (root.nodeName() == "rss")
	{
		QDomElement e = root.toElement();
		if (e.hasAttribute("version") && e.attribute("version") == "2.0")
		{
			return RSS20;
		}
	}

	// check atom 1.0
	// FIXME: is this the only way to check Atom 1.0 feeds?
	// what about version number?
	if (root.nodeName() == "feed")
		return ATOM10;

	return NONE;
}

void FeedParser::parseRSS20(const QByteArray &xml_data)
{
	QDomDocument doc;
	doc.setContent(xml_data);
	QDomElement root = doc.documentElement();
	Q_ASSERT(root.nodeName() == "rss");

	QDomNode n = root.firstChild();
	while (!n.isNull())
	{
		if (n.nodeName() == "channel")
			parseRSS20Channel(n, &feed_info);
		n = n.nextSibling();
	}
}

void FeedParser::parseAtom10(const QByteArray &xml_data)
{
	QDomDocument doc;
	doc.setContent(xml_data);
	QDomElement root = doc.documentElement();
	Q_ASSERT(root.nodeName() == "feed");

	QDomNode n = root.firstChild();

	while (!n.isNull())
	{
		if (n.nodeName() == "entry")
			parseATOM10Entry(n, &feed_info);
		else if (n.nodeName() == "title")
		{
			QDomElement e = n.toElement();
			feed_info.feed_title = e.text();
		}
		else if (n.nodeName() == "updated")
		{
			QDomElement e = n.toElement();
			feed_info.feed_last_updated = e.text();
		}

		n = n.nextSibling();
	}
}


// ------------------------------------------
// Implementation of static methods
// ------------------------------------------

static void parseRSS20Item(QDomNode item, FeedData *feed_data)
{
	Q_ASSERT(item.toElement().tagName() == "item");

	FeedItemInfo feed_item;

	QDomNode n = item.firstChild();
	while(!n.isNull())
	{
		if (n.isElement())
		{
			QDomElement e = n.toElement();
			// FIXME: spec says that at least one between title and descritpion must be present
			// what about empty fields?
			if (e.tagName() == "title")
				feed_item.title = e.text();
			else if (e.tagName() == "description")
				feed_item.description = e.text();
			else if (e.tagName() == "link")
				feed_item.link = e.text();
			else if (e.tagName() == "pubDate")
				feed_item.last_updated = e.text();
		}
		n = n.nextSibling();
	}

	feed_data->entry_list.append(feed_item);
}

static void parseRSS20Channel(QDomNode channel, FeedData *feed_data)
{
	Q_ASSERT(channel.toElement().tagName() == "channel");

	QDomNode n = channel.firstChild();
	while (!n.isNull())
	{
		if (n.isElement())
		{
			QDomElement e = n.toElement();
			if (e.tagName() == "title")
				feed_data->feed_title = e.text();
			else if (e.tagName() == "item")
				parseRSS20Item(e, feed_data);
			else if (e.tagName() == "pubDate")
				feed_data->feed_last_updated = e.text();
		}
		n = n.nextSibling();
	}
}

static void parseATOM10Entry(QDomNode entry, FeedData *data)
{
	Q_ASSERT(entry.toElement().tagName() == "entry");
	FeedItemInfo feed_entry;
	QDomNode n = entry.firstChild();
	while (!n.isNull())
	{
		if (n.isElement())
		{
			QDomElement e = n.toElement();
			if (e.tagName() == "title")
				feed_entry.title = e.text();
			else if (e.tagName() == "summary")
				feed_entry.description = e.text();
			else if (e.tagName() == "link")
				feed_entry.link = e.text();
			else if (e.tagName() == "updated")
				feed_entry.last_updated = e.text();
		}
		n = n.nextSibling();
	}
	data->entry_list.append(feed_entry);
}

