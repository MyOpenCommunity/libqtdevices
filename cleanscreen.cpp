#include "cleanscreen.h"
#include "xml_functions.h" // getChildWithId
#include "fontmanager.h" // bt_global::font
#include "icondispatcher.h" // bt_global::icons_cache
#include "main.h" // IMG_PATH

#include <QLabel>

#include <assert.h>


CleanScreen::CleanScreen(QString img_clean, int clean_time)
{
	connect(&secs_timer, SIGNAL(timeout()), SLOT(update()));

	connect(&timer, SIGNAL(timeout()), &secs_timer, SLOT(stop()));
	connect(&timer, SIGNAL(timeout()), SIGNAL(Closed()));
	timer.setSingleShot(true);

	time_label = new QLabel(this);
	time_label->setFont(bt_global::font->get(FontManager::TEXT));
	time_label->setAlignment(Qt::AlignHCenter);
	time_label->setGeometry(TIME_LABEL_X, TIME_LABEL_Y, TIME_LABEL_WIDTH, TIME_LABEL_HEIGHT);

	icon_label = new QLabel(this);
	icon_label->setPixmap(*bt_global::icons_cache.getIcon(img_clean));
	icon_label->setAlignment(Qt::AlignHCenter);
	icon_label->setGeometry(ICON_LABEL_X, ICON_LABEL_Y, ICON_LABEL_WIDTH, ICON_LABEL_HEIGHT);

	wait_time_sec = clean_time;
}

void CleanScreen::showEvent(QShowEvent *e)
{
	secs_timer.start(1 * 1000);
	timer.start(wait_time_sec * 1000);
	end_time.restart();
}


void CleanScreen::paintEvent(QPaintEvent *e)
{
	// we use QTime only to format the output
	QTime remaining_time;
	// The following setText() 'floors' the number of seconds remaining, so
	// add 900 to display correctly the number of remaining seconds without
	// using ceil (which is for floating point numbers)
	remaining_time = remaining_time.addMSecs(wait_time_sec * 1000 - end_time.elapsed() + 900);
	if (remaining_time.minute())
		time_label->setText(remaining_time.toString("mm:ss"));
	else
		time_label->setText(remaining_time.toString("ss"));
}
