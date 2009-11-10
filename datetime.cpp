/*!
 * \datetime.cpp
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#include "datetime.h"
#include "btbutton.h"
#include "main.h" // bt_global::config
#include "skinmanager.h"

#include <QLayout>


BtButton *getButton(QString img, QWidget *parent, bool autorepeat)
{
	BtButton *btn = new BtButton(parent);
	btn->setImage(img);
	if (autorepeat)
		btn->setAutoRepeat(true);
	return btn;
}

BtTimeEdit::BtTimeEdit(QWidget *parent, DisplayType type)
		: QWidget(parent),
		_time(0, 0, 0),
		_display_type(type)
{
	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setSpacing(0);
	main_layout->setContentsMargins(0, 0, 0, 0);

	BtButton *btn1, *btn2, *btn3;
	QHBoxLayout *hbox = new QHBoxLayout();
	const QString btn_up_img = bt_global::skin->getImage("arrow_up");

	btn1 = getButton(btn_up_img, this, true);
	connect(btn1, SIGNAL(clicked()), this, SLOT(incHours()));
	hbox->addWidget(btn1);

	btn2 = getButton(btn_up_img, this, true);
	connect(btn2, SIGNAL(clicked()), this, SLOT(incMin()));
	hbox->addWidget(btn2);

	if (_display_type == DISPLAY_SECONDS)
	{
		btn3 = getButton(btn_up_img, this, true);
		connect(btn3, SIGNAL(clicked()), this, SLOT(incSec()));
		hbox->addWidget(btn3);
	}

	main_layout->addLayout(hbox);

	num = new QLCDNumber(this);
	num->setSegmentStyle(QLCDNumber::Flat);
	num->setNumDigits(_display_type == DISPLAY_SECONDS ? 8 : 5);
	num->setFrameStyle(QFrame::NoFrame);
	main_layout->addWidget(num, 1);

	hbox = new QHBoxLayout();
	const QString btn_down_img = bt_global::skin->getImage("arrow_down");

	btn1 = getButton(btn_down_img, this, true);
	connect(btn1, SIGNAL(clicked()), this, SLOT(decHours()));
	hbox->addWidget(btn1);

	btn2 = getButton(btn_down_img, this, true);
	connect(btn2, SIGNAL(clicked()), this, SLOT(decMin()));
	hbox->addWidget(btn2);

	if (_display_type == DISPLAY_SECONDS)
	{
		btn3 = getButton(btn_down_img, this, true);
		connect(btn3, SIGNAL(clicked()), this, SLOT(decSec()));
		hbox->addWidget(btn3);
	}

	main_layout->addLayout(hbox);

	displayTime();
}

void BtTimeEdit::setTimeWithSeconds(const BtTimeSeconds& time)
{
	_time = time;
	displayTime();
}

void BtTimeEdit::setMaxHours(int hours)
{
	_time.setMaxHours(hours);
}

void BtTimeEdit::setMaxMinutes(int minutes)
{
	_time.setMaxMinutes(minutes);
}

void BtTimeEdit::setMaxSeconds(int seconds)
{
	_time.setMaxSeconds(seconds);
}

BtTime BtTimeEdit::time()
{
	return BtTime(_time);
}

BtTimeSeconds BtTimeEdit::timeWithSeconds()
{
	return _time;
}

void BtTimeEdit::incHours()
{
	_time = _time.addHour(1);
	displayTime();
}

void BtTimeEdit::incMin()
{
	_time = _time.addMinute(1);
	displayTime();
}

void BtTimeEdit::incSec()
{
	_time = _time.addSecond(1);
	displayTime();
}

void BtTimeEdit::decHours()
{
	_time = _time.addHour(-1);
	displayTime();
}

void BtTimeEdit::decMin()
{
	_time = _time.addMinute(-1);
	displayTime();
}

void BtTimeEdit::decSec()
{
	_time = _time.addSecond(-1);
	displayTime();
}

void BtTimeEdit::displayTime()
{
	QString str;
	if (_display_type == DISPLAY_SECONDS)
		str.sprintf("%02u:%02u:%02u", _time.hour(), _time.minute(), _time.second());
	else
		str.sprintf("%u:%02u", _time.hour(), _time.minute());

	num->display(str);
}

QString BtDateEdit::FORMAT_STRING;


BtDateEdit::BtDateEdit(QWidget *parent)
		: QWidget(parent),
		_date(QDate::currentDate()),
		_allow_past_dates(false)
{
	_date = _date.addDays(1);
	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setSpacing(0);
	main_layout->setContentsMargins(0, 0, 0, 0);
	// Buttons to increase day, month, year
	BtButton *btn_top_left, *btn_top_center, *btn_top_right;
	// Buttons to decrease day, month, year
	BtButton *btn_bottom_left, *btn_bottom_center, *btn_bottom_right;

	const QString btn_up_img = bt_global::skin->getImage("arrow_up");
	btn_top_left = getButton(btn_up_img, this, true);
	btn_top_center = getButton(btn_up_img, this, true);
	btn_top_right = getButton(btn_up_img, this, true);

	QHBoxLayout *h_up_box = new QHBoxLayout();
	h_up_box->addWidget(btn_top_left);
	h_up_box->addWidget(btn_top_center);
	h_up_box->addWidget(btn_top_right);
	main_layout->addLayout(h_up_box);

	date_display = new QLCDNumber(this);
	date_display->setSegmentStyle(QLCDNumber::Flat);
	date_display->setNumDigits(8);
	date_display->setFrameStyle(QFrame::NoFrame);
	main_layout->addWidget(date_display, 1);

	const QString btn_down_img = bt_global::skin->getImage("arrow_down");

	btn_bottom_left = getButton(btn_down_img, this, true);
	btn_bottom_center = getButton(btn_down_img, this, true);
	btn_bottom_right = getButton(btn_down_img, this, true);

	QHBoxLayout *h_down_box = new QHBoxLayout();
	h_down_box->addWidget(btn_bottom_left);
	h_down_box->addWidget(btn_bottom_center);
	h_down_box->addWidget(btn_bottom_right);
	main_layout->addLayout(h_down_box);

	DateFormat fmt = static_cast<DateFormat>(bt_global::config[DATE_FORMAT].toInt());

	switch (fmt)
	{
	case USA_DATE:
		FORMAT_STRING = "MM.dd.yy";
		connect(btn_top_center, SIGNAL(clicked()), this, SLOT(incDay()));
		connect(btn_top_left, SIGNAL(clicked()), this, SLOT(incMonth()));
		connect(btn_top_right, SIGNAL(clicked()), this, SLOT(incYear()));
		connect(btn_bottom_center, SIGNAL(clicked()), this, SLOT(decDay()));
		connect(btn_bottom_left, SIGNAL(clicked()), this, SLOT(decMonth()));
		connect(btn_bottom_right, SIGNAL(clicked()), this, SLOT(decYear()));
		break;
	case EUROPEAN_DATE:
	default:
		FORMAT_STRING = "dd.MM.yy";
		connect(btn_top_left, SIGNAL(clicked()), this, SLOT(incDay()));
		connect(btn_top_center, SIGNAL(clicked()), this, SLOT(incMonth()));
		connect(btn_top_right, SIGNAL(clicked()), this, SLOT(incYear()));
		connect(btn_bottom_left, SIGNAL(clicked()), this, SLOT(decDay()));
		connect(btn_bottom_center, SIGNAL(clicked()), this, SLOT(decMonth()));
		connect(btn_bottom_right, SIGNAL(clicked()), this, SLOT(decYear()));
		break;
	}

	date_display->display(_date.toString(FORMAT_STRING));
}

void BtDateEdit::setAllowPastDates(bool v)
{
	_allow_past_dates = v;
}

void BtDateEdit::setDate(const QDate &d)
{
	_date = d;
	date_display->display(_date.toString(FORMAT_STRING));
}

QDate BtDateEdit::date()
{
	return _date;
}

void BtDateEdit::incDay()
{
	setDate(_date.addDays(1));
}

void BtDateEdit::incMonth()
{
	setDate(_date.addMonths(1));
}

void BtDateEdit::incYear()
{
	setDate(_date.addYears(1));
}

void BtDateEdit::decDay()
{
	if (_allow_past_dates || _date.addDays(-1) >= QDate::currentDate())
		setDate(_date.addDays(-1));
}

void BtDateEdit::decMonth()
{
	if (_allow_past_dates || _date.addMonths(-1) >= QDate::currentDate())
		setDate(_date.addMonths(-1));
}

void BtDateEdit::decYear()
{
	if (_allow_past_dates || _date.addYears(-1) >= QDate::currentDate())
		setDate(_date.addYears(-1));
}
