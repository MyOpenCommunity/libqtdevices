#include "lansettings.h"
#include "btbutton.h"
#include "main.h" // bt_global::config
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


Text2Column::Text2Column()
{
	main_layout = new QGridLayout(this);
	main_layout->setSpacing(0);
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
	box_text->setStyleSheet("background-color:#f0f0f0; color:#000000;");
	box_text->setFrameStyle(QFrame::Panel | QFrame::Raised);
	box_text->setLineWidth(3);
	box_text->setFont(bt_global::font->get(FontManager::SMALLTEXT));

	box_text->addRow(bt_global::config[MODEL]);
	box_text->addRow("");
	box_text->addRow(bt_global::config[NAME]);
	box_text->addRow(tr("Mac"), "");
	box_text->addRow(tr("IP"), "");
	box_text->addRow(tr("Subnet mask"), "");
	box_text->addRow(tr("Gateway"), "");
	box_text->addRow(tr("DNS"), "");
	box_text->addRow("", "");

	QWidget *content = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(content);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);

	QHBoxLayout *label_layout = new QHBoxLayout;
	label_layout->setContentsMargins(5, 0, 5, 0);
	label_layout->addWidget(box_text);
	main_layout->addLayout(label_layout);

	toggle_btn = new BtButton;
	toggle_btn->setOnOff();
	toggle_btn->setImage(bt_global::skin->getImage("network_disable"), BtButton::NO_FLAG);
	toggle_btn->setPressedImage(bt_global::skin->getImage("network_enable"));
	lan_status = false; // This value must be keep in sync with the icon of toggle_btn.
	connect(toggle_btn, SIGNAL(clicked()), SLOT(toggleLan()));
	main_layout->addWidget(toggle_btn, 0, Qt::AlignHCenter);

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	buildPage(content, nav_bar);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	dev = bt_global::add_device_to_cache(new PlatformDevice);
	connect(dev, SIGNAL(status_changed(const StatusList&)), SLOT(status_changed(const StatusList&)));

	// Set the network to the initial status
	saved_status = getTextChild(config_node, "value").toInt();
}

void LanSettings::inizializza()
{
	qDebug() << "LanSettings::inizializza()";
	dev->enableLan(saved_status);
	requestNetworkInfo(dev);
}

void LanSettings::showPage()
{
	requestNetworkInfo(dev);
	Page::showPage();
}

void LanSettings::toggleLan()
{
	dev->enableLan(!lan_status);
}

void LanSettings::status_changed(const StatusList &status_list)
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

	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
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
				// TODO needs to be modified when removing CONFIG_BTOUCH
				setCfgValue("value", lan_status, LANSETTINGS);
			}
		}
		++it;
	}
}

