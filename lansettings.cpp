#include "lansettings.h"
#include "btbutton.h"
#include "main.h" // bt_global::config, IMG_PATH
#include "landevice.h"
#include "devices_cache.h" // bt_global::devices_cache

#include <QLabel>
#include <QDomNode>
#include <QBoxLayout>

static const char *IMG_BACK = IMG_PATH "arrlf.png";
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

	QVBoxLayout *main_layout = new QVBoxLayout;
	main_layout->setContentsMargins(0, 5, 0, 10);

	QHBoxLayout *label_layout = new QHBoxLayout;
	label_layout->setContentsMargins(5, 0, 5, 0);
	label_layout->addWidget(box_text);
	main_layout->addLayout(label_layout);

	activate_btn = new BtButton;
	activate_btn->setImage(ACTIVATE_ICON);
	main_layout->addWidget(activate_btn, 0, Qt::AlignHCenter);

	back_btn = new BtButton;
	back_btn->setImage(IMG_BACK);
	connect(back_btn, SIGNAL(clicked()), this, SIGNAL(Closed()));
	main_layout->addWidget(back_btn, 0, Qt::AlignLeft);
	setLayout(main_layout);

	dev = new LanDevice;
	bt_global::devices_cache.add_device(dev);
}

