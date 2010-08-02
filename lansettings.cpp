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


#include "lansettings.h"
#include "state_button.h"
#include "main.h" // (*bt_global::config)
#include "platform_device.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "fontmanager.h" // bt_global::font
#include "xml_functions.h" // getTextChild
#include "skinmanager.h" // bt_global::skin, SkinContext
#include "generic_functions.h" // setCfgValue
#include "navigation_bar.h"

#include <QDebug>
#include <QLabel>
#include <QDomNode>
#include <QBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QHash>


#define TIMEOUT_CONNECTION 5

namespace
{
	void requestNetworkInfo(PlatformDevice *dev)
	{
		dev->requestStatus();
		dev->requestIp();
		dev->requestNetmask();
		dev->requestMacAddress();
		dev->requestGateway();
		dev->requestDNS1();
		dev->requestDNS2();
	}
}


using namespace LanSettingsPrivate;

ConnectionTester::ConnectionTester(QObject *parent) : QObject(parent)
{
	urls = QStringList() << "http://www.google.it" << "http://www.bticino.it";
	manager = new QNetworkAccessManager(this);
	timeout_timer = new QTimer(this);
	timeout_timer->setSingleShot(true);
	timeout_timer->setInterval(TIMEOUT_CONNECTION * 1000);
	connect(timeout_timer, SIGNAL(timeout()), SLOT(downloadFailed()));
}

void ConnectionTester::test()
{
	current_url = 0;
	startTest();
}

void ConnectionTester::startTest()
{
	QString url = urls.at(current_url);
	qDebug() << "start testing the connection with" << url;
	current_reply = manager->get(QNetworkRequest(QUrl(url)));
	connect(current_reply, SIGNAL(finished()), SLOT(downloadFinished()));
	timeout_timer->start();
}

void ConnectionTester::downloadFailed()
{
	qDebug() << "connection failed for" <<  urls.at(current_url);
	current_reply->disconnect();
	current_reply->deleteLater();
	if (current_url + 1 >= urls.size())
	{
		emit testFailed();
		return;
	}
	++current_url;
	startTest();
}

void ConnectionTester::downloadFinished()
{
	timeout_timer->stop();
	if (current_reply->error() != QNetworkReply::NoError)
	{
		downloadFailed();
		return;
	}
	qDebug() << "connection established for" <<  urls.at(current_url);
	current_reply->disconnect();
	current_reply->deleteLater();
	emit testPassed();
}


Text2Column::Text2Column()
{
	main_layout = new QGridLayout(this);
	main_layout->setSpacing(0);
}

void Text2Column::setSpacing(int spacing)
{
	main_layout->setSpacing(spacing);
}

void Text2Column::addRow(QString text, Qt::Alignment align)
{
	main_layout->addWidget(new QLabel(text), main_layout->rowCount(), 0, 1, 2, align);
}

void Text2Column::addRow(QString label, QString text)
{
	int row = main_layout->rowCount();
	main_layout->addWidget(new QLabel(label), row, 0, 1, 1);
	main_layout->addWidget(new QLabel(text), row, 1, 1, 1);
}

void Text2Column::setText(int row, QString text)
{
	if (QWidget *w = main_layout->itemAtPosition(row, 1)->widget())
		static_cast<QLabel*>(w)->setText(text);
}


LanSettings::LanSettings(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	box_text = new Text2Column;
#ifdef LAYOUT_BTOUCH
	box_text->setStyleSheet("background-color:#f0f0f0; color:#000000;");
	box_text->setFrameStyle(QFrame::Panel | QFrame::Raised);
	box_text->setLineWidth(3);
	box_text->setFont(bt_global::font->get(FontManager::SMALLTEXT));
#else
	box_text->setFont(bt_global::font->get(FontManager::TEXT));
#endif
	box_text->addRow((*bt_global::config)[MODEL]);
	box_text->addRow("");
	box_text->addRow((*bt_global::config)[NAME]);
	box_text->addRow(tr("Mac"), "");
	box_text->addRow(tr("IP"), "");
	box_text->addRow(tr("Subnet mask"), "");
	box_text->addRow(tr("Gateway"), "");
	box_text->addRow(tr("DNS"), "");
	box_text->addRow("", "");
	box_text->addRow(tr("Connection Status"), "");
	tester = new ConnectionTester(this);
	connect(tester, SIGNAL(testFailed()), SLOT(connectionDown()));
	connect(tester, SIGNAL(testPassed()), SLOT(connectionUp()));

	QWidget *content = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(content);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);

	QHBoxLayout *label_layout = new QHBoxLayout;
	label_layout->setContentsMargins(5, 0, 5, 0);
	label_layout->addWidget(box_text);
	main_layout->addLayout(label_layout);

	toggle_btn = new StateButton;
	toggle_btn->setOnOff();
	toggle_btn->setOffImage(bt_global::skin->getImage("network_disable"));
	toggle_btn->setOnImage(bt_global::skin->getImage("network_enable"));
	lan_status = false; // This value must be keep in sync with the icon of toggle_btn.
	connect(toggle_btn, SIGNAL(clicked()), SLOT(toggleLan()));
	main_layout->addWidget(toggle_btn, 0, Qt::AlignHCenter);

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	buildPage(content, nav_bar, getTextChild(config_node, "descr"), TINY_TITLE_HEIGHT);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	dev = bt_global::add_device_to_cache(new PlatformDevice);
	connect(dev, SIGNAL(valueReceived(const DeviceValues&)), SLOT(valueReceived(const DeviceValues&)));

	// Set the network to the initial status
#ifdef CONFIG_BTOUCH
	saved_status = getTextChild(config_node, "value").toInt();
	item_id = 0;
#else
	saved_status = getTextChild(config_node, "enable").toInt();
	item_id = getTextChild(config_node, "itemID").toInt();
#endif
}

void LanSettings::connectionDown()
{
	box_text->setText(10, tr("Down"));
}

void LanSettings::connectionUp()
{
	box_text->setText(10, tr("Up"));
}

void LanSettings::inizializza()
{
	qDebug() << "LanSettings::inizializza()";
	dev->enableLan(saved_status);
	requestNetworkInfo(dev);
	tester->test();
}

void LanSettings::showPage()
{
	requestNetworkInfo(dev);
	tester->test();
	Page::showPage();
}

void LanSettings::toggleLan()
{
	dev->enableLan(!lan_status);
}

void LanSettings::valueReceived(const DeviceValues &values_list)
{
	const int MACADDR_ROW = 4;
	const int IP_ROW = 5;
	const int NETMASK_ROW = 6;
	const int GATEWAY_ROW = 7;
	const int DNS1_ROW = 8;
	const int DNS2_ROW = 9;

	QHash<int, int> dim_to_row;
	dim_to_row[PlatformDevice::DIM_IP] = IP_ROW;
	dim_to_row[PlatformDevice::DIM_NETMASK] = NETMASK_ROW;
	dim_to_row[PlatformDevice::DIM_MACADDR] = MACADDR_ROW;
	dim_to_row[PlatformDevice::DIM_GATEWAY] = GATEWAY_ROW;
	dim_to_row[PlatformDevice::DIM_DNS1] = DNS1_ROW;
	dim_to_row[PlatformDevice::DIM_DNS2] = DNS2_ROW;

	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		if (dim_to_row.contains(it.key()))
			box_text->setText(dim_to_row[it.key()], it.value().toString());
		else if (it.key() == PlatformDevice::DIM_STATUS)
		{
			lan_status = it.value().toBool();
			toggle_btn->setStatus(lan_status);
			if (lan_status != saved_status)
			{
				saved_status = lan_status;
#ifdef CONFIG_BTOUCH
				setCfgValue("value", lan_status, LANSETTINGS);
#else
				setCfgValue("enable", lan_status, item_id);
#endif
			}
		}
		++it;
	}
}

