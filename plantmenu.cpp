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
#include "device_cache.h"

#include <qregexp.h>

static const QString i_right_arrow = QString("%1%2").arg(IMG_PATH).arg("arrrg.png");
static const QString i_zone = QString("%1%2").arg(IMG_PATH).arg("zona.png");
static const QString i_thermr = QString("%1%2").arg(IMG_PATH).arg("centrale.png");

PlantMenu::PlantMenu(QWidget *parent, char *name, QDomNode conf, QColor bg, QColor fg, QColor fg2) :
	sottoMenu(parent, name),
	items_submenu(parent, "items submenu", 4, MAX_WIDTH, MAX_HEIGHT, 1),  // submenu with one item per page
	signal_mapper(0, "")
{
	conf_root = conf;
	setBGColor(bg);
	setFGColor(fg);
	second_fg = fg2;
	// FIXME: bisogna ricordarsi che per C4z ind_centrale != ""
	ind_centrale = conf_root.namedItem("ind_centrale").toElement().text();

	items_submenu.setBGColor(paletteBackgroundColor());
	items_submenu.setFGColor(paletteForegroundColor());

	QDomNode n = conf_root.firstChild();
	int banner_id = 0;
	while (!n.isNull())
	{
		if (n.nodeName().contains(QRegExp("item(\\d\\d?)")))
		{
			qDebug("[TERMO] createPlantMenu: item = %s", n.nodeName().ascii());
			bannPuls *bp = 0;
			QString descr = findNamedNode(n, "descr").toElement().text();
			if (descr.isNull())
				qDebug("[TERMO] PlantMenu::PlantMenu, ``descr'' is null, prepare for strangeness...");

			int id = n.namedItem("id").toElement().text().toInt();
			QString item_addr, where_composed;
			device *dev = 0;
			switch (id)
			{
				case TERMO_99Z:
					// FIXME: set the correct address when the device is implemented
					bp = addMenuItem(n, i_thermr, descr, fs_99z_thermal_regulator);
					break;
				case TERMO_4Z:
					bp = addMenuItem(n, i_thermr, descr, fs_4z_thermal_regulator);
					break;
				case TERMO_99Z_PROBE:
					bp = addMenuItem(n, i_zone, descr, fs_99z_probe);
					break;
				case TERMO_99Z_PROBE_FANCOIL:
					item_addr = n.namedItem("where").toElement().text();
					where_composed = item_addr;
					qDebug("[TERMO] TERMO_99Z_PROBE_FANCOIL PlantMenu, where=%s", item_addr.ascii());
					dev = btouch_device_cache.get_temperature_probe_controlled(item_addr.ascii(), THERMO_Z99,
							true, ind_centrale.ascii(), item_addr.ascii());
					bp = addMenuItem(n, i_zone, descr, fs_99z_fancoil);
					break;
				case TERMO_4Z_PROBE:
					bp = addMenuItem(n, i_zone, descr, fs_4z_probe);
					break;
				case TERMO_4Z_PROBE_FANCOIL:
					item_addr = n.namedItem("where").toElement().text();
					where_composed = item_addr + "#" + ind_centrale;
					qDebug("[TERMO] TERMO_4Z_PROBE_FANCOIL PlantMenu, where=%s", where_composed.ascii());
					dev = btouch_device_cache.get_temperature_probe_controlled(where_composed.ascii(), THERMO_Z4,
							true, ind_centrale.ascii(), item_addr.ascii());
					bp = addMenuItem(n, i_zone, descr, fs_4z_fancoil);
					break;
			}

			signal_mapper.setMapping(bp, banner_id);
			connect(bp, SIGNAL(sxClick()), &signal_mapper, SLOT(map()));
			connect(bp, SIGNAL(sxClick()), &items_submenu, SLOT(show()));
			connect(bp, SIGNAL(sxClick()), &items_submenu, SLOT(raise()));
			connect(bp, SIGNAL(sxClick()), this, SLOT(hide()));
			connect(&signal_mapper, SIGNAL(mapped(int)), &items_submenu, SLOT(showItem(int)));

			++ banner_id;
		}
		n = n.nextSibling();
	}
	connect(&items_submenu, SIGNAL(Closed()), this, SLOT(show()));
	connect(&items_submenu, SIGNAL(Closed()), this, SLOT(raise()));
	connect(&items_submenu, SIGNAL(Closed()), &items_submenu, SLOT(hide()));
}

bannPuls *PlantMenu::addMenuItem(QDomNode n, QString central_icon, QString descr, BannID type)
{
	/*
	 * Create little banner in selection menu.
	 */
	bannPuls *bp = new bannPuls(this, descr.ascii());
	bp->SetIcons(i_right_arrow.ascii(), 0, central_icon.ascii());
	initBanner(bp, n);
	elencoBanner.append(bp);
	connectLastBanner();

	/*
	 * Create full screen banner in detail menu.
	 */
	BannFullScreen *fsb = getBanner(type, &items_submenu, n, ind_centrale);
	initBanner(fsb, n);
	fsb->setSecondForeground(second_fg);
	items_submenu.appendBanner(fsb);

	return bp;
}
