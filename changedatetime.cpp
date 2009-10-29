#include "changedatetime.h"
#include "datetime.h"
#include "btbutton.h"
#include "skinmanager.h"
#include "main.h" // getConfElement, bt_global::config

#include <QLabel>
#include <QVBoxLayout>

// ChangeTime implementation

ChangeTime::ChangeTime()
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
	killTimer(timer_id);
}

void ChangeTime::showPage()
{
	// display current time and start the update timer
	edit->setTimeWithSeconds(QTime::currentTime());
	timer_id = startTimer(1000);

	Page::showPage();
}

void ChangeTime::timerEvent(QTimerEvent *event)
{
	// update displayed time
	BtTimeSeconds t = edit->timeWithSeconds().addSecond(1);
	edit->setTimeWithSeconds(t);
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

void ChangeDate::acceptDate()
{
	QString f = "*#13**#1*00*" + edit->date().toString("dd*MM*yyyy") + "##";
	sendFrame(f);
	emit Closed();
}
