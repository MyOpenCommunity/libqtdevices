/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "plantmenu.h"
#include "device.h"
#include "xml_functions.h"
#include "main.h" //(*bt_global::config)

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
			connect(bp, SIGNAL(sxClick()), &items_submenu, SLOT(showPage()));

			connect(&signal_mapper, SIGNAL(mapped(int)), &items_submenu, SLOT(showItem(int)));

			++ banner_id;
		}
		n = n.nextSibling();
	}
	connect(&items_submenu, SIGNAL(Closed()), this, SLOT(showPage()));
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
	TemperatureScale scale = static_cast<TemperatureScale>((*bt_global::config)[TEMPERATURE_SCALE].toInt());
	BannFullScreen *fsb = getBanner(type, &items_submenu, n, ind_centrale, scale);
	fsb->setText(getTextChild(n, "descr"));
	items_submenu.appendBanner(fsb);

	return bp;
}
