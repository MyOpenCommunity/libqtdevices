#include "changedatetime.h"
#include "datetime.h"
#include "btbutton.h"
#include "skinmanager.h"
#include "main.h" // getConfElement, bt_global::config
#include "platform_device.h"
#include "devices_cache.h"

#include <QLabel>
#include <QVBoxLayout>


// ChangeTime implementation

ChangeTime::ChangeTime()
	: timer_id(0)
{
	dev = bt_global::add_device_to_cache(new PlatformDevice);

	QLabel *img = new QLabel;
	img->setPixmap(bt_global::skin->getImage("time_icon"));

	edit = new BtTimeEdit(this, BtTimeEdit::DISPLAY_SECONDS);

	BtButton *ok = new BtButton;
	ok->setImage(bt_global::skin->getImage("ok"));

	date = new ChangeDate();

	connect(ok, SIGNAL(clicked()), SLOT(acceptTime()));
	connect(date, SIGNAL(Closed()), SIGNAL(Closed()));

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(10, 0, 10, 0);
	l->setSpacing(0);

	l->addWidget(img, 0, Qt::AlignCenter);
	l->addWidget(edit, 1);
	l->addWidget(ok);
}

void ChangeTime::acceptTime()
{
	dev->setTime(edit->timeWithSeconds());

	// go to page date and stop the timer to update seconds
	date->showPage();
}

void ChangeTime::startTimeUpdate()
{
	// display current time and start the update timer
	edit->setTimeWithSeconds(QTime::currentTime());
	if (timer_id == 0)
		timer_id = startTimer(1000);
}

void ChangeTime::showEvent(QShowEvent *event)
{
	startTimeUpdate();
}

void ChangeTime::hideEvent(QHideEvent *event)
{
	if (timer_id != 0)
		killTimer(timer_id);
	timer_id = 0;
}

void ChangeTime::timerEvent(QTimerEvent *event)
{
	// update displayed time
	BtTime t = edit->timeWithSeconds().addSecond(1);
	edit->setTimeWithSeconds(t);
}

void ChangeTime::showPage()
{
	startTimeUpdate();

	Page::showPage();
}


// ChangeDate implementation

ChangeDate::ChangeDate()
{
	dev = bt_global::add_device_to_cache(new PlatformDevice);

	QLabel *img = new QLabel;
	img->setPixmap(bt_global::skin->getImage("date_icon"));

	edit = new BtDateEdit(this);
	edit->setAllowPastDates(true);

	BtButton *ok = new BtButton;
	ok->setImage(bt_global::skin->getImage("ok"));

	connect(ok, SIGNAL(clicked()), SLOT(acceptDate()));

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(10, 0, 10, 0);
	l->setSpacing(0);

	l->addWidget(img, 0, Qt::AlignCenter);
	l->addWidget(edit, 1);
	l->addWidget(ok);
}

void ChangeDate::showPage()
{
	edit->setDate(QDate::currentDate());

	Page::showPage();
}

void ChangeDate::acceptDate()
{
	dev->setDate(edit->date());
	emit Closed();
}
