#include "load_management.h"
#include "xml_functions.h"
#include "bannercontent.h"
#include "main.h"
#include "bann_energy.h"
#include "skinmanager.h" // SkinContext
#include "fontmanager.h" // FontManager
#include "navigation_bar.h" //NavigationBar
#include "bann1_button.h" // BannSinglePuls

#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>


namespace
{
	QString getDescriptionWithPriority(const QDomNode &n)
	{
		QString where = getTextChild(n, "where");
		int pos = where.indexOf('#');
		Q_ASSERT_X(pos > -1, "getDescriptionWithPriority", "Device where must have a '#' character");
		QString descr = getTextChild(n, "descr");
		// remove part after '#'
		QString priority = where.left(pos);
		// remove first 7
		priority = priority.mid(1);
		return priority + ". " + descr;
	}
}
LoadManagement::LoadManagement(const QDomNode &config_node) :
	BannerPage(0)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
}

void LoadManagement::loadItems(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
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
	banner *b = 0;
	switch (id)
	{
	case LOAD_WITH_CU:
	{
		bool advanced = getTextChild(item_node, "advanced").toInt();
		BannLoadWithCU *bann = new BannLoadWithCU(getDescriptionWithPriority(item_node),
			advanced ? BannLoadWithCU::ADVANCED_MODE : BannLoadWithCU::BASE_MODE);
		if (advanced)
		{
			Page *p = new LoadDataPage(item_node);
			bann->connectRightButton(p);
			connect(p, SIGNAL(Closed()), bann, SIGNAL(pageClosed()));
		}
		Page *d = new DeactivationTimePage(item_node);
		connect(bann, SIGNAL(deactivateDevice()), d, SLOT(showPage()));
		connect(d, SIGNAL(Closed()), bann, SIGNAL(pageClosed()));

		b = bann;
	}
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
	current_consumption->setText("Current consumption");
	current_consumption->setFont(bt_global::font->get(FontManager::SUBTITLE));

	BannSinglePuls *first_period = new BannSinglePuls(0);
	first_period->initBanner(bt_global::skin->getImage("ok"), bt_global::skin->getImage("empty_background"), "data/ora del reset");
	first_period->setCentralText("Total consumption 1");
	connect(first_period, SIGNAL(rightClick()), SIGNAL(firstReset()));
	BannSinglePuls *second_period = new BannSinglePuls(0);
	second_period->initBanner(bt_global::skin->getImage("ok"), bt_global::skin->getImage("empty_background"), "data/ora del reset");
	second_period->setCentralText("Total consumption 2");
	connect(second_period, SIGNAL(rightClick()), SIGNAL(secondReset()));
	QVBoxLayout *main = new QVBoxLayout(this);
	main->setContentsMargins(0, 0, 0, 0);
	main->setSpacing(0);
	main->addWidget(current_consumption, 0, Qt::AlignHCenter);
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

	QLabel *page_title = new QLabel(getDescriptionWithPriority(config_node));
	page_title->setFont(bt_global::font->get(FontManager::TEXT));

	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("currency_exchange"));
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	QVBoxLayout *main = new QVBoxLayout(this);
	main->setContentsMargins(0, 5, 0, 10);
	main->setSpacing(0);
	main->addWidget(page_title, 0, Qt::AlignHCenter);
	main->addWidget(content, 1);
	main->addWidget(nav_bar);
}



DeactivationTimeContent::DeactivationTimeContent()
{
	DeactivationTime *t = new DeactivationTime(BtTime(2, 30, 0));
	QVBoxLayout *main = new QVBoxLayout(this);
	main->setContentsMargins(0, 0, 0, 0);
	main->setSpacing(0);
	main->addWidget(t);
}



DeactivationTimePage::DeactivationTimePage(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	content = new DeactivationTimeContent;

	QLabel *page_title = new QLabel(getDescriptionWithPriority(config_node));
	page_title->setFont(bt_global::font->get(FontManager::TEXT));

	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("ok"));
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	// TODO: cancel user selection?
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(sendDeactivateDevice()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(Closed()));

	QVBoxLayout *main = new QVBoxLayout(this);
	main->setContentsMargins(0, 5, 0, 10);
	main->setSpacing(0);
	main->addWidget(page_title, 0, Qt::AlignHCenter);
	main->addWidget(content, 1);
	main->addWidget(nav_bar);
}

void DeactivationTimePage::sendDeactivateDevice()
{
	qDebug() << "Deactivating device for time: xxx";
}
