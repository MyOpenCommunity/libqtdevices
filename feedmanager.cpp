#include "feedmanager.h"
#include "feeditemwidget.h"
#include "itemlist.h"
#include "navigation_bar.h"
#include "main.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "skinmanager.h"

#include <qregexp.h>
#include <QVBoxLayout>
#include <QDebug>
#include <QVector>
#include <QPixmap>

#define ROWS_PER_PAGE 4


// FeedManager implementation

FeedManager::FeedManager(const QDomNode &conf_node)
{
	ItemList *feeds = new ItemList(this, ROWS_PER_PAGE);
	NavigationBar *nav_bar = new NavigationBar;

	buildPage(feeds, nav_bar, getTextChild(conf_node, "descr"), 35);
	layout()->setContentsMargins(0, 5, 25, 10);

	feed_items = new FeedItemList;

	connect(&parser, SIGNAL(feedReady()), SLOT(feedReady()));

	connect(nav_bar, SIGNAL(upClick()), feeds, SLOT(prevItem()));
	connect(nav_bar, SIGNAL(downClick()), feeds, SLOT(nextItem()));
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	connect(feeds, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));
	connect(feeds, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));

	connect(feed_items, SIGNAL(Closed()), SLOT(feedClosed()));

	// read and display feed list
	loadFeedList(conf_node);
}

void FeedManager::loadFeedList(const QDomNode &conf_node)
{
	foreach (const QDomNode &item, getChildren(conf_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());

		QString descr = getTextChild(item, "descr");
		QString url = getTextChild(item, "url");
		QString icon = bt_global::skin->getImage("feed_icon");

		feed_list.append(FeedInfo(url, descr, icon));
	}

	QList<ItemList::ItemInfo> item_list;
	for (int i = 0; i < feed_list.size(); ++i)
	{
		ItemList::ItemInfo item(feed_list[i].desc, QString(),
					feed_list[i].icon, bt_global::skin->getImage("forward_icon"));

		item_list.append(item);
	}

	page_content->setList(item_list, 0);
	page_content->showList();
}

void FeedManager::itemIsClicked(int item)
{
	current_feed = item;
	parser.parse(feed_list[item].path);
}

void FeedManager::hideEvent(QHideEvent *e)
{
	// do not display the feed if the user moves away from the home page
	parser.abort();
}

void FeedManager::feedReady()
{
	feed_items->setFeedInfo(feed_list[current_feed].current_page, parser.getFeedData());
	feed_items->showPage();
}

void FeedManager::feedClosed()
{
	feed_list[current_feed].current_page = feed_items->currentPage();
	showPage();
}


// FeedItemList implementation

FeedItemList::FeedItemList()
{
	forward_icon = bt_global::skin->getImage("forward_icon");
	feed_icon = bt_global::skin->getImage("feed_icon");

	ItemList *feed_items = new ItemList(this, ROWS_PER_PAGE);
	title_widget = new PageTitleWidget("", 35);
	NavigationBar *nav_bar = new NavigationBar;

	buildPage(feed_items, nav_bar, 0, title_widget);
	layout()->setContentsMargins(0, 5, 25, 10);

	feed_item = new FeedItem();

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(upClick()), feed_items, SLOT(prevItem()));
	connect(nav_bar, SIGNAL(downClick()), feed_items, SLOT(nextItem()));

	connect(feed_items, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));
	connect(feed_items, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));
	connect(feed_items, SIGNAL(contentScrolled(int, int)), title_widget, SLOT(setCurrentPage(int, int)));

	connect(feed_item, SIGNAL(Closed()), SLOT(showPage()));
}

void FeedItemList::setFeedInfo(int page, const FeedData &feed_data)
{
	data = feed_data;

	QList<ItemList::ItemInfo> item_list;
	for (int i = 0; i < data.entry_list.size(); ++i)
	{
		ItemList::ItemInfo item(data.entry_list[i].title, data.entry_list[i].last_updated,
					feed_icon, forward_icon);

		item_list.append(item);
	}
	title_widget->setTitle(data.feed_title);

	page_content->setList(item_list, page);
	page_content->showList();
}

void FeedItemList::itemIsClicked(int item)
{
	feed_item->setInfo(data.feed_title, data.entry_list[item]);
	feed_item->showPage();
}

int FeedItemList::currentPage()
{
	return page_content->getCurrentPage();
}


// FeedItem implementation

FeedItem::FeedItem()
{
	item_widget = new FeedItemWidget;
	title_widget = new PageTitleWidget("", 35);
	NavigationBar *nav_bar = new NavigationBar;

	buildPage(item_widget, nav_bar, 0, title_widget);

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(upClick()), item_widget, SLOT(scrollUp()));
	connect(nav_bar, SIGNAL(downClick()), item_widget, SLOT(scrollDown()));
}

void FeedItem::setInfo(const QString &feed_title, const FeedItemInfo &feed_item)
{
	title_widget->setTitle(feed_title);
	item_widget->setFeedInfo(feed_item);
}

