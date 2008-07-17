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

#include <openwebnet.h>

BannOff::BannOff(QWidget *parent, const char *name, thermal_regulator *_dev)
	: bann3But(parent, name)
{
	const QString i_off = QString("%1%2").arg(IMG_PATH).arg("off.png");

	SetIcons(0, 0, 0, i_off.ascii());
	dev = _dev;
	connect(this, SIGNAL(centerClick()), this, SLOT(performAction()));
	connect(this, SIGNAL(centerClick()), this, SIGNAL(clicked()));
}

void BannOff::performAction()
{
	qDebug("[TERMO] BannOff::performAction: action performed");
	dev->setOff();
}

BannAntifreeze::BannAntifreeze(QWidget *parent, const char *name, thermal_regulator *_dev)
	: bann3But(parent, name)
{
	const QString i_antifreeze = QString("%1%2").arg(IMG_PATH).arg("antigelo.png");

	SetIcons(0, 0, 0, i_antifreeze.ascii());
	dev = _dev;
	connect(this, SIGNAL(centerClick()), this, SLOT(performAction()));
	connect(this, SIGNAL(centerClick()), this, SIGNAL(clicked()));
}

void BannAntifreeze::performAction()
{
	qDebug("[TERMO] BannAntifreeze::performAction: action performed");
	dev->setProtection();
}

BannSummerWinter::BannSummerWinter(QWidget *parent, const char *name, thermal_regulator *_dev)
	: bann4But(parent, name)
{
	const QString i_summer = QString("%1%2").arg(IMG_PATH).arg("estate.png");
	const QString i_winter = QString("%1%2").arg(IMG_PATH).arg("inverno.png");

	// see banner:195 (don't know why it works, really)
	SetIcons(0, 0, i_summer.ascii(), i_winter.ascii());
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

BannWeekly::BannWeekly(QWidget *parent, const char *name)
	: bannPuls(parent, name)
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
