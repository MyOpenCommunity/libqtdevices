/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


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
#include "icondispatcher.h"

#include <QPainter>
#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QGridLayout>

#define SPLIT_ERROR_PAGE_TIMEOUT 5000

typedef AdvancedAirConditioningDevice::Mode Mode;
typedef AdvancedAirConditioningDevice::Swing Swing;
typedef AdvancedAirConditioningDevice::Velocity Velocity;

AirConditionerStatus AirConditioningAdvanced::parseSettings(const QDomNode &values_node)
{
	AirConditionerStatus st;
	// init values, temperature is always present so it will be initialized always
	if (!values_node.isNull())
	{
		int tmp = getTextChild(values_node, "mode").toInt();
		Q_ASSERT_X(st.mode != -1, "AirConditioningAdvanced::parseSettings", "Mode cannot be disabled");
		st.mode = static_cast<AdvancedAirConditioningDevice::Mode>(tmp);
		st.temp = getTextChild(values_node, "setpoint").toInt();
	}
	else
	{
		st.mode = AdvancedAirConditioningDevice::MODE_OFF;
		st.temp = 200;
	}

	int speed = 0;
	if (!values_node.isNull())
		speed = getTextChild(values_node, "speed").toInt();
	st.vel = static_cast<AdvancedAirConditioningDevice::Velocity>(speed);

	int swing = AdvancedAirConditioningDevice::SWING_OFF;
	// read values from conf if present
	if (!values_node.isNull())
		swing = getTextChild(values_node, "fan_swing").toInt();
	st.swing = static_cast<AdvancedAirConditioningDevice::Swing>(swing);
	return st;
}


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
	SkinContext context(getTextChild(config_node, "cid").toInt());
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
}

banner *AirConditioning::getBanner(const QDomNode &item_node)
{
	int id = getTextChild(item_node, "id").toInt();
	SkinContext context(getTextChild(item_node, "cid").toInt());
	QString descr = getTextChild(item_node, "descr");
#ifdef CONFIG_BTOUCH
	bool has_commands = !getChildren(item_node, "cmd").isEmpty();
	QDomNode linked_page = item_node;
#else
	bool has_commands = !(getTextChild(item_node, "lnk_pageID").isEmpty());
	QDomNode linked_page = getPageNodeFromChildNode(item_node, "lnk_pageID");
#endif

	banner *b = 0;
	switch (id)
	{
	case AIR_SPLIT:
	{
		QString where = getTextChild(item_node, "where");
		int oid = getTextChild(item_node, "openserver_id").toInt();
		QString off_cmd = getElement(item_node, "off/command").text();
		AirConditioningDevice *dev = bt_global::add_device_to_cache(new AirConditioningDevice(where, oid));
		dev->setOffCommand(off_cmd);

		SingleSplit *bann = new SingleSplit(descr, has_commands, dev, createProbeDevice(item_node));
		b = bann;
		if (has_commands)
			bann->connectRightButton(new SplitPage(item_node, dev));
		device_container.append(dev);
		break;
	}
	case AIR_SPLIT_ADV:
	{
		QString where = getTextChild(item_node, "where");
		int oid = getTextChild(item_node, "openserver_id").toInt();
		AdvancedAirConditioningDevice *dev = bt_global::add_device_to_cache(new AdvancedAirConditioningDevice(where, oid));

		AdvancedSplitPage *p = new AdvancedSplitPage(item_node, dev);
		SingleSplit *bann = new AdvancedSingleSplit(descr, p, dev, createProbeDevice(item_node));
		b = bann;
		bann->connectRightButton(p);
		device_container.append(dev);
		break;
	}
	case AIR_GENERAL:
		b = createGeneralBanner(!has_commands ? 0 : new GeneralSplitPage(linked_page), descr);
		break;
	case AIR_GENERAL_ADV:
		b = createGeneralBanner(!has_commands ? 0 : new AdvancedGeneralSplitPage(linked_page), descr);
		break;
	}

	if (b)
		b->setId(id);

	return b;
}

NonControlledProbeDevice *AirConditioning::createProbeDevice(const QDomNode &item_node)
{
	NonControlledProbeDevice *dev = 0;
	QString where_probe = getTextChild(item_node, "where_probe");
	if (where_probe != "000")
	{
		dev = new NonControlledProbeDevice(where_probe, NonControlledProbeDevice::INTERNAL,
			getTextChild(item_node, "openserver_id_probe").toInt());
		dev = bt_global::add_device_to_cache(dev);
	}
	return dev;
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
	int off_button = getElement(config_node, "off/list").text().toInt();
#else
	int off_button = getElement(config_node, "off/presence").text().toInt();
#endif

#ifdef LAYOUT_BTOUCH
	NavigationBar *nav_bar;

	if (off_button) // show the off button
	{
		nav_bar = new NavigationBar(bt_global::skin->getImage("off"));
		connect(this, SIGNAL(forwardClick()), SLOT(setDeviceOff()));
	}
	else
		nav_bar = new NavigationBar;

	buildPage(new BannerContent, nav_bar, getTextChild(config_node, "descr"));
#else
	buildPage(getTextChild(config_node, "descr"));

	if (off_button) // show the off button
	{
		Bann2Buttons *b = new Bann2Buttons;
		b->initBanner(bt_global::skin->getImage("off"), QString(), tr("Off"));
		page_content->appendBanner(b);

		connect(b, SIGNAL(leftClicked()), SLOT(setDeviceOff()));
	}
#endif

	loadScenarios(config_node);
}

void SplitPage::loadScenarios(const QDomNode &config_node)
{
#ifdef CONFIG_BTOUCH
	foreach (const QDomNode &scenario, getChildren(config_node, "cmd"))
#else
	foreach (const QDomNode &scenario, getChildren(getPageNodeFromChildNode(config_node, "lnk_pageID"), "item"))
#endif
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
	int off_button = getElement(config_node, "off/list").text().toInt();
#else
	int off_button = getElement(config_node, "off/presence").text().toInt();
#endif

#ifdef LAYOUT_BTOUCH
	NavigationBar *nav_bar;

	if (off_button) // show the off button
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
	buildPage(banners, new NavigationBar, getTextChild(config_node, "descr"));

	if (off_button) // show the off button
	{
		Bann2Buttons *b = new Bann2Buttons;
		b->initBanner(bt_global::skin->getImage("off"), QString(), tr("Off"));
		page_content->appendBanner(b);

		connect(b, SIGNAL(leftClicked()), SLOT(setDeviceOff()));
	}
#endif

	dev = d;
	loadScenarios(config_node, d);

	error_page = new SplitErrorPage(bt_global::skin->getImage("setstate_error"));
	connect(d, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
}

void AdvancedSplitPage::valueReceived(const DeviceValues &values_list)
{
	if (!values_list.contains(AdvancedAirConditioningDevice::DIM_SETSTATUS_ERROR))
		return;

	error_page->showPage();
}

void AdvancedSplitPage::loadScenarios(const QDomNode &config_node, AdvancedAirConditioningDevice *d)
{
	int id = getTextChild(config_node, "id").toInt();
	CustomScenario *bann = new CustomScenario(d);
	QDomNode params = getChildWithName(config_node, "par");
	SplitSettings *split = new SplitSettings(QDomNode(), params);
	connect(split, SIGNAL(splitSettingsChanged(const AirConditionerStatus &)), bann,
		SLOT(splitValuesChanged(const AirConditionerStatus &)));
	page_content->appendBanner(bann);

#ifdef CONFIG_BTOUCH
	foreach (const QDomNode &scenario, getChildren(config_node, "cmd"))
#else
	foreach (const QDomNode &scenario, getChildren(getPageNodeFromChildNode(config_node, "lnk_pageID"), "item"))
#endif
	{
		AdvancedSplitScenario *b = new AdvancedSplitScenario(AirConditioningAdvanced::parseSettings(scenario), getTextChild(scenario, "descr"), d);
		b->setId(id);
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
#ifdef LAYOUT_BTOUCH
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("ok"));
	nav_bar->displayScrollButtons(false);
	buildPage(new BannerContent, nav_bar, getTextChild(config_node, "descr"));

	connect(nav_bar, SIGNAL(forwardClick()), SLOT(acceptChanges()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(Closed()));
#else
	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	buildPage(new QWidget, nav_bar, getTextChild(config_node, "descr"), SMALL_TITLE_HEIGHT);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
#endif
	connect(nav_bar, SIGNAL(backClick()), SLOT(resetChanges()));

	AirConditionerStatus st = AirConditioningAdvanced::parseSettings(values_node);
	current_mode = st.mode;
	current_temp = st.temp;
	current_fan_speed = st.vel;
	current_swing = st.swing;

	QDomNode mode_node = getChildWithName(config_node, "mode");
	readModeConfig(mode_node, current_mode);

	QDomNode temp_node = getChildWithName(config_node, "setpoint");
	readTempConfig(temp_node, current_temp);
	connect(mode, SIGNAL(modeChanged(int)), temperature, SLOT(currentModeChanged(int)));

	QDomNode speed_node = getChildWithName(config_node, "speed");
	readSpeedConfig(speed_node);

	QDomNode swing_node = getChildWithName(config_node, "fan_swing");
	readSwingConfig(swing_node);

#ifdef LAYOUT_BTOUCH
	page_content->appendBanner(mode);
	page_content->appendBanner(temperature);
	if (speed)
		page_content->appendBanner(speed);
	if (swing)
		page_content->appendBanner(swing);
#else
	BtButton *ok = new BtButton;
	ok->setImage(bt_global::skin->getImage("ok"));

	connect(ok, SIGNAL(clicked()), SLOT(acceptChanges()));
	connect(ok, SIGNAL(clicked()), SIGNAL(Closed()));

	QGridLayout *l = new QGridLayout(page_content);
	l->setContentsMargins(0, 0, 25, 35);
	l->setSpacing(10);

	for (int i = 0; i < 8; ++i)
		l->setColumnStretch(i, 1);
	l->setRowStretch(4, 1);

	l->addWidget(temperature, 0, 2, 1, 4);
	l->addWidget(mode, 1, 1, 1, 4);
	if (speed)
		l->addWidget(speed, 2, 1, 1, 4);
	if (swing)
		l->addWidget(swing, 3, 1, 2, 4, Qt::AlignTop);
	l->addWidget(ok, 4, 7, 2, 1, Qt::AlignRight|Qt::AlignBottom);
#endif
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
#ifdef CONFIG_BTOUCH
	foreach (const QDomNode &val, getChildren(mode_node, "val"))
		modes.append(val.toElement().text().toInt());
#else
	static const char *tags[] = { "off", "heating", "cooling", "fan", "dry", "automatic" };

	for (int i = 0; i < 6; ++i)
		modes.append(getTextChild(mode_node, tags[i]).toInt());
#endif

	mode = new SplitMode(modes, init_mode);
}

void SplitSettings::readTempConfig(const QDomNode &temp_node, int init_temp)
{
	int min = getTextChild(temp_node, "min").toInt();
	int max = getTextChild(temp_node, "max").toInt();
	int step = getTextChild(temp_node, "step").toInt();

	temperature = new SplitTemperature(init_temp, max, min, step, current_mode);
}

void SplitSettings::readSpeedConfig(const QDomNode &speed_node)
{

#ifdef CONFIG_BTOUCH
	if (getTextChild(speed_node, "val1").toInt() != -1)
#else
	if (getTextChild(speed_node, "presence").toInt())
#endif
	{
		QList <int> speeds;
#ifdef CONFIG_BTOUCH
		foreach (const QDomNode &val, getChildren(speed_node, "val"))
			speeds.append(val.toElement().text().toInt());
#else
		static const char *tags[] = { "automatic", "low", "medium", "high", "silent" };

		for (int i = 0; i < 5; ++i)
			speeds.append(getTextChild(speed_node, tags[i]).toInt());
#endif

		speed = new SplitSpeed(speeds, current_fan_speed);
	}
	else
		speed = 0;
}

void SplitSettings::readSwingConfig(const QDomNode &swing_node)
{
#ifdef CONFIG_BTOUCH
	if (getTextChild(swing_node, "val1").toInt() != -1)
#else
	if (getTextChild(swing_node, "presence").toInt())
#endif
	{
		Q_ASSERT_X(current_swing == 0 || current_swing == 1, "SplitSettings::readSwingConfig",
			"Using a value that is not bool.");
		swing = new SplitSwing(tr("SWING"), current_swing);
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
#ifdef CONFIG_BTOUCH
	foreach (const QDomNode &scenario, getChildren(config_node, "cmd"))
#else
	foreach (const QDomNode &scenario, getChildren(config_node, "item"))
#endif
	{
		GeneralSplitScenario *b = new GeneralSplitScenario(getTextChild(scenario, "descr"));
		foreach (const QDomNode &split, getChildren(scenario, "split"))
		{
			int oid = getTextChild(split, "openserver_id").toInt();
			AirConditioningDevice *dev = new AirConditioningDevice(getTextChild(split, "where"), oid);
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
#ifdef CONFIG_BTOUCH
	foreach (const QDomNode &scenario, getChildren(config_node, "cmd"))
#else
	foreach (const QDomNode &scenario, getChildren(config_node, "item"))
#endif
	{
		GeneralSplitScenario *b = new GeneralSplitScenario(getTextChild(scenario, "descr"));
		foreach (const QDomNode &split, getChildren(scenario, "split"))
		{
			int oid = getTextChild(split, "openserver_id").toInt();
			AdvancedAirConditioningDevice *dev = new AdvancedAirConditioningDevice(getTextChild(split, "where"), oid);
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


SplitErrorPage::SplitErrorPage(const QString &image)
{
	previous_page = NULL;
	icon = *(bt_global::icons_cache.getIcon(image));

	timeout.setSingleShot(true);
	timeout.setInterval(SPLIT_ERROR_PAGE_TIMEOUT);
	connect(&timeout, SIGNAL(timeout()), SLOT(handleClose()));
}

void SplitErrorPage::showPage()
{
	if (previous_page)
		return;

	// WARNING: this assumes that no antintrusion alarm will go off while the
	//          page is shown; it also ignores the screen saver (should be safe,
	//          since this should happen right after a command) and transitions
	previous_page = currentPage();
	timeout.start();
	Page::showPage();
}

void SplitErrorPage::handleClose()
{
	if (!previous_page)
		return;
	timeout.stop();
	previous_page->showPage();
	previous_page = NULL;
}

void SplitErrorPage::mouseReleaseEvent(QMouseEvent *e)
{
	Page::mouseReleaseEvent(e);

	handleClose();
}

void SplitErrorPage::paintEvent(QPaintEvent *e)
{
	Page::paintEvent(e);

	QPainter p(this);
	p.drawPixmap(0, 0, icon);
}
