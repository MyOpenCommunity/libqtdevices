#include "airconditioning.h"
#include "skinmanager.h"
#include "main.h"
#include "bann2_buttons.h" // BannOnOffNew
#include "bann1_button.h" // BannLeft
#include "xml_functions.h"
#include "bannercontent.h"
#include "bann_airconditioning.h"
#include "navigation_bar.h"

#include <QDomNode>
#include <QString>


AirConditioning::AirConditioning(const QDomNode &config_node)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
}

banner *AirConditioning::getBanner(const QDomNode &item_node)
{
	int id = getTextChild(item_node, "id").toInt();
	SkinContext context(getTextChild(item_node, "cid").toInt());
	QString descr = getTextChild(item_node, "descr");
	QString img_off = bt_global::skin->getImage("off");
	QString img_air_single = bt_global::skin->getImage("air_single");
	QString img_air_gen = bt_global::skin->getImage("air_general");
	QString img_forward = bt_global::skin->getImage("forward");

	banner *b = 0;
	switch (id)
	{
	case AIR_SPLIT:
	{
		SingleSplit *bann = new SingleSplit(0, descr);
		b = bann;
		bann->connectRightButton(new SplitPage(item_node));
		break;
	}
	case AIR_GENERAL:
	{
		GeneralSplit *bann = new GeneralSplit(0, descr);
		b = bann;
		break;
	}
	case AIR_SPLIT_ADV:
	{
		BannOnOffNew *bann = new BannOnOffNew(0);
		bann->initBanner(img_off, img_air_single, img_forward, descr);
		bann->connectRightButton(new AdvancedSplitPage(item_node));
		b = bann;
		break;
	}
	case AIR_GENERAL_ADV:
	{
		BannOnOffNew *bann = new BannOnOffNew(0);
		bann->initBanner(img_off, img_air_gen, img_forward, descr);
		b = bann;
		break;
	}
	}

	if (b)
		b->setId(id);

	return b;
}

void AirConditioning::loadItems(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		// TODO: il generale va messo nel top_widget se presente?
		if (banner *b = getBanner(item))
		{
			page_content->appendBanner(b);
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}
		else
		{
			int id = getTextChild(item, "id").toInt();
			qFatal("Type of item %d not handled on aircondition page!", id);
		}
	}
}


SplitPage::SplitPage(const QDomNode &config_node)
{
	NavigationBar *nav_bar;
	if (getElement(config_node, "off/list").text().toInt() == 1) // show the off button
		nav_bar = new NavigationBar(bt_global::skin->getImage("off"));
	else
		nav_bar = new NavigationBar;

	buildPage(new BannerContent, nav_bar);
	loadScenarios(config_node);
}

void SplitPage::loadScenarios(const QDomNode &config_node)
{
	foreach (const QDomNode &scenario, getChildren(config_node, "cmd"))
	{
		BannLeft *b = new BannLeft(0);
		b->initBanner(bt_global::skin->getImage("split_cmd"), getTextChild(scenario, "descr"));
		page_content->appendBanner(b);
	}
}


AdvancedSplitPage::AdvancedSplitPage(const QDomNode &config_node)
{
	NavigationBar *nav_bar;
	if (getElement(config_node, "off/list").text().toInt() == 1) // show the off button
		nav_bar = new NavigationBar(bt_global::skin->getImage("off"));
	else
		nav_bar = new NavigationBar;

	buildPage(new BannerContent, nav_bar);
	loadScenarios(config_node);
}

void AdvancedSplitPage::loadScenarios(const QDomNode &config_node)
{
	foreach (const QDomNode &scenario, getChildren(config_node, "cmd"))
	{
		AdvancedSplit *b = new AdvancedSplit(0, getTextChild(scenario, "descr"));
		b->connectSxButton(new SplitSettings);
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		page_content->appendBanner(b);
	}
}


SplitSettings::SplitSettings()
{
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("ok"));
	nav_bar->displayScrollButtons(false);
	buildPage(new BannerContent, nav_bar);

	page_content->appendBanner(new TemperatureSplit(0));
}

