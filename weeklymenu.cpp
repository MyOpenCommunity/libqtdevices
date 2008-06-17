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
#include "bannpuls.h"

#include <qregexp.h>

WeeklyMenu::WeeklyMenu(QWidget *parent, const char *name, QDomNode conf)
	: sottoMenu(parent, name)
{
	status = SUMMER;
	conf_root = conf;
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
			bannPuls *bp = new bannPuls(this, 0);
			bp->SetIcons(i_ok.ascii(), 0, i_central.ascii());
			elencoBanner.append(bp);
			// connect clicked() signal
			// TODO: create a special banner?
			p = p.nextSibling();
		}
	}
}

void WeeklyMenu::status_changed(QPtrList<device_status> list)
{
	// send frame Open
}
