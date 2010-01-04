#include "airconditioning.h"
#include "skinmanager.h"
#include "main.h"
#include "xml_functions.h" // getTextChild, getElement
#include "bannercontent.h"
#include "bann_airconditioning.h"
#include "navigation_bar.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "probe_device.h" // NonControlledProbeDevice

#include <QDomNode>
#include <QString>
#include <QDebug>

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

void AirConditioning::inizializza()
{
	page_content->initBanners();
}

banner *AirConditioning::getBanner(const QDomNode &item_node)
{
	int id = getTextChild(item_node, "id").toInt();
	SkinContext context(getTextChild(item_node, "cid").toInt());
	QString descr = getTextChild(item_node, "descr");

	banner *b = 0;
	switch (id)
	{
	case AIR_SPLIT:
	{
		QString where = getTextChild(item_node, "where");
		QString off_cmd = getElement(item_node, "off/command").text();
		AirConditioningDevice *dev = bt_global::add_device_to_cache(new AirConditioningDevice(where));
		dev->setOffCommand(off_cmd);

		SingleSplit *bann = new SingleSplit(descr, dev, createProbeDevice(item_node));
		b = bann;
		bann->connectRightButton(new SplitPage(item_node, dev));
		device_container.append(dev);
		break;
	}
	case AIR_SPLIT_ADV:
	{
		QString where = getTextChild(item_node, "where");
		AdvancedAirConditioningDevice *dev = bt_global::add_device_to_cache(new AdvancedAirConditioningDevice(where));

		AdvancedSplitPage *p = new AdvancedSplitPage(item_node, dev);
		SingleSplit *bann = new AdvancedSingleSplit(descr, p, dev, createProbeDevice(item_node));
		b = bann;
		bann->connectRightButton(p);
		// TODO: do we REALLY need this device_container?? maybe it's possible to do without it...
		device_container.append(dev);
		break;
	}
	case AIR_GENERAL:
		b = createGeneralBanner(new GeneralSplitPage(item_node), descr);
		break;
	case AIR_GENERAL_ADV:
		b = createGeneralBanner(new AdvancedGeneralSplitPage(item_node), descr);
		break;
	}

	if (b)
		b->setId(id);

	return b;
}

NonControlledProbeDevice *AirConditioning::createProbeDevice(const QDomNode &item_node)
{
	NonControlledProbeDevice *d = 0;
	QString where_probe = getTextChild(item_node, "where_probe");
	if (!where_probe.isNull())
		d = bt_global::add_device_to_cache(new NonControlledProbeDevice(where_probe, NonControlledProbeDevice::INTERNAL));
	return d;
}

GeneralSplit *AirConditioning::createGeneralBanner(Page *gen_split_page, const QString &descr)
{
	GeneralSplit *bann = new GeneralSplit(descr);
	QObject::connect(bann, SIGNAL(sendGeneralOff()), &device_container, SLOT(sendGeneralOff()));
	bann->connectRightButton(gen_split_page);
	return bann;
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
		connect(this, SIGNAL(forwardClick()), SLOT(setDeviceOff()));
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

void SplitPage::setDeviceOff()
{
	dev->turnOff();
}


AdvancedSplitPage::AdvancedSplitPage(const QDomNode &config_node, AdvancedAirConditioningDevice *d)
{
	NavigationBar *nav_bar;
	if (getElement(config_node, "off/list").text().toInt() == 1) // show the off button
		nav_bar = new NavigationBar(bt_global::skin->getImage("off"));
	else
		nav_bar = new NavigationBar;

	buildPage(new BannerContent, nav_bar);
	loadScenarios(config_node, d);
}

void AdvancedSplitPage::loadScenarios(const QDomNode &config_node, AdvancedAirConditioningDevice *d)
{
	int id = getTextChild(config_node, "id").toInt();
	foreach (const QDomNode &scenario, getChildren(config_node, "cmd"))
	{
		AdvancedSplitScenario *b = new AdvancedSplitScenario(getTextChild(scenario, "descr"), scenario.nodeName(), d);
		SplitSettings *sp = new SplitSettings(scenario, getChildWithName(config_node, "par"));
		b->setCurrentValues(sp->getCurrentStatus());
		b->connectRightButton(sp);
		b->setId(id);
		connect(sp, SIGNAL(splitSettingsChanged(const AirConditionerStatus &)), b, SLOT(splitValuesChanged(const AirConditionerStatus &)));
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		page_content->appendBanner(b);
	}
}

void AdvancedSplitPage::setSerialNumber(int ser)
{
	for (int i = 0; i < page_content->bannerCount(); ++i)
	{
		banner *b = page_content->getBanner(i);
		b->setSerNum(ser);
	}
}

SplitSettings::SplitSettings(const QDomNode &values_node, const QDomNode &config_node)
{
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("ok"));
	nav_bar->displayScrollButtons(false);
	buildPage(new BannerContent, nav_bar);
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(acceptChanges()));
	connect(nav_bar, SIGNAL(backClick()), SLOT(handleClose()));


	QDomNode mode_node = getChildWithName(config_node, "mode");
	readModeConfig(mode_node, values_node);

	QDomNode temp_node = getChildWithName(config_node, "setpoint");
	readTempConfig(temp_node, values_node);

	QDomNode speed_node = getChildWithName(config_node, "speed");
	readSpeedConfig(speed_node, values_node);

	QDomNode swing_node = getChildWithName(config_node, "fan_swing");
	readSwingConfig(swing_node, values_node);
}

/*
 * Returns the current status. Useful at startup, since this page is the only thing that reads the current
 * configuration from config file.
 */
AirConditionerStatus SplitSettings::getCurrentStatus()
{
	readBannerValues();
	AirConditionerStatus status(static_cast<Mode>(current_mode), current_temp,
		static_cast<Velocity>(current_fan_speed), static_cast<Swing>(current_swing));
	return status;
}

void SplitSettings::readModeConfig(const QDomNode &mode_node, const QDomNode &values)
{
	int m = getTextChild(mode_node, "val1").toInt();
	Q_ASSERT_X(m != -1, "SplitSettings::readModeConfig", "Mode cannot be disabled");
	QList <int> modes;
	foreach (const QDomNode &val, getChildren(mode_node, "val"))
		modes.append(val.toElement().text().toInt());

	int current_mode = getTextChild(values, "mode").toInt();
	mode = new SplitMode(modes, current_mode);
	page_content->appendBanner(mode);
}

void SplitSettings::readTempConfig(const QDomNode &temp_node, const QDomNode &values)
{
	int min = getTextChild(temp_node, "min").toInt();
	int max = getTextChild(temp_node, "max").toInt();
	int step = getTextChild(temp_node, "step").toInt();

	int current_temp = getTextChild(values, "setpoint").toInt();
	temperature = new SplitTemperature(current_temp, max, min, step);
	page_content->appendBanner(temperature);
}

void SplitSettings::readSpeedConfig(const QDomNode &speed_node, const QDomNode &values)
{
	if (getTextChild(speed_node, "val1").toInt() != -1)
	{
		QList <int> speeds;
		foreach (const QDomNode &val, getChildren(speed_node, "val"))
			speeds.append(val.toElement().text().toInt());

		int current_speed = getTextChild(values, "speed").toInt();
		speed = new SplitSpeed(speeds, current_speed);
		page_content->appendBanner(speed);
	}
}

void SplitSettings::readSwingConfig(const QDomNode &swing_node, const QDomNode &values)
{
	if (getTextChild(swing_node, "val1").toInt() != -1)
	{
		bool swing_on = getTextChild(values, "fan_swing").toInt();
		swing = new SplitSwing(tr("SWING"), swing_on);
		page_content->appendBanner(swing);
	}
}

void SplitSettings::readBannerValues()
{
	current_fan_speed = speed->currentState();
	current_mode = mode->currentState();
	current_temp = temperature->temperature();
	current_swing = swing->swing();
}

void SplitSettings::showEvent(QShowEvent *)
{
	readBannerValues();
}

void SplitSettings::acceptChanges()
{
	sendUpdatedValues();
}

void SplitSettings::sendUpdatedValues()
{
	readBannerValues();
	AirConditionerStatus status(static_cast<Mode>(current_mode), current_temp,
		static_cast<Velocity>(current_fan_speed), static_cast<Swing>(current_swing));
	emit splitSettingsChanged(status);
}

void SplitSettings::handleClose()
{
	speed->setCurrentState(current_fan_speed);
	mode->setCurrentState(current_mode);
	temperature->setTemperature(current_temp);
	swing->setSwingOn(current_swing);
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
			// this is ok, since general scenarios are taken from conf file and never change
			b->appendDevice(dev->commandToString(AirConditionerStatus(m, t, v, s)),
				bt_global::add_device_to_cache(dev));
		}
		page_content->appendBanner(b);

	}
}
