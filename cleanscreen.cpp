#include "cleanscreen.h"
#include "fontmanager.h" // bt_global::font
#include "icondispatcher.h" // bt_global::icons_cache
#include "displaycontrol.h" // bt_global::display

#include <QLabel>
#include <QVBoxLayout>

CleanScreen::CleanScreen(QString img_clean, int clean_time)
{
	connect(&secs_timer, SIGNAL(timeout()), SLOT(updateRemainingTime()));

	connect(&timer, SIGNAL(timeout()), SLOT(handleClose()));
	timer.setSingleShot(true);

	icon_label = new QLabel;
	icon_label->setPixmap(*bt_global::icons_cache.getIcon(img_clean));
	icon_label->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

	time_label = new QLabel;
	time_label->setFont(bt_global::font->get(FontManager::TEXT));
	time_label->setAlignment(Qt::AlignHCenter);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setSpacing(0);
	l->setContentsMargins(0, 20, 0, 20);

	l->addWidget(icon_label, 1);
	l->addWidget(time_label);
	l->addStretch(1);

	wait_time_sec = clean_time;
}

void CleanScreen::updateRemainingTime()
{
	++secs_counter;
	update();
}

void CleanScreen::showWindow()
{
	if (!secs_timer.isActive())
	{
		qDebug("reset timer cleanscreen");
		secs_timer.start(1 * 1000);
		timer.start(wait_time_sec * 1000);
		secs_counter = 0;
		bt_global::display.forceOperativeMode(true);
	}
	Window::showWindow();
}

void CleanScreen::handleClose()
{
	bt_global::display.forceOperativeMode(false);
	secs_timer.stop();
	emit Closed();
}

void CleanScreen::paintEvent(QPaintEvent *e)
{
	Window::paintEvent(e);

	// we use QTime only to format the output
	QTime remaining_time;
	remaining_time = remaining_time.addMSecs((wait_time_sec - secs_counter) * 1000);
	if (remaining_time.minute())
		time_label->setText(remaining_time.toString("mm:ss"));
	else
		time_label->setText(remaining_time.toString("ss"));
}
