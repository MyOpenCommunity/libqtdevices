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
			// create full screen banner
			int id = n.namedItem("id").toElement().text().toInt();
			QString icon;
			QString item_addr, where_composed;
			BannID bann_type;
			device *dev = 0;
			bool fancoil = false;
			switch (id)
			{
				case TERMO_99Z:
					// FIXME: set the correct address when the device is implemented
					icon = i_thermr;
					bann_type = fs_99z_thermal_regulator;
					item_addr = "0";
					where_composed = item_addr;
					fancoil = false;
					//dev = btouch_device_cache.get_temperature_probe_controlled(where_composed, THERMO_Z99,
					//		fancoil, ind_centrale.ascii(), item_addr.ascii());
					break;
				case TERMO_4Z:
					// FIXME: create 2 banners for each thermal regulator device
					// - one for the thermal regulator
					// - one for information about the internal probe
					// FIXME: the address of the internal probe is #ind_centrale#ind_centrale
					icon = i_thermr;
					bann_type = fs_4z_thermal_regulator;
					item_addr = "0";
					//where_composed = QString("#") + item_addr + "#" + ind_centrale;
					where_composed = item_addr + "#" + ind_centrale;
					fancoil = false;
					qDebug("[TERMO] TERMO_4Z PlantMenu, where=%s", where_composed.ascii());
					dev = btouch_device_cache.get_thermal_regulator(where_composed.ascii(), THERMO_Z4,
							ind_centrale.ascii(), item_addr.ascii());
					break;
				case TERMO_99Z_PROBE:
					icon = i_zone;
					bann_type = fs_99z_probe;
					item_addr = n.namedItem("where").toElement().text();
					where_composed = item_addr;
					fancoil = false;
					qDebug("[TERMO] TERMO_99Z_PROBE PlantMenu, where=%s", item_addr.ascii());
					dev = btouch_device_cache.get_temperature_probe_controlled(item_addr.ascii(), THERMO_Z99,
							fancoil, ind_centrale.ascii(), item_addr.ascii());
					break;
				case TERMO_99Z_PROBE_FANCOIL:
					icon = i_zone;
					bann_type = fs_99z_fancoil;
					item_addr = n.namedItem("where").toElement().text();
					where_composed = item_addr;
					fancoil = true;
					qDebug("[TERMO] TERMO_99Z_PROBE_FANCOIL PlantMenu, where=%s", item_addr.ascii());
					dev = btouch_device_cache.get_temperature_probe_controlled(item_addr.ascii(), THERMO_Z99,
							fancoil, ind_centrale.ascii(), item_addr.ascii());
					break;
				case TERMO_4Z_PROBE:
					icon = i_zone;
					bann_type = fs_4z_probe;
					item_addr = n.namedItem("where").toElement().text();
					//where_composed = QString("#") + item_addr + "#" + ind_centrale;
					where_composed = item_addr + "#" + ind_centrale;
					fancoil = false;
					qDebug("[TERMO] TERMO_4Z_PROBE PlantMenu, where=%s", where_composed.ascii());
					dev = btouch_device_cache.get_temperature_probe_controlled(where_composed.ascii(), THERMO_Z4,
							fancoil, ind_centrale.ascii(), item_addr.ascii());
					break;
				case TERMO_4Z_PROBE_FANCOIL:
					icon = i_zone;
					bann_type = fs_4z_fancoil;
					item_addr = n.namedItem("where").toElement().text();
					//where_composed = QString("#") + item_addr + "#" + ind_centrale;
					where_composed = item_addr + "#" + ind_centrale;
					fancoil = true;
					qDebug("[TERMO] TERMO_4Z_PROBE_FANCOIL PlantMenu, where=%s", where_composed.ascii());
					dev = btouch_device_cache.get_temperature_probe_controlled(where_composed.ascii(), THERMO_Z4,
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

bannPuls *PlantMenu::addMenuItem(QDomNode n, QString central_icon, QString descr, BannID type, 
		QString addr, device *dev)
{
	/*
	 * Create little banner in selection menu.
	 */
	bannPuls *bp = new bannPuls(this, descr.ascii());
	bp->SetIcons(i_right_arrow.ascii(), 0, central_icon.ascii());
	initBanner(bp, n);
	elencoBanner.append(bp);

	/*
	 * Create full screen banner in detail menu.
	 */
	BannFullScreen *fsb = FSBannFactory::getInstance()->getBanner(type, &items_submenu, n);
	initBanner(fsb, n);
	fsb->setSecondForeground(second_fg);
	items_submenu.appendBanner(fsb);
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			fsb, SLOT(status_changed(QPtrList<device_status>)));
	fsb->setAddress(addr.ascii());
	if (type == fs_4z_thermal_regulator)
		create4zSettings(&items_submenu);
	if (type == fs_99z_thermal_regulator)
		create99zSettings(&items_submenu);

	return bp;
}

void PlantMenu::create4zSettings(sottoMenu *sm)
{
	const QString i_weekly = QString("%1%2").arg(IMG_PATH).arg("settimanale.png");
	const QString i_manual = QString("%1%2").arg(IMG_PATH).arg("manuale.png");
	const QString i_scenarios = QString("%1%2").arg(IMG_PATH).arg("scenari.png");
	const QString i_off = QString("%1%2").arg(IMG_PATH).arg("off.png");
	const QString i_antifreeze = QString("%1%2").arg(IMG_PATH).arg("antigelo.png");
	const QString i_summer_winter = QString("%1%2").arg(IMG_PATH).arg("estate.png");

	sottoMenu *modes = new sottoMenu(0, "modes");
	modes->setBGColor(paletteBackgroundColor());
	modes->setFGColor(paletteForegroundColor());
	connect(sm, SIGNAL(goDx()), modes, SLOT(show()));
	connect(sm, SIGNAL(goDx()), modes, SLOT(raise()));
	connect(sm, SIGNAL(goDx()), sm, SLOT(hide()));

	connect(modes, SIGNAL(Closed()), sm, SLOT(show()));
	connect(modes, SIGNAL(Closed()), modes, SLOT(hide()));

	// week banner
	bannPuls *weekly = new bannPuls(modes, "weekly");
	weekly->SetIcons(i_right_arrow.ascii(), 0, i_weekly.ascii());
	weekly->SetTextU(tr("Weekly operation", "weekly program in thermal regulation"));
	modes->appendBanner(weekly);

	// manual banner
	bannPuls *manual = new bannPuls(modes, "Manual");
	manual->SetIcons(i_right_arrow.ascii(), 0, i_manual.ascii());
	manual->SetTextU(tr("Manual operation", "manual settings in thermal regulation"));
	modes->appendBanner(manual);

	// scenario banner
	bannPuls *scenarios = new bannPuls(modes, "scenarios");
	scenarios->SetIcons(i_right_arrow.ascii(), 0, i_scenarios.ascii());
	scenarios->SetTextU(tr("Scenarios", "scenario menu in thermal regulation"));
	modes->appendBanner(scenarios);

	// TODO: feriale e festivo

	// off banner
	// FIXME: change banner type, this needs to be a big button in the center
	bannPuls *off = new bannPuls(modes, "OFF");
	off->SetIcons(0, 0, i_off.ascii());
	off->SetTextU(tr("OFF", "Set thermal regulator device to off"));
	modes->appendBanner(off);

	// antifreeze banner
	bannPuls *antifreeze = new bannPuls(modes, "antifreeze");
	antifreeze->SetIcons(0, 0, i_antifreeze.ascii());
	antifreeze->SetTextU(tr("antifreeze", "Set thermal regulator in anti freeze mode"));
	modes->appendBanner(antifreeze);

	// summer_winter banner
	bannPuls *summer_winter = new bannPuls(modes, "Summer/Winter");
	summer_winter->SetIcons(0, 0, i_summer_winter.ascii());
	summer_winter->SetTextU(tr("Summer/Winter", "Set thermal regulator in summer/winter mode"));
	modes->appendBanner(summer_winter);
}

void PlantMenu::create99zSettings(sottoMenu *sm)
{
}
