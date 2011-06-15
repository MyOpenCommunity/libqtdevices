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


#include "stopngo.h"
#include "fontmanager.h" // bt_global::font
#include "icondispatcher.h" // bt_global::icons_cache
#include "devices_cache.h" // bt_global::devices_cache
#include "stopandgo_device.h"
#include "skinmanager.h"
#include "xml_functions.h" // getTextChild
#include "state_button.h"
#include "navigation_bar.h"
#include "energy_management.h" // EnergyManagement::isBuilt
#include "main.h" // ENERGY_MANAGEMENT, SUPERVISION

#include <QVBoxLayout>
#include <QLabel>

#include <QDebug>

enum
{
	STOP_AND_GO = 6101,
	STOP_AND_GO_PLUS = 6102,
	STOP_AND_GO_BTEST = 6103,
};



namespace
{
	QVBoxLayout* getCommandButton(StateButton *button, const QString &on_image, const QString &off_image, QLabel *label, QObject *obj, const char *slot)
	{
		QVBoxLayout *button_layout = new QVBoxLayout;
		button_layout->setSpacing(5);
		button->setOnImage(bt_global::skin->getImage(on_image));
		button->setOffImage(bt_global::skin->getImage(off_image));
		QObject::connect(button, SIGNAL(clicked()), obj, slot);
		button_layout->addWidget(button, 0, Qt::AlignHCenter);

		label->setAlignment(Qt::AlignHCenter);
		label->setFont(bt_global::font->get(FontManager::BANNERDESCRIPTION));
		button_layout->addWidget(label, Qt::AlignHCenter);

		return button_layout;
	}

	QVBoxLayout* getCommandButton(StateButton *button, const QString &on_image, const QString &off_image, const QString &descr, QObject *obj, const char *slot)
	{
		return getCommandButton(button, on_image, off_image, new QLabel(descr), obj, slot);
	}
}


BannStopAndGo::BannStopAndGo(StopAndGoDevice *dev, const QString &left, const QString &right, const QString &descr, QWidget *parent) :
	Bann2Buttons(parent)
{
	using namespace StopAndGo;
	status_icons[STATUS_CLOSED] = bt_global::skin->getImage("status_closed");
	status_icons[STATUS_OPENED] = bt_global::skin->getImage("status_opened");
	status_icons[STATUS_LOCKED] = bt_global::skin->getImage("status_locked");
	status_icons[STATUS_FAIL] = bt_global::skin->getImage("status_cc");
	status_icons[STATUS_GROUND_FAIL] = bt_global::skin->getImage("status_ground_fail");
	status_icons[STATUS_VMAX] = bt_global::skin->getImage("status_vmax");

	initBanner(bt_global::skin->getImage(left), status_icons[STATUS_CLOSED], bt_global::skin->getImage(right), descr);

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
}

void BannStopAndGo::showButtons(bool show)
{
	left_button->setVisible(show);
	right_button->setVisible(show);
}

void BannStopAndGo::valueReceived(const DeviceValues &values_list)
{
	using namespace StopAndGo;
	StopAndGo::Status st;
	QString icon;

	if (!values_list[StopAndGoDevice::DIM_OPENED].toBool())
		st = STATUS_CLOSED;
	else
	{
		if (values_list[StopAndGoDevice::DIM_LOCKED].toBool())
			st = STATUS_LOCKED;
		else if (values_list[StopAndGoDevice::DIM_OPENED_LE_N].toBool())
			st = STATUS_FAIL;
		else if (values_list[StopAndGoDevice::DIM_OPENED_GROUND].toBool())
			st = STATUS_GROUND_FAIL;
		else if (values_list[StopAndGoDevice::DIM_OPENED_VMAX].toBool())
			st = STATUS_VMAX;
		else
			st = STATUS_OPENED;
	}

	emit statusChanged(st);
	icon = status_icons[st];
	setBackgroundImage(icon);
}


StopAndGoMenu::StopAndGoMenu(const QDomNode &config_node)
{
#ifdef LAYOUT_TS_3_5
	buildPage();
#else
	buildPage(getTextChild(config_node, "descr"));
#endif

	SkinContext context(getTextChild(config_node, "cid").toInt());

	loadItems(config_node);
}

int StopAndGoMenu::sectionId() const
{
	return EnergyManagement::isBuilt() ? ENERGY_MANAGEMENT : SUPERVISION;
}

void StopAndGoMenu::showPage()
{
	if (next_page)
		next_page->showPage();
	else
		BannerPage::showPage();
}

void StopAndGoMenu::loadItems(const QDomNode &config_node)
{
	QList<QDomNode> items = getChildren(config_node, "item");
	foreach (const QDomNode &item, items)
	{
		int id = getTextChild(item, "id").toInt();
		// BannStopAndGo is the same for the StopAndGoMenu and the related stop & go
		// pages. We have to differentiate the icons for the various kind of
		// stop&go, but only in the StopAndGoMenu. So we add the context before
		// creating the banners and remove it before creating the pages.
		bt_global::skin->addToContext(getTextChild(item, "cid").toInt());
		QString descr = getTextChild(item, "descr");
		QString where = getTextChild(item, "where");
		BannStopAndGo *banner = 0;
		Page *p = 0;

		switch (id)
		{
		case STOP_AND_GO:
			{
				StopAndGoDevice *dev = bt_global::add_device_to_cache(new StopAndGoDevice(where));
				banner = new BannStopAndGo(dev, "", "forward", descr);
				bt_global::skin->removeFromContext();
				p = new StopAndGoPage(descr, dev);
			}
			break;
		case STOP_AND_GO_PLUS:
			{
				StopAndGoPlusDevice *dev = bt_global::add_device_to_cache(new StopAndGoPlusDevice(where));
				banner = new BannStopAndGo(dev, "", "forward", descr);
				bt_global::skin->removeFromContext();
				p = new StopAndGoPlusPage(descr, dev);
			}
			break;
		case STOP_AND_GO_BTEST:
			{
				StopAndGoBTestDevice *dev = bt_global::add_device_to_cache(new StopAndGoBTestDevice(where));
				banner = new BannStopAndGo(dev, "", "forward", descr);
				bt_global::skin->removeFromContext();
				p = new StopAndGoBTestPage(descr, dev);
			}
			break;
		default:
			Q_ASSERT_X(false, "StopAndGoMenu::StopAndGoMenu", qPrintable(QString("Unknown id %1").arg(id)));
		}

		banner->connectRightButton(p);
		connect(banner, SIGNAL(pageClosed()), SLOT(showPage()));

		page_content->appendBanner(banner);

		next_page = p;
	}

	// skip list if there's only one item
	if (items.size() > 1)
		next_page = NULL;
	else
		connect(next_page, SIGNAL(Closed()), SIGNAL(Closed()));
}


StopAndGoPage::StopAndGoPage(const QString &title, StopAndGoDevice *device)
{
	dev = device;
	autoreset_button = new StateButton;
	QWidget *content = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout(content);

#ifdef LAYOUT_TS_3_5
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(10);
	QLabel *l = new QLabel(title);
	l->setFont(bt_global::font->get(FontManager::TEXT));
	layout->addWidget(l, 0, Qt::AlignHCenter);
#endif

	BannStopAndGo *status_banner = new BannStopAndGo(dev, "", "");
	layout->addWidget(status_banner, 0, Qt::AlignHCenter);
	layout->addSpacing(30);

	layout->addLayout(getCommandButton(autoreset_button, "autoreset_enabled",
									   "autoreset_disabled", tr("Enable"),
									   this, SLOT(switchAutoReset())));

	layout->addStretch();

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));

	buildPage(content, nav_bar, title, TITLE_HEIGHT);

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
}

int StopAndGoPage::sectionId() const
{
	return EnergyManagement::isBuilt() ? ENERGY_MANAGEMENT : SUPERVISION;
}

void StopAndGoPage::valueReceived(const DeviceValues &values_list)
{
	if (!values_list.contains(StopAndGoDevice::DIM_AUTORESET_DISACTIVE))
		return;

	autoreset_button->setStatus(!values_list[StopAndGoDevice::DIM_AUTORESET_DISACTIVE].toBool());
}

void StopAndGoPage::switchAutoReset()
{
	if (autoreset_button->getStatus() == StateButton::OFF)
		dev->sendAutoResetActivation();
	else
		dev->sendAutoResetDisactivation();
}


StopAndGoPlusPage::StopAndGoPlusPage(const QString &title, StopAndGoPlusDevice *device)
{
	dev = device;
	autoreset_button = new StateButton;
	tracking_button = new StateButton;
	tracking_label = new QLabel(tr("Test"));
	QWidget *content = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout(content);

#ifdef LAYOUT_TS_3_5
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(10);

	QLabel *l = new QLabel(title);
	l->setFont(bt_global::font->get(FontManager::TEXT));
	layout->addWidget(l, 0, Qt::AlignHCenter);
#else
	layout->setSpacing(40);
#endif

	status_banner = new BannStopAndGo(dev, "close", "open");
	connect(status_banner, SIGNAL(statusChanged(StopAndGo::Status)), SLOT(statusChanged(StopAndGo::Status)));
	connect(status_banner, SIGNAL(leftClicked()), dev, SLOT(sendClose()));
	connect(status_banner, SIGNAL(rightClicked()), dev, SLOT(sendOpen()));
	layout->addWidget(status_banner, 0, Qt::AlignHCenter);

	QHBoxLayout *buttons_layout = new QHBoxLayout;
#ifdef LAYOUT_TS_10
	buttons_layout->setContentsMargins(124, 0, 126, 0);
#else
	buttons_layout->setContentsMargins(30, 18, 30, 18);
#endif
	buttons_layout->addLayout(getCommandButton(autoreset_button, "autoreset_enabled",
											   "autoreset_disabled", tr("Enable"),
											   this, SLOT(switchAutoReset())));
	buttons_layout->addStretch();
	buttons_layout->addLayout(getCommandButton(tracking_button, "tracking_enabled",
											   "tracking_disabled", tracking_label,
											   this, SLOT(switchTracking())));
	layout->addLayout(buttons_layout);
	layout->addStretch();

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));

	buildPage(content, nav_bar, title, TITLE_HEIGHT);

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
}

void StopAndGoPlusPage::statusChanged(StopAndGo::Status st)
{
	bool show = (st == StopAndGo::STATUS_OPENED || st == StopAndGo::STATUS_CLOSED || st == StopAndGo::STATUS_VMAX);
	status_banner->showButtons(show);
}

int StopAndGoPlusPage::sectionId() const
{
	return EnergyManagement::isBuilt() ? ENERGY_MANAGEMENT : SUPERVISION;
}

void StopAndGoPlusPage::valueReceived(const DeviceValues &values_list)
{
	if (!values_list.contains(StopAndGoDevice::DIM_AUTORESET_DISACTIVE) ||
		!values_list.contains(StopAndGoDevice::DIM_TRACKING_DISACTIVE) ||
		!values_list.contains(StopAndGoDevice::DIM_OPENED_LE_N) ||
		!values_list.contains(StopAndGoDevice::DIM_OPENED_GROUND))
	{
		return;
	}

	autoreset_button->setStatus(!values_list[StopAndGoDevice::DIM_AUTORESET_DISACTIVE].toBool());
	tracking_button->setStatus(!values_list[StopAndGoDevice::DIM_TRACKING_DISACTIVE].toBool());

	// Show the autotest button only if opened for cc between n or ground fail.
	bool show_track = values_list[StopAndGoDevice::DIM_OPENED_LE_N].toBool() ||
		values_list[StopAndGoDevice::DIM_OPENED_GROUND].toBool();

	tracking_button->setVisible(show_track);
	tracking_label->setVisible(show_track);
}

void StopAndGoPlusPage::switchAutoReset()
{
	if (autoreset_button->getStatus() == StateButton::OFF)
		dev->sendAutoResetActivation();
	else
		dev->sendAutoResetDisactivation();
}

void StopAndGoPlusPage::switchTracking()
{
	if (tracking_button->getStatus() == StateButton::OFF)
		dev->sendTrackingSystemActivation();
	else
		dev->sendTrackingSystemDisactivation();
}


StopAndGoBTestPage::StopAndGoBTestPage(const QString &title, StopAndGoBTestDevice *device)
{
	dev = device;
	autoreset_button = new StateButton;
	autotest_button = new StateButton;
	autotest_label = new QLabel(tr("Self-test"));
	autotest_banner = new BannLCDRange;
	QWidget *content = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout(content);

#ifdef LAYOUT_TS_10
	layout->setContentsMargins(5, 5, 5, 17);
#else
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(10);

	QLabel *l = new QLabel(title);
	l->setFont(bt_global::font->get(FontManager::TEXT));
	layout->addWidget(l, 0, Qt::AlignHCenter);
#endif

	BannStopAndGo *status_banner = new BannStopAndGo(dev, "", "");
	layout->addWidget(status_banner, 0, Qt::AlignHCenter);
	layout->addSpacing(30);

	QHBoxLayout *buttons_layout = new QHBoxLayout;
#ifdef LAYOUT_TS_10
	buttons_layout->setContentsMargins(156, 0, 151, 0);
#else
	buttons_layout->setContentsMargins(0, 0, 0, 0);
#endif
	buttons_layout->addLayout(getCommandButton(autoreset_button, "autoreset_enabled",
											   "autoreset_disabled", tr("Enable"),
											   this, SLOT(switchAutoReset())));
	buttons_layout->addStretch();
	buttons_layout->addLayout(getCommandButton(autotest_button, "autocheck_enabled",
											   "autocheck_disabled", autotest_label,
											   this, SLOT(switchAutoTest())));
	layout->addLayout(buttons_layout);

	layout->addStretch();

	autotest_banner->setRange(1, 180);
	autotest_banner->setNumDigits(3);
	autotest_banner->setValue(180);
	connect(autotest_banner, SIGNAL(valueChanged(int)), dev, SLOT(sendSelftestFreq(int)));
	layout->addWidget(autotest_banner, 0, Qt::AlignHCenter);

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));

	buildPage(content, nav_bar, title, TITLE_HEIGHT);

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
};

int StopAndGoBTestPage::sectionId() const
{
	return EnergyManagement::isBuilt() ? ENERGY_MANAGEMENT : SUPERVISION;
}

void StopAndGoBTestPage::valueReceived(const DeviceValues &values_list)
{
	// Show the autotest button only if closed.
	if (values_list.contains(StopAndGoDevice::DIM_OPENED))
	{
		bool show = !values_list[StopAndGoDevice::DIM_OPENED].toBool();
		autotest_button->setVisible(show);
		autotest_label->setVisible(show);
	}

	if (values_list.contains(StopAndGoDevice::DIM_AUTORESET_DISACTIVE) &&
		values_list.contains(StopAndGoDevice::DIM_AUTOTEST_DISACTIVE))
	{
		autoreset_button->setStatus(!values_list[StopAndGoDevice::DIM_AUTORESET_DISACTIVE].toBool());
		autotest_button->setStatus(!values_list[StopAndGoDevice::DIM_AUTOTEST_DISACTIVE].toBool());
	}
	else if (values_list.contains(StopAndGoBTestDevice::DIM_AUTOTEST_FREQ))
	{
		autotest_banner->setValue(values_list[StopAndGoBTestDevice::DIM_AUTOTEST_FREQ].toInt());
	}
}

void StopAndGoBTestPage::switchAutoReset()
{
	if (autoreset_button->getStatus() == StateButton::OFF)
		dev->sendAutoResetActivation();
	else
		dev->sendAutoResetDisactivation();
}

void StopAndGoBTestPage::switchAutoTest()
{
	if (autotest_button->getStatus() == StateButton::OFF)
		dev->sendDiffSelftestActivation();
	else
		dev->sendDiffSelftestDisactivation();
}
