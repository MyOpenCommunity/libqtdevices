#include "lansettings.h"
#include "btbutton.h"
#include "main.h" // bt_global::config, IMG_PATH
#include "landevice.h"
#include "devices_cache.h" // bt_global::devices_cache

#include <QLabel>
#include <QDebug>
#include <QDomNode>
#include <QBoxLayout>

static const char *TOGGLE_ICON_ON = IMG_PATH "beepon.png";
static const char *TOGGLE_ICON_OFF = IMG_PATH "beepoff.png";

#define MACADDR_ROW 3
#define IP_ROW 4
#define NETMASK_ROW 5
#define GATEWAY_ROW 6
#define DNS_ROW 7


LanSettings::LanSettings(const QDomNode &config_node)
{
	box_text = new QLabel;
	box_text->setStyleSheet("background-color:#f0f0f0; color:#000000;");
	box_text->setFrameStyle(QFrame::Panel | QFrame::Raised);

	text << bt_global::config[MODEL] << "";
	text << bt_global::config[NAME]  << "Mac address" << "IP" << "Subnet Mask" << "Gateway" << "DNS";
	box_text->setText(text.join("\n"));

	QHBoxLayout *label_layout = new QHBoxLayout;
	label_layout->setContentsMargins(5, 0, 5, 0);
	label_layout->addWidget(box_text);
	main_layout->addLayout(label_layout);

	toggle_btn = new BtButton;
	toggle_btn->setOnOff();
	toggle_btn->setImage(TOGGLE_ICON_OFF, BtButton::NO_FLAG);
	toggle_btn->setPressedImage(TOGGLE_ICON_ON);
	lan_status = false; // This value must be keep in sync with the icon of toggle_btn.
	connect(toggle_btn, SIGNAL(clicked()), SLOT(toggleLan()));
	main_layout->addWidget(toggle_btn, 0, Qt::AlignHCenter);

	addBackButton();

	LanDevice *d = new LanDevice;
	// TODO: verificare le altre parti del codice dove viene usata la add_device
	// se questa puo' portare a memory leak!
	dev = static_cast<LanDevice*>(bt_global::devices_cache.add_device(d));
	connect(dev, SIGNAL(status_changed(QHash<int, QVariant>)),
			SLOT(status_changed(QHash<int, QVariant>)));
	if (dev != d)
		delete d;
}

void LanSettings::inizializza()
{
	qDebug() << "LanSettings::inizializza()";
	dev->requestStatus();
	dev->requestIp();
	dev->requestNetmask();
	dev->requestMacAddress();
	dev->requestGateway();
	dev->requestDNS1();
	dev->requestDNS2();
}

void LanSettings::toggleLan()
{
	dev->enableLan(!lan_status);
}

void LanSettings::status_changed(QHash<int, QVariant> status_list)
{
	QHash<int, int> dim_to_row;
	dim_to_row[LanDevice::DIM_IP] = IP_ROW;
	dim_to_row[LanDevice::DIM_NETMASK] = NETMASK_ROW;
	dim_to_row[LanDevice::DIM_MACADDR] = MACADDR_ROW;
	dim_to_row[LanDevice::DIM_GATEWAY] = GATEWAY_ROW;
	dim_to_row[LanDevice::DIM_DNS1] = DNS_ROW;

	bool update_text = false;
	QHashIterator<int, QVariant> it(status_list);
	while (it.hasNext())
	{
		it.next();
		if (dim_to_row.contains(it.key()))
		{
			text[dim_to_row[it.key()]] = it.value().toString();
			update_text = true;
		}
		else if (it.key() == LanDevice::DIM_STATUS)
		{
			lan_status = it.value().toBool();
			toggle_btn->setStatus(lan_status);
		}
	}

	if (update_text)
		box_text->setText(text.join("\n"));
}

