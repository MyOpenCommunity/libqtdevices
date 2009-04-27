#include "screensaverpage.h"
#include "screensaver.h"
#include "displaycontrol.h" // bt_global::display

#include <QAbstractButton>

ScreenSaverPage::ScreenSaverPage() : SingleChoicePage(true)
{
	addBanner(tr("No screensaver"), ScreenSaver::NONE);
	addBanner(tr("Line"), ScreenSaver::LINES);
	addBanner(tr("Balls"), ScreenSaver::BALLS);
	addBanner(tr("Time"), ScreenSaver::TIME);
	addBanner(tr("Text"), ScreenSaver::TEXT);
	addBanner(tr("Deform"), ScreenSaver::DEFORM);
	connect(this, SIGNAL(Closed()), SLOT(handleClose()));
}

void ScreenSaverPage::showPage()
{
	initial_id = buttons.checkedId();
	SingleChoicePage::showPage();
}

void ScreenSaverPage::handleClose()
{
	QAbstractButton *b = buttons.button(initial_id);
	b->setChecked(true);
}

int ScreenSaverPage::getCurrentId()
{
	return bt_global::display.currentScreenSaver();
}

void ScreenSaverPage::bannerSelected(int id)
{
	bt_global::display.setScreenSaver(static_cast<ScreenSaver::Type>(id));
	if (id == ScreenSaver::NONE)
		bt_global::display.setBrightness(BRIGHTNESS_OFF);
}
