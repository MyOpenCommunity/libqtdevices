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


#ifndef FEEDPARSER_H
#define FEEDPARSER_H

#include <QString>
#include <QHttp>
#include <QList>

/*!
	\ingroup Core
	\brief Holds relevant information for a single Item in a feed.
*/
struct FeedItemInfo
{
	QString title;
	QString description;
	QString last_updated;
	QString link;
};


/*!
	\ingroup Core
	\brief Hold relevant information about the feed.
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


/*!
	\ingroup Core
	\brief Download and parse an RSS/ATOM feed.

	Use the parse() method to download and parse a feed available at a given url,
	or the abort() method to cancel the current request. When the parsing
	is finished the feedReady() is emitted, and the correspondent data can be
	retrieved using the getFeedData() method.
*/
class FeedParser : public QObject
{
Q_OBJECT
public:
	FeedParser();

	/*!
		\brief Download and parse the feed of the given \a url.

		When parsing is finished, emits feedReady() signal.
	*/
	void parse(QString url);

	/*!
		\brief Aborts the current request.
	*/
	void abort();

	/*!
		\brief Return the parsed data.
	*/
	FeedData getFeedData();

	/*!
		The type of feeds supported.
	*/
	enum FeedType
	{
		RSS20 = 0, /*!< RSS version 2.0 */
		ATOM10,    /*!< ATOM version 1.0 */
		NONE,      /*!< Invalid value */
	};

signals:
	/*!
		\brief Emitted when the download and parsing of the feed requested is finished.
		\sa parse()
	*/
	void feedReady();

private slots:
	void downloadFinished(bool error);

	// Analyse the response header. It's currently used to check for redirections.
	void responseHeader(const QHttpResponseHeader &h);

private:
	// Parses the downloaded feed and fills the FeedData structure provided by the user.
	void extractFeedData(const QByteArray &xml_data);

	// Finds the type of the feed: rss 1.0, rss 2.0, atom 1.0 etc.
	FeedType extractFeedType(const QByteArray &xml_data);

	// Parses a RSS feed and fills the FeedData structure.
	void parseRSS20(const QByteArray &xml_data);

	// Parses a Atom feed and fills the FeedData structure.
	void parseAtom10(const QByteArray &xml_data);

	// Enqueues a request to the connection, for example a redirect request.
	void appendRequest(const QString &url);

	FeedParser(const FeedParser &);
	FeedParser &operator=(FeedParser &);

	// Http connection
	QHttp connection;
	// User supplied FeedData structure to fill
	FeedData feed_info;
	// A flag that indicate when the parser is already performing a request
	bool busy;

};

#endif // FEEDPARSER_H
