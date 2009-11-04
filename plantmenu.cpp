/*!
 * \plantmenu.cpp
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "plantmenu.h"
#include "device.h"
#include "xml_functions.h"
#include "main.h" //bt_global::config
#include "bann1_button.h"
#include "content_widget.h"
#include "navigation_bar.h"
#include "skinmanager.h"

#include <QVariant>
#include <QRegExp>


static const QString i_right_arrow = QString("%1%2").arg(IMG_PATH).arg("arrrg.png");
static const QString i_zone = QString("%1%2").arg(IMG_PATH).arg("zona.png");
static const QString i_thermr = QString("%1%2").arg(IMG_PATH).arg("centrale.png");


PlantMenu::PlantMenu(QWidget *parent, QDomNode conf) : Page(parent)
{
	buildPage(new ContentWidget, new NavigationBar);

	conf_root = conf;

	QDomNode thermr_address = conf_root.namedItem("ind_centrale");
	if (thermr_address.isNull())
		ind_centrale = "0";
	else
		ind_centrale = thermr_address.toElement().text();

	QDomNode n = conf_root.firstChild();
	int banner_id = 0;
	NavigationPage *first = 0, *prev = 0;
	while (!n.isNull())
	{
		if (n.nodeName().contains(QRegExp("item(\\d\\d?)")))
		{
			NavigationPage *pg = 0;
			QString descr = getTextChild(n, "descr");
			if (descr.isNull())
				qDebug("[TERMO] PlantMenu::PlantMenu, ``descr'' is null, prepare for strangeness...");

			SkinContext context(getTextChild(n, "cid").toInt());

			int id = n.namedItem("id").toElement().text().toInt();
			switch (id)
			{
				case TERMO_99Z:
					pg = addMenuItem(n, i_thermr, descr, fs_99z_thermal_regulator);
					break;
				case TERMO_4Z:
					pg = addMenuItem(n, i_thermr, descr, fs_4z_thermal_regulator);
					break;
				case TERMO_99Z_PROBE:
					pg = addMenuItem(n, i_zone, descr, fs_99z_probe);
					break;
				case TERMO_99Z_PROBE_FANCOIL:
					pg = addMenuItem(n, i_zone, descr, fs_99z_fancoil);
					break;
				case TERMO_4Z_PROBE:
					pg = addMenuItem(n, i_zone, descr, fs_4z_probe);
					break;
				case TERMO_4Z_PROBE_FANCOIL:
					pg = addMenuItem(n, i_zone, descr, fs_4z_fancoil);
					break;
			}

			++ banner_id;

			if (prev)
			{
				connect(prev, SIGNAL(downClick()), pg, SLOT(showPage()));
				connect(pg, SIGNAL(upClick()), prev, SLOT(showPage()));
			}
			connect(pg, SIGNAL(backClick()), SLOT(showPage()));

			prev = pg;
			if (!first)
				first = pg;
		}
		n = n.nextSibling();
	}

	connect(prev, SIGNAL(downClick()), first, SLOT(showPage()));
	connect(first, SIGNAL(upClick()), prev, SLOT(showPage()));
}

void PlantMenu::inizializza()
{
	content_widget->initBanners();
}

NavigationPage *PlantMenu::addMenuItem(QDomNode n, QString central_icon, QString descr, BannID type)
{
	/*
	 * Create little banner in selection menu.
	 */
	bannPuls *bp = new bannPuls(this);
	bp->SetIcons(i_right_arrow, QString(), central_icon);
	bp->setText(getTextChild(n, "descr"));
	bp->Draw();
	content_widget->appendBanner(bp);

	/*
	 * Create page in detail menu.
	 */
	TemperatureScale scale = static_cast<TemperatureScale>(bt_global::config[TEMPERATURE_SCALE].toInt());
	NavigationPage *p = getPage(type, this, n, ind_centrale, scale);
	connect(p, SIGNAL(Closed()), SLOT(showPage()));
	connect(bp, SIGNAL(sxClick()), p, SLOT(showPage()));

	return p;
}
