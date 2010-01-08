
#include "bann_thermal_regulation.h"
#include "fontmanager.h" // bt_global::font
#include "btbutton.h"
#include "scaleconversion.h"
#include "icondispatcher.h" // bt_global::icons_cache
#include "devices_cache.h" // bt_global::devices_cache
#include "thermal_device.h"
#include "probe_device.h"
#include "datetime.h"
#include "xml_functions.h"
#include "thermalmenu.h"
#include "main.h" // bt_global::config
#include "navigation_bar.h"
#include "bannercontent.h"
#include "skinmanager.h"

#include <QVariant>
#include <QLabel>
#include <QDebug>

enum
{
	BANNER_PROGRAMS = 11,
	BANNER_SCENARIOS = 12,
};

QLabel *getLabelWithPixmap(const QString &img, QWidget *parent, int alignment)
{
	QLabel *tmp = new QLabel(parent);
	tmp->setPixmap(*bt_global::icons_cache.getIcon(img));
	tmp->setAlignment((Qt::Alignment)alignment);
	return tmp;
}

// Helper page for the settings list
class SettingsPage : public BannerPage
{
public:
	SettingsPage(QDomNode node, QWidget *parent = 0);

	void appendBanner(banner *b);
	void resetIndex();
};

SettingsPage::SettingsPage(QDomNode n, QWidget *parent)
	: BannerPage(parent)
{
	buildPage(getTextChild(n, "descr"));
}

void SettingsPage::appendBanner(banner *b)
{
	page_content->appendBanner(b);
}

void SettingsPage::resetIndex()
{
	page_content->resetIndex();
}

ThermalNavigation::ThermalNavigation(QWidget *parent)
	: QWidget(parent)
{
	QHBoxLayout *l = new QHBoxLayout(this);

	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);

	BtButton *back = new BtButton();
	connect(back, SIGNAL(clicked()), SIGNAL(backClicked()));
	back->setImage(bt_global::skin->getImage("back"));
	l->addWidget(back);

	l->addStretch(1);

	BtButton *ok = new BtButton();
	connect(ok, SIGNAL(clicked()), SIGNAL(okClicked()));
	ok->setImage(bt_global::skin->getImage("ok"));
	l->addWidget(ok);
}


NavigationPage *getPage(BannID id, QDomNode n, QString ind_centrale, TemperatureScale scale)
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

	switch (id)
	{
	case fs_nc_probe:
		p = new PageSimpleProbe(n, scale);
		break;
	case fs_4z_probe:
		{
			ControlledProbeDevice *dev = static_cast<ControlledProbeDevice *>(
					bt_global::devices_cache.get_temperature_probe_controlled(
						where_composed, THERMO_Z4, false, ind_centrale, simple_address));
			QString thermr_where = QString("0#") + ind_centrale;
			ThermalDevice *thermo_reg = static_cast<ThermalDevice *>(
					bt_global::devices_cache.get_thermal_regulator(thermr_where, THERMO_Z4));
			p = new PageProbe(n, dev, thermo_reg, scale);
		}
		break;
	case fs_99z_probe:
		{
			ControlledProbeDevice *dev = static_cast<ControlledProbeDevice *>(
					bt_global::devices_cache.get_temperature_probe_controlled(
						simple_address, THERMO_Z99, false, ind_centrale, simple_address));
			QString thermr_where = ind_centrale;
			ThermalDevice *thermo_reg = static_cast<ThermalDevice *>(
					bt_global::devices_cache.get_thermal_regulator(thermr_where, THERMO_Z99));
			p = new PageProbe(n, dev, thermo_reg, scale);
		}
		break;
	case fs_4z_fancoil:
		{
			ControlledProbeDevice *dev = static_cast<ControlledProbeDevice *>(
					bt_global::devices_cache.get_temperature_probe_controlled(
						where_composed, THERMO_Z4, true, ind_centrale, simple_address));
			QString thermr_where = QString("0#") + ind_centrale;
			ThermalDevice *thermo_reg = static_cast<ThermalDevice *>(
					bt_global::devices_cache.get_thermal_regulator(thermr_where, THERMO_Z4));
			p = new PageFancoil(n, dev, thermo_reg, scale);
		}
		break;
	case fs_99z_fancoil:
		{
			ControlledProbeDevice *dev = static_cast<ControlledProbeDevice *>(
					bt_global::devices_cache.get_temperature_probe_controlled(
						simple_address, THERMO_Z99, true, ind_centrale, simple_address));
			QString thermr_where = ind_centrale;
			ThermalDevice *thermo_reg = static_cast<ThermalDevice *>(
					bt_global::devices_cache.get_thermal_regulator(thermr_where, THERMO_Z99));
			p = new PageFancoil(n, dev, thermo_reg, scale);
		}
		break;
	case fs_4z_thermal_regulator:
	{
		where_composed = QString("0#") + ind_centrale;
		ThermalDevice4Zones *dev = static_cast<ThermalDevice4Zones *>(
			bt_global::devices_cache.get_thermal_regulator(where_composed, THERMO_Z4));
		p = new PageTermoReg4z(page_node, dev);
	}
		break;
	case fs_99z_thermal_regulator:
	{
		where_composed = ind_centrale;
		ThermalDevice99Zones *dev = static_cast<ThermalDevice99Zones *>(
			bt_global::devices_cache.get_thermal_regulator(where_composed, THERMO_Z99));
		p = new PageTermoReg99z(page_node, dev);
	}
		break;
	default:
		qFatal("Unknown banner type %d on bannfullscreen", id);
	}

	return p;
}

NavigationPage::NavigationPage()
	: nav_bar(0)
{
	content.setLayout(&main_layout);
	main_layout.setSpacing(0);
	main_layout.setContentsMargins(0, 0, 0, 0);
}

NavigationBar *NavigationPage::createNavigationBar(const QString &icon)
{
	nav_bar = new NavigationBar(icon);

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(backClick()));
	connect(nav_bar, SIGNAL(upClick()), SIGNAL(upClick()));
	connect(nav_bar, SIGNAL(downClick()), SIGNAL(downClick()));

	buildPage(&content, nav_bar, "", TITLE_HEIGHT);

	return nav_bar;
}

PageSimpleProbe::PageSimpleProbe(QDomNode n, TemperatureScale scale)
	: temp_scale(scale)
{
	descr_label = new QLabel(this);
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
	setDescription(n.namedItem("descr").toElement().text());

	createNavigationBar(bt_global::skin->getImage("probe_manual"));
	nav_bar->forward_button->hide();
}

void PageSimpleProbe::setDescription(const QString &descr)
{
	descr_label->setText(descr);
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

void PageSimpleProbe::status_changed(const StatusList &sl)
{
	if (!sl.contains(ControlledProbeDevice::DIM_TEMPERATURE))
		return;

	setTemperature(sl[ControlledProbeDevice::DIM_TEMPERATURE].toInt());
}


PageProbe::PageProbe(QDomNode n, ControlledProbeDevice *_dev, ThermalDevice *thermo_reg,
	TemperatureScale scale) : PageSimpleProbe(n, scale),
	delta_setpoint(false),
	setpoint_delay(2000),
	setpoint_delta(5)
{
	probe_icon_auto = bt_global::skin->getImage("probe_auto");
	probe_icon_manual = bt_global::skin->getImage("probe_manual");

	status = AUTOMATIC;
	probe_type = thermo_reg->type();

	conf_root = n;
	dev = _dev;

	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(changeStatus()));
	//install compressor
	dev->installFrameCompressor(setpoint_delay);

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

void PageProbe::inizializza()
{
	dev->requestStatus();
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
	nav_bar->forward_button->setVisible(probe_type == THERMO_Z99 && !isOff && !isAntigelo);
	local_temp_label->setText(local_temp);
}

void PageProbe::status_changed(const StatusList &sl)
{
	bool update = false;

	if (sl.contains(ControlledProbeDevice::DIM_SETPOINT))
	{
		unsigned sp = sl[ControlledProbeDevice::DIM_SETPOINT].toInt();

		if (delta_setpoint)
		{
			switch (temp_scale)
			{
			case FAHRENHEIT:
				sp = fahrenheit2Bt(sp);
				break;
			default:
				qWarning("BannProbe: unknown temperature scale, defaulting to celsius");
				// fall through
			case CELSIUS:
				sp = celsius2Bt(sp);
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

	if (sl.contains(ControlledProbeDevice::DIM_LOCAL_STATUS))
	{
		int stat = sl[ControlledProbeDevice::DIM_LOCAL_STATUS].toInt();

		if (stat == ControlledProbeDevice::ST_NORMAL)
		{
			int off = sl[ControlledProbeDevice::DIM_OFFSET].toInt();

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

	if (sl.contains(ControlledProbeDevice::DIM_STATUS))
	{
		switch (sl[ControlledProbeDevice::DIM_STATUS].toInt())
		{
		case ControlledProbeDevice::ST_MANUAL:
			status = MANUAL;
			nav_bar->forward_button->setImage(probe_icon_auto);
			update = true;
			break;
		case ControlledProbeDevice::ST_AUTO:
			status = AUTOMATIC;
			nav_bar->forward_button->setImage(probe_icon_manual);
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

	PageSimpleProbe::status_changed(sl);
}

PageFancoil::PageFancoil(QDomNode n, ControlledProbeDevice *_dev, ThermalDevice *thermo_reg,
	TemperatureScale scale) : PageProbe(n, _dev, thermo_reg, scale), fancoil_buttons(this)
{
	dev = _dev;
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));

	createFancoilButtons();
	fancoil_buttons.setExclusive(true);
	setFancoilStatus(0);

	connect(&fancoil_buttons, SIGNAL(buttonClicked(int)), SLOT(handleFancoilButtons(int)));
}

void PageFancoil::createFancoilButtons()
{
	QHBoxLayout *hbox = new QHBoxLayout();

	hbox->setAlignment(Qt::AlignHCenter);
	hbox->setSpacing(10);
#ifdef LAYOUT_TOUCHX
	hbox->setContentsMargins(0, 0, 0, 60);
#endif

	for (int id = 0; id < 4; ++id)
	{
		QString path = bt_global::skin->getImage(QString("fan_%1_off").arg(id + 1));
		QString path_pressed = bt_global::skin->getImage(QString("fan_%1_on").arg(id + 1));
		BtButton *btn = new BtButton(this);
		btn->setImage(path);
		btn->setPressedImage(path_pressed);
		btn->setCheckable(true);

		hbox->addWidget(btn);
		fancoil_buttons.addButton(btn, id);

		speed_to_btn_tbl[(id + 1) % 4] = id;
		btn_to_speed_tbl[id] = (id + 1) % 4;
	}
	main_layout.insertLayout(-1, hbox);
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

void PageFancoil::status_changed(const StatusList &sl)
{
	if (sl.contains(ControlledProbeDevice::DIM_FANCOIL_STATUS))
	{
		int spd = sl[ControlledProbeDevice::DIM_FANCOIL_STATUS].toInt();

		// Set the fancoil Button in the buttons bar
		if (speed_to_btn_tbl.contains(spd))
			setFancoilStatus(speed_to_btn_tbl[spd]);
		else
			qDebug("Fancoil speed val out of range (%d)", spd);
	}

	PageProbe::status_changed(sl);
}

PageManual::PageManual(ThermalDevice *_dev, TemperatureScale scale)
	: temp_scale(scale), dev(_dev), setpoint_delta(5)
{
	descr_label = new QLabel(this);
	descr_label->setFont(bt_global::font->get(FontManager::TEXT));
	descr_label->setAlignment(Qt::AlignTop|Qt::AlignHCenter);

	content.setLayout(&main_layout);
	main_layout.setSpacing(0);
	main_layout.setContentsMargins(0, 0, 0, 10);
	main_layout.addWidget(descr_label);
	main_layout.addStretch(1);

	switch (temp_scale)
	{
	case CELSIUS:
		maximum_manual_temp = bt2Celsius(dev->maximumTemp());
		minimum_manual_temp = bt2Celsius(dev->minimumTemp());
		temp = 200;
		break;
	case FAHRENHEIT:
		maximum_manual_temp = bt2Fahrenheit(dev->maximumTemp());
		minimum_manual_temp = bt2Fahrenheit(dev->minimumTemp());
		temp = 680;
		break;
	default:
		qWarning("BannManual ctor: wrong scale, defaulting to celsius");
		maximum_manual_temp = bt2Celsius(dev->maximumTemp());
		minimum_manual_temp = bt2Celsius(dev->minimumTemp());
		temp = 200;
		temp_scale = CELSIUS;
	}

	temp_label = new QLabel(this);
	temp_label->setFont(bt_global::font->get(FontManager::REGULATOR_TEMPERATURE));
	temp_label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	temp_label->setProperty("SecondFgColor", true);
	QHBoxLayout *hbox = new QHBoxLayout();

	BtButton *btn = new BtButton(this);
	btn->setImage(bt_global::skin->getImage("minus"));
	btn->setAutoRepeat(true);
	connect(btn, SIGNAL(clicked()), this, SLOT(decSetpoint()));
	hbox->addWidget(btn);
	hbox->addStretch(1);

	hbox->addWidget(temp_label);

	hbox->addStretch(1);
	btn = new BtButton(this);
	btn->setImage(bt_global::skin->getImage("plus"));
	btn->setAutoRepeat(true);
	connect(btn, SIGNAL(clicked()), this, SLOT(incSetpoint()));
	hbox->addWidget(btn);

	main_layout.addLayout(hbox);

	nav_bar = new ThermalNavigation;
	buildPage(&content, nav_bar);

	connect(nav_bar, SIGNAL(okClicked()), SLOT(performAction()));
	connect(nav_bar, SIGNAL(backClicked()), SIGNAL(Closed()));

	connect(dev, SIGNAL(status_changed(const StatusList &)),
		SLOT(status_changed(const StatusList &)));

	setDescription(tr("Manual"));
	updateTemperature();
}

void PageManual::performAction()
{
	unsigned bt_temp;
	switch (temp_scale)
	{
	case CELSIUS:
		bt_temp = celsius2Bt(temp);
		break;
	case FAHRENHEIT:
		bt_temp = fahrenheit2Bt(temp);
		break;
	default:
		qWarning("BannManual::performAction: unknown scale, defaulting to celsius");
		bt_temp = celsius2Bt(temp);
	}
	emit(temperatureSelected(bt_temp));
}

void PageManual::incSetpoint()
{
	// FIXME: forse c'e' da modificare i controlli in caso di fahrenheit
	if (temp >= maximum_manual_temp)
		return;
	else
		temp += setpoint_delta;
	updateTemperature();
}

void PageManual::decSetpoint()
{
	if (temp <= minimum_manual_temp)
		return;
	else
		temp -= setpoint_delta;
	updateTemperature();
}

void PageManual::setDescription(const QString &descr)
{
	descr_label->setText(descr);
}

void PageManual::updateTemperature()
{
	switch (temp_scale)
	{
	case CELSIUS:
		temp_label->setText(celsiusString(temp));
		break;
	case FAHRENHEIT:
		temp_label->setText(fahrenheitString(temp));
		break;
	default:
		qWarning("BannSimpleProbe: unknown temperature scale, defaulting to Celsius");
		temp_label->setText(celsiusString(temp));
	}
}

void PageManual::status_changed(const StatusList &sl)
{
	// TODO check why only for 4-zone regulator
	if (dev->type() != THERMO_Z4)
		return;

	if (!sl.contains(ThermalDevice::DIM_TEMPERATURE))
		return;

	unsigned temperature = sl[ThermalDevice::DIM_TEMPERATURE].toInt();

	switch (temp_scale)
	{
	case CELSIUS:
		temp = bt2Celsius(temperature);
		break;
	case FAHRENHEIT:
		temp = bt2Fahrenheit(temperature);
		break;
	default:
		qWarning("BannSimpleProbe: unknown temperature scale, defaulting to celsius");
		temp = bt2Celsius(temperature);
	}

	updateTemperature();
}

PageManualTimed::PageManualTimed(ThermalDevice4Zones *_dev, TemperatureScale scale)
	: PageManual(_dev, scale),
	dev(_dev)
{
	time_edit = new BtTimeEdit(this);
	// TODO refactor widget creation
	main_layout.insertWidget(main_layout.count() - 1, time_edit);

	connect(dev, SIGNAL(status_changed(const StatusList &)),
		SLOT(status_changed(const StatusList &)));
	connect(nav_bar, SIGNAL(okClicked()), SLOT(performAction()));
}

void PageManualTimed::performAction()
{
	unsigned bt_temp;
	switch (temp_scale)
	{
	case CELSIUS:
		bt_temp = celsius2Bt(temp);
		break;
	case FAHRENHEIT:
		bt_temp = fahrenheit2Bt(temp);
		break;
	default:
		qWarning("BannManual::performAction: unknown scale, defaulting to celsius");
		bt_temp = celsius2Bt(temp);
	}
	emit(timeAndTempSelected(time_edit->time(), bt_temp));
}

void PageManualTimed::setMaxHours(int max)
{
	time_edit->setMaxHours(max);
}

void PageManualTimed::setMaxMinutes(int max)
{
	time_edit->setMaxMinutes(max);
}

PageSetDate::PageSetDate()
{
	content.setLayout(&main_layout);

	QLabel *top = new QLabel(this);
	top->setPixmap(*bt_global::icons_cache.getIcon(bt_global::skin->getImage("date_icon")));
	main_layout.addWidget(top, 0, Qt::AlignHCenter);

	date_edit = new BtDateEdit(this);
	main_layout.addWidget(date_edit);
	main_layout.setSpacing(0);
	main_layout.setContentsMargins(0, 0, 0, 10);

	ThermalNavigation *nav = new ThermalNavigation;
	buildPage(&content, nav);

	connect(nav, SIGNAL(okClicked()), SLOT(performAction()));
	connect(nav, SIGNAL(backClicked()), SIGNAL(Closed()));
}

QDate PageSetDate::date()
{
	return date_edit->date();
}

void PageSetDate::performAction()
{
	emit dateSelected(date());
}

PageSetTime::PageSetTime()
{
	content.setLayout(&main_layout);

	QLabel *top = new QLabel(this);
	top->setPixmap(*bt_global::icons_cache.getIcon(bt_global::skin->getImage("time_icon")));
	main_layout.addWidget(top, 0, Qt::AlignHCenter);

	time_edit = new BtTimeEdit(this);
	main_layout.addWidget(time_edit);
	main_layout.setSpacing(0);
	main_layout.setContentsMargins(0, 0, 0, 10);

	ThermalNavigation *nav = new ThermalNavigation;
	buildPage(&content, nav);

	connect(nav, SIGNAL(okClicked()), SLOT(performAction()));
	connect(nav, SIGNAL(backClicked()), SIGNAL(Closed()));
}

BtTime PageSetTime::time()
{
	return time_edit->time();
}

void PageSetTime::performAction()
{
	emit timeSelected(time());
}

PageSetDateTime::PageSetDateTime()
{
	content.setLayout(&main_layout);

	BtButton *program = new BtButton(this);
	program->setImage(bt_global::skin->getImage("settings"));

	date_edit = new BtDateEdit(this);
	time_edit = new BtTimeEdit(this);

	top_layout.addWidget(date_edit);
	top_layout.addStretch(1);
	top_layout.addWidget(time_edit);
	top_layout.setSpacing(20);

	main_layout.addLayout(&top_layout);
	main_layout.addWidget(program, 1, Qt::AlignRight|Qt::AlignVCenter);
	main_layout.setContentsMargins(40, 0, 40, 0);

	NavigationBar *nav = new NavigationBar;
	nav->displayScrollButtons(false);
	buildPage(&content, nav, "Change title dynamically", TITLE_HEIGHT);

	connect(program, SIGNAL(clicked()), SLOT(performAction()));
	connect(nav, SIGNAL(backClick()), SIGNAL(Closed()));
}

QDate PageSetDateTime::date()
{
	return date_edit->date();
}

BtTime PageSetDateTime::time()
{
	return time_edit->time();
}

void PageSetDateTime::performAction()
{
	emit dateTimeSelected(date(), time());
}

// match the order of enum Status in ThermalDevice
static QString status_icons_ids[ThermalDevice::ST_COUNT] =
{
	"regulator_off", "regulator_antifreeze", "regulator_manual", "regulator_manual_timed",
	"regulator_weekend", "regulator_program", "regulator_scenario", "regulator_holiday"
};

#ifdef CONFIG_BTOUCH

void parseBTouchProgramList(QDomNode conf_root, QString season, QString what, QMap<QString, QString> &entries)
{
	QDomElement program = getElement(conf_root, season + "/" + what);
	// The leaves we are looking for start with either "p" or "s"
	QString name = what.left(1);

	int index = 0;
	foreach (const QDomNode &node, getChildren(program, name))
	{
		QString text;
		if (node.isElement())
			text = node.toElement().text();

		entries[season + QString::number(++index)] = text;
	}
}

#else

void parseTouchXProgramList(QDomNode page, QMap<QString, QString> &entries)
{
	int index_summer = 0, index_winter = 0;
	foreach (const QDomNode &node, getChildren(page, "item"))
	{
		int cid = getTextChild(node, "cid").toInt();
		QString text = getTextChild(node, "descr");

		// TODO: must change config file to have different ID for winter/summer items
		if (cid == 11000 || cid == 12000)
			entries["winter" + QString::number(++index_winter)] = text;
		else
			entries["summer" + QString::number(++index_summer)] = text;
	}
}

#endif

PageTermoReg::PageTermoReg(QDomNode n)
{
	SkinContext context(getTextChild(n, "cid").toInt());

	icon_summer = bt_global::skin->getImage("summer_flat");
	icon_winter = bt_global::skin->getImage("winter_flat");

	for (int i = 0; i < ThermalDevice::ST_COUNT; ++i)
		status_icons.append(bt_global::skin->getImage(status_icons_ids[i]));

#ifdef CONFIG_BTOUCH
	// parse program/scenario list
	if (n.nodeName().contains(QRegExp("item(\\d\\d?)")) == 0)
	{
		qFatal("[TERMO] WeeklyMenu:wrong node in config file");
	}

	parseBTouchProgramList(n, "summer", "prog", programs);
	parseBTouchProgramList(n, "winter", "prog", programs);
	parseBTouchProgramList(n, "summer", "scen", scenarios);
	parseBTouchProgramList(n, "winter", "scen", scenarios);
#else
	// parse program/scenario list
	foreach (const QDomNode &item, getChildren(getPageNodeFromChildNode(n, "h_lnk_pageID"), "item"))
	{
		int id = getTextChild(item, "id").toInt();

		if (id == BANNER_PROGRAMS) // programs
			parseTouchXProgramList(getPageNodeFromChildNode(item, "lnk_pageID"), programs);
		else if (id == BANNER_SCENARIOS) // scenarios
			parseTouchXProgramList(getPageNodeFromChildNode(item, "lnk_pageID"), scenarios);
	}
#endif

	// Put a sensible default for the description
	QDomNode descr = n.namedItem("descr");
	QString description;
	if (!descr.isNull())
		description = descr.toElement().text();
	else
	{
		qDebug("[TERMO] PageTermoReg ctor: no description found, maybe wrong node conf?");
		description = "Wrong node";
	}
	description_label = new QLabel(this);
	description_label->setFont(bt_global::font->get(FontManager::REGULATOR_DESCRIPTION));
	description_label->setAlignment(Qt::AlignHCenter);
	description_label->setProperty("SecondFgColor", true);

	season_icon = getLabelWithPixmap(bt_global::skin->getImage("summer_flat"), this, Qt::AlignHCenter);

	mode_icon = getLabelWithPixmap(bt_global::skin->getImage("regulator"), this, Qt::AlignHCenter);

	main_layout.addWidget(mode_icon);
	main_layout.addWidget(description_label);
	main_layout.addWidget(season_icon);
	main_layout.addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Maximum));
	main_layout.setAlignment(Qt::AlignHCenter);

	date_edit = 0;
	time_edit = 0;
	date_time_edit = 0;
	program_choice = 0;
	temp_scale = static_cast<TemperatureScale>(bt_global::config[TEMPERATURE_SCALE].toInt());

	createNavigationBar(bt_global::skin->getImage("settings"));
	showDescription(description);
}

void PageTermoReg::showDescription(const QString &desc)
{
	description_label->setText(desc);
	description_label->setVisible(true);
}

void PageTermoReg::hideDescription()
{
	description_label->setVisible(false);
}

void PageTermoReg::status_changed(const StatusList &sl)
{
	ThermalDevice::Season season = ThermalDevice::SE_SUMMER;

	if (sl.contains(ThermalDevice::DIM_SEASON))
		season = static_cast<ThermalDevice::Season>(sl[ThermalDevice::DIM_SEASON].toInt());

	setSeason(season);

	int status = sl[ThermalDevice::DIM_STATUS].toInt();
	if (status < status_icons.count())
		mode_icon->setPixmap(*bt_global::icons_cache.getIcon(status_icons[status]));

	switch (status)
	{
	case ThermalDevice::ST_OFF:
		{
			hideDescription();
		}
		break;
	case ThermalDevice::ST_PROTECTION:
		{
			hideDescription();
		}
		break;
	case ThermalDevice::ST_MANUAL:
	case ThermalDevice::ST_MANUAL_TIMED:
		{
			unsigned temperature = sl[ThermalDevice::DIM_TEMPERATURE].toInt();
			// remember: stat_var::get_val() returns an int
			QString description;
			switch (temp_scale)
			{
			case CELSIUS:
				description = celsiusString(bt2Celsius(temperature));
				break;
			case FAHRENHEIT:
				description = fahrenheitString(bt2Fahrenheit(temperature));
				break;
			default:
				qWarning("TermoReg status_changed: unknown scale, defaulting to celsius");
				description = celsiusString(temperature);
			}
			showDescription(description);
		}
		break;
	case ThermalDevice::ST_PROGRAM:
		{
			// now search the description in the DOM
			int program = sl[ThermalDevice::DIM_PROGRAM].toInt();
			QString description;
			switch (season)
			{
			case ThermalDevice::SE_SUMMER:
				description = lookupProgramDescription("summer", "prog", program);
				break;
			case ThermalDevice::SE_WINTER:
				description = lookupProgramDescription("winter", "prog", program);
				break;
			}
			showDescription(description);
		}
		break;
	case ThermalDevice::ST_SCENARIO:
		{
			int scenario = sl[ThermalDevice::DIM_SCENARIO].toInt();
			QString description;
			switch (season)
			{
			case ThermalDevice::SE_SUMMER:
				description = lookupProgramDescription("summer", "scen", scenario);
				break;
			case ThermalDevice::SE_WINTER:
				description = lookupProgramDescription("winter", "scen", scenario);
				break;
			}
			showDescription(description);
		}
		break;
	case ThermalDevice::ST_HOLIDAY:
		{
			hideDescription();
		}
		break;
	case ThermalDevice::ST_WEEKEND:
		{
			hideDescription();
		}
		break;
	default:
		break;
	}
}

void PageTermoReg::setSeason(int new_season)
{
	if (new_season == ThermalDevice::SE_SUMMER || new_season == ThermalDevice::SE_WINTER)
	{
		QString img;
		if (new_season == ThermalDevice::SE_SUMMER)
			img = icon_summer;
		else
			img = icon_winter;
		QPixmap *icon = bt_global::icons_cache.getIcon(img);
		season_icon->setPixmap(*icon);
		program_choice->setSeason(static_cast<Season>(new_season));
		program_menu->setSeason(static_cast<Season>(new_season));
	}
	else
		qWarning("Received season is not SUMMER or WINTER, ignoring");
}

QString PageTermoReg::lookupProgramDescription(QString season, QString what, int program_number)
{
	// summer/prog/p[program_number]
	Q_ASSERT_X(what == "prog" || what == "scen", "PageTermoReg::lookupProgramDescription",
		"'what' must be either 'prog' or 'scen'");

	const QMap<QString, QString> &map = what == "prog" ? programs : scenarios;
	QString key = season + QString::number(program_number);
	if (!map.contains(key))
		return "";
	else
		return map[key];
}

void PageTermoReg::createButtonsBanners(SettingsPage *settings, ThermalDevice *dev)
{
	// TODO: when we have the small button icons for BTouch,
	//       remove the code to create the two separate banners
#ifdef LAYOUT_TOUCHX
	// off_antifreeze banner
	BannOffAntifreeze *off = new BannOffAntifreeze(settings, dev);
	settings->appendBanner(off);
	connect(off, SIGNAL(clicked()), SLOT(showPage()));
#else
	// off banner
	BannOff *off = new BannOff(settings, dev);
	settings->appendBanner(off);
	connect(off, SIGNAL(clicked()), SLOT(showPage()));

	// antifreeze banner
	BannAntifreeze *antifreeze = new BannAntifreeze(settings, dev);
	settings->appendBanner(antifreeze);
	connect(antifreeze, SIGNAL(clicked()), SLOT(showPage()));
#endif

	// summer_winter banner
	BannSummerWinter *summer_winter = new BannSummerWinter(settings, dev);
	settings->appendBanner(summer_winter);
	connect(summer_winter, SIGNAL(clicked()), SLOT(showPage()));
}

PageTermoReg4z::PageTermoReg4z(QDomNode n, ThermalDevice4Zones *device)
	: PageTermoReg(n)
{
	_dev = device;
	connect(_dev, SIGNAL(status_changed(const StatusList &)),
		SLOT(status_changed(const StatusList &)));
	createSettingsMenu(n);
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(showSettingsMenu()));
}

ThermalDevice *PageTermoReg4z::dev()
{
	return _dev;
}

void PageTermoReg4z::showSettingsMenu()
{
	settings->resetIndex();
	settings->showPage();
}

void PageTermoReg4z::createSettingsMenu(QDomNode regulator_node)
{
#ifdef CONFIG_BTOUCH
	QDomNode n = regulator_node;
#else
	QDomNode n = getPageNodeFromChildNode(regulator_node, "h_lnk_pageID");
	SkinContext context(getTextChild(n, "cid").toInt());
#endif

	settings = new SettingsPage(n);
	connect(settings, SIGNAL(Closed()), SLOT(showPage()));

	weekSettings(settings, programs, _dev);
	manualSettings(settings, _dev);

	timedManualSettings(settings, _dev);

	holidaySettings(settings, programs, _dev);

	weekendSettings(settings, programs, _dev);

	createButtonsBanners(settings, _dev);
}

PageTermoReg99z::PageTermoReg99z(QDomNode n, ThermalDevice99Zones *device)
	: PageTermoReg(n)
{
	_dev = device;
	connect(_dev, SIGNAL(status_changed(const StatusList &)),
		SLOT(status_changed(const StatusList &)));
	createSettingsMenu(n);
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(showSettingsMenu()));
}

ThermalDevice *PageTermoReg99z::dev()
{
	return _dev;
}

void PageTermoReg99z::setSeason(Season new_season)
{
	if (new_season == SUMMER || new_season == WINTER)
		scenario_menu->setSeason(new_season);
	else
		qWarning("Received season is not SUMMER or WINTER, ignoring");
	PageTermoReg::setSeason(new_season);
}

void PageTermoReg99z::showSettingsMenu()
{
	settings->resetIndex();
	settings->showPage();
}

void PageTermoReg99z::createSettingsMenu(QDomNode regulator_node)
{
#ifdef CONFIG_BTOUCH
	QDomNode n = regulator_node;
#else
	QDomNode n = getPageNodeFromChildNode(regulator_node, "h_lnk_pageID");
	SkinContext context(getTextChild(n, "cid").toInt());
#endif

	settings = new SettingsPage(n);
	connect(settings, SIGNAL(Closed()), SLOT(showPage()));

	weekSettings(settings, programs, _dev);
	manualSettings(settings, _dev);

	scenarioSettings(settings, scenarios, _dev);

	holidaySettings(settings, programs, _dev);

	weekendSettings(settings, programs, _dev);

	createButtonsBanners(settings, _dev);
}

//
// ------------- Utility functions to create thermal regulator settings menus -------------------
//
void PageTermoReg::manualSettings(SettingsPage *settings, ThermalDevice *dev)
{
	PageManual *manual_page = new PageManual(dev, temp_scale);

	// manual banner
	BannSinglePuls *manual = new BannSinglePuls(settings);
	manual->initBanner(bt_global::skin->getImage("forward"), bt_global::skin->getImage("regulator_manual"), "");
	manual->connectRightButton(manual_page);
	settings->appendBanner(manual);

	// when operation is activated, return to probe menu
	connect(manual_page, SIGNAL(temperatureSelected(unsigned)), SLOT(manualSelected(unsigned)));
	// when operation is cancelled, return to settings page
	connect(manual, SIGNAL(pageClosed()), settings, SLOT(showPage()));
}

void PageTermoReg::manualSelected(unsigned temp)
{
	dev()->setManualTemp(temp);
	showPage();
}

void PageTermoReg::weekSettings(SettingsPage *settings, QMap<QString, QString> programs, ThermalDevice *dev)
{
	program_menu = new WeeklyMenu(0, programs);

	BannSinglePuls *weekly = new BannSinglePuls(settings);
	weekly->initBanner(bt_global::skin->getImage("forward"), bt_global::skin->getImage("regulator_program"), "");
	weekly->connectRightButton(program_menu);
	settings->appendBanner(weekly);

	connect(weekly, SIGNAL(pageClosed()), settings, SLOT(showPage()));
	connect(program_menu, SIGNAL(programClicked(int)), SLOT(weekProgramSelected(int)));
}

void PageTermoReg::weekProgramSelected(int program)
{
	dev()->setWeekProgram(program);
	showPage();
}

void PageTermoReg::holidaySettings(SettingsPage *settings, QMap<QString, QString> programs, ThermalDevice *dev)
{
	BannSinglePuls *bann = createHolidayWeekendBanner(settings, bt_global::skin->getImage("regulator_holiday"));
	connect(bann, SIGNAL(rightClick()), SLOT(holidaySettingsStart()));
#ifdef LAYOUT_TOUCHX
	if (!date_time_edit)
		date_time_edit = createDateTimeEdit(settings);
#else
	if (!date_edit)
		date_edit = createDateEdit(settings);
	if (!time_edit)
		time_edit = createTimeEdit(settings);
#endif
	if (!program_choice)
		program_choice = createProgramChoice(settings, programs, dev);
}

void PageTermoReg::weekendSettings(SettingsPage *settings, QMap<QString, QString> programs, ThermalDevice *dev)
{
	BannSinglePuls *bann = createHolidayWeekendBanner(settings, bt_global::skin->getImage("regulator_weekend"));
	connect(bann, SIGNAL(rightClick()), SLOT(weekendSettingsStart()));
#ifdef LAYOUT_TOUCHX
	if (!date_time_edit)
		date_time_edit = createDateTimeEdit(settings);
#else
	if (!date_edit)
		date_edit = createDateEdit(settings);
	if (!time_edit)
		time_edit = createTimeEdit(settings);
#endif
	if (!program_choice)
		program_choice = createProgramChoice(settings, programs, dev);
}

BannSinglePuls *PageTermoReg::createHolidayWeekendBanner(SettingsPage *settings, QString icon)
{
	BannSinglePuls *bann = new BannSinglePuls(settings);
	bann->initBanner(bt_global::skin->getImage("forward"), icon, "");
	settings->appendBanner(bann);
	return bann;
}

PageSetDate *PageTermoReg::createDateEdit(SettingsPage *settings)
{
	PageSetDate *date_edit = new PageSetDate;
	connect(date_edit, SIGNAL(Closed()), settings, SLOT(showPage()));
	connect(date_edit, SIGNAL(dateSelected(QDate)), SLOT(dateSelected(QDate)));
	return date_edit;
}

PageSetDateTime *PageTermoReg::createDateTimeEdit(SettingsPage *settings)
{
	PageSetDateTime *date_time_edit = new PageSetDateTime;
	connect(date_time_edit, SIGNAL(Closed()), settings, SLOT(showPage()));
	connect(date_time_edit, SIGNAL(dateTimeSelected(QDate, BtTime)), SLOT(dateTimeSelected(QDate, BtTime)));
	return date_time_edit;
}

PageSetTime *PageTermoReg::createTimeEdit(SettingsPage *settings)
{
	PageSetTime *time_edit = new PageSetTime;
	connect(time_edit, SIGNAL(timeSelected(BtTime)), SLOT(timeSelected(BtTime)));
	connect(time_edit, SIGNAL(Closed()), SLOT(timeCancelled()));
	return time_edit;
}

WeeklyMenu *PageTermoReg::createProgramChoice(SettingsPage *settings, QMap<QString, QString> programs, device *dev)
{
	WeeklyMenu *program_choice = new WeeklyMenu(0, programs);
	connect(program_choice, SIGNAL(programClicked(int)), SLOT(weekendHolidaySettingsEnd(int)));
	connect(program_choice, SIGNAL(Closed()), SLOT(programCancelled()));
	return program_choice;
}

void PageTermoReg::holidaySettingsStart()
{
	weekendHolidayStatus = HOLIDAY;
#ifdef LAYOUT_TOUCHX
	date_time_edit->showPage();
#else
	date_edit->showPage();
#endif
}

void PageTermoReg::weekendSettingsStart()
{
	weekendHolidayStatus = WEEKEND;
#ifdef LAYOUT_TOUCHX
	date_time_edit->showPage();
#else
	date_edit->showPage();
#endif
}

void PageTermoReg::dateSelected(QDate d)
{
	date_end = d;
	time_edit->showPage();
}

void PageTermoReg::dateTimeSelected(QDate d, BtTime t)
{
	date_end = d;
	time_end = t;
	program_choice->showPage();
}

void PageTermoReg::timeCancelled()
{
	date_edit->showPage();
}

void PageTermoReg::timeSelected(BtTime t)
{
	time_end = t;
	program_choice->showPage();
}

void PageTermoReg::programCancelled()
{
	time_edit->showPage();
}

void PageTermoReg::weekendHolidaySettingsEnd(int program)
{
	if (weekendHolidayStatus == WEEKEND)
		dev()->setWeekendDateTime(date_end, time_end, program);
	else if (weekendHolidayStatus == HOLIDAY)
		dev()->setHolidayDateTime(date_end, time_end, program);
	else
		qWarning("PageTermoReg::weekendHolidaySettingsEnd: unknown status");
	showPage();
}

void PageTermoReg4z::timedManualSettings(SettingsPage *settings, ThermalDevice4Zones *dev)
{
	PageManualTimed *timed_manual_page = new PageManualTimed(dev, temp_scale);
	timed_manual_page->setMaxHours(25);

	// timed manual banner
	BannSinglePuls *manual_timed = new BannSinglePuls(settings);
	manual_timed->initBanner(bt_global::skin->getImage("forward"), bt_global::skin->getImage("regulator_manual_timed"), "");
	manual_timed->connectRightButton(timed_manual_page);
	settings->appendBanner(manual_timed);

	connect(manual_timed, SIGNAL(pageClosed()), settings, SLOT(showPage()));
	connect(timed_manual_page, SIGNAL(timeAndTempSelected(BtTime, int)), SLOT(manualTimedSelected(BtTime, int)));
}

void PageTermoReg4z::manualTimedSelected(BtTime time, int temp)
{
	_dev->setManualTempTimed(temp, time);
	showPage();
}

void PageTermoReg99z::scenarioSettings(SettingsPage *settings, QMap<QString, QString> scenarios, ThermalDevice99Zones *dev)
{
	scenario_menu = new ScenarioMenu(0, scenarios);

	BannSinglePuls *scenario = new BannSinglePuls(settings);
	scenario->initBanner(bt_global::skin->getImage("forward"), bt_global::skin->getImage("regulator_scenario"), "");
	scenario->connectRightButton(scenario_menu);
	settings->appendBanner(scenario);

	connect(scenario, SIGNAL(pageClosed()), settings, SLOT(showPage()));
	connect(scenario_menu, SIGNAL(programClicked(int)), SLOT(scenarioSelected(int)));
}

void PageTermoReg99z::scenarioSelected(int scenario)
{
	_dev->setScenario(scenario);
	showPage();
}


BannOff::BannOff(QWidget *parent, ThermalDevice *_dev) : BannCenteredButton(parent)
{
	initBanner(bt_global::skin->getImage("off_button"));
	dev = _dev;
	connect(center_button, SIGNAL(clicked()), SLOT(performAction()));
	connect(center_button, SIGNAL(clicked()), SIGNAL(clicked()));
}

void BannOff::performAction()
{
	dev->setOff();
}


BannAntifreeze::BannAntifreeze(QWidget *parent, ThermalDevice *_dev) : BannCenteredButton(parent)
{
	initBanner(bt_global::skin->getImage("regulator_antifreeze"));
	dev = _dev;
	connect(center_button, SIGNAL(clicked()), SLOT(performAction()));
	connect(center_button, SIGNAL(clicked()), SIGNAL(clicked()));
}

void BannAntifreeze::performAction()
{
	dev->setProtection();
}


BannOffAntifreeze::BannOffAntifreeze(QWidget *parent, ThermalDevice *_dev)
	: Bann2CentralButtons(parent)
{
	initBanner(bt_global::skin->getImage("off_button"), bt_global::skin->getImage("regulator_antifreeze"), "");
	dev = _dev;

	connect(center_left, SIGNAL(clicked()), SLOT(setOff()));
	connect(center_left, SIGNAL(clicked()), SIGNAL(clicked()));
	connect(center_right, SIGNAL(clicked()), SLOT(setAntifreeze()));
	connect(center_right, SIGNAL(clicked()), SIGNAL(clicked()));
}

void BannOffAntifreeze::setOff()
{
	dev->setOff();
}

void BannOffAntifreeze::setAntifreeze()
{
	dev->setProtection();
}


BannSummerWinter::BannSummerWinter(QWidget *parent, ThermalDevice *_dev) : Bann2CentralButtons(parent)
{
	initBanner(bt_global::skin->getImage("winter"), bt_global::skin->getImage("summer"), "");
	dev = _dev;

	connect(center_left, SIGNAL(clicked()), SLOT(setWinter()));
	connect(center_left, SIGNAL(clicked()), SIGNAL(clicked()));
	connect(center_right, SIGNAL(clicked()), SLOT(setSummer()));
	connect(center_right, SIGNAL(clicked()), SIGNAL(clicked()));
}

void BannSummerWinter::setSummer()
{
	dev->setSummer();
}

void BannSummerWinter::setWinter()
{
	dev->setWinter();
}


BannWeekly::BannWeekly(QWidget *parent, int _index)
	: BannSinglePuls(parent)
{
	index = _index;
	connect(right_button, SIGNAL(clicked()), SLOT(performAction()));
}

void BannWeekly::performAction()
{
	emit programNumber(index);
}

