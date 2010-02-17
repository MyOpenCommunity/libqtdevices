/*!
 * \feedparser.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief A feed parser class
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef FEEDPARSER_H
#define FEEDPARSER_H

#include <qstring.h>
#include <qhttp.h>
#include <qbuffer.h>
#include <qdatetime.h>
#include <QList>

/**
 * Holds relevant information for a single Item in a feed.
 * Some fields may be null strings.
 */
struct FeedItemInfo
{
	QString title;
	QString description;
	QString last_updated;
	QString link;
};


/**
 * Holds only relevant information about the feed.
 */
struct FeedData
{
	/// Title of the feed
	QString feed_title;
	/// Last update time of the whole feed
	QString feed_last_updated;
	/// List of feed items
	QList<FeedItemInfo> entry_list;
};


class FeedParser : public QObject
{
Q_OBJECT
public:
	FeedParser();

	/**
	 * Download and parse the feed given. When parsing is finished, emits
	 * parsingFinished() signal.
	 * \param url The url of the feed.
	 */
	void parse(QString url);

	// aborts the current request
	void abort();

	/**
	 * Get parsed data.
	 * \return The parsed data.
	 */
	FeedData getFeedData();

	enum FeedType
	{
		RSS20 = 0,
		ATOM10,
		NONE,
	};

private slots:
	void downloadFinished(bool error);

	/**
	 * Analyse the response header. It's currently used to check for redirections.
	 */
	void responseHeader(const QHttpResponseHeader &h);

private:
	/**
	 * Parses the downloaded feed and fills the FeedData structure
	 * provided by the user
	 */
	void extractFeedData(const QByteArray &xml_data);

	/**
	 * Finds the type of the feed: rss 1.0, rss 2.0, atom 1.0 etc.
	 * \param feed_data The whole feed downloaded before
	 * \return The type of the feed
	 */
	FeedType extractFeedType(const QByteArray &xml_data);

	/**
	 * Parses a RSS feed and fills the FeedData structure.
	 * \param feed The previously downloaded feed data
	 * \param data The struct that holds the feed's fields
	 */
	void parseRSS20(const QByteArray &xml_data);

	/**
	 * Parses a Atom feed and fills the FeedData structure.
	 * \param feed The previously downloaded feed data
	 * \param data The struct that holds the feed's fields
	 */
	void parseAtom10(const QByteArray &xml_data);

	/**
	 * Enqueues a request to the connection, for example a redirect request.
	 * \param url The url to fetch
	 */
	void appendRequest(const QString &url);

	FeedParser(const FeedParser &);
	FeedParser &operator=(FeedParser &);

	/// Http connection
	QHttp connection;
	/// User supplied FeedData structure to fill
	FeedData feed_info;
	/// A flag that indicate when the parser is already performing a request
	bool busy;

signals:
	void feedReady();
};

#endif // FEEDPARSER_H
