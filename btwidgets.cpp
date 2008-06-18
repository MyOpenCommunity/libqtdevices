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
	: QWidget(parent, name)
{
	hours = 0;
	minutes = 1;
	max_hours = 9;
	max_minutes = 9;

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
	num->setNumDigits(3);
	num->display(QString("%1:%2").arg(hours).arg(minutes));
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
	if (hours < max_hours)
	{
		hours++;
		qDebug("[TERMO] about to display %s", QString("%1:%2").arg(hours).arg(minutes).ascii());
		num->display(QString("%1:%2").arg(hours).arg(minutes));
		emit valueChanged(hours, minutes);
	}
}

void BtTimeEdit::incMin()
{
	if (minutes < max_minutes)
	{
		minutes++;
		qDebug("[TERMO] about to display %s", QString("%1:%2").arg(hours).arg(minutes).ascii());
		num->display(QString("%1:%2").arg(hours).arg(minutes));
		emit valueChanged(hours, minutes);
	}
}

void BtTimeEdit::decHours()
{
	if (hours > 0)
	{
		hours--;
		num->display(QString("%1:%2").arg(hours).arg(minutes));
		emit valueChanged(hours, minutes);
	}
}

void BtTimeEdit::decMin()
{
	if (minutes > 0)
	{
		minutes--;
		num->display(QString("%1:%2").arg(hours).arg(minutes));
		emit valueChanged(hours, minutes);
	}
}
