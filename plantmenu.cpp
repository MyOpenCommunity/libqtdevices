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
#include "bannsettings.h"
#include "weeklymenu.h"

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
	sottoMenu *settings = 0;
	if (type == fs_4z_thermal_regulator)
	{
		settings = create4zSettings(n);
		if (!settings)
			qFatal("[TERMO] could not create settings menu");
	}
	if (type == fs_99z_thermal_regulator)
	{
		settings = create99zSettings(n);
		if (!settings)
			qFatal("[TERMO] could not create settings menu");
	}
	if (settings)
	{
		connect(&items_submenu, SIGNAL(goDx()), settings, SLOT(show()));
		connect(&items_submenu, SIGNAL(goDx()), settings, SLOT(raise()));
		connect(&items_submenu, SIGNAL(goDx()), &items_submenu, SLOT(hide()));

		connect(settings, SIGNAL(Closed()), &items_submenu, SLOT(show()));
		connect(settings, SIGNAL(Closed()), settings, SLOT(hide()));
	}

	return bp;
}

sottoMenu *PlantMenu::create4zSettings(QDomNode conf)
{
	const QString i_antifreeze = QString("%1%2").arg(IMG_PATH).arg("antigelo.png");

	sottoMenu *settings = new sottoMenu(0, "settings");

	weekSettings(settings, conf);
	manualSettings(settings);

	timedManualSettings(settings);

	holidaySettings(settings, conf);

	// off banner
	BannOff *off = new BannOff(settings, "OFF");
	settings->appendBanner(off);

	// antifreeze banner
	bann3But *antifreeze = new bann3But(settings, "antifreeze");
	antifreeze->SetIcons(0, 0, 0, i_antifreeze.ascii());
	antifreeze->SetTextU(tr("Antifreeze", "Set thermal regulator in anti freeze mode"));
	settings->appendBanner(antifreeze);

	// summer_winter banner
	BannSummerWinter *summer_winter = new BannSummerWinter(settings, "Summer/Winter");
	settings->appendBanner(summer_winter);

	settings->setAllFGColor(paletteForegroundColor());
	settings->setAllBGColor(paletteBackgroundColor());

	return settings;
}

sottoMenu *PlantMenu::create99zSettings(QDomNode conf)
{
	const QString i_scenarios = QString("%1%2").arg(IMG_PATH).arg("scenari.png");

	sottoMenu *settings = new sottoMenu(0, "settings");

	weekSettings(settings, conf);
	manualSettings(settings);

	// scenario banner
	bannPuls *scenarios = new bannPuls(0, "scenarios");//modes, "scenarios");
	scenarios->SetIcons(i_right_arrow.ascii(), 0, i_scenarios.ascii());
	scenarios->SetTextU(tr("Scenarios", "scenario menu in thermal regulation"));
	scenarios->setBGColor(paletteBackgroundColor());
	scenarios->setFGColor(paletteForegroundColor());
	//modes->appendBanner(scenarios);

	settings->setAllFGColor(paletteForegroundColor());
	settings->setAllBGColor(paletteBackgroundColor());
}

void PlantMenu::manualSettings(sottoMenu *settings)
{
	const QString i_manual = QString("%1%2").arg(IMG_PATH).arg("manuale.png");
	// manual banner
	bannPuls *manual = new bannPuls(settings, "Manual");
	manual->SetIcons(i_right_arrow.ascii(), 0, i_manual.ascii());
	manual->SetTextU(tr("Manual operation", "manual settings in thermal regulation"));

	settings->appendBanner(manual);
	sottoMenu *sm = new sottoMenu(0, "manual", 10, MAX_WIDTH, MAX_HEIGHT, 1);

	BannFullScreen *bann = FSBannFactory::getInstance()->getBanner(fs_manual, sm, QDomNode());
	bann->setSecondForeground(second_fg);

	sm->appendBanner(bann);

	sm->setAllFGColor(paletteForegroundColor());
	sm->setAllBGColor(paletteBackgroundColor());

	connect(manual, SIGNAL(sxClick()), sm, SLOT(show()));
	connect(manual, SIGNAL(sxClick()), sm, SLOT(raise()));
	connect(manual, SIGNAL(sxClick()), settings, SLOT(hide()));

	connect(sm, SIGNAL(Closed()), settings, SLOT(show()));
	connect(sm, SIGNAL(Closed()), sm, SLOT(hide()));
}

void PlantMenu::timedManualSettings(sottoMenu *settings)
{
	const QString i_manual = QString("%1%2").arg(IMG_PATH).arg("manuale.png");
	// timed manual banner
	bannPuls *manual_timed = new bannPuls(settings, "manual_timed");
	manual_timed->SetIcons(i_right_arrow.ascii(), 0, i_manual.ascii());
	manual_timed->SetTextU(tr("Manual timed operation", "manual_timed settings in thermal regulation"));

	settings->appendBanner(manual_timed);
	sottoMenu *sm = new sottoMenu(0, "manual_timed", 10, MAX_WIDTH, MAX_HEIGHT, 1);

	BannFullScreen *bann = FSBannFactory::getInstance()->getBanner(fs_manual_timed, sm, QDomNode());
	bann->setSecondForeground(second_fg);

	sm->appendBanner(bann);

	sm->setAllFGColor(paletteForegroundColor());
	sm->setAllBGColor(paletteBackgroundColor());

	connect(manual_timed, SIGNAL(sxClick()), sm, SLOT(show()));
	connect(manual_timed, SIGNAL(sxClick()), sm, SLOT(raise()));
	connect(manual_timed, SIGNAL(sxClick()), settings, SLOT(hide()));

	connect(sm, SIGNAL(Closed()), settings, SLOT(show()));
	connect(sm, SIGNAL(Closed()), sm, SLOT(hide()));
}

void PlantMenu::weekSettings(sottoMenu *settings, QDomNode conf)
{
	const QString i_weekly = QString("%1%2").arg(IMG_PATH).arg("settimanale.png");

	bannPuls *weekly = new bannPuls(settings, "weekly");
	weekly->SetIcons(i_right_arrow.ascii(), 0, i_weekly.ascii());
	weekly->SetTextU(tr("Weekly operation", "weekly program in thermal regulation"));
	settings->appendBanner(weekly);

	WeeklyMenu *weekmenu = new WeeklyMenu(0, "weekly", conf);
	weekmenu->setAllBGColor(paletteBackgroundColor());
	weekmenu->setAllFGColor(paletteForegroundColor());

	connect(weekly, SIGNAL(sxClick()), weekmenu, SLOT(show()));
	connect(weekly, SIGNAL(sxClick()), weekmenu, SLOT(raise()));
	connect(weekly, SIGNAL(sxClick()), settings, SLOT(hide()));

	connect(weekmenu, SIGNAL(programClicked(int)), settings, SLOT(show()));
	connect(weekmenu, SIGNAL(programClicked(int)), weekmenu, SLOT(hide()));
	connect(weekmenu, SIGNAL(Closed()), settings, SLOT(show()));
	connect(weekmenu, SIGNAL(Closed()), weekmenu, SLOT(hide()));

	OpenFrameSender *frame_sender = new OpenFrameSender(weekmenu, this);
}

void PlantMenu::holidaySettings(sottoMenu *settings, QDomNode conf)
{
	const QString i_holiday = QString("%1%2").arg(IMG_PATH).arg("feriale.png");

	bannPuls *holiday = new bannPuls(settings, "holiday");
	holiday->SetIcons(i_right_arrow.ascii(), 0, i_holiday.ascii());
	holiday->SetTextU(tr("Holiday"));
	settings->appendBanner(holiday);

	// the banner inside date_edit does not have second foreground set
	DateEditMenu *date_edit = new DateEditMenu(0, "date edit");
	date_edit->setAllBGColor(paletteBackgroundColor());
	date_edit->setAllFGColor(paletteForegroundColor());

	connect(holiday, SIGNAL(sxClick()), date_edit, SLOT(show()));
	connect(holiday, SIGNAL(sxClick()), date_edit, SLOT(raise()));
	connect(holiday, SIGNAL(sxClick()), settings, SLOT(hide()));

	connect(date_edit, SIGNAL(Closed()), settings, SLOT(show()));
	connect(date_edit, SIGNAL(Closed()), date_edit, SLOT(hide()));


	TimeEditMenu *time_edit = new TimeEditMenu(0, "time edit");
	time_edit->setAllBGColor(paletteBackgroundColor());
	time_edit->setAllFGColor(paletteForegroundColor());

	connect(date_edit, SIGNAL(goDx()), time_edit, SLOT(show()));
	connect(date_edit, SIGNAL(goDx()), time_edit, SLOT(raise()));
	connect(date_edit, SIGNAL(goDx()), date_edit, SLOT(hide()));

	connect(time_edit, SIGNAL(Closed()), date_edit, SLOT(show()));
	connect(time_edit, SIGNAL(Closed()), date_edit, SLOT(raise()));
	connect(time_edit, SIGNAL(Closed()), time_edit, SLOT(hide()));

	WeeklyMenu *weekly = new WeeklyMenu(0, "weekly program edit", conf);
	weekly->setAllBGColor(paletteBackgroundColor());
	weekly->setAllFGColor(paletteForegroundColor());

	connect(time_edit, SIGNAL(goDx()), weekly, SLOT(show()));
	connect(time_edit, SIGNAL(goDx()), weekly, SLOT(raise()));
	connect(time_edit, SIGNAL(goDx()), time_edit, SLOT(hide()));

	connect(weekly, SIGNAL(Closed()), time_edit, SLOT(show()));
	connect(weekly, SIGNAL(Closed()), time_edit, SLOT(raise()));
	connect(weekly, SIGNAL(Closed()), weekly, SLOT(hide()));

	connect(weekly, SIGNAL(programClicked(int)), this, SLOT(show()));
	connect(weekly, SIGNAL(programClicked(int)), this, SLOT(raise()));
	connect(weekly, SIGNAL(programClicked(int)), weekly, SLOT(hide()));

	OpenFrameSender *frame_sender = new OpenFrameSender(date_edit, time_edit, weekly, this);
}

OpenFrameSender::OpenFrameSender(DateEditMenu *_date_edit, TimeEditMenu *_time_edit, WeeklyMenu *_program_menu, QObject *parent)
	: QObject(parent)
{
	date_edit = _date_edit;
	time_edit = _time_edit;
	program_menu = _program_menu;
	connect(program_menu, SIGNAL(programClicked(int)), this, SLOT(holidaySettingsEnd(int)));
}

OpenFrameSender::OpenFrameSender(WeeklyMenu *_program_menu, QObject *parent)
{
	program_menu = _program_menu;
	connect(program_menu, SIGNAL(programClicked(int)), this, SLOT(weekSettingsEnd(int)));
}

void OpenFrameSender::holidaySettingsEnd(int program)
{
	QDate date = date_edit->date();
	QTime time = time_edit->time();
	if (!th_regulator_where.isEmpty())
	{
		// TODO: send frame Open
		//
	}
	else
		qDebug("[TERMO] You are trying to send a frame with no address!!");
}

void OpenFrameSender::weekSettingsEnd(int program)
{
	// TODO: send fram Open
}

void OpenFrameSender::setAddress(QString where)
{
	th_regulator_where = where;
}
