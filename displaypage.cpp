#include "displaypage.h"
#include "bannondx.h"
#include "bann_settings.h"
#include "cleanscreen.h"
#include "brightnesspage.h"
#include "xml_functions.h" // getElement
#include "screensaverpage.h"


static const char *ICON_BRIGHTNESS = IMG_PATH "btlum.png";


DisplayPage::DisplayPage(QDomNode config_node)
{
	loadItems(config_node);
}

void DisplayPage::loadItems(QDomNode config_node)
{
	banner *b;

	int wait_time = 45; // Default waiting time in seconds.
	QDomElement n = getElement(config_node, "cleaning/time");
	if (!n.isNull())
		wait_time = n.text().toInt();

	b = new bannOnDx(this, ICON_INFO, new CleanScreen(wait_time));
	b->setText(tr("Clean Screen"));
	appendBanner(b);

	b = new calibration(this, ICON_INFO);
	connect(b, SIGNAL(startCalib()), this, SIGNAL(startCalib()));
	connect(b, SIGNAL(endCalib()), this, SIGNAL(endCalib()));
	b->setText(tr("Calibration"));
	appendBanner(b);

	b = new bannOnDx(this, ICON_BRIGHTNESS, new BrightnessPage());
	b->setText(tr("Brightness"));
	appendBanner(b);

	b = new bannOnDx(this, ICON_INFO, new ScreenSaverPage());
	b->setText(tr("Screen Saver"));
	appendBanner(b);
}

