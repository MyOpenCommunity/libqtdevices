/*!
 * \weeklymenu.cpp
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#include "weeklymenu.h"
#include "main.h"
#include "bannsettings.h"

#include <qregexp.h>

ProgramMenu::ProgramMenu(QWidget *parent, const char *name, QDomNode conf)
	: sottoMenu(parent, name)
{
	conf_root = conf;
}

void ProgramMenu::status_changed(QPtrList<device_status> list)
{
	// change menus
}

WeeklyMenu::WeeklyMenu(QWidget *parent, const char *name, QDomNode conf)
	: ProgramMenu(parent, name, conf)
{
	status = SUMMER;
	createSummerBanners();
}

void WeeklyMenu::createSummerBanners()
{
	const QString i_ok = QString("%1%2").arg(IMG_PATH).arg("btnok.png");
	const QString i_central = QString("%1%2").arg(IMG_PATH).arg("programma_estivo.png");

	if (conf_root.nodeName().contains(QRegExp("item(\\d\\d?)")) == 0)
	{
		qFatal("[TERMO] WeeklyMenu: wrong node in config file");
	}

	QDomNode summer = conf_root.namedItem("summer");
	if (!summer.isNull())
	{
		QDomNode programs = summer.namedItem("prog");
		QDomNode p;
		if (!programs.isNull())
			p = programs.firstChild();
		while (!p.isNull())
		{
			BannWeekly *bp = new BannWeekly(this, 0);
			bp->SetIcons(i_ok.ascii(), 0, i_central.ascii());
			connect(bp, SIGNAL(programNumber(int)), this, SIGNAL(programClicked(int)));
			// set Text taken from conf.xml
			if (p.isElement())
			{
				bp->SetTextU(p.toElement().text());
				QRegExp re("(\\d)");
				int index = re.search(p.nodeName());
				if (index != -1)
					bp->setProgram(re.cap(1).toInt());
			}
			elencoBanner.append(bp);
			p = p.nextSibling();
		}
	}
}

void WeeklyMenu::createWinterBanners()
{
	const QString i_ok = QString("%1%2").arg(IMG_PATH).arg("btnok.png");
	const QString i_central = QString("%1%2").arg(IMG_PATH).arg("programma_invernale.png");

	if (conf_root.nodeName().contains(QRegExp("item(\\d\\d?)")) == 0)
	{
		qFatal("[TERMO] WeeklyMenu: wrong node in config file");
	}

	QDomNode winter = conf_root.namedItem("winter");
	if (!winter.isNull())
	{
		QDomNode programs = winter.namedItem("prog");
		QDomNode p;
		if (!programs.isNull())
			p = programs.firstChild();
		while (!p.isNull())
		{
			BannWeekly *bp = new BannWeekly(this, 0);
			bp->SetIcons(i_ok.ascii(), 0, i_central.ascii());
			connect(bp, SIGNAL(programNumber(int)), this, SIGNAL(programClicked(int)));
			// set Text taken from conf.xml
			if (p.isElement())
			{
				bp->SetTextU(p.toElement().text());
				QRegExp re("(\\d)");
				int index = re.search(p.nodeName());
				if (index != -1)
					bp->setProgram(re.cap(1).toInt());
			}
			elencoBanner.append(bp);
			p = p.nextSibling();
		}
	}
}

TimeEditMenu::TimeEditMenu(QWidget *parent, const char *name)
	: sottoMenu(parent, name, 10, MAX_WIDTH, MAX_HEIGHT, 1)
{
	time_edit = new FSBannTime(this, 0);
	elencoBanner.append(time_edit);
	connect(time_edit, SIGNAL(timeChanged(QTime)), this, SIGNAL(timeChanged(QTime)));
}

QTime TimeEditMenu::time()
{
	return time_edit->time();
}

DateEditMenu::DateEditMenu(QWidget *parent, const char *name)
	: sottoMenu(parent, name, 10, MAX_WIDTH, MAX_HEIGHT, 1)
{
	date_edit = new FSBannDate(this, 0);
	elencoBanner.append(date_edit);
	connect(date_edit, SIGNAL(dateChanged(QDate)), this, SIGNAL(dateChanged(QDate)));
}

QDate DateEditMenu::date()
{
	return date_edit->date();
}
