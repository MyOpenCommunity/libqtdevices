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
#include "thermalregulator.h"
#include "xml_functions.h"
#include "main.h" // bt_global::config
#include "bann1_button.h"
#include "skinmanager.h"
#include "probe_device.h"
#include "devices_cache.h"
#include "fontmanager.h"
#include "state_button.h"
#include "scaleconversion.h"
#include "icondispatcher.h"

#include <QVariant>
#include <QRegExp>
#include <QLabel>


enum
{
#ifdef CONFIG_BTOUCH
	TERMO_99Z = 66,
	TERMO_4Z = 68,
	TERMO_99Z_PROBE = 22,
	TERMO_99Z_PROBE_FANCOIL = 52,
	TERMO_4Z_PROBE = 53,
	TERMO_4Z_PROBE_FANCOIL = 54,
	TERMO_NC_EXTPROBE = 55,
	TERMO_NC_PROBE = 56,
#else
	TERMO_99Z = 8011,
	TERMO_4Z = 8041,
	TERMO_99Z_PROBE = 8012,
	TERMO_99Z_PROBE_FANCOIL = 8012,
	TERMO_4Z_PROBE = 8042,
	TERMO_4Z_PROBE_FANCOIL = 8042,
	TERMO_NC_EXTPROBE = 8021,
	TERMO_NC_PROBE = 8031,
#endif
};

namespace
{
	QLabel *getLabelWithPixmap(const QString &img, QWidget *parent, int alignment)
	{
		QLabel *tmp = new QLabel(parent);
		tmp->setPixmap(*bt_global::icons_cache.getIcon(img));
		tmp->setAlignment((Qt::Alignment)alignment);
		return tmp;
	}
}


NavigationPage *getThermalPage(ThermalPageID id, QDomNode n, QString ind_centrale, int openserver_id, TemperatureScale scale, banner *bann)
{
	NavigationPage *p = 0;
	QString simple_address = getTextChild(n, "where");
	QString where_composed;
	if (id != fs_4z_thermal_regulator && id != fs_99z_thermal_regulator && !simple_address.isEmpty())
		where_composed = simple_address + "#" + ind_centrale;
#ifdef CONFIG_BTOUCH
	QDomNode page_node = n;
#else
	QDomNode page_node = getPageNodeFromChildNode(n, "lnk_pageID");
#endif

	ThermalDevice *thermal_device = 0;
	switch (id)
	{
	case fs_4z_probe:
	{
		ControlledProbeDevice *dev = bt_global::add_device_to_cache(new ControlledProbeDevice(where_composed,
			ind_centrale, simple_address, ControlledProbeDevice::CENTRAL_4ZONES, ControlledProbeDevice::NORMAL, openserver_id));

		QString thermr_where = QString("0#") + ind_centrale;

		thermal_device = bt_global::add_device_to_cache(new ThermalDevice4Zones(thermr_where, openserver_id));
		p = new PageProbe(n, dev, thermal_device, scale);
	}
		break;
	case fs_99z_probe:
	{
		ControlledProbeDevice *dev = bt_global::add_device_to_cache(new ControlledProbeDevice(simple_address,
			ind_centrale, simple_address, ControlledProbeDevice::CENTRAL_99ZONES, ControlledProbeDevice::NORMAL, openserver_id));

		QString thermr_where = ind_centrale;
		thermal_device = bt_global::add_device_to_cache(new ThermalDevice99Zones(thermr_where, openserver_id));
		p = new PageProbe(n, dev, thermal_device, scale);
	}
		break;
	case fs_4z_fancoil:
	{
		ControlledProbeDevice *dev = bt_global::add_device_to_cache(new ControlledProbeDevice(where_composed,
			ind_centrale, simple_address, ControlledProbeDevice::CENTRAL_4ZONES, ControlledProbeDevice::FANCOIL, openserver_id));

		QString thermr_where = QString("0#") + ind_centrale;
		thermal_device = bt_global::add_device_to_cache(new ThermalDevice4Zones(thermr_where, openserver_id));
		p = new PageFancoil(n, dev, thermal_device, scale);
	}
		break;
	case fs_99z_fancoil:
	{
		ControlledProbeDevice *dev = bt_global::add_device_to_cache(new ControlledProbeDevice(simple_address,
			ind_centrale, simple_address, ControlledProbeDevice::CENTRAL_99ZONES, ControlledProbeDevice::FANCOIL, openserver_id));

		QString thermr_where = ind_centrale;
		thermal_device = bt_global::add_device_to_cache(new ThermalDevice99Zones(thermr_where, openserver_id));
		p = new PageFancoil(n, dev, thermal_device, scale);
	}
		break;
	case fs_4z_thermal_regulator:
		thermal_device = bt_global::add_device_to_cache(new ThermalDevice4Zones(QString("0#") + ind_centrale, openserver_id));
		p = new PageTermoReg4z(page_node, static_cast<ThermalDevice4Zones*>(thermal_device));
		break;
	case fs_99z_thermal_regulator:
		thermal_device = bt_global::add_device_to_cache(new ThermalDevice99Zones(ind_centrale, openserver_id));
		p = new PageTermoReg99z(page_node, static_cast<ThermalDevice99Zones*>(thermal_device));
		break;
	default:
		qFatal("Unknown banner type %d on getPage", id);
	}

	if (thermal_device->isConnected())
		bann->connectionUp();
	else
		bann->connectionDown();

	QObject::connect(thermal_device, SIGNAL(connectionDown()), bann, SLOT(connectionDown()));
	QObject::connect(thermal_device, SIGNAL(connectionUp()), bann, SLOT(connectionUp()));
	return p;
}


PlantMenu::PlantMenu(const QDomNode &conf) : BannerPage(0)
{
	SkinContext cxt(getTextChild(conf, "cid").toInt());

	buildPage(getTextChild(conf, "descr"));
	openserver_id = 0;
	loadItems(conf);
}

#ifdef CONFIG_BTOUCH
void PlantMenu::loadItems(const QDomNode &conf)
{
	QDomNode thermr_address = conf.namedItem("ind_centrale");
	if (thermr_address.isNull())
		ind_centrale = "0";
	else
		ind_centrale = thermr_address.toElement().text();

	QDomNode n = conf.firstChild();
	NavigationPage *first = 0, *prev = 0;
	while (!n.isNull())
	{
		if (n.nodeName().contains(QRegExp("item(\\d\\d?)")))
		{
			NavigationPage *pg = 0;

			SkinContext context(getTextChild(n, "cid").toInt());

			int id = n.namedItem("id").toElement().text().toInt();
			switch (id)
			{
				case TERMO_99Z:
					pg = addMenuItem(n, bt_global::skin->getImage("regulator"), fs_99z_thermal_regulator);
					break;
				case TERMO_4Z:
					pg = addMenuItem(n, bt_global::skin->getImage("regulator"), fs_4z_thermal_regulator);
					break;
				case TERMO_99Z_PROBE:
					pg = addMenuItem(n, bt_global::skin->getImage("zone"), fs_99z_probe);
					break;
				case TERMO_99Z_PROBE_FANCOIL:
					pg = addMenuItem(n, bt_global::skin->getImage("zone"), fs_99z_fancoil);
					break;
				case TERMO_4Z_PROBE:
					pg = addMenuItem(n, bt_global::skin->getImage("zone"), fs_4z_probe);
					break;
				case TERMO_4Z_PROBE_FANCOIL:
					pg = addMenuItem(n, bt_global::skin->getImage("zone"), fs_4z_fancoil);
					break;
			}

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
#else
void PlantMenu::loadItems(const QDomNode &config_node)
{
	// find thermal regulator address
	foreach (const QDomNode& item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();

		if (id == TERMO_4Z)
		{
			ind_centrale = getTextChild(item, "where");
			openserver_id = getTextChild(item, "openserver_id").toInt();
			break;
		}
		else if (id == TERMO_99Z)
		{
			ind_centrale = "0";
			openserver_id = getTextChild(item, "openserver_id").toInt();
			break;
		}
	}


	NavigationPage *first = 0, *prev = 0;
	foreach (const QDomNode& item, getChildren(config_node, "item"))
	{
		SkinContext context(getTextChild(item, "cid").toInt());
		int id = getTextChild(item, "id").toInt();

		NavigationPage *pg = 0;

		QString icon = bt_global::skin->getImage("central_icon");
		bool fancoil = getTextChild(item, "fancoil").toInt();

		switch (id)
		{
		case TERMO_99Z:
			pg = addMenuItem(item, icon, fs_99z_thermal_regulator);
			break;
		case TERMO_4Z:
			pg = addMenuItem(item, icon, fs_4z_thermal_regulator);
			break;
#ifdef CONFIG_BTOUCH
		case TERMO_99Z_PROBE:
			pg = addMenuItem(item, icon, fs_99z_probe);
			break;
		case TERMO_99Z_PROBE_FANCOIL:
			pg = addMenuItem(item, icon, fs_99z_fancoil);
			break;
		case TERMO_4Z_PROBE:
			pg = addMenuItem(item, icon, fs_4z_probe);
			break;
		case TERMO_4Z_PROBE_FANCOIL:
			pg = addMenuItem(item, icon, fs_4z_fancoil);
			break;
#else
		case TERMO_99Z_PROBE:
			pg = addMenuItem(item, icon, fancoil ? fs_99z_fancoil : fs_99z_probe);
			break;
		case TERMO_4Z_PROBE:
			pg = addMenuItem(item, icon, fancoil ? fs_4z_fancoil : fs_4z_probe);
			break;
#endif
		}

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

	connect(prev, SIGNAL(downClick()), first, SLOT(showPage()));
	connect(first, SIGNAL(upClick()), prev, SLOT(showPage()));
}
#endif

banner *PlantMenu::getBanner(const QDomNode &item_node)
{
	return NULL;
}

NavigationPage *PlantMenu::addMenuItem(QDomNode n, QString central_icon, ThermalPageID type)
{
	/*
	 * Create little banner in selection menu.
	 */
	BannSinglePuls *bp = new BannSinglePuls(this);
	bp->initBanner(bt_global::skin->getImage("forward"), central_icon, getTextChild(n, "descr"));
	page_content->appendBanner(bp);

	/*
	 * Create page in detail menu.
	 */
	TemperatureScale scale = static_cast<TemperatureScale>((*bt_global::config)[TEMPERATURE_SCALE].toInt());
	NavigationPage *p = getThermalPage(type, n, ind_centrale, openserver_id, scale, bp);
	bp->connectRightButton(p);
	connect(bp, SIGNAL(pageClosed()), SLOT(showPage()));

	return p;
}


ThermalNavigation::ThermalNavigation(QWidget *parent) : AbstractNavigationBar(parent)
{
	// For now, it used only in BTouch code.
#ifdef LAYOUT_BTOUCH
	createButton(bt_global::skin->getImage("back"), SIGNAL(backClick()), 0);
	createButton(bt_global::skin->getImage("ok"), SIGNAL(forwardClick()), 3);
#endif
}


NavigationPage::NavigationPage()
	: nav_bar(0)
{
	content.setLayout(&main_layout);
	main_layout.setSpacing(0);
	main_layout.setContentsMargins(0, 0, 0, 0);
}

NavigationBar *NavigationPage::createNavigationBar(const QString &icon, int title_height)
{
#ifdef LAYOUT_BTOUCH
	nav_bar = new NavigationBar(icon);
#else
	nav_bar = new NavigationBar();
#endif

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(backClick()));
	connect(nav_bar, SIGNAL(upClick()), SIGNAL(upClick()));
	connect(nav_bar, SIGNAL(downClick()), SIGNAL(downClick()));

	buildPage(&content, nav_bar, "", title_height);

	return nav_bar;
}


PageSimpleProbe::PageSimpleProbe(QDomNode n, TemperatureScale scale)
	: temp_scale(scale)
{
	QLabel *descr_label = new QLabel(getTextChild(n, "descr"));
	descr_label->setFont(bt_global::font->get(FontManager::TEXT));
	descr_label->setAlignment(Qt::AlignHCenter);

	main_layout.addWidget(descr_label);
	main_layout.setContentsMargins(0, 0, 0, 0);
	main_layout.setSpacing(0);

	temp_label = new QLabel(this);
	temp_label->setFont(bt_global::font->get(FontManager::PROBE_TEMPERATURE));
	temp_label->setAlignment(Qt::AlignHCenter);

	main_layout.addWidget(temp_label);
	main_layout.setAlignment(Qt::AlignHCenter);

	setTemperature(1235);

	createNavigationBar(bt_global::skin->getImage("probe_manual"));
#ifdef LAYOUT_BTOUCH
	toggle_mode = nav_bar->forward_button;
#else
	toggle_mode = new BtButton;
	toggle_mode->setImage(bt_global::skin->getImage("probe_manual"));
#endif

	toggle_mode->hide();
}

void PageSimpleProbe::setTemperature(unsigned temp)
{
	switch (temp_scale)
	{
	case CELSIUS:
		temp_label->setText(celsiusString(bt2Celsius(temp)));
		break;
	case FAHRENHEIT:
		temp_label->setText(fahrenheitString(bt2Fahrenheit(temp)));
		break;
	default:
		qWarning("BannSimpleProbe: unknown temperature scale, defaulting to celsius");
		temp_label->setText(celsiusString(temp));
	}
}

void PageSimpleProbe::valueReceived(const DeviceValues &values_list)
{
	if (!values_list.contains(ControlledProbeDevice::DIM_TEMPERATURE))
		return;

	setTemperature(values_list[ControlledProbeDevice::DIM_TEMPERATURE].toInt());
}


PageProbe::PageProbe(QDomNode n, ControlledProbeDevice *_dev, ThermalDevice *thermo_reg,
	TemperatureScale scale) : PageSimpleProbe(n, scale),
	delta_setpoint(false),
	setpoint_delta(5)
{
	probe_icon_auto = bt_global::skin->getImage("probe_auto");
	probe_icon_manual = bt_global::skin->getImage("probe_manual");

	status = AUTOMATIC;
	probe_type = thermo_reg->type();

	dev = _dev;

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
	connect(toggle_mode, SIGNAL(clicked()), SLOT(changeStatus()));
	//install compressor

	QHBoxLayout *hbox = new QHBoxLayout();

	btn_minus = new BtButton(this);
	btn_minus->setImage(bt_global::skin->getImage("minus"));
	btn_minus->hide();
	btn_minus->setAutoRepeat(true);
	connect(btn_minus, SIGNAL(clicked()), SLOT(decSetpoint()));
#ifdef LAYOUT_TOUCHX
	hbox->addStretch(1);
#endif
	hbox->addWidget(btn_minus);

	setpoint_label = new QLabel(this);
	setpoint_label->setFont(bt_global::font->get(FontManager::PROBE_SETPOINT));
	setpoint_label->setAlignment(Qt::AlignHCenter);
	setpoint_label->setProperty("SecondFgColor", true);

	icon_antifreeze = getLabelWithPixmap(bt_global::skin->getImage("probe_antifreeze"), this, Qt::AlignHCenter);
	hbox->addWidget(icon_antifreeze, 1, Qt::AlignCenter);

	icon_off = getLabelWithPixmap(bt_global::skin->getImage("probe_off"), this, Qt::AlignHCenter);
	hbox->addWidget(icon_off, 1, Qt::AlignCenter);

	hbox->addWidget(setpoint_label, 1, Qt::AlignVCenter);
	btn_plus = new BtButton(this);
	btn_plus->setImage(bt_global::skin->getImage("plus"));
	btn_plus->hide();
	btn_plus->setAutoRepeat(true);
	connect(btn_plus, SIGNAL(clicked()), SLOT(incSetpoint()));
	hbox->addWidget(btn_plus);
#ifdef LAYOUT_TOUCHX
	hbox->addStretch(1);
#endif

	main_layout.addLayout(hbox);

	local_temp_label = new QLabel(this);

	local_temp_label->setFont(bt_global::font->get(FontManager::TEXT));
	local_temp_label->setAlignment(Qt::AlignHCenter);

	main_layout.addWidget(local_temp_label);

	// avoid moving of fancoil buttons bar
	main_layout.addStretch();

	// layout for the control buttons and fancoil icons
	bottom_icons.setAlignment(Qt::AlignHCenter);
	bottom_icons.setSpacing(10);

	main_layout.addLayout(&bottom_icons);

#ifdef LAYOUT_TOUCHX
	bottom_icons.addWidget(toggle_mode);
	bottom_icons.setContentsMargins(0, 0, 0, 60);
#endif

	switch (temp_scale)
	{
	case CELSIUS:
		maximum_manual_temp = bt2Celsius(thermo_reg->maximumTemp());
		minimum_manual_temp = bt2Celsius(thermo_reg->minimumTemp());
		// bticino absolute minimum temperature is -23.5 Celsius
		setpoint = -235;
		break;
	case FAHRENHEIT:
		maximum_manual_temp = bt2Fahrenheit(thermo_reg->maximumTemp());
		minimum_manual_temp = bt2Fahrenheit(thermo_reg->minimumTemp());
		setpoint = -103;
		break;
	default:
		qWarning("BannProbe ctor: wrong scale, defaulting to celsius");
		maximum_manual_temp = bt2Celsius(thermo_reg->maximumTemp());
		minimum_manual_temp = bt2Celsius(thermo_reg->minimumTemp());
		setpoint = -235;
		temp_scale = CELSIUS;
	}

	local_temp = "0";
	isOff = false;
	isAntigelo = false;

	updatePointLabel();
	updateControlState();
}

void PageProbe::setDeviceToManual()
{
	unsigned bt_temp;
	switch (temp_scale)
	{
	case CELSIUS:
		bt_temp = celsius2Bt(setpoint);
		break;
	case FAHRENHEIT:
		bt_temp = fahrenheit2Bt(setpoint);
		break;
	default:
		qWarning("BannProbe::setDeviceToManual: unknown scale, defaulting to celsius");
		bt_temp = celsius2Bt(setpoint);
	}
	dev->setManual(bt_temp);
}

void PageProbe::changeStatus()
{
	if (status == AUTOMATIC)
		setDeviceToManual();
	else
		dev->setAutomatic();
}

void PageProbe::incSetpoint()
{
	if (setpoint >= maximum_manual_temp)
		return;
	else
		setpoint += setpoint_delta;
	updatePointLabel();
	setDeviceToManual();
	delta_setpoint = true;
}

void PageProbe::decSetpoint()
{
	if (setpoint <= minimum_manual_temp)
		return;
	else
		setpoint -= setpoint_delta;
	updatePointLabel();
	setDeviceToManual();
	delta_setpoint = true;
}

void PageProbe::updatePointLabel()
{
	switch (temp_scale)
	{
	case CELSIUS:
		setpoint_label->setText(celsiusString(setpoint));
		break;
	case FAHRENHEIT:
		setpoint_label->setText(fahrenheitString(setpoint));
		break;
	default:
		qWarning("BannProbe: unknown temperature scale, defaulting to celsius");
		setpoint_label->setText(celsiusString(setpoint));
	}
}

void PageProbe::updateControlState()
{
	btn_minus->setVisible(status == MANUAL && probe_type == THERMO_Z99 && !isOff && !isAntigelo);
	btn_plus->setVisible(status == MANUAL && probe_type == THERMO_Z99 && !isOff && !isAntigelo);
	setpoint_label->setVisible(!isOff && !isAntigelo);
	local_temp_label->setVisible(!isOff && !isAntigelo && local_temp != "0");
	icon_off->setVisible(isOff);
	icon_antifreeze->setVisible(isAntigelo);
#ifdef LAYOUT_BTOUCH
	toggle_mode->setVisible(probe_type == THERMO_Z99 && !isOff && !isAntigelo);
#else
	// TODO needs to be checked after ticket #17 is resolved
	toggle_mode->setVisible(probe_type == THERMO_Z99);
#endif
	local_temp_label->setText(local_temp);
}

void PageProbe::valueReceived(const DeviceValues &values_list)
{
	bool update = false;

	if (values_list.contains(ControlledProbeDevice::DIM_SETPOINT))
	{
		unsigned sp = values_list[ControlledProbeDevice::DIM_SETPOINT].toInt();

		if (delta_setpoint)
		{
			switch (temp_scale)
			{
			case FAHRENHEIT:
				sp = fahrenheit2Bt(setpoint);
				break;
			default:
				qWarning("BannProbe: unknown temperature scale, defaulting to celsius");
				// fall through
			case CELSIUS:
				sp = celsius2Bt(setpoint);
				break;
			}

			delta_setpoint = false;
		}

		switch (temp_scale)
		{
		case CELSIUS:
			setpoint = bt2Celsius(sp);
			break;
		case FAHRENHEIT:
			setpoint = bt2Fahrenheit(sp);
			break;
		default:
			qWarning("BannProbe: unknown temperature scale, defaulting to celsius");
			setpoint = bt2Celsius(sp);
		}

		updatePointLabel();
	}

	if (values_list.contains(ControlledProbeDevice::DIM_LOCAL_STATUS))
	{
		int stat = values_list[ControlledProbeDevice::DIM_LOCAL_STATUS].toInt();

		if (stat == ControlledProbeDevice::ST_NORMAL)
		{
			int off = values_list[ControlledProbeDevice::DIM_OFFSET].toInt();

			isOff = false;
			isAntigelo = false;
			if (off != 0)
				local_temp.sprintf("%+d", off);
			else
				local_temp = "0";
		}
		else if (stat == ControlledProbeDevice::ST_OFF)
		{
			local_temp = "0";
			isOff = true;
			isAntigelo = false;
		}
		else if (stat == ControlledProbeDevice::ST_PROTECTION)
		{
			local_temp = "0";
			isOff = false;
			isAntigelo = true;
		}

		update = true;
	}

	if (values_list.contains(ControlledProbeDevice::DIM_STATUS))
	{
		switch (values_list[ControlledProbeDevice::DIM_STATUS].toInt())
		{
		case ControlledProbeDevice::ST_MANUAL:
			status = MANUAL;
			toggle_mode->setImage(probe_icon_auto);
			update = true;
			break;
		case ControlledProbeDevice::ST_AUTO:
			status = AUTOMATIC;
			toggle_mode->setImage(probe_icon_manual);
			update = true;
			break;
		case ControlledProbeDevice::ST_PROTECTION:
			isOff = false;
			isAntigelo = true;
			update = true;
			break;
		case ControlledProbeDevice::ST_OFF:
			isOff = true;
			isAntigelo = false;
			update = true;
			break;
		default:
			update = false;
			break;
		}
	}

	if (update)
		updateControlState();

	PageSimpleProbe::valueReceived(values_list);
}


PageFancoil::PageFancoil(QDomNode n, ControlledProbeDevice *_dev, ThermalDevice *thermo_reg,
	TemperatureScale scale) : PageProbe(n, _dev, thermo_reg, scale), fancoil_buttons(this)
{
	dev = _dev;
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));

	createFancoilButtons();
	fancoil_buttons.setExclusive(true);
	setFancoilStatus(0);

	connect(&fancoil_buttons, SIGNAL(buttonClicked(int)), SLOT(handleFancoilButtons(int)));
}

void PageFancoil::createFancoilButtons()
{
	for (int id = 0; id < 4; ++id)
	{
		QString path = bt_global::skin->getImage(QString("fan_%1_off").arg(id + 1));
		QString path_pressed = bt_global::skin->getImage(QString("fan_%1_on").arg(id + 1));
		StateButton *btn = new StateButton(this);
		btn->setOnOff();
		btn->setOffImage(path);
		btn->setOnImage(path_pressed);
		btn->setCheckable(true);

		bottom_icons.addWidget(btn);
		fancoil_buttons.addButton(btn, id);

		speed_to_btn_tbl[(id + 1) % 4] = id;
		btn_to_speed_tbl[id] = (id + 1) % 4;
	}
}

void PageFancoil::setFancoilStatus(int status)
{
	fancoil_buttons.button(status)->setChecked(true);
}

void PageFancoil::handleFancoilButtons(int pressedButton)
{
	dev->setFancoilSpeed(btn_to_speed_tbl[pressedButton]);
	/*
	 * Read back the set value to force an update to other devices
	 * monitoring this dimension.
	 *
	 * This code was present in the original termopage. I leave it here to avoid breaking something.
	 */
	dev->requestFancoilStatus();
}

void PageFancoil::valueReceived(const DeviceValues &values_list)
{
	if (values_list.contains(ControlledProbeDevice::DIM_FANCOIL_STATUS))
	{
		int spd = values_list[ControlledProbeDevice::DIM_FANCOIL_STATUS].toInt();

		// Set the fancoil Button in the buttons bar
		if (speed_to_btn_tbl.contains(spd))
			setFancoilStatus(speed_to_btn_tbl[spd]);
		else
			qDebug("Fancoil speed val out of range (%d)", spd);
	}

	PageProbe::valueReceived(values_list);
}

