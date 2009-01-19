#include "screensaverpage.h"
#include "generic_functions.h" // setCfgValue


ScreenSaverPage::ScreenSaverPage(ScreenSaver::Type type) : SingleChoicePage(true)
{
	addBanner(tr("No screensaver"), ScreenSaver::NONE);
	addBanner(tr("Line"), ScreenSaver::LINES);
	addBanner(tr("Balls"), ScreenSaver::BALLS);
	addBanner(tr("Time"), ScreenSaver::TIME);
	current_type = type;
}

int ScreenSaverPage::getCurrentId()
{
	return current_type;
}

void ScreenSaverPage::bannerSelected(int id)
{
	setCfgValue("type", QString::number(id), DISPLAY);
	current_type = static_cast<ScreenSaver::Type>(id);
}
