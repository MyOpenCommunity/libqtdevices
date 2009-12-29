#include "airconditioning.h"
#include "skinmanager.h"
#include "main.h"
#include "xml_functions.h" // getTextChild, getElement
#include "bannercontent.h"
#include "bann_airconditioning.h"
#include "navigation_bar.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "airconditioning_device.h"
#include "probe_device.h" // NonControlledProbeDevice

#include <QDomNode>
#include <QString>

typedef AdvancedAirConditioningDevice::Mode Mode;
typedef AdvancedAirConditioningDevice::Swing Swing;
typedef AdvancedAirConditioningDevice::Velocity Velocity;


void AirConditioningPrivate::DeviceContainer::append(AirConditioningInterface *d)
{
	devices_list.append(d);
}

void AirConditioningPrivate::DeviceContainer::sendGeneralOff()
{
	foreach (AirConditioningInterface *dev, devices_list)
		dev->turnOff();
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
		AirConditioningDevice *dev = bt_global::add_device_to_cache(new AirConditioningDevice(where));
		dev->setOffCommand(off_cmd);

		NonControlledProbeDevice *dev_probe = 0;
		QString where_probe = getTextChild(item_node, "where_probe");
		if (!where_probe.isNull())
			dev_probe = new NonControlledProbeDevice(where_probe, NonControlledProbeDevice::INTERNAL);
		SingleSplit *bann = new SingleSplit(descr, dev, dev_probe);
		b = bann;
		bann->connectRightButton(new SplitPage(item_node, dev));
		device_container.append(dev);
		break;
	}
	case AIR_GENERAL:
	case AIR_GENERAL_ADV:
	{
		GeneralSplit *bann = new GeneralSplit(descr);
		QObject::connect(bann, SIGNAL(sendGeneralOff()), &device_container, SLOT(sendGeneralOff()));
		bann->connectRightButton(new GeneralSplitPage(item_node));
		b = bann;
		break;
	}
	case AIR_SPLIT_ADV:
	{
		// TODO: replace BannOnOffNew with a specialized class
		BannOnOffNew *bann = new BannOnOffNew(0);
		bann->initBanner(img_off, img_air_single, img_forward, descr);
		bann->connectRightButton(new AdvancedSplitPage(item_node));
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
		SplitScenario *b = new SplitScenario(getTextChild(scenario, "descr"),
			getTextChild(scenario, "command"), dev);
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
		AdvancedSplitScenario *b = new AdvancedSplitScenario(0, getTextChild(scenario, "descr"));
		b->connectRightButton(new SplitSettings(scenario, getChildWithName(config_node, "par")));
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		page_content->appendBanner(b);
	}
}


SplitSettings::SplitSettings(const QDomNode &values_node, const QDomNode &config_node)
{
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("ok"));
	nav_bar->displayScrollButtons(false);
	buildPage(new BannerContent, nav_bar);
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(acceptChanges()));


	QDomNode mode_node = getChildWithName(config_node, "mode");
	if (getTextChild(mode_node, "val1").toInt() != -1) // TODO: verificare se puo' essere disabilitato o no!
	{
		QList <int> modes;
		foreach (const QDomNode &val, getChildren(mode_node, "val"))
			modes.append(val.toElement().text().toInt());

		int current_mode = getTextChild(values_node, "mode").toInt();
		SplitMode *m = new SplitMode(modes, current_mode);
		connect(m, SIGNAL(currentStateChanged(int)), SLOT(modeChanged(int)));
		page_content->appendBanner(m);
	}

	QDomNode temp_node = getChildWithName(config_node, "setpoint");
	int min = getTextChild(temp_node, "min").toInt();
	int max = getTextChild(temp_node, "max").toInt();
	int step = getTextChild(temp_node, "step").toInt();

	int current_temp = getTextChild(values_node, "setpoint").toInt();
	page_content->appendBanner(new SplitTemperature(current_temp, max, min, step));

	QDomNode speed_node = getChildWithName(config_node, "speed");
	if (getTextChild(speed_node, "val1").toInt() != -1)
	{
		QList <int> speeds;
		foreach (const QDomNode &val, getChildren(speed_node, "val"))
			speeds.append(val.toElement().text().toInt());

		int current_speed = getTextChild(values_node, "speed").toInt();
		SplitSpeed *s = new SplitSpeed(speeds, current_speed);
		connect(s, SIGNAL(currentStateChanged(int)), SLOT(speedChanged(int)));
		page_content->appendBanner(s);
	}

	QDomNode swing = getChildWithName(config_node, "fan_swing");
	if (getTextChild(swing, "val1").toInt() != -1)
		page_content->appendBanner(new SplitSwing(tr("SWING")));
}

void SplitSettings::acceptChanges()
{
	// TODO: to be implemented
}

void SplitSettings::modeChanged(int m)
{
	selected_mode = m;
}

void SplitSettings::speedChanged(int s)
{
	selected_fan_speed = s;
}


GeneralSplitPage::GeneralSplitPage(const QDomNode &config_node)
{
	buildPage();
	loadScenarios(config_node);
}

void GeneralSplitPage::loadScenarios(const QDomNode &config_node)
{
	foreach (const QDomNode &scenario, getChildren(config_node, "cmd"))
	{
		GeneralSplitScenario *b = new GeneralSplitScenario(getTextChild(scenario, "descr"));
		foreach (const QDomNode &split, getChildren(scenario, "split"))
		{
			AirConditioningDevice *dev = new AirConditioningDevice(getTextChild(split, "where"));
			b->appendDevice(getTextChild(split, "command"), bt_global::add_device_to_cache(dev));
		}

		page_content->appendBanner(b);
	}
}


AdvancedGeneralSplitPage::AdvancedGeneralSplitPage(const QDomNode &config_node)
{
	buildPage();
	loadScenarios(config_node);
}

void AdvancedGeneralSplitPage::loadScenarios(const QDomNode &config_node)
{
	foreach (const QDomNode &scenario, getChildren(config_node, "cmd"))
	{
		GeneralSplitScenario *b = new GeneralSplitScenario(getTextChild(scenario, "descr"));
		foreach (const QDomNode &split, getChildren(scenario, "split"))
		{
			AdvancedAirConditioningDevice *dev = new AdvancedAirConditioningDevice(getTextChild(split, "where"));
			Mode m = static_cast<Mode>(getTextChild(split, "mode").toInt());
			int t = getTextChild(split, "setpoint").toInt();
			Velocity v = static_cast<Velocity>(getTextChild(split, "speed").toInt());
			Swing s = static_cast<Swing>(getTextChild(split, "fan_swing").toInt());
			// TODO: this is wrong since the command can be modified using the custom page
			b->appendDevice(dev->commandToString(AirConditionerStatus(m, t, v, s)),
				bt_global::add_device_to_cache(dev));
		}
		page_content->appendBanner(b);

	}
}
