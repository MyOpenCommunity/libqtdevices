/*!
 * \btwidgets.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#include "btwidgets.h"
#include "btbutton.h"
#include "main.h"

#include <qlayout.h>


BtButton *getButton(QString img, QString imgp, QWidget *parent, bool autorepeat)
{
	QPixmap *icon         = icons_library.getIcon(img);
	QPixmap *pressed_icon = icons_library.getIcon(imgp);
	if (!icon)
		qDebug("[TERMO] BtDateEdit: could not get icon, prepare for strangeness");
	if (!pressed_icon)
		qDebug("[TERMO] BtDateEdit: could not get pressed icon, prepare for strangeness");

	BtButton *btn = new BtButton(parent);
	btn->setPixmap(*icon);
	btn->setPressedPixmap(*pressed_icon);
	if (autorepeat)
		btn->setAutoRepeat(true);
	return btn;
}

BtTimeEdit::BtTimeEdit(QWidget *parent, const char *name) : QWidget(parent), _time(0, 0)
{
	QVBoxLayout *main_layout = new QVBoxLayout(this);

	BtButton *btn1, *btn2;
	const QString btn_up_img = QString("%1%2").arg(IMG_PATH).arg("arrup.png");
	const QString btn_up_img_press = QString("%1%2").arg(IMG_PATH).arg("arrupp.png");
	btn1 = getButton(btn_up_img, btn_up_img_press, this, true);
	btn2 = getButton(btn_up_img, btn_up_img_press, this, true);

	connect(btn1, SIGNAL(clicked()), this, SLOT(incHours()));
	connect(btn2, SIGNAL(clicked()), this, SLOT(incMin()));
	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(btn1);
	hbox->addWidget(btn2);
	main_layout->addLayout(hbox);

	num = new QLCDNumber(this);
	num->setSegmentStyle(QLCDNumber::Flat);
	num->setNumDigits(5);
	num->display(_time.toString());
	num->setFrameStyle(QFrame::NoFrame);
	main_layout->addWidget(num);

	const QString btn_down_img = QString("%1%2").arg(IMG_PATH).arg("arrdw.png");
	const QString btn_down_img_press = QString("%1%2").arg(IMG_PATH).arg("arrdwp.png");
	btn1 = getButton(btn_down_img, btn_down_img_press, this, true);
	btn2 = getButton(btn_down_img, btn_down_img_press, this, true);

	connect(btn1, SIGNAL(clicked()), this, SLOT(decHours()));
	connect(btn2, SIGNAL(clicked()), this, SLOT(decMin()));
	hbox = new QHBoxLayout();
	hbox->addWidget(btn1);
	hbox->addWidget(btn2);
	main_layout->addLayout(hbox);
}

void BtTimeEdit::setMaxHours(int hours)
{
	_time.setMaxHours(hours);
}

void BtTimeEdit::setMaxMinutes(int minutes)
{
	_time.setMaxMinutes(minutes);
}

BtTime BtTimeEdit::time()
{
	return _time;
}

void BtTimeEdit::incHours()
{
	_time = _time.addHour(1);
	num->display(_time.toString());
}

void BtTimeEdit::incMin()
{
	_time = _time.addMinute(1);
	num->display(_time.toString());
}

void BtTimeEdit::decHours()
{
	_time = _time.addHour(-1);
	num->display(_time.toString());
}

void BtTimeEdit::decMin()
{
	_time = _time.addMinute(-1);
	num->display(_time.toString());
}

QString BtDateEdit::DATE_FORMAT;


BtDateEdit::BtDateEdit(QWidget *parent, const char *name) : QWidget(parent),
	_date(QDate::currentDate())
{
	_date = _date.addDays(1);
	QVBoxLayout *main_layout = new QVBoxLayout(this);
	// Buttons to increase day, month, year
	BtButton *btn_top_left, *btn_top_center, *btn_top_right;
	// Buttons to decrease day, month, year
	BtButton *btn_bottom_left, *btn_bottom_center, *btn_bottom_right;

	const QString btn_up_img = QString("%1%2").arg(IMG_PATH).arg("arrup.png");
	const QString btn_up_img_press = QString("%1%2").arg(IMG_PATH).arg("arrupp.png");
	btn_top_left = getButton(btn_up_img, btn_up_img_press, this, true);
	btn_top_center = getButton(btn_up_img, btn_up_img_press, this, true);
	btn_top_right = getButton(btn_up_img, btn_up_img_press, this, true);

	QHBoxLayout *h_up_box = new QHBoxLayout();
	h_up_box->addWidget(btn_top_left);
	h_up_box->addWidget(btn_top_center);
	h_up_box->addWidget(btn_top_right);
	main_layout->addLayout(h_up_box);

	date_display = new QLCDNumber(this);
	date_display->setSegmentStyle(QLCDNumber::Flat);
	date_display->setNumDigits(8);
	date_display->setFrameStyle(QFrame::NoFrame);
	main_layout->addWidget(date_display);

	const QString btn_down_img = QString("%1%2").arg(IMG_PATH).arg("arrdw.png");
	const QString btn_down_img_press = QString("%1%2").arg(IMG_PATH).arg("arrdwp.png");

	btn_bottom_left = getButton(btn_down_img, btn_down_img_press, this, true);
	btn_bottom_center = getButton(btn_down_img, btn_down_img_press, this, true);
	btn_bottom_right = getButton(btn_down_img, btn_down_img_press, this, true);

	QHBoxLayout *h_down_box = new QHBoxLayout();
	h_down_box->addWidget(btn_bottom_left);
	h_down_box->addWidget(btn_bottom_center);
	h_down_box->addWidget(btn_bottom_right);
	main_layout->addLayout(h_down_box);

	// read date format and set connection accordingly
	QString conf_path("setup/generale/clock/dateformat");
	QDomElement e = getConfElement(conf_path);
	DateFormat fmt;
	if (!e.isNull())
		fmt = static_cast<DateFormat>(e.text().toInt());
	else
		fmt = NONE;

	switch (fmt)
	{
	case USA_DATE:
		DATE_FORMAT = "MM.dd.yy";
		connect(btn_top_center, SIGNAL(clicked()), this, SLOT(incDay()));
		connect(btn_top_left, SIGNAL(clicked()), this, SLOT(incMonth()));
		connect(btn_top_right, SIGNAL(clicked()), this, SLOT(incYear()));
		connect(btn_bottom_center, SIGNAL(clicked()), this, SLOT(decDay()));
		connect(btn_bottom_left, SIGNAL(clicked()), this, SLOT(decMonth()));
		connect(btn_bottom_right, SIGNAL(clicked()), this, SLOT(decYear()));
		break;
	default:
		qWarning("No date format found, defaulting to European format");
		// fall below
	case EUROPEAN_DATE:
		DATE_FORMAT = "dd.MM.yy";
		connect(btn_top_left, SIGNAL(clicked()), this, SLOT(incDay()));
		connect(btn_top_center, SIGNAL(clicked()), this, SLOT(incMonth()));
		connect(btn_top_right, SIGNAL(clicked()), this, SLOT(incYear()));
		connect(btn_bottom_left, SIGNAL(clicked()), this, SLOT(decDay()));
		connect(btn_bottom_center, SIGNAL(clicked()), this, SLOT(decMonth()));
		connect(btn_bottom_right, SIGNAL(clicked()), this, SLOT(decYear()));
		break;
	}

	date_display->display(_date.toString(DATE_FORMAT));
}

QDate BtDateEdit::date()
{
	return _date;
}

void BtDateEdit::incDay()
{
	_date = _date.addDays(1);
	date_display->display(_date.toString(DATE_FORMAT));
}

void BtDateEdit::incMonth()
{
	_date = _date.addMonths(1);
	date_display->display(_date.toString(DATE_FORMAT));
}

void BtDateEdit::incYear()
{
	_date = _date.addYears(1);
	date_display->display(_date.toString(DATE_FORMAT));
}

void BtDateEdit::decDay()
{
	if (_date.addDays(-1) >= QDate::currentDate())
	{
		_date = _date.addDays(-1);
		date_display->display(_date.toString(DATE_FORMAT));
	}
}

void BtDateEdit::decMonth()
{
	if (_date.addMonths(-1) >= QDate::currentDate())
	{
		_date = _date.addMonths(-1);
		date_display->display(_date.toString(DATE_FORMAT));
	}
}

void BtDateEdit::decYear()
{
	if (_date.addYears(-1) >= QDate::currentDate())
	{
		_date = _date.addYears(-1);
		date_display->display(_date.toString(DATE_FORMAT));
	}
}
