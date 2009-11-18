#include "pagecontainer.h"
#include "btbutton.h"
#include "pagefactory.h" // getPage
#include "xml_functions.h" // getTextChild, getChildren
#include "hardware_functions.h" // rearmWDT
#include "main.h" // IMG_PATH
#include "navigation_bar.h"
#include "skinmanager.h"

#include <QDomNode>
#include <QDebug>
#include <QTime>
#include <QGridLayout>

#define DIM_BUT 80
#define BACK_BUTTON_X    0
#define BACK_BUTTON_Y  250
#define BACK_BUTTON_DIM 60
#define IMG_BACK_BUTTON     IMG_PATH "arrlf.png"


PageContainer::PageContainer(const QDomNode &config_node)
	: buttons_group(this)
{
	connect(&buttons_group, SIGNAL(buttonClicked(int)), SLOT(clicked(int)));
	loadItems(config_node);
}

void PageContainer::buildPage(IconContent *content, NavigationBar *nav_bar)
{
	Page::buildPage(content, nav_bar);

	// TODO duplicated in BannerPage
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(this, SIGNAL(Closed()), content, SLOT(resetIndex()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(forwardClick()));
	connect(nav_bar, SIGNAL(upClick()), content, SLOT(pgUp()));
	connect(nav_bar, SIGNAL(downClick()), content, SLOT(pgDown()));
	connect(content, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));
}

void PageContainer::loadItems(const QDomNode &config_node)
{
	if (hardwareType() == TOUCH_X)
		// TODO hide back button in homepage
		buildPage(new IconContent, new NavigationBar);

	QTime wdtime;
	wdtime.start(); // Start counting for wd refresh

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
#ifdef LAYOUT_BTOUCH
		int id = getTextChild(item, "id").toInt();
		QString img1 = IMG_PATH + getTextChild(item, "cimg1");
		int x = getTextChild(item, "left").toInt();
		int y = getTextChild(item, "top").toInt();

		// Within the pagemenu element, it can exists items that are not a page.
		if (Page *p = getPage(id))
			addPage(p, id, img1, x, y);
#else
		SkinContext cxt(getTextChild(item, "cid").toInt());
		QString icon = bt_global::skin->getImage("link_icon");
		int link_id = getTextChild(item, "id").toInt();

		// TODO some ids are not links
		int pageid = getTextChild(item, "lnk_pageID").toInt();
		if (Page *p = getPage(pageid))
			addPage(p, pageid, icon, 0, 0);
#endif

		if (wdtime.elapsed() > 1000)
		{
			wdtime.restart();
			rearmWDT();
		}
	}
}

void PageContainer::addPage(Page *page, int id, QString iconName, int x, int y)
{
	BtButton *b = new BtButton(this);
	if (page_content == NULL)
		b->setGeometry(x, y, DIM_BUT, DIM_BUT);
	else
		page_content->addPage(b);
	b->setImage(iconName);

	buttons_group.addButton(b, id);
	page_list[id] = page;
	connect(page, SIGNAL(Closed()), this, SLOT(showPage()));
}

void PageContainer::addBackButton()
{
	BtButton *b = new BtButton(this);
	b->setGeometry(BACK_BUTTON_X, BACK_BUTTON_Y, BACK_BUTTON_DIM, BACK_BUTTON_DIM);
	b->setImage(IMG_BACK_BUTTON);
	connect(b, SIGNAL(clicked()), SIGNAL(Closed()));
}

void PageContainer::clicked(int id)
{
	page_list[id]->showPage();
}


IconContent::IconContent(QWidget *parent) : QWidget(parent)
{
	current_page = 0;
	QGridLayout *l = new QGridLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(28);
	l->setColumnStretch(5, 1);
	need_update = true;
}

void IconContent::addPage(QWidget *button)
{
	Q_ASSERT_X(layout() != 0, "IconContent::addPage", "Unable to call addPage without a layout");
	items.append(button);
	button->hide();
}

void IconContent::resetIndex()
{
	need_update = true;
	current_page = 0;
}

void IconContent::showEvent(QShowEvent *e)
{
	drawContent();
	QWidget::showEvent(e);
}

void IconContent::drawContent()
{
	if (!need_update)
		return;

	QGridLayout *l = qobject_cast<QGridLayout*>(layout());

	if (pages.size() == 0)
	{
		int total_height[4];
		int area_height = contentsRect().height();

		for (int k = 0; k < 4; ++k)
			total_height[k] = 0;
		pages.append(0);

		for (int i = 0; i < items.size(); i += 4)
		{
			for (int j = 0; j < 4 && i + j < items.size(); ++j)
			{
				l->addWidget(items.at(i + j), i / 4, j);
				total_height[j] += items.at(i + j)->sizeHint().height();
			}
			for (int j = 0; j < 4; ++j)
			{
				if (total_height[j] > area_height)
				{
					for (int k = 0; k < 4; ++k)
						total_height[k] = 0;
					pages.append(i);
					i -= 4;
					break;
				}
			}
		}

		pages.append(items.size());
		l->setRowStretch(l->rowCount(), 1);
	}

	emit displayScrollButtons(pageCount() > 1);

	need_update = false;

	for (int i = 0; i < items.size(); ++i)
		items[i]->setVisible(i >= pages[current_page] && i < pages[current_page + 1]);
}

void IconContent::pgUp()
{
	current_page = (current_page - 1 + pageCount()) % pageCount();
	need_update = true;
	drawContent();
}

void IconContent::pgDown()
{
	current_page = (current_page + 1) % pageCount();
	need_update = true;
	drawContent();
}

int IconContent::pageCount() const
{
	return pages.size() - 1;
}

