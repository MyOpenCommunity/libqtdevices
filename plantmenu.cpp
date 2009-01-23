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

#include <QVariant>
#include <QRegExp>


static const QString i_right_arrow = QString("%1%2").arg(IMG_PATH).arg("arrrg.png");
static const QString i_zone = QString("%1%2").arg(IMG_PATH).arg("zona.png");
static const QString i_thermr = QString("%1%2").arg(IMG_PATH).arg("centrale.png");


PlantMenu::PlantMenu(QWidget *parent, QDomNode conf) : sottoMenu(parent),
	items_submenu(0, 3, MAX_WIDTH, MAX_HEIGHT, 1),  // submenu with one item per page
	signal_mapper(0)
{
	conf_root = conf;

	QDomNode thermr_address = conf_root.namedItem("ind_centrale");
	if (thermr_address.isNull())
		ind_centrale = "0";
	else
		ind_centrale = thermr_address.toElement().text();

	// hide children
	connect(this, SIGNAL(hideChildren()), &items_submenu, SLOT(hide()));

	QDomNode n = conf_root.firstChild();
	int banner_id = 0;
	while (!n.isNull())
	{
		if (n.nodeName().contains(QRegExp("item(\\d\\d?)")))
		{
			bannPuls *bp = 0;
			QString descr = getTextChild(n, "descr");
			if (descr.isNull())
				qDebug("[TERMO] PlantMenu::PlantMenu, ``descr'' is null, prepare for strangeness...");

			int id = n.namedItem("id").toElement().text().toInt();
			switch (id)
			{
				case TERMO_99Z:
					bp = addMenuItem(n, i_thermr, descr, fs_99z_thermal_regulator);
					break;
				case TERMO_4Z:
					bp = addMenuItem(n, i_thermr, descr, fs_4z_thermal_regulator);
					break;
				case TERMO_99Z_PROBE:
					bp = addMenuItem(n, i_zone, descr, fs_99z_probe);
					break;
				case TERMO_99Z_PROBE_FANCOIL:
					bp = addMenuItem(n, i_zone, descr, fs_99z_fancoil);
					break;
				case TERMO_4Z_PROBE:
					bp = addMenuItem(n, i_zone, descr, fs_4z_probe);
					break;
				case TERMO_4Z_PROBE_FANCOIL:
					bp = addMenuItem(n, i_zone, descr, fs_4z_fancoil);
					break;
			}

			signal_mapper.setMapping(bp, banner_id);
			connect(bp, SIGNAL(sxClick()), &signal_mapper, SLOT(map()));
			connect(bp, SIGNAL(sxClick()), &items_submenu, SLOT(show()));
			connect(bp, SIGNAL(sxClick()), &items_submenu, SLOT(raise()));

			connect(&signal_mapper, SIGNAL(mapped(int)), &items_submenu, SLOT(showItem(int)));

			++ banner_id;
		}
		n = n.nextSibling();
	}
	connect(&items_submenu, SIGNAL(Closed()), &items_submenu, SLOT(hide()));
}

bannPuls *PlantMenu::addMenuItem(QDomNode n, QString central_icon, QString descr, BannID type)
{
	/*
	 * Create little banner in selection menu.
	 */
	bannPuls *bp = new bannPuls(this);
	bp->SetIcons(i_right_arrow, QString(), central_icon);
	bp->setText(getTextChild(n, "descr"));
	elencoBanner.append(bp);
	connectLastBanner();

	/*
	 * Create full screen banner in detail menu.
	 */
	TemperatureScale scale = static_cast<TemperatureScale>(bt_global::config[TEMPERATURE_SCALE].toInt());
	BannFullScreen *fsb = getBanner(type, &items_submenu, n, ind_centrale, scale);
	fsb->setText(getTextChild(n, "descr"));
	items_submenu.appendBanner(fsb);

	return bp;
}
