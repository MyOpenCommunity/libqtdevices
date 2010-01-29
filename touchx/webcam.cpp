#include "webcam.h"
#include "navigation_bar.h"
#include "skinmanager.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "imagelabel.h"
#include "btbutton.h"

#include <QDomNode>
#include <QLayout>
#include <QDebug>
#include <QBoxLayout>
#include <QHttp>
#include <QUrl>


WebcamPage::WebcamPage()
{
	image = new ImageLabel;
	QWidget *content = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(content);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(5);
	main_layout->addWidget(image);

	QHBoxLayout *buttons_layout = new QHBoxLayout;
	buttons_layout->setAlignment(Qt::AlignHCenter);
	buttons_layout->setContentsMargins(0, 0, 0, 0);
	buttons_layout->setSpacing(5);

	BtButton* prev_btn = new BtButton;
	prev_btn->setImage(bt_global::skin->getImage("previous"));
	connect(prev_btn, SIGNAL(clicked()), this, SIGNAL(prevImage()));
	buttons_layout->addWidget(prev_btn);

	BtButton* reload_btn = new BtButton;
	reload_btn->setImage(bt_global::skin->getImage("reload"));
	connect(reload_btn, SIGNAL(clicked()), this, SIGNAL(reloadImage()));
	buttons_layout->addWidget(reload_btn);

	BtButton* next_btn = new BtButton;
	next_btn->setImage(bt_global::skin->getImage("next"));
	connect(next_btn, SIGNAL(clicked()), this, SIGNAL(nextImage()));
	buttons_layout->addWidget(next_btn);

	main_layout->addLayout(buttons_layout);

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));
	page_title = new PageTitleWidget(tr(""), 20);
	buildPage(content, nav_bar, 0, page_title);
	http = new QHttp(this);
	connect(http, SIGNAL(requestFinished(int,bool)), SLOT(downloadFinished(int, bool)));
}

void WebcamPage::setImage(QUrl url, QString descr)
{
	description = descr;
	http->setHost(url.host());
	http->get(url.path());
}

void WebcamPage::downloadFinished(int id, bool error)
{
	Q_UNUSED(id);
	if (error)
	{
		qWarning() << "Error" << http->errorString() << "on downloading webcam image";
		return;
	}

	QPixmap p;
	p.loadFromData(http->readAll());
	image->setPixmap(p);
	page_title->setTitle(description);
	update();
}


WebcamListPage::WebcamListPage(const QDomNode &config_node)
{
	current_image = -1;
	ItemList *item_list = new ItemList(0, 4);

	PageTitleWidget *title_widget = new PageTitleWidget(tr("Webcam"), 35);
	NavigationBar *nav_bar = new NavigationBar;

	connect(item_list, SIGNAL(itemIsClicked(int)), SLOT(itemSelected(int)));
	connect(item_list, SIGNAL(contentScrolled(int, int)), title_widget, SLOT(setCurrentPage(int, int)));
	connect(item_list, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));

	buildPage(item_list, nav_bar, 0, title_widget);
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));
	layout()->setContentsMargins(0, 5, 25, 10);
	loadWebcams(config_node);
	webcam_page = new WebcamPage;
	connect(webcam_page, SIGNAL(Closed()), SLOT(showPage()));
	connect(webcam_page, SIGNAL(reloadImage()), SLOT(reloadImage()));
	connect(webcam_page, SIGNAL(prevImage()), SLOT(prevImage()));
	connect(webcam_page, SIGNAL(nextImage()), SLOT(nextImage()));
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

void WebcamListPage::itemSelected(int index)
{
	current_image = index;
	QString url = webcam_list.at(index).data.toString();
	webcam_page->setImage(QUrl(url), webcam_list.at(index).name);
	webcam_page->showPage();
}

void WebcamListPage::reloadImage()
{
	itemSelected(current_image);
}

void WebcamListPage::prevImage()
{
	if (current_image - 1 >= 0)
		itemSelected(current_image - 1);
	else
		itemSelected(webcam_list.size() - 1);
}

void WebcamListPage::nextImage()
{
	if (current_image < webcam_list.size() - 1)
		itemSelected(current_image + 1);
	else
		itemSelected(0);
}

