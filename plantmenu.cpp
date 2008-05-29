/*!
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "plantmenu.h"
#include "bannfullscreen.h"

#include <qregexp.h>

static const char *I_RIGHT_ARROW = IMG_PATH "arrrg.png";
static const char *I_ZONE = IMG_PATH "zona.png";
static const char *I_PLANT = IMG_PATH "impianto.png";

static const char *I_RIGHT_ARROW_P = IMG_PATH "arrrgp.png";

PlantMenu::PlantMenu(QWidget *parent, char *name, QDomNode conf, QColor fg2) :
	sottoMenu(parent, name),
	items_submenu(parent, "items submenu", 4, MAX_WIDTH, MAX_HEIGHT, 1),  // submenu with one item per page
	signal_mapper(0, "")
{
	conf_root = conf;
	// FIXME: bisogna ricordarsi che per C4z ind_centrale != ""
	ind_centrale = conf_root.namedItem("ind_centrale").toElement().text();
	
	items_submenu.setBGColor(paletteBackgroundColor());
	items_submenu.setFGColor(paletteForegroundColor());

	// TODO: verificare se la memoria regge 99 banner per la centrale
	// 99 zone

	second_fg = fg2;

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
			switch (id)
			{
			case TERMO_99Z:
				icon = I_PLANT;
				break;
			case TERMO_4Z:
				icon = I_PLANT;
				break;
			case TERMO_99Z_PROBE:
				icon = I_ZONE;
				break;
			case TERMO_99Z_PROBE_FANCOIL:
				icon = I_ZONE;
				break;
			case TERMO_4Z_PROBE:
				icon = I_ZONE;
				break;
			case TERMO_4Z_PROBE_FANCOIL:
				icon = I_ZONE;
				break;
			}

			QString descr = findNamedNode(n, "descr").toElement().text();
			bannPuls *bp = addMenuItem(n, icon, descr);

			signal_mapper.setMapping(bp, banner_id);
			connect(bp, SIGNAL(sxClick()), &signal_mapper, SLOT(map()));
			connect(&signal_mapper, SIGNAL(mapped(int)), &items_submenu, SLOT(showItem(int)));

			++ banner_id;
		}
		n = n.nextSibling();
	}
	connect(&items_submenu, SIGNAL(Closed()), this, SLOT(show()));
	connect(&items_submenu, SIGNAL(Closed()), &items_submenu, SLOT(hide()));
}

bannPuls *PlantMenu::addMenuItem(QDomNode n, const char *central_icon, QString descr)
{	
	/*
	 * Create little banner in selection menu.
	 */
	bannPuls *bp = new bannPuls(this, descr.ascii());

	bp->SetIcons(I_RIGHT_ARROW, 0, central_icon);

	initBanner(bp, n);

	elencoBanner.append(bp);
	// boh?
	//connect(this, SIGNAL(hideChildren()), elencoBanner.getLast(), SLOT(hide()));


	/*
	 * Create full screen banner in detail menu.
	 */
	BannFullScreen *fsb = new BannFullScreen(this, descr.ascii(), paletteBackgroundColor(), paletteForegroundColor(), second_fg);
	initBanner(fsb, n);
	items_submenu.appendBanner(fsb);
	fsb->show();
	//connect(&items_submenu, SIGNAL(hideChildren()), fsb, SLOT(hide()));

	return bp;
}
