#include "lansettings.h"
#include "btbutton.h"
#include "main.h" // bt_global::config, IMG_PATH
#include "landevice.h"
#include "devices_cache.h" // bt_global::devices_cache

#include <QLabel>
#include <QDebug>
#include <QDomNode>
#include <QBoxLayout>

static const char *ACTIVATE_ICON = IMG_PATH "btnplus.png";


LanSettings::LanSettings(const QDomNode &config_node)
{
	box_text = new QLabel;
	box_text->setStyleSheet("background-color:#c0c0c0; color:#000000;");
	box_text->setFrameStyle(QFrame::Panel | QFrame::Raised);

	QStringList text;
	text << bt_global::config[MODEL] << "";
	text << bt_global::config[NAME]  << "Mac address" << "IP" << "Subnet Mask" << "Gateway" << "DNS";
	box_text->setText(text.join("\n"));

	QHBoxLayout *label_layout = new QHBoxLayout;
	label_layout->setContentsMargins(5, 0, 5, 0);
	label_layout->addWidget(box_text);
	main_layout->addLayout(label_layout);

	activate_btn = new BtButton;
	activate_btn->setImage(ACTIVATE_ICON);
	main_layout->addWidget(activate_btn, 0, Qt::AlignHCenter);

	LanDevice *d = new LanDevice;
	// TODO: verificare le altre parti del codice dove viene usata la add_device
	// se questa puo' portare a memory leak!
	dev = static_cast<LanDevice*>(bt_global::devices_cache.add_device(d));
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

