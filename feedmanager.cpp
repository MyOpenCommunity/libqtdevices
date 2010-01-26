#include "feedmanager.h"
#include "feeditemwidget.h"
#include "listbrowser.h"
#include "navigation_bar.h"
#include "main.h"
#include "xml_functions.h" // getChildren, getTextChild

#include <qregexp.h>
#include <QVBoxLayout>
#include <QDebug>
#include <QVector>
#include <QPixmap>

#define ROWS_PER_PAGE 4


FeedManager::FeedManager(const QDomNode &conf_node)
{
	loadFeedList(conf_node);
	status = SELECTION;

	QWidget *content = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(content);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);

	list_browser = new ListBrowser(this, ROWS_PER_PAGE);
	main_layout->addWidget(list_browser, 1);

	feed_widget = new FeedItemWidget(this);
	feed_widget->hide();
	main_layout->addWidget(feed_widget, 1);

	NavigationBar *nav_bar = new NavigationBar;
	buildPage(content, nav_bar);

	connect(list_browser, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));
	connect(feed_widget, SIGNAL(Closed()), feed_widget, SLOT(hide()));
	connect(&parser, SIGNAL(feedReady()), SLOT(feedReady()));

	// bannNavigazione up/down signals are inverted...
	connect(nav_bar, SIGNAL(upClick()), SLOT(downClick()));
	connect(nav_bar, SIGNAL(downClick()), SLOT(upClick()));
	connect(nav_bar, SIGNAL(backClick()), SLOT(backClick()));
}

void FeedManager::loadFeedList(const QDomNode &conf_node)
{
	foreach (const QDomNode &item, getChildren(conf_node, "item"))
	{
		QString descr = getTextChild(item, "descr");
		QString url = getTextChild(item, "url");

		feed_list.append(FeedPath(url, descr));
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
		qFatal("Feed status not handled!");
		break;
	}

	prepareTransition();
	list_browser->setList(item_list, page);
	list_browser->showList();
	startTransition();
}

void FeedManager::itemIsClicked(int item)
{
	switch (status)
	{
	case SELECTION:
		Q_ASSERT_X(item >= 0 && item < (int)feed_list.size(), "FeedManager::itemIsClicked",
			"Item index out of range!");
		page_indexes["/"] = list_browser->getCurrentPage();
		qDebug() << "parse url: " << feed_list[item].path;
		parser.parse(feed_list[item].path);
		break;

	case BROWSING:
	{
		Q_ASSERT_X(item >= 0 && item < (int)data.entry_list.size(), "FeedManager::itemIsClicked",
			"Item index out of range!");
		page_indexes[data.feed_title] = list_browser->getCurrentPage();
		feed_widget->setFeedInfo(data.entry_list[item]);
		prepareTransition();
		feed_widget->show();
		list_browser->hide();
		startTransition();
		status = READING;
		break;
	}
	default:
		qFatal("Feed status not handled!");
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
	{
		status = BROWSING;
		prepareTransition();
		feed_widget->hide();
		list_browser->show();
		startTransition();
		break;
	}
	default:
		qFatal("Feed status not handled!");
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
		qFatal("Feed status not handled!");
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
		qFatal("Feed status not handled!");
		break;
	}
}
