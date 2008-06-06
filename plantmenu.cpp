/*!
 * \plantmenu.cpp
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "plantmenu.h"
#include "device.h"
#include "device_cache.h"

#include <qregexp.h>

static const char *I_RIGHT_ARROW = IMG_PATH "arrrg.png";
static const char *I_ZONE = IMG_PATH "zona.png";
static const char *I_PLANT = IMG_PATH "impianto.png";

static const char *I_RIGHT_ARROW_P = IMG_PATH "arrrgp.png";

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
			// create full screen banner
			int id = n.namedItem("id").toElement().text().toInt();
			const char *icon;
			QString item_addr, where_composed;
			BannID bann_type;
			device *dev = 0;
			bool fancoil = false;
			switch (id)
			{
			case TERMO_99Z:
				// FIXME: set the correct address when the device is implemented
				icon = I_PLANT;
				bann_type = fs_99z;
				item_addr = "0";
				where_composed = item_addr;
				fancoil = false;
				//dev = btouch_device_cache.get_thermr_device(where_composed, THERMO_Z99,
				//		fancoil, ind_centrale.ascii(), item_addr.ascii());
				break;
			case TERMO_4Z:
				// FIXME: set the correct address when the device is implemented
				icon = I_PLANT;
				bann_type = fs_nc_probe;
				item_addr = "0";
				where_composed = item_addr;
				fancoil = false;
				//dev = btouch_device_cache.get_thermr_device(where_composed, THERMO_Z4,
				//		fancoil, ind_centrale.ascii(), item_addr.ascii());
				break;
			case TERMO_99Z_PROBE:
				icon = I_ZONE;
				bann_type = fs_99z_probe;
				item_addr = n.namedItem("where").toElement().text();
				where_composed = item_addr;
				fancoil = false;
				dev = btouch_device_cache.get_thermr_device(item_addr.ascii(), THERMO_Z99,
						fancoil, ind_centrale.ascii(), item_addr.ascii());
				break;
			case TERMO_99Z_PROBE_FANCOIL:
				icon = I_ZONE;
				bann_type = fs_99z_fancoil;
				item_addr = n.namedItem("where").toElement().text();
				where_composed = item_addr;
				fancoil = true;
				dev = btouch_device_cache.get_thermr_device(item_addr.ascii(), THERMO_Z99,
						fancoil, ind_centrale.ascii(), item_addr.ascii());
				break;
			case TERMO_4Z_PROBE:
				icon = I_ZONE;
				bann_type = fs_4z_probe;
				item_addr = n.namedItem("where").toElement().text();
				where_composed = item_addr + "#" + ind_centrale;
				fancoil = false;
				dev = btouch_device_cache.get_thermr_device(where_composed.ascii(), THERMO_Z4,
						fancoil, ind_centrale.ascii(), item_addr.ascii());
				break;
			case TERMO_4Z_PROBE_FANCOIL:
				icon = I_ZONE;
				bann_type = fs_4z_fancoil;
				item_addr = n.namedItem("where").toElement().text();
				where_composed = item_addr + "#" + ind_centrale;
				fancoil = true;
				dev = btouch_device_cache.get_thermr_device(where_composed.ascii(), THERMO_Z4,
						fancoil, ind_centrale.ascii(), item_addr.ascii());
				break;
			}

			QString descr = findNamedNode(n, "descr").toElement().text();
			bannPuls *bp = addMenuItem(n, icon, descr, bann_type, where_composed, dev);

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

bannPuls *PlantMenu::addMenuItem(QDomNode n, const char *central_icon, QString descr, BannID type, 
		QString addr, device *dev)
{
	/*
	 * Create little banner in selection menu.
	 */
	bannPuls *bp = new bannPuls(this, descr.ascii());
	bp->SetIcons(I_RIGHT_ARROW, 0, central_icon);
	initBanner(bp, n);
	elencoBanner.append(bp);

	/*
	 * Create full screen banner in detail menu.
	 */
	qDebug("[TERMO] addMenuItem: before factory");
	BannFullScreen *fsb = FSBannFactory::getInstance()->getBanner(type, &items_submenu, n);
	initBanner(fsb, n);
	fsb->setSecondForeground(second_fg);
	items_submenu.appendBanner(fsb);
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			fsb, SLOT(status_changed(QPtrList<device_status>)));
	fsb->setAddress(addr.ascii());
	qDebug("[TERMO] addMenuItem: factory done");

	return bp;
}
