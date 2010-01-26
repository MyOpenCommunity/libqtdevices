#include "displaypage.h"
#include "bann1_button.h" // bannOnDx
#include "bann_settings.h"
#include "cleanscreen.h"
#include "brightnesspage.h"
#include "xml_functions.h" // getElement
#include "screensaverpage.h"
#include "skinmanager.h" // SkinContext, bt_global::skin
#include "bannercontent.h"
#include "btmain.h" // bt_global::btmain


DisplayPage::DisplayPage(const QDomNode &config_node)
{
	buildPage();
	loadItems(config_node);
}

void DisplayPage::loadItems(const QDomNode &config_node)
{
	banner *b;

	SkinContext context(getTextChild(config_node, "cid").toInt());
	QString img_items = bt_global::skin->getImage("display_items");
	QString img_clean = bt_global::skin->getImage("cleanscreen");

	int wait_time = 45; // Default waiting time in seconds.
	QDomElement n = getElement(config_node, "cleaning/time");
	if (!n.isNull())
		wait_time = n.text().toInt();

	Window *w = new CleanScreen(img_clean, wait_time);

	b = new BannSimple(img_clean);
	connect(b, SIGNAL(clicked()), w, SLOT(showWindow()));
	page_content->appendBanner(b);
#ifndef BT_HARDWARE_X11
	b = new calibration(this, img_items);
	connect(b, SIGNAL(startCalib()), SLOT(startCalibration()));
	connect(b, SIGNAL(endCalib()), SLOT(endCalibration()));
	b->setText(tr("Calibration"));
	b->Draw();
	page_content->appendBanner(b);

	b = new bannOnDx(this,img_items, new BrightnessPage());
	connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	b->setText(tr("Brightness"));
	b->Draw();
	page_content->appendBanner(b);
#endif
	b = new bannOnDx(this, img_items, new ScreenSaverPage());
	connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	b->setText(tr("Screen Saver"));
	b->Draw();
	page_content->appendBanner(b);
}

void DisplayPage::startCalibration()
{
	bt_global::btmain->startCalib();
}

void DisplayPage::endCalibration()
{
	bt_global::btmain->endCalib();
}

