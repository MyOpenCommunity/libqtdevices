#include "displaypage.h"
#include "bann1_button.h" // bannOnDx
#include "bann_settings.h"
#include "cleanscreen.h"
#include "brightnesspage.h"
#include "xml_functions.h" // getElement
#include "screensaverpage.h"
#include "skinmanager.h" // bt_global::skin

#include <QDebug>



DisplayPage::DisplayPage(const QDomNode &config_node)
{
	loadItems(config_node);
}

void DisplayPage::loadItems(const QDomNode &config_node)
{
	banner *b;

	bt_global::skin->setCid(getTextChild(config_node, "cid").toInt());
	int wait_time = 45; // Default waiting time in seconds.
	QDomElement n = getElement(config_node, "cleaning/time");
	if (!n.isNull())
		wait_time = n.text().toInt();

	b = new bannOnDx(this, bt_global::skin->getImage("display_items"), new CleanScreen(wait_time));
	b->setText(tr("Clean Screen"));
	appendBanner(b);

	b = new calibration(this, bt_global::skin->getImage("display_items"));
	connect(b, SIGNAL(startCalib()), this, SIGNAL(startCalib()));
	connect(b, SIGNAL(endCalib()), this, SIGNAL(endCalib()));
	b->setText(tr("Calibration"));
	appendBanner(b);

	b = new bannOnDx(this, bt_global::skin->getImage("display_items"), new BrightnessPage());
	b->setText(tr("Brightness"));
	appendBanner(b);

	b = new bannOnDx(this, bt_global::skin->getImage("display_items"), new ScreenSaverPage());
	b->setText(tr("Screen Saver"));
	appendBanner(b);
}

