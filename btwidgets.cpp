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

BtTimeEdit::BtTimeEdit(QWidget *parent, const char *name)
	: QWidget(parent, name),
	_time(0, 0)
{
	QVBoxLayout *main_layout = new QVBoxLayout(this);

	QPixmap *icon, *pressed_icon;
	BtButton *btn1, *btn2;

	const QString btn_up_img = QString("%1%2").arg(IMG_PATH).arg("arrup.png");
	const QString btn_up_img_press = QString("%1%2").arg(IMG_PATH).arg("arrupp.png");
	icon         = icons_library.getIcon(btn_up_img);
	pressed_icon = icons_library.getIcon(btn_up_img_press);
	btn1 = new BtButton(this, 0);
	btn2 = new BtButton(this, 0);
	btn1->setPixmap(*icon);
	btn1->setPressedPixmap(*pressed_icon);
	btn2->setPixmap(*icon);
	btn2->setPressedPixmap(*pressed_icon);
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
	icon         = icons_library.getIcon(btn_down_img);
	pressed_icon = icons_library.getIcon(btn_down_img_press);
	btn1 = new BtButton(this, 0);
	btn2 = new BtButton(this, 0);
	btn1->setPixmap(*icon);
	btn1->setPressedPixmap(*pressed_icon);
	btn2->setPixmap(*icon);
	btn2->setPressedPixmap(*pressed_icon);
	connect(btn1, SIGNAL(clicked()), this, SLOT(decHours()));
	connect(btn2, SIGNAL(clicked()), this, SLOT(decMin()));
	hbox = new QHBoxLayout(main_layout);
	hbox->addWidget(btn1);
	hbox->addWidget(btn2);

	setMaxHours(23);
	setMaxMins(59);
}

QTime BtTimeEdit::time()
{
	return _time;
}

void BtTimeEdit::setMaxHours(int h)
{
	if (h > 0)
	{
		max_hours = h;
		num->setNumDigits(QString::number(max_hours).length() + QString::number(max_minutes).length() + 1);
	}
}

void BtTimeEdit::setMaxMins(int m)
{
	if (m > 0)
	{
		max_minutes = m;
		num->setNumDigits(QString::number(max_hours).length() + QString::number(max_minutes).length() + 1);
	}
}

void BtTimeEdit::incHours()
{
	if (_time.hour() < max_hours)
	{
		_time.setHMS(_time.hour() + 1, _time.minute(), _time.second());
		num->display(_time.toString("h:mm"));
		qDebug("[TERMO] about to display %s", _time.toString("h:mm").ascii());
		emit valueChanged(_time.hour(), _time.minute());
	}
}

void BtTimeEdit::incMin()
{
	if (_time.minute() < max_minutes)
	{
		_time.setHMS(_time.hour(), _time.minute() + 1, _time.second());
		num->display(_time.toString("h:mm"));
		qDebug("[TERMO] about to display %s", _time.toString("h:mm").ascii());
		emit valueChanged(_time.hour(), _time.minute());
	}
}

void BtTimeEdit::decHours()
{
	if (_time.hour() > 0)
	{
		_time.setHMS(_time.hour() - 1, _time.minute(), _time.second());
		num->display(_time.toString("h:mm"));
		qDebug("[TERMO] about to display %s", _time.toString("h:mm").ascii());
		emit valueChanged(_time.hour(), _time.minute());
	}
}

void BtTimeEdit::decMin()
{
	if (_time.minute() > 0)
	{
		_time.setHMS(_time.hour(), _time.minute() - 1, _time.second());
		num->display(_time.toString("h:mm"));
		qDebug("[TERMO] about to display %s", _time.toString("h:mm").ascii());
		emit valueChanged(_time.hour(), _time.minute());
	}
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

	QPixmap *icon, *pressed_icon;
	const QString btn_up_img = QString("%1%2").arg(IMG_PATH).arg("arrup.png");
	const QString btn_up_img_press = QString("%1%2").arg(IMG_PATH).arg("arrupp.png");
	icon         = icons_library.getIcon(btn_up_img);
	pressed_icon = icons_library.getIcon(btn_up_img_press);
	if (!icon)
		qDebug("[TERMO] BtDateEdit: could not get icon, prepare for strangeness");
	if (!pressed_icon)
		qDebug("[TERMO] BtDateEdit: could not get pressed icon, prepare for strangeness");
	btn_up_day = new BtButton(this, 0);
	btn_up_month = new BtButton(this, 0);
	btn_up_year = new BtButton(this, 0);
	btn_up_day->setPixmap(*icon);
	btn_up_day->setPressedPixmap(*pressed_icon);
	btn_up_month->setPixmap(*icon);
	btn_up_month->setPressedPixmap(*pressed_icon);
	btn_up_year->setPixmap(*icon);
	btn_up_year->setPressedPixmap(*pressed_icon);

	connect(btn_up_day, SIGNAL(clicked()), this, SLOT(incDay()));
	connect(btn_up_month, SIGNAL(clicked()), this, SLOT(incMonth()));
	connect(btn_up_year, SIGNAL(clicked()), this, SLOT(incYear()));

	QHBoxLayout *h_up_box = new QHBoxLayout(main_layout);
	h_up_box->addWidget(btn_up_day);
	h_up_box->addWidget(btn_up_month);
	h_up_box->addWidget(btn_up_year);

	date_display = new QLCDNumber(this);
	date_display->setSegmentStyle(QLCDNumber::Flat);
	date_display->setNumDigits(10);
	date_display->display(QString("%1:%2:%3").arg(_date.day()).arg(_date.month()).arg(_date.year()));
	date_display->setFrameStyle(QFrame::NoFrame);
	main_layout->addWidget(date_display);

	const QString btn_down_img = QString("%1%2").arg(IMG_PATH).arg("arrdw.png");
	const QString btn_down_img_press = QString("%1%2").arg(IMG_PATH).arg("arrdwp.png");
	icon         = icons_library.getIcon(btn_down_img);
	pressed_icon = icons_library.getIcon(btn_down_img_press);
	if (!icon)
		qDebug("[TERMO] BtDateEdit: could not get icon, prepare for strangeness");
	if (!pressed_icon)
		qDebug("[TERMO] BtDateEdit: could not get pressed icon, prepare for strangeness");
	btn_down_day = new BtButton(this, 0);
	btn_down_month = new BtButton(this, 0);
	btn_down_year = new BtButton(this, 0);
	btn_down_day->setPixmap(*icon);
	btn_down_day->setPressedPixmap(*pressed_icon);
	btn_down_month->setPixmap(*icon);
	btn_down_month->setPressedPixmap(*pressed_icon);
	btn_down_year->setPixmap(*icon);
	btn_down_year->setPressedPixmap(*pressed_icon);

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
	date_display->display(QString("%1:%2:%3").arg(_date.day()).arg(_date.month()).arg(_date.year()));
	emit valueChanged(_date);
}

void BtDateEdit::incMonth()
{
	_date = _date.addMonths(1);
	date_display->display(QString("%1:%2:%3").arg(_date.day()).arg(_date.month()).arg(_date.year()));
	emit valueChanged(_date);
}

void BtDateEdit::incYear()
{
	_date = _date.addYears(1);
	date_display->display(QString("%1:%2:%3").arg(_date.day()).arg(_date.month()).arg(_date.year()));
	emit valueChanged(_date);
}

void BtDateEdit::decDay()
{
	if (_date.addDays(-1) >= QDate::currentDate())
	{
		_date = _date.addDays(-1);
		date_display->display(QString("%1:%2:%3").arg(_date.day()).arg(_date.month()).arg(_date.year()));
		emit valueChanged(_date);
	}
}

void BtDateEdit::decMonth()
{
	if (_date.addMonths(-1) >= QDate::currentDate())
	{
		_date = _date.addMonths(-1);
		date_display->display(QString("%1:%2:%3").arg(_date.day()).arg(_date.month()).arg(_date.year()));
		emit valueChanged(_date);
	}
}

void BtDateEdit::decYear()
{
	if (_date.addYears(-1) >= QDate::currentDate())
	{
		_date = _date.addYears(-1);
		date_display->display(QString("%1:%2:%3").arg(_date.day()).arg(_date.month()).arg(_date.year()));
		emit valueChanged(_date);
	}
}
