#include "webcam_list.h"
#include "navigation_bar.h"
#include "skinmanager.h"
#include "xml_functions.h" // getChildren, getTextChild

#include <QDomNode>
#include <QLayout>
#include <QDebug>


WebcamListPage::WebcamListPage(const QDomNode &config_node)
{
	ItemList *item_list = new ItemList(0, 4);

	PageTitleWidget *title_widget = new PageTitleWidget(tr("Webcam"), 35);
	NavigationBar *nav_bar = new NavigationBar;

	connect(item_list, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));
	connect(item_list, SIGNAL(contentScrolled(int, int)), title_widget, SLOT(setCurrentPage(int, int)));
	connect(item_list, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));

	buildPage(item_list, nav_bar, 0, title_widget);
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));
	layout()->setContentsMargins(0, 5, 25, 10);
	loadWebcams(config_node);
}

void WebcamListPage::loadWebcams(const QDomNode &config_node)
{

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		ItemList::ItemInfo info(getTextChild(item, "descr"), QString(),
							bt_global::skin->getImage("webcam_icon"),
							bt_global::skin->getImage("forward"),
							getTextChild(item, "url"));

		webcam_list.append(info);
	}
	page_content->setList(webcam_list);
	page_content->showList();
}

void WebcamListPage::itemIsClicked(int index)
{
	QString url = webcam_list.at(index).data.toString();
	qDebug() << "URL: " << url;
}
