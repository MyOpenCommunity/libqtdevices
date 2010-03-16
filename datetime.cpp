/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "datetime.h"
#include "btbutton.h"
#include "main.h" // bt_global::config
#include "fontmanager.h"
#include "skinmanager.h"
#include "navigation_bar.h" // NavigationBar

#include <QLayout>
#include <QLabel>
#include <QLCDNumber>


BtButton *getButton(QString img, QWidget *parent, bool autorepeat)
{
	BtButton *btn = new BtButton(parent);
	btn->setImage(img);
	if (autorepeat)
		btn->setAutoRepeat(true);
	return btn;
}

#ifdef LAYOUT_TOUCHX

QLabel *getLabel(QString text = "")
{
	QLabel *l = new QLabel(text);
	l->setFont(bt_global::font->get(FontManager::DATE_TIME));
	l->setAlignment(Qt::AlignCenter);

	return l;
}

#endif

BtTimeEdit::BtTimeEdit(QWidget *parent, DisplayType type)
		: QWidget(parent),
		_time(0, 0, 0),
		_display_type(type)
{
	QGridLayout *main_layout = new QGridLayout(this);
	main_layout->setSpacing(0);
	main_layout->setContentsMargins(0, 0, 0, 0);

	BtButton *btn1, *btn2, *btn3;
	const QString btn_up_img = bt_global::skin->getImage("arrow_up");

	btn1 = getButton(btn_up_img, this, true);
	connect(btn1, SIGNAL(clicked()), this, SLOT(incHours()));
	main_layout->addWidget(btn1, 0, 0);

	btn2 = getButton(btn_up_img, this, true);
	connect(btn2, SIGNAL(clicked()), this, SLOT(incMin()));
	main_layout->addWidget(btn2, 0, 2);

	if (_display_type == DISPLAY_SECONDS)
	{
		btn3 = getButton(btn_up_img, this, true);
		connect(btn3, SIGNAL(clicked()), this, SLOT(incSec()));
		main_layout->addWidget(btn3, 0, 4);
	}

#ifdef LAYOUT_TOUCHX
	hour = getLabel();
	main_layout->addWidget(hour, 1, 0);

	main_layout->addWidget(getLabel(":"), 1, 1);

	minute = getLabel();
	main_layout->addWidget(minute, 1, 2);

	if (_display_type == DISPLAY_SECONDS)
	{
		main_layout->addWidget(getLabel(":"), 1, 3);

		second = getLabel();
		main_layout->addWidget(second, 1, 4);
	}
	else
		second = NULL;

	main_layout->setRowStretch(1, 1);
	main_layout->setColumnMinimumWidth(1, 25);
	if (second)
		main_layout->setColumnMinimumWidth(3, 25);
#else
	num = new QLCDNumber(this);
	num->setSegmentStyle(QLCDNumber::Flat);
	num->setNumDigits(_display_type == DISPLAY_SECONDS ? 8 : 5);
	num->setFrameStyle(QFrame::NoFrame);
	main_layout->addWidget(num, 1, 0, 1, main_layout->columnCount());
	main_layout->setRowStretch(1, 1);
#endif

	const QString btn_down_img = bt_global::skin->getImage("arrow_down");

	btn1 = getButton(btn_down_img, this, true);
	connect(btn1, SIGNAL(clicked()), this, SLOT(decHours()));
	main_layout->addWidget(btn1, 2, 0);

	btn2 = getButton(btn_down_img, this, true);
	connect(btn2, SIGNAL(clicked()), this, SLOT(decMin()));
	main_layout->addWidget(btn2, 2, 2);

	if (_display_type == DISPLAY_SECONDS)
	{
		btn3 = getButton(btn_down_img, this, true);
		connect(btn3, SIGNAL(clicked()), this, SLOT(decSec()));
		main_layout->addWidget(btn3, 2, 4);
	}

	// add stretch for the columns containing buttons
	for (int i = 0; i < main_layout->columnCount(); i += 2)
		main_layout->setColumnStretch(i, 1);

	displayTime();
}

void BtTimeEdit::setTime(const BtTime& time)
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
#ifdef LAYOUT_TOUCHX
	hour->setText(QString("%1").arg(_time.hour(), 2, 10, QChar('0')));
	minute->setText(QString("%1").arg(_time.minute(), 2, 10, QChar('0')));

	if (second)
		second->setText(QString("%1").arg(_time.second(), 2, 10, QChar('0')));
#else
	QString str;
	if (_display_type == DISPLAY_SECONDS)
		str.sprintf("%02u:%02u:%02u", _time.hour(), _time.minute(), _time.second());
	else
		str.sprintf("%u:%02u", _time.hour(), _time.minute());

	num->display(str);
#endif
}

QString BtDateEdit::FORMAT_STRING;


BtDateEdit::BtDateEdit(QWidget *parent)
		: QWidget(parent),
		_date(QDate::currentDate()),
		_allow_past_dates(false)
{
	DateFormat fmt = static_cast<DateFormat>(bt_global::config[DATE_FORMAT].toInt());

	_date = _date.addDays(1);

	QGridLayout *main_layout = new QGridLayout(this);
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

	main_layout->addWidget(btn_top_left, 0, 0);
	main_layout->addWidget(btn_top_center, 0, 2);
	main_layout->addWidget(btn_top_right, 0, 4);

#ifdef LAYOUT_TOUCHX
	day = getLabel();
	main_layout->addWidget(day, 1, fmt == USA_DATE ? 2 : 0);

	main_layout->addWidget(getLabel("/"), 1, 1);

	month = getLabel();
	main_layout->addWidget(month, 1, fmt == USA_DATE ? 0 : 2);

	main_layout->addWidget(getLabel("/"), 1, 3);

	year = getLabel();
	main_layout->addWidget(year, 1, 4);

	main_layout->setRowStretch(1, 1);
	main_layout->setColumnMinimumWidth(1, 25);
	main_layout->setColumnMinimumWidth(3, 25);
#else
	date_display = new QLCDNumber(this);
	date_display->setSegmentStyle(QLCDNumber::Flat);
	date_display->setNumDigits(8);
	date_display->setFrameStyle(QFrame::NoFrame);
	main_layout->addWidget(date_display, 1, 0, 1, 5);
	main_layout->setRowStretch(1, 1);
#endif

	const QString btn_down_img = bt_global::skin->getImage("arrow_down");

	btn_bottom_left = getButton(btn_down_img, this, true);
	btn_bottom_center = getButton(btn_down_img, this, true);
	btn_bottom_right = getButton(btn_down_img, this, true);

	main_layout->addWidget(btn_bottom_left, 2, 0);
	main_layout->addWidget(btn_bottom_center, 2, 2);
	main_layout->addWidget(btn_bottom_right, 2, 4);

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

	// add stretch for the columns containing buttons
	for (int i = 0; i < main_layout->columnCount(); i += 2)
		main_layout->setColumnStretch(i, 1);

	displayDate();
}

void BtDateEdit::setAllowPastDates(bool v)
{
	_allow_past_dates = v;
}

void BtDateEdit::setDate(const QDate &d)
{
	_date = d;
	displayDate();
}

void BtDateEdit::displayDate()
{
#ifdef LAYOUT_TOUCHX
	year->setText(QString("%1").arg(_date.year(), 4, 10, QChar('0')));
	month->setText(QString("%1").arg(_date.month(), 2, 10, QChar('0')));
	day->setText(QString("%1").arg(_date.day(), 2, 10, QChar('0')));
#else
	date_display->display(_date.toString(FORMAT_STRING));
#endif
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



PageSetDateTime::PageSetDateTime(const QString &extra_button_icon, bool allow_past_dates)
	: title_widget("Change title dynamically", TITLE_HEIGHT)
{
	content.setLayout(&main_layout);

	BtButton *program = new BtButton(this);
	program->setImage(extra_button_icon);

	date_edit = new BtDateEdit(this);
	date_edit->setAllowPastDates(allow_past_dates);
	time_edit = new BtTimeEdit(this);

	top_layout.addWidget(date_edit);
	top_layout.addStretch(1);
	top_layout.addWidget(time_edit);
	top_layout.setSpacing(20);

	main_layout.addLayout(&top_layout);
	main_layout.addWidget(program, 1, Qt::AlignRight|Qt::AlignVCenter);
	main_layout.setContentsMargins(40, 0, 40, 0);

	NavigationBar *nav = new NavigationBar;
	nav->displayScrollButtons(false);
	buildPage(&content, nav, NULL, &title_widget);

	connect(program, SIGNAL(clicked()), SLOT(performAction()));
	connect(nav, SIGNAL(backClick()), SIGNAL(Closed()));
}

QDate PageSetDateTime::date()
{
	return date_edit->date();
}

BtTime PageSetDateTime::time()
{
	return time_edit->time();
}

void PageSetDateTime::setTitle(QString title)
{
	title_widget.setTitle(title);
}

void PageSetDateTime::performAction()
{
	emit dateTimeSelected(date(), time());
}

