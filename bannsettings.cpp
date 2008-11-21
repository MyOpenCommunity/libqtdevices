/*!
 * \bannsettings.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief
 *
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#include "bannsettings.h"
#include "main.h"
#include "device.h"


BannOff::BannOff(QWidget *parent, thermal_regulator *_dev) : bann3But(parent)
{
	QString i_off = QString("%1%2").arg(IMG_PATH).arg("off.png");

	SetIcons(QString(), QString(), QString(), i_off);
	dev = _dev;
	connect(this, SIGNAL(centerClick()), this, SLOT(performAction()));
	connect(this, SIGNAL(centerClick()), this, SIGNAL(clicked()));
}

void BannOff::performAction()
{
	qDebug("[TERMO] BannOff::performAction: action performed");
	dev->setOff();
}

BannAntifreeze::BannAntifreeze(QWidget *parent, thermal_regulator *_dev) : bann3But(parent)
{
	QString i_antifreeze = QString("%1%2").arg(IMG_PATH).arg("antigelo.png");

	SetIcons(QString(), QString(), QString(), i_antifreeze);
	dev = _dev;
	connect(this, SIGNAL(centerClick()), this, SLOT(performAction()));
	connect(this, SIGNAL(centerClick()), this, SIGNAL(clicked()));
}

void BannAntifreeze::performAction()
{
	qDebug("[TERMO] BannAntifreeze::performAction: action performed");
	dev->setProtection();
}

BannSummerWinter::BannSummerWinter(QWidget *parent, thermal_regulator *_dev) : bann4But(parent)
{
	QString i_summer = QString("%1%2").arg(IMG_PATH).arg("estate.png");
	QString i_winter = QString("%1%2").arg(IMG_PATH).arg("inverno.png");

	// see banner:195 (don't know why it works, really)
	SetIcons(QString(), QString(), i_summer, i_winter);
	dev = _dev;

	connect(this, SIGNAL(csxClick()), this, SLOT(setWinter()));
	connect(this, SIGNAL(cdxClick()), this, SLOT(setSummer()));
	connect(this, SIGNAL(csxClick()), this, SIGNAL(clicked()));
	connect(this, SIGNAL(cdxClick()), this, SIGNAL(clicked()));
}

void BannSummerWinter::setSummer()
{
	qDebug("[TERMO] BannSummerWinter::setSummer(): summer is very hot indeed!");
	dev->setSummer();
}

void BannSummerWinter::setWinter()
{
	qDebug("[TERMO]BannSummerWinter::setWinter(): winter is cold...");
	dev->setWinter();
}

BannWeekly::BannWeekly(QWidget *parent) : bannPuls(parent)
{
	program = 1;
	connect(this, SIGNAL(click()), this, SLOT(performAction()));
}

void BannWeekly::setProgram(int prog)
{
	// TODO: check if program number is correct?
	program = prog;
}

void BannWeekly::performAction()
{
	emit programNumber(program);
}
