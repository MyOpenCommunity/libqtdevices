/*!
 * \feedparser.cpp
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief A class to parse rss feeds
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "feedparser.h"

#include <qurl.h>
#include <qdom.h>
#include <QDebug>

#include <assert.h>


static void parseRSS20Item(QDomNode item, FeedData *feed_data);
static void parseRSS20Channel(QDomNode channel, FeedData *feed_data);
static void parseATOM10Entry(QDomNode entry, FeedData *data);
static QDateTime parseDate(QString text_date);


FeedParser::FeedParser()
{
	busy = false;
	connect(&connection, SIGNAL(done(bool)), SLOT(downloadFinished(bool)));
}

FeedData FeedParser::getFeedData()
{
	return feed_info;
}

void FeedParser::parse(QString url)
{
	if (busy)
		return;

	busy = true;
	feed_info = FeedData();
	connection.setHost(QUrl(url).host());
	connection.get(url);
}

void FeedParser::downloadFinished(bool error)
{
	busy = false;
	if (error)
		qWarning("Connection error on download feed!");
	else
	{
		QByteArray xml_data = connection.readAll();
		assert(xml_data.size() != 0);
		extractFeedData(xml_data);
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
	assert(root.nodeName() == "rss");

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
	assert(root.nodeName() == "feed");

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
	assert(item.toElement().tagName() == "item");

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
	assert(channel.toElement().tagName() == "channel");

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
	assert(entry.toElement().tagName() == "entry");
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

QDateTime parseDate(QString text_date)
{
	// try Qt default format
	QDateTime date = QDateTime::fromString(text_date);
	if (!date.isValid())
	{
		// try ISO format
		date = QDateTime::fromString(text_date, Qt::ISODate);
		if (!date.isValid())
			qWarning("Date is not valid");
	}
	// it may be invalid
	return date;
}
