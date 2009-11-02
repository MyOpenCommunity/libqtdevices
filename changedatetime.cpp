#include "changedatetime.h"
#include "datetime.h"
#include "btbutton.h"
#include "skinmanager.h"
#include "main.h" // getConfElement, bt_global::config

#include <QLabel>
#include <QVBoxLayout>


// ChangeTime implementation

ChangeTime::ChangeTime()
	: timer_id(0)
{
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
	BtTimeSeconds t = edit->timeWithSeconds();
	QString f;
	f.sprintf("*#13**#0*%02u*%02u*%02u**##", t.hour(), t.minute(), t.second());
	sendFrame(f);

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
	BtTimeSeconds t = edit->timeWithSeconds().addSecond(1);
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
	QString f = "*#13**#1*00*" + edit->date().toString("dd*MM*yyyy") + "##";
	sendFrame(f);
	emit Closed();
}
