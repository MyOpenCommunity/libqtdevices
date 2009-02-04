#include "screensaverpage.h"
#include "screensaver.h"
#include "displaycontrol.h" // bt_global::display


ScreenSaverPage::ScreenSaverPage() : SingleChoicePage(true)
{
	addBanner(tr("No screensaver"), ScreenSaver::NONE);
	addBanner(tr("Line"), ScreenSaver::LINES);
	addBanner(tr("Balls"), ScreenSaver::BALLS);
	addBanner(tr("Time"), ScreenSaver::TIME);
	addBanner(tr("Text"), ScreenSaver::TEXT);
	addBanner(tr("Deform"), ScreenSaver::DEFORM);
}

int ScreenSaverPage::getCurrentId()
{
	return bt_global::display.currentScreenSaver();
}

void ScreenSaverPage::bannerSelected(int id)
{
	bt_global::display.setScreenSaver(static_cast<ScreenSaver::Type>(id));
}
