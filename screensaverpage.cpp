#include "screensaverpage.h"
#include "screensaver.h"


ScreenSaverPage::ScreenSaverPage()
{
	addBanner(tr("No screensaver"), ScreenSaver::NONE);
	addBanner(tr("Line"), ScreenSaver::LINES);
	addBanner(tr("Balls"), ScreenSaver::BALLS);
	addBanner(tr("Time"), ScreenSaver::TIME);
}

int ScreenSaverPage::getCurrentId()
{
	return 1;
}

void ScreenSaverPage::bannerSelected(int id)
{
}
