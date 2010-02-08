#include "load_management.h"
#include "xml_functions.h"
#include "bannercontent.h"
#include "main.h"
#include "bann_energy.h"
#include "skinmanager.h" // SkinContext
#include "fontmanager.h" // FontManager
#include "navigation_bar.h" //NavigationBar

#include <QLabel>
#include <QVBoxLayout>

LoadManagement::LoadManagement(const QDomNode &config_node) :
	BannerPage(0)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
}

void LoadManagement::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		banner *b = getBanner(item);
		if (b)
		{
			page_content->appendBanner(b);
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}
	}
}

banner *LoadManagement::getBanner(const QDomNode &item_node)
{
	SkinContext context(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	QString where = getTextChild(item_node, "where");

	banner *b = 0;
	switch (id)
	{
	case LOAD_WITH_CU:
		break;
	case LOAD_WITHOUT_CU:
	{
		BannLoadNoCU *bann = new BannLoadNoCU(getTextChild(item_node, "descr"));
		Page *p = new LoadDataPage(item_node);
		bann->connectRightButton(p);
		connect(p, SIGNAL(Closed()), bann, SIGNAL(pageClosed()));
		b = bann;
	}
		break;
	}

	if (b)
		b->setId(id);
	return b;
}



LoadDataContent::LoadDataContent()
{
	current_consumption = new QLabel;
	current_consumption->setText("ciao ciao");
	current_consumption->setFont(bt_global::font->get(FontManager::SUBTITLE));

	BannSinglePuls *first_period = new BannSinglePuls(0);
	first_period->initBanner(bt_global::skin->getImage("ok"), bt_global::skin->getImage("empty_background"), "data/ora del reset");
	first_period->setCentralText("TESTO AL CENTRO");
	connect(first_period, SIGNAL(rightClick()), SIGNAL(firstReset()));
	BannSinglePuls *second_period = new BannSinglePuls(0);
	second_period->initBanner(bt_global::skin->getImage("ok"), bt_global::skin->getImage("empty_background"), "data/ora del reset");
	second_period->setCentralText("TESTO AL CENTRO");
	connect(second_period, SIGNAL(rightClick()), SIGNAL(secondReset()));
	QVBoxLayout *main = new QVBoxLayout(this);
	main->setContentsMargins(0, 0, 0, 0);
	main->setSpacing(0);
	main->addWidget(current_consumption);
	main->addWidget(first_period);
	main->addWidget(second_period);
}

void LoadDataContent::currentConsumptionChanged(int new_value)
{
	// TODO: to be implemented
}


LoadDataPage::LoadDataPage(const QDomNode &config_node)
{
	// TODO: this should be ported when merging into master
	SkinContext context(getTextChild(config_node, "cid").toInt());
	content = new LoadDataContent;

	QLabel *page_title = new QLabel(getTextChild(config_node, "descr"));
	page_title->setFont(bt_global::font->get(FontManager::TEXT));

	NavigationBar *nav_bar = new NavigationBar;
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	QVBoxLayout *main = new QVBoxLayout(this);
	main->setContentsMargins(0, 5, 0, 10);
	main->setSpacing(0);
	main->addWidget(page_title, 1, Qt::AlignHCenter);
	main->addWidget(content, 1);
	main->addWidget(nav_bar);
}
