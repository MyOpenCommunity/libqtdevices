#include "feedmanager.h"
#include "feeditemwidget.h"
#include "listbrowser.h"
#include "bannfrecce.h"
#include "main.h"

#include <qregexp.h>
#include <QVBoxLayout>
#include <QDebug>
#include <QVector>

#include <assert.h>

#define ROWS_PER_PAGE 4


FeedManager::FeedManager()
{
	loadFeedList();
	status = SELECTION;

	list_browser = new ListBrowser(this, ROWS_PER_PAGE);
	main_layout->addWidget(list_browser, 1);

	feed_widget = new FeedItemWidget(this);
	feed_widget->hide();
	main_layout->addWidget(feed_widget, 1);

	bannNavigazione = new bannFrecce(this, 3);
	main_layout->addWidget(bannNavigazione);

	connect(list_browser, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));
	connect(feed_widget, SIGNAL(Closed()), feed_widget, SLOT(hide()));
	connect(&parser, SIGNAL(feedReady()), SLOT(feedReady()));

	// bannNavigazione up/down signals are inverted...
	connect(bannNavigazione, SIGNAL(upClick()), SLOT(downClick()));
	connect(bannNavigazione, SIGNAL(downClick()), SLOT(upClick()));
	connect(bannNavigazione, SIGNAL(backClick()), SLOT(backClick()));
}

void FeedManager::loadFeedList()
{
	QDomNode node_page = getPageNode(FEED_READER);
	if (node_page.isNull())
	{
		qDebug("[FEED] ERROR loading configuration");
		return;
	}

	QDomNode n = node_page.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName().contains(QRegExp("item\\d{1,2}")))
		{
			QString descr, url;
			QDomNode child = n.firstChild();
			while (!child.isNull())
			{
				if (child.nodeName() == "descr")
					descr = child.toElement().text();
				else if (child.nodeName() == "url")
					url = child.toElement().text();

				if (descr.length() > 0 && url.length() > 0)
					break;
				child = child.nextSibling();
			}
			if (descr.length() > 0 && url.length() > 0)
				feed_list.append(FeedPath(url, descr));
			else
				qDebug() << "[FEED] Error loading feed item " << n.nodeName();
		}
		n = n.nextSibling();
	}
}

void FeedManager::showPage()
{
	setupPage();
	Page::showPage();
}

void FeedManager::setupPage()
{
	int page = 0;
	QVector<QString> item_list;

	switch (status)
	{
	case SELECTION:
		for (int i = 0; i < feed_list.size(); ++i)
			item_list.append(feed_list[i].desc);

		if (page_indexes.contains("/"))
			page = page_indexes["/"];
		break;

	case BROWSING:
		for (int i = 0; i < data.entry_list.size(); ++i)
			item_list.append(data.entry_list[i].title);

		if (page_indexes.contains("/"))
			page = page_indexes["/"];
		break;

	default:
		assert(!"Feed status not handled!");
		break;
	}

	initTransition();
	list_browser->setList(item_list, page);
	list_browser->showList();
	startTransition();
}

void FeedManager::itemIsClicked(int item)
{
	switch (status)
	{
	case SELECTION:
		assert(item >= 0 && item < (int)feed_list.size() && "Item index out of range!");
		page_indexes["/"] = list_browser->getCurrentPage();
		qDebug() << "parse url: " << feed_list[item].path;
		parser.parse(feed_list[item].path);
		break;

	case BROWSING:
		assert(item >= 0 && item < (int)data.entry_list.size() && "Item index out of range!");
		page_indexes[data.feed_title] = list_browser->getCurrentPage();
		feed_widget->setFeedInfo(data.entry_list[item]);
		initTransition();
		feed_widget->show();
		list_browser->hide();
		startTransition();
		status = READING;
		break;

	default:
		assert(!"Feed status not handled!");
		break;
	}
}

void FeedManager::feedReady()
{
	data = parser.getFeedData();
	status = BROWSING;
	setupPage();
}

void FeedManager::backClick()
{
	switch (status)
	{
	case SELECTION:
		emit Closed();
		break;
	case BROWSING:
		// va salvato anche il page index?
		status = SELECTION;
		setupPage();
		break;
	case READING:
		status = BROWSING;
		initTransition();
		feed_widget->hide();
		list_browser->show();
		startTransition();
		break;
	default:
		assert(!"Feed status not handled!");
		break;
	}
}

void FeedManager::upClick()
{
	switch (status)
	{
	case SELECTION:
	case BROWSING:
		list_browser->prevItem();
		break;
	case READING:
		feed_widget->scrollUp();
		break;
	default:
		assert(!"Feed status not handled!");
		break;
	}
}

void FeedManager::downClick()
{
	switch (status)
	{
	case SELECTION:
	case BROWSING:
		list_browser->nextItem();
		break;
	case READING:
		feed_widget->scrollDown();
		break;
	default:
		assert(!"Feed status not handled!");
		break;
	}
}
