#include "airconditioning.h"
#include "skinmanager.h"
#include "main.h"
#include "bann2_buttons.h" // BannOnOffNew
#include "bann1_button.h" // BannLeft
#include "xml_functions.h" // getTextChild, getElement
#include "bannercontent.h"
#include "bann_airconditioning.h"
#include "navigation_bar.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "airconditioning_device.h"

#include <QDomNode>
#include <QString>


void AirConditioningPrivate::DeviceContainer::append(AirConditioningDevice *d)
{
	devices_list.append(d);
}

void AirConditioningPrivate::DeviceContainer::sendGeneralOff()
{
	foreach (AirConditioningDevice *dev, devices_list)
		dev->sendOff();
}

AirConditioningPrivate::DeviceContainer device_container;



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
		QString where = getTextChild(item_node, "where");
		QString off_cmd = getElement(item_node, "off/command").text();
		AirConditioningDevice *dev = bt_global::add_device_to_cache(new AirConditioningDevice(where, off_cmd));
		SingleSplit *bann = new SingleSplit(descr, dev);
		b = bann;
		bann->connectRightButton(new SplitPage(item_node, dev));
		device_container.append(dev);
		break;
	}
	case AIR_GENERAL:
	{
		GeneralSplit *bann = new GeneralSplit(descr);
		QObject::connect(bann, SIGNAL(sendGeneralOff()), &device_container, SLOT(sendGeneralOff()));
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


SplitPage::SplitPage(const QDomNode &config_node, AirConditioningDevice *d)
{
	dev = d;
	NavigationBar *nav_bar;
	if (getElement(config_node, "off/list").text().toInt() == 1) // show the off button
	{
		nav_bar = new NavigationBar(bt_global::skin->getImage("off"));
		connect(this, SIGNAL(forwardClick()), dev, SLOT(sendOff()));
	}
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
		b->connectSxButton(new SplitSettings(scenario, getChildWithName(config_node, "par")));
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		page_content->appendBanner(b);
	}
}


SplitSettings::SplitSettings(const QDomNode &values_node, const QDomNode &config_node)
{
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("ok"));
	nav_bar->displayScrollButtons(false);
	buildPage(new BannerContent, nav_bar);

	page_content->appendBanner(new SplitTemperature(0));

	QDomNode mode_node = getChildWithName(config_node, "mode");
	if (getTextChild(mode_node, "val1").toInt() != -1)
	{
		QList <int> modes;
		foreach (const QDomNode &val, getChildren(mode_node, "val"))
			modes.append(val.toElement().text().toInt());

		int current_mode = getTextChild(values_node, "mode").toInt();
		page_content->appendBanner(new SplitMode(modes, current_mode));
	}

	QDomNode speed_node = getChildWithName(config_node, "_speed");
	if (getTextChild(speed_node, "val1").toInt() != -1)
	{
		QList <int> speeds;
		foreach (const QDomNode &val, getChildren(speed_node, "val"))
			speeds.append(val.toElement().text().toInt());

		int current_speed = getTextChild(values_node, "speed").toInt();
		page_content->appendBanner(new SplitSpeed(speeds, current_speed));
	}

}

