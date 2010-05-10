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

#include <QVBoxLayout>
#include <QLabel>


enum
{
	STOP_AND_GO = 6101,
	STOP_AND_GO_PLUS = 6102,
	STOP_AND_GO_BTEST = 6103,
};

enum
{
	STATUS_CLOSED = 0,
	STATUS_OPENED,
	STATUS_LOCKED,
	STATUS_FAIL,
	STATUS_GROUND_FAIL,
	STATUS_VMAX
};


namespace
{
	void addCommandButton(QBoxLayout *layout, const QString &on_image, const QString &off_image, const QString &descr, QObject *obj, const char *slot)
	{
		QVBoxLayout *button_layout = new QVBoxLayout;
		StateButton *button = new StateButton;
		button->setOnImage(bt_global::skin->getImage(on_image));
		button->setOffImage(bt_global::skin->getImage(off_image));
		button->setCheckable(true);
		QObject::connect(button, SIGNAL(clicked(bool)), obj, slot);
		button_layout->addWidget(button, 0, Qt::AlignHCenter);

		QLabel *label = new QLabel(descr);
		label->setAlignment(Qt::AlignHCenter);
		label->setFont(bt_global::font->get(FontManager::BANNERDESCRIPTION));
		button_layout->addWidget(label, Qt::AlignHCenter);

		layout->addLayout(button_layout);
	}
}


BannStopAndGo::BannStopAndGo(StopAndGoDevice *dev, const QString &left, const QString &right, QWidget *parent) :
	Bann2Buttons(parent)
{
	status_icons[STATUS_CLOSED] = bt_global::skin->getImage("status_closed");
	status_icons[STATUS_OPENED] = bt_global::skin->getImage("status_opened");
	status_icons[STATUS_LOCKED] = bt_global::skin->getImage("status_locked");
	status_icons[STATUS_FAIL] = bt_global::skin->getImage("status_cc");
	status_icons[STATUS_GROUND_FAIL] = bt_global::skin->getImage("status_ground_fail");
	status_icons[STATUS_VMAX] = bt_global::skin->getImage("status_vmax");

	initBanner(left, status_icons[STATUS_CLOSED], right, "");

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(statusChanged(DeviceValues)));
}

void BannStopAndGo::statusChanged(const DeviceValues &values_list)
{
	QString icon;

	if (!values_list[StopAndGoDevice::DIM_OPENED].toBool())
		icon = status_icons[STATUS_CLOSED];
	else
	{
		if (values_list[StopAndGoDevice::DIM_LOCKED].toBool())
			icon = status_icons[STATUS_LOCKED];
		else if (values_list[StopAndGoDevice::DIM_OPENED_LE_N].toBool())
			icon = status_icons[STATUS_FAIL];
		else if (values_list[StopAndGoDevice::DIM_OPENED_GROUND].toBool())
			icon = status_icons[STATUS_GROUND_FAIL];
		else if (values_list[StopAndGoDevice::DIM_OPENED_VMAX].toBool())
			icon = status_icons[STATUS_VMAX];
		else
			icon = status_icons[STATUS_OPENED];
	}
	setBackgroundImage(icon);
}


StopAndGoMenu::StopAndGoMenu(const QDomNode &conf_node)
{
#ifdef LAYOUT_BTOUCH
	buildPage();
#else
	buildPage(getTextChild(conf_node, "descr"));
#endif

	SkinContext context(getTextChild(conf_node, "cid").toInt());
	QString forward_icon = bt_global::skin->getImage("forward");

	QList<QDomNode> items = getChildren(conf_node, "item");
	foreach (const QDomNode &item, items)
	{
		int id = getTextChild(item, "id").toInt();
		QString descr = getTextChild(item, "descr");
		QString where = getTextChild(item, "where");
		BannStopAndGo *banner = 0;
		Page *p = 0;

		switch (id)
		{
		case STOP_AND_GO:
			{
				StopAndGoDevice *dev = bt_global::add_device_to_cache(new StopAndGoDevice(where));
				banner = new BannStopAndGo(dev, "", forward_icon);
				p = new StopAndGoPage(descr, dev);
			}
			break;
		case STOP_AND_GO_PLUS:
			{
				StopAndGoPlusDevice *dev = bt_global::add_device_to_cache(new StopAndGoPlusDevice(where));
				banner = new BannStopAndGo(dev, "", forward_icon);
				p = new StopAndGoPlusPage(descr, dev);
			}
			break;
		case STOP_AND_GO_BTEST:
			{
				StopAndGoBTestDevice *dev = bt_global::add_device_to_cache(new StopAndGoBTestDevice(where));
				banner = new BannStopAndGo(dev, "", forward_icon);
				p = new StopAndGoBTestPage(descr, dev);
			}
			break;
		default:
			Q_ASSERT_X(false, "StopAndGoMenu::StopAndGoMenu", "Unknown id");
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

void StopAndGoMenu::showPage()
{
	if (next_page)
		next_page->showPage();
	else
		BannerPage::showPage();
}


StopAndGoPage::StopAndGoPage(const QString &title, StopAndGoDevice *device) :
	Page(), dev(device)
{
	QWidget *content = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout;

	BannStopAndGo *status_banner = new BannStopAndGo(dev, "", "");
	layout->addWidget(status_banner, 0, Qt::AlignHCenter);
	layout->addSpacing(30);

	// TODO: Set the state of the command button depending on the device.
	addCommandButton(layout, "autoreset_enabled", "autoreset_disabled", tr("Enable"), this, SLOT(turnOnOff(bool)));

	layout->addStretch();

	content->setLayout(layout);

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));

	buildPage(content, nav_bar, title, TITLE_HEIGHT);
}

void StopAndGoPage::turnOnOff(bool checked)
{
	if (checked)
		dev->sendAutoResetActivation();
	else
		dev->sendAutoResetDisactivation();
}


StopAndGoPlusPage::StopAndGoPlusPage(const QString &title, StopAndGoPlusDevice *device) :
	Page(), dev(device)
{
}


StopAndGoBTestPage::StopAndGoBTestPage(const QString &title, StopAndGoBTestDevice *device) :
	Page(), dev(device)
{
};
