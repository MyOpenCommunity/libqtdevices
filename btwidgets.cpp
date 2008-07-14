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

BtButton *getButton(const char *img, const char *imgp, QWidget *parent, bool autorepeat)
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

BtTimeEdit::BtTimeEdit(QWidget *parent, const char *name)
	: QWidget(parent, name),
	_time(0, 0)
{
	QVBoxLayout *main_layout = new QVBoxLayout(this);

	BtButton *btn1, *btn2;
	const QString btn_up_img = QString("%1%2").arg(IMG_PATH).arg("arrup.png");
	const QString btn_up_img_press = QString("%1%2").arg(IMG_PATH).arg("arrupp.png");
	btn1 = getButton(btn_up_img, btn_up_img_press, this, true);
	btn2 = getButton(btn_up_img, btn_up_img_press, this, true);

	connect(btn1, SIGNAL(clicked()), this, SLOT(incHours()));
	connect(btn2, SIGNAL(clicked()), this, SLOT(incMin()));
	QHBoxLayout *hbox = new QHBoxLayout(main_layout);
	hbox->addWidget(btn1);
	hbox->addWidget(btn2);

	num = new QLCDNumber(this);
	num->setSegmentStyle(QLCDNumber::Flat);
	num->setNumDigits(5);
	num->display(_time.toString("h:mm"));
	num->setFrameStyle(QFrame::NoFrame);
	main_layout->addWidget(num);

	const QString btn_down_img = QString("%1%2").arg(IMG_PATH).arg("arrdw.png");
	const QString btn_down_img_press = QString("%1%2").arg(IMG_PATH).arg("arrdwp.png");
	btn1 = getButton(btn_down_img, btn_down_img_press, this, true);
	btn2 = getButton(btn_down_img, btn_down_img_press, this, true);

	connect(btn1, SIGNAL(clicked()), this, SLOT(decHours()));
	connect(btn2, SIGNAL(clicked()), this, SLOT(decMin()));
	hbox = new QHBoxLayout(main_layout);
	hbox->addWidget(btn1);
	hbox->addWidget(btn2);
}

QTime BtTimeEdit::time()
{
	return _time;
}

void BtTimeEdit::incHours()
{
	_time = _time.addSecs(3600);
	num->display(_time.toString("h:mm"));
}

void BtTimeEdit::incMin()
{
	_time = _time.addSecs(60);
	num->display(_time.toString("h:mm"));
}

void BtTimeEdit::decHours()
{
	_time = _time.addSecs(-3600);
	num->display(_time.toString("h:mm"));
}

void BtTimeEdit::decMin()
{
	_time = _time.addSecs(-60);
	num->display(_time.toString("h:mm"));
}

BtDateEdit::BtDateEdit(QWidget *parent, const char *name)
	: QWidget(parent, name)
{
	_date = QDate::currentDate();
	_date = _date.addDays(1);
	QVBoxLayout *main_layout = new QVBoxLayout(this);
	// Buttons to increase day, month, year
	BtButton *btn_up_day, *btn_up_month, *btn_up_year;
	// Buttons to decrease day, month, year
	BtButton *btn_down_day, *btn_down_month, *btn_down_year;

	const QString btn_up_img = QString("%1%2").arg(IMG_PATH).arg("arrup.png");
	const QString btn_up_img_press = QString("%1%2").arg(IMG_PATH).arg("arrupp.png");
	btn_up_day = getButton(btn_up_img, btn_up_img_press, this, true);
	btn_up_month = getButton(btn_up_img, btn_up_img_press, this, true);
	btn_up_year = getButton(btn_up_img, btn_up_img_press, this, true);

	connect(btn_up_day, SIGNAL(clicked()), this, SLOT(incDay()));
	connect(btn_up_month, SIGNAL(clicked()), this, SLOT(incMonth()));
	connect(btn_up_year, SIGNAL(clicked()), this, SLOT(incYear()));

	QHBoxLayout *h_up_box = new QHBoxLayout(main_layout);
	h_up_box->addWidget(btn_up_day);
	h_up_box->addWidget(btn_up_month);
	h_up_box->addWidget(btn_up_year);

	date_display = new QLCDNumber(this);
	date_display->setSegmentStyle(QLCDNumber::Flat);
	date_display->setNumDigits(8);
	date_display->display(_date.toString("dd.MM.yy"));
	date_display->setFrameStyle(QFrame::NoFrame);
	main_layout->addWidget(date_display);

	const QString btn_down_img = QString("%1%2").arg(IMG_PATH).arg("arrdw.png");
	const QString btn_down_img_press = QString("%1%2").arg(IMG_PATH).arg("arrdwp.png");

	btn_down_day = getButton(btn_down_img, btn_down_img_press, this, true);
	btn_down_month = getButton(btn_down_img, btn_down_img_press, this, true);
	btn_down_year = getButton(btn_down_img, btn_down_img_press, this, true);

	connect(btn_down_day, SIGNAL(clicked()), this, SLOT(decDay()));
	connect(btn_down_month, SIGNAL(clicked()), this, SLOT(decMonth()));
	connect(btn_down_year, SIGNAL(clicked()), this, SLOT(decYear()));

	QHBoxLayout *h_down_box = new QHBoxLayout(main_layout);
	h_down_box->addWidget(btn_down_day);
	h_down_box->addWidget(btn_down_month);
	h_down_box->addWidget(btn_down_year);
}

QDate BtDateEdit::date()
{
	return _date;
}

void BtDateEdit::incDay()
{
	_date = _date.addDays(1);
	date_display->display(_date.toString("dd.MM.yy"));
}

void BtDateEdit::incMonth()
{
	_date = _date.addMonths(1);
	date_display->display(_date.toString("dd.MM.yy"));
}

void BtDateEdit::incYear()
{
	_date = _date.addYears(1);
	date_display->display(_date.toString("dd.MM.yy"));
}

void BtDateEdit::decDay()
{
	if (_date.addDays(-1) >= QDate::currentDate())
	{
		_date = _date.addDays(-1);
		date_display->display(_date.toString("dd.MM.yy"));
	}
}

void BtDateEdit::decMonth()
{
	if (_date.addMonths(-1) >= QDate::currentDate())
	{
		_date = _date.addMonths(-1);
		date_display->display(_date.toString("dd.MM.yy"));
	}
}

void BtDateEdit::decYear()
{
	if (_date.addYears(-1) >= QDate::currentDate())
	{
		_date = _date.addYears(-1);
		date_display->display(_date.toString("dd.MM.yy"));
	}
}
