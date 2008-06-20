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
#include "../bt_stackopen/common_files/openwebnet.h"

BannOff::BannOff(QWidget *parent, const char *name)
	: bann3But(parent, name)
{
	const QString i_off = QString("%1%2").arg(IMG_PATH).arg("off.png");

	SetIcons(0, 0, 0, i_off.ascii());
	connect(this, SIGNAL(centerClick()), this, SLOT(performAction()));
}

void BannOff::performAction()
{
	qDebug("[TERMO] BannOff::performAction: action performed");
	openwebnet msg_open;
	QString msg = QString("*4*303*#%1##").arg(getAddress());
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	emit sendFrame(msg_open.frame_open);
}

BannAntifreeze::BannAntifreeze(QWidget *parent, const char *name)
	: bann3But(parent, name)
{
	const QString i_antifreeze = QString("%1%2").arg(IMG_PATH).arg("antigelo.png");

	SetIcons(0, 0, 0, i_antifreeze.ascii());
	connect(this, SIGNAL(centerClick()), this, SLOT(performAction()));
}

void BannAntifreeze::performAction()
{
	qDebug("[TERMO] BannAntifreeze::performAction: action performed");
	openwebnet msg_open;
	QString msg = QString("*4*302*#%1##").arg(getAddress());
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	emit sendFrame(msg_open.frame_open);
}

BannSummerWinter::BannSummerWinter(QWidget *parent, const char *name)
	: bann4But(parent, name)
{
	const QString i_summer = QString("%1%2").arg(IMG_PATH).arg("estate.png");
	const QString i_winter = QString("%1%2").arg(IMG_PATH).arg("inverno.png");

	// BUT3 = central left button, see banner.cpp:555
	SetIcons(0, 0, i_summer.ascii(), i_winter.ascii());
	//SetIcons(BUT3, i_winter.ascii());
	// BUT4 = central right button
	//SetIcons(BUT4, i_summer.ascii());

	connect(this, SIGNAL(csxClick()), this, SLOT(setWinter()));
	connect(this, SIGNAL(cdxClick()), this, SLOT(setSummer()));
}

void BannSummerWinter::setSummer()
{
	qDebug("[TERMO] BannSummerWinter::setSummer(): summer is very hot indeed!");
	openwebnet msg_open;
	QString msg = QString("*4*0*#%1##").arg(getAddress());
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	emit sendFrame(msg_open.frame_open);
}

void BannSummerWinter::setWinter()
{
	qDebug("[TERMO]BannSummerWinter::setWinter(): winter is cold...");
	openwebnet msg_open;
	QString msg = QString("*4*1*#%1##").arg(getAddress());
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	emit sendFrame(msg_open.frame_open);
}

BannWeekly::BannWeekly(QWidget *parent, const char *name)
	: bannPuls(parent, name)
{
	program = "1";
	connect(this, SIGNAL(click()), this, SLOT(performAction()));
}

void BannWeekly::setProgram(QString prog)
{
	// TODO: check if program number is correct?
	program = prog;
}

void BannWeekly::performAction()
{
	emit programNumber(program.toInt());
}
