#include "airconditioning.h"
#include "skinmanager.h"
#include "main.h"
#include "xml_functions.h" // getTextChild, getElement
#include "bannercontent.h"
#include "bann_airconditioning.h"
#include "navigation_bar.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "probe_device.h" // NonControlledProbeDevice
#include "btbutton.h"

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QGridLayout>

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

	QList<QDomNode> commands = getChildren(item_node, "cmd");

	banner *b = 0;
	switch (id)
	{
	case AIR_SPLIT:
	{
		QString where = getTextChild(item_node, "where");
#ifdef CONFIG_BTOUCH
		QString off_cmd = getElement(item_node, "off/command").text();
#else
		QString off_cmd = getTextChild(item_node, "off_command");
#endif
		AirConditioningDevice *dev = bt_global::add_device_to_cache(new AirConditioningDevice(where));
		dev->setOffCommand(off_cmd);

		SingleSplit *bann = new SingleSplit(descr, !commands.isEmpty(), dev, createProbeDevice(item_node));
		b = bann;
		if (!commands.isEmpty())
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
		device_container.append(dev);
		break;
	}
	case AIR_GENERAL:
		b = createGeneralBanner(commands.isEmpty() ? 0 : new GeneralSplitPage(item_node), descr);
		break;
	case AIR_GENERAL_ADV:
		b = createGeneralBanner(commands.isEmpty() ? 0 : new AdvancedGeneralSplitPage(item_node), descr);
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
	if (where_probe != "000")
		d = bt_global::add_device_to_cache(new NonControlledProbeDevice(where_probe, NonControlledProbeDevice::INTERNAL));
	return d;
}

GeneralSplit *AirConditioning::createGeneralBanner(Page *gen_split_page, const QString &descr)
{
	GeneralSplit *bann = new GeneralSplit(descr, gen_split_page != 0);
	QObject::connect(bann, SIGNAL(sendGeneralOff()), &device_container, SLOT(sendGeneralOff()));
	if (gen_split_page)
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
#ifdef CONFIG_BTOUCH
	int off_list = getElement(config_node, "off/list").text().toInt();
#else
	int off_list = getTextChild(config_node, "off_list").toInt();
#endif

#ifdef LAYOUT_BTOUCH
	NavigationBar *nav_bar;

	if (off_list == 1) // show the off button
	{
		nav_bar = new NavigationBar(bt_global::skin->getImage("off"));
		connect(this, SIGNAL(forwardClick()), SLOT(setDeviceOff()));
	}
	else
		nav_bar = new NavigationBar;

	buildPage(new BannerContent, nav_bar, getTextChild(config_node, "descr"));
#else
	if (off_list == 1) // show the off button
	{
		BannerContent *banners = new BannerContent;
		QWidget *cnt = new QWidget;
		QVBoxLayout *l = new QVBoxLayout(cnt);
		l->setContentsMargins(0, 0, 25, 35);
		BtButton *off = new BtButton;
		off->setImage(bt_global::skin->getImage("off"));

		l->addWidget(banners, 1);
		l->addWidget(off, 0, Qt::AlignRight);

		buildPage(cnt, banners, new NavigationBar, getTextChild(config_node, "descr"));
	}
	else
		buildPage(getTextChild(config_node, "descr"));
#endif

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
	single_page = 0;
#ifdef CONFIG_BTOUCH
	int off_list = getElement(config_node, "off/list").text().toInt();
#else
	int off_list = getTextChild(config_node, "off_list").toInt();
#endif

#ifdef LAYOUT_BTOUCH
	NavigationBar *nav_bar;

	if (off_list == 1) // show the off button
	{
		nav_bar = new NavigationBar(bt_global::skin->getImage("off"));
		connect(nav_bar, SIGNAL(forwardClick()), SLOT(setDeviceOff()));
	}
	else
		nav_bar = new NavigationBar;

	buildPage(new BannerContent, nav_bar, getTextChild(config_node, "descr"));
#else
	BannerContent *banners = new BannerContent;
	static_cast<QGridLayout *>(banners->layout())->setVerticalSpacing(20);

	if (off_list == 1) // show the off button
	{
		QWidget *cnt = new QWidget;
		QVBoxLayout *l = new QVBoxLayout(cnt);
		l->setContentsMargins(0, 0, 25, 35);
		BtButton *off = new BtButton;
		off->setImage(bt_global::skin->getImage("off"));

		l->addWidget(banners, 1);
		l->addWidget(off, 0, Qt::AlignRight);

		buildPage(cnt, banners, new NavigationBar, getTextChild(config_node, "descr"));
	}
	else
		buildPage(banners, new NavigationBar, getTextChild(config_node, "descr"));
#endif

	dev = d;
	loadScenarios(config_node, d);
}

void AdvancedSplitPage::loadScenarios(const QDomNode &config_node, AdvancedAirConditioningDevice *d)
{
	int id = getTextChild(config_node, "id").toInt();
	CustomScenario *bann = new CustomScenario(d);
#ifdef CONFIG_BTOUCH
	QDomNode params = getChildWithName(config_node, "par");
#else
	QDomNode params = config_node;
#endif
	SplitSettings *split = new SplitSettings(QDomNode(), params);
	connect(split, SIGNAL(splitSettingsChanged(const AirConditionerStatus &)), bann,
		SLOT(splitValuesChanged(const AirConditionerStatus &)));
	page_content->appendBanner(bann);

	foreach (const QDomNode &scenario, getChildren(config_node, "cmd"))
	{
		int item_id = getTextChild(scenario, "itemID").toInt();
		AdvancedSplitScenario *b = new AdvancedSplitScenario(getTextChild(scenario, "descr"), item_id, scenario.nodeName(), d);
		SplitSettings *sp = new SplitSettings(scenario, params);
		b->setCurrentValues(sp->getCurrentStatus());
		b->connectRightButton(sp);
		b->setId(id);
		connect(sp, SIGNAL(splitSettingsChanged(const AirConditionerStatus &)), b, SLOT(splitValuesChanged(const AirConditionerStatus &)));
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		page_content->appendBanner(b);
	}

	// skip showing scenario page (_this_ page) if we have only the custom button
	if (page_content->bannerCount() == 1)
	{
		single_page = split;
		connect(single_page, SIGNAL(Closed()), SIGNAL(Closed()));
	}
	else
	{
		bann->connectRightButton(split);
		connect(bann, SIGNAL(pageClosed()), SLOT(showPage()));
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

void AdvancedSplitPage::setDeviceOff()
{
	dev->turnOff();
}

void AdvancedSplitPage::showPage()
{
	if (single_page)
		single_page->showPage();
	else
		BannerPage::showPage();
}



SplitSettings::SplitSettings(const QDomNode &values_node, const QDomNode &config_node)
{
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("ok"));
	nav_bar->displayScrollButtons(false);
	buildPage(new BannerContent, nav_bar, getTextChild(config_node, "descr"));
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(acceptChanges()));
	connect(nav_bar, SIGNAL(backClick()), SLOT(resetChanges()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(Closed()));

	// init values, temperature is always present so it will be initialized always
	if (!values_node.isNull())
	{
		current_mode = getTextChild(values_node, "mode").toInt();
		Q_ASSERT_X(current_mode != -1, "SplitSettings::readModeConfig", "Mode cannot be disabled");
		current_temp = getTextChild(values_node, "setpoint").toInt();
	}
	else
	{
		current_mode = AdvancedAirConditioningDevice::MODE_OFF;
		current_temp = 200;
	}

	QDomNode mode_node = getChildWithName(config_node, "mode");
	readModeConfig(mode_node, current_mode);

	QDomNode temp_node = getChildWithName(config_node, "setpoint");
	readTempConfig(temp_node, current_temp);
	connect(mode, SIGNAL(modeChanged(int)), temperature, SLOT(currentModeChanged(int)));

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

void SplitSettings::readModeConfig(const QDomNode &mode_node, int init_mode)
{
	QList <int> modes;
	foreach (const QDomNode &val, getChildren(mode_node, "val"))
		modes.append(val.toElement().text().toInt());

	mode = new SplitMode(modes, init_mode);
	page_content->appendBanner(mode);
}

void SplitSettings::readTempConfig(const QDomNode &temp_node, int init_temp)
{
	int min = getTextChild(temp_node, "min").toInt();
	int max = getTextChild(temp_node, "max").toInt();
	int step = getTextChild(temp_node, "step").toInt();

	temperature = new SplitTemperature(init_temp, max, min, step, current_mode);
	page_content->appendBanner(temperature);
}

void SplitSettings::readSpeedConfig(const QDomNode &speed_node, const QDomNode &values)
{
#ifdef CONFIG_BTOUCH
	if (getTextChild(speed_node, "val1").toInt() != -1)
#else
	if (getTextChild(speed_node, "val").toInt() != -1)
#endif
	{
		QList <int> speeds;
		foreach (const QDomNode &val, getChildren(speed_node, "val"))
			speeds.append(val.toElement().text().toInt());

		int current_speed = 0;
		if (!values.isNull())
			current_speed = getTextChild(values, "speed").toInt();
		speed = new SplitSpeed(speeds, current_speed);
		page_content->appendBanner(speed);
	}
	else
		speed = 0;
}

void SplitSettings::readSwingConfig(const QDomNode &swing_node, const QDomNode &values)
{
#ifdef CONFIG_BTOUCH
	if (getTextChild(swing_node, "val1").toInt() != -1)
#else
	if (getTextChild(swing_node, "val").toInt() != -1)
#endif
	{
		bool swing_on = false;
		// read values from conf if present
		if (!values.isNull())
			swing_on = getTextChild(values, "fan_swing").toInt();
		swing = new SplitSwing(tr("SWING"), swing_on);
		page_content->appendBanner(swing);
	}
	else
		swing = 0;
}

void SplitSettings::readBannerValues()
{
	current_mode = mode->currentState();
	current_temp = temperature->temperature();
	current_fan_speed = speed ? speed->currentState() : AdvancedAirConditioningDevice::VEL_INVALID;
	// silent a warning here
	current_swing = swing ? (int) swing->swing() : AdvancedAirConditioningDevice::SWING_INVALID;
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

void SplitSettings::resetChanges()
{
	mode->setCurrentState(current_mode);
	// Beware: this call must stay in the same method of mode->setCurrentState()
	// Reason: on user cancel, we call setTemperature(), which updates the displayed temp. However, displayed
	// temperature also depends on the current mode, which is also reset on user cancel. Since we don't know
	// which mode we will land on, we rely on mode::modeChanged() signal to set the correct mode on reset.
	// If the methods setCurrentState() and setTemperature() are called in the same function, we are guaranteed
	// that the signal will be processed some time after the end of the function, leading to correct results.
	temperature->setTemperature(current_temp);
	if (speed)
		speed->setCurrentState(current_fan_speed);
	if (swing)
	{
		Q_ASSERT_X(current_swing == 0 || current_swing == 1, "SplitSettings::resetChanges",
			"Using a value that is not bool.");
		// this function takes a bool: watch out when changing the values that swing can take
		swing->setSwingOn(static_cast<bool>(current_swing));
	}
}


GeneralSplitPage::GeneralSplitPage(const QDomNode &config_node)
{
	buildPage(getTextChild(config_node, "descr"));
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
	buildPage(getTextChild(config_node, "descr"));
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
