
#include "bann_thermal_regulation.h"
#include "fontmanager.h" // bt_global::font
#include "btbutton.h"
#include "device.h"
#include "scaleconversion.h"
#include "icondispatcher.h" // bt_global::icons_cache
#include "devices_cache.h" // bt_global::devices_cache
#include "thermal_device.h"
#include "datetime.h"
#include "xml_functions.h"
#include "thermalmenu.h"
#include "main.h" // bt_global::config
#include "navigation_bar.h"
#include "content_widget.h"
#include "skinmanager.h"

#include <QVariant>
#include <QLabel>
#include <QDebug>

#define I_OK  IMG_PATH"btnok.png"
#define IMG_SETTINGS IMG_PATH"set.png"
#define IMG_PLUS IMG_PATH "btnplus.png"
#define IMG_MINUS IMG_PATH "btnmin.png"
#define IMG_AUTO IMG_PATH "btnauto.png"
#define IMG_MAN IMG_PATH "btnman.png"
#define IMG_RIGHT_ARROW IMG_PATH "arrrg.png"
#define IMG_SUMMER_S IMG_PATH "estate_s.png"
#define IMG_THERMR IMG_PATH "centrale.png"
#define IMG_OFF_S IMG_PATH "sondaoff.png"
#define IMG_ANTIFREEZE_S IMG_PATH "sondaantigelo.png"


QLabel *getLabelWithPixmap(const char *img, QWidget *parent, int alignment)
{
	QLabel *tmp = new QLabel(parent);
	tmp->setPixmap(*bt_global::icons_cache.getIcon(img));
	tmp->setAlignment((Qt::Alignment)alignment);
	return tmp;
}

static const char *FANCOIL_ICONS[] = {"fancoil1off.png", "fancoil1on.png", "fancoil2off.png", "fancoil2on.png",
	"fancoil3off.png", "fancoil3on.png", "fancoilAoff.png", "fancoilAon.png"};

// Helper page for the settings list
class SettingsPage : public Page
{
public:
	SettingsPage(QWidget *parent = 0);

	virtual void inizializza();

	void appendBanner(banner *b);
	void resetIndex();
};

SettingsPage::SettingsPage(QWidget *parent)
	: Page(parent)
{
	buildPage(new ContentWidget, new NavigationBar);
}

void SettingsPage::inizializza()
{
	content_widget->initBanners();
}

void SettingsPage::appendBanner(banner *b)
{
	b->Draw(); // kill Draw
	content_widget->appendBanner(b);
}

void SettingsPage::resetIndex()
{
	content_widget->resetIndex();
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


NavigationPage *getPage(BannID id, QWidget *parent, QDomNode n, QString ind_centrale,
			TemperatureScale scale)
{
	NavigationPage *p = 0;
	QString simple_address = getTextChild(n, "where");
	QString where_composed;
	if (!simple_address.isNull())
		where_composed = simple_address + "#" + ind_centrale;

	switch (id)
	{
	case fs_nc_probe:
		p = new PageSimpleProbe(NULL, n, scale);
		break;
	case fs_4z_probe:
		{
			temperature_probe_controlled *dev = static_cast<temperature_probe_controlled *>(
					bt_global::devices_cache.get_temperature_probe_controlled(
						where_composed, THERMO_Z4, false, ind_centrale, simple_address));
			QString thermr_where = QString("0#") + ind_centrale;
			ThermalDevice *thermo_reg = static_cast<ThermalDevice *>(
					bt_global::devices_cache.get_thermal_regulator(thermr_where, THERMO_Z4));
			p = new PageProbe(n, dev, thermo_reg, NULL, scale);
		}
		break;
	case fs_99z_probe:
		{
			temperature_probe_controlled *dev = static_cast<temperature_probe_controlled *>(
					bt_global::devices_cache.get_temperature_probe_controlled(
						simple_address, THERMO_Z99, false, ind_centrale, simple_address));
			QString thermr_where = ind_centrale;
			ThermalDevice *thermo_reg = static_cast<ThermalDevice *>(
					bt_global::devices_cache.get_thermal_regulator(thermr_where, THERMO_Z99));
			p = new PageProbe(n, dev, thermo_reg, NULL, scale);
		}
		break;
	case fs_4z_fancoil:
		{
			temperature_probe_controlled *dev = static_cast<temperature_probe_controlled *>(
					bt_global::devices_cache.get_temperature_probe_controlled(
						where_composed, THERMO_Z4, true, ind_centrale, simple_address));
			QString thermr_where = QString("0#") + ind_centrale;
			ThermalDevice *thermo_reg = static_cast<ThermalDevice *>(
					bt_global::devices_cache.get_thermal_regulator(thermr_where, THERMO_Z4));
			p = new PageFancoil(n, dev, thermo_reg, NULL, scale);
		}
		break;
	case fs_99z_fancoil:
		{
			temperature_probe_controlled *dev = static_cast<temperature_probe_controlled *>(
					bt_global::devices_cache.get_temperature_probe_controlled(
						simple_address, THERMO_Z99, true, ind_centrale, simple_address));
			QString thermr_where = ind_centrale;
			ThermalDevice *thermo_reg = static_cast<ThermalDevice *>(
					bt_global::devices_cache.get_thermal_regulator(thermr_where, THERMO_Z99));
			p = new PageFancoil(n, dev, thermo_reg, NULL, scale);
		}
		break;
	case fs_4z_thermal_regulator:
		{
			where_composed = QString("0#") + ind_centrale;
			ThermalDevice4Zones *dev = static_cast<ThermalDevice4Zones *>(
					bt_global::devices_cache.get_thermal_regulator(where_composed, THERMO_Z4));
			p = new PageTermoReg4z(n, dev, NULL, parent);
		}
		break;
	case fs_99z_thermal_regulator:
		{
			where_composed = ind_centrale;
			ThermalDevice99Zones *dev = static_cast<ThermalDevice99Zones *>(
					bt_global::devices_cache.get_thermal_regulator(where_composed, THERMO_Z99));
			p = new PageTermoReg99z(n, dev, NULL, parent);
		}
		break;
	default:
		qFatal("Unknown banner type %d on bannfullscreen", id);
	}

	return p;
}

NavigationPage::NavigationPage(QWidget *parent)
	: Page(0), nav_bar(0)
{
	main_layout.setSpacing(0);
	main_layout.setContentsMargins(0, 0, 0, 0);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 5, 0, 10);
	l->setSpacing(0);

	l->addLayout(&main_layout, 1);
}

NavigationBar *NavigationPage::createNavigationBar(const QString &icon)
{
	if (nav_bar)
		return nav_bar;

	nav_bar = new NavigationBar(icon);

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(backClick()));
	connect(nav_bar, SIGNAL(upClick()), SIGNAL(upClick()));
	connect(nav_bar, SIGNAL(downClick()), SIGNAL(downClick()));

	layout()->addWidget(nav_bar);

	return nav_bar;
}

PageSimpleProbe::PageSimpleProbe(QWidget *parent, QDomNode n, TemperatureScale scale)
	: NavigationPage(parent), temp_scale(scale)
{
	descr_label = new QLabel(this);
	descr_label->setFont(bt_global::font->get(FontManager::TEXT));
	descr_label->setAlignment(Qt::AlignHCenter);

	main_layout.addWidget(descr_label);
	main_layout.setContentsMargins(0, 0, 0, 0);
	main_layout.setSpacing(0);

	temp_label = new QLabel(this);
	temp_label->setFont(bt_global::font->get(FontManager::TITLE));
	temp_label->setAlignment(Qt::AlignHCenter);

	main_layout.addWidget(temp_label);
	main_layout.setAlignment(Qt::AlignHCenter);

	setTemperature(1235);
	setDescription(n.namedItem("descr").toElement().text());

	createNavigationBar(IMG_MAN);
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

void PageSimpleProbe::status_changed(QList<device_status*> sl)
{
	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (ds->get_type() == device_status::TEMPERATURE_PROBE)
		{
			stat_var curr_temp(stat_var::TEMPERATURE);
			ds->read(device_status_temperature_probe::TEMPERATURE_INDEX, curr_temp);
			setTemperature(curr_temp.get_val());
		}
	}
}


PageProbe::PageProbe(QDomNode n, temperature_probe_controlled *_dev, ThermalDevice *thermo_reg, QWidget *parent,
	TemperatureScale scale) : PageSimpleProbe(parent, n, scale),
	delta_setpoint(false),
	setpoint_delay(2000),
	setpoint_delta(5)
{
	status = AUTOMATIC;
	probe_type = thermo_reg->type();

	conf_root = n;
	dev = _dev;

	connect(dev, SIGNAL(status_changed(QList<device_status*>)), SLOT(status_changed(QList<device_status*>)));
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(changeStatus()));
	//install compressor
	dev->installFrameCompressor(setpoint_delay);

	QHBoxLayout *hbox = new QHBoxLayout();

	btn_minus = new BtButton(this);
	btn_minus->setImage(IMG_MINUS);
	btn_minus->hide();
	btn_minus->setAutoRepeat(true);
	connect(btn_minus, SIGNAL(clicked()), SLOT(decSetpoint()));
	hbox->addWidget(btn_minus);
	hbox->addStretch();

	setpoint_label = new QLabel(this);
	setpoint_label->setGeometry(SETPOINT_X, SETPOINT_Y, SETPOINT_WIDTH, SETPOINT_HEIGHT);
	setpoint_label->setFont(bt_global::font->get(FontManager::SUBTITLE));
	setpoint_label->setAlignment(Qt::AlignHCenter);
	setpoint_label->setProperty("SecondFgColor", true);

	icon_antifreeze = getLabelWithPixmap(IMG_ANTIFREEZE_S, this, Qt::AlignHCenter);
	hbox->addWidget(icon_antifreeze);

	icon_off = getLabelWithPixmap(IMG_OFF_S, this, Qt::AlignHCenter);
	hbox->addWidget(icon_off);

	hbox->addStretch();
	btn_plus = new BtButton(this);
	btn_plus->setImage(IMG_PLUS);
	btn_plus->hide();
	btn_plus->setAutoRepeat(true);
	connect(btn_plus, SIGNAL(clicked()), SLOT(incSetpoint()));
	hbox->addWidget(btn_plus);

	main_layout.addLayout(hbox);

	// avoid moving of fancoil buttons bar
	main_layout.addStretch();

	local_temp_label = new QLabel(this);
	local_temp_label->setGeometry(LOCAL_TEMP_X, LOCAL_TEMP_Y,
				      LOCAL_TEMP_WIDTH, LOCAL_TEMP_HEIGHT);

	local_temp_label->setFont(bt_global::font->get(FontManager::TEXT));
	local_temp_label->setAlignment(Qt::AlignHCenter);

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
	nav_bar->forward_button->setVisible(probe_type == THERMO_Z99 && !isOff && !isAntigelo);
	local_temp_label->setText(local_temp);
}

void PageProbe::status_changed(QList<device_status*> sl)
{
	bool update = false;

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (ds->get_type() == device_status::TEMPERATURE_PROBE_EXTRA)
		{
			stat_var curr_sp(stat_var::SP);
			ds->read(device_status_temperature_probe_extra::SP_INDEX, curr_sp);
			if (delta_setpoint)
			{
				int sp;
				switch (temp_scale)
				{
				case FAHRENHEIT:
					sp = static_cast<int>(fahrenheit2Bt(setpoint));
					break;
				default:
					qWarning("BannProbe: unknown temperature scale, defaulting to celsius");
					// fall through
				case CELSIUS:
					sp = static_cast<int>(celsius2Bt(setpoint));
					break;
				}

				curr_sp.set_val(sp);
				ds->write_val(device_status_temperature_probe_extra::SP_INDEX, curr_sp);
				ds->read(device_status_temperature_probe_extra::SP_INDEX, curr_sp);
				delta_setpoint = false;
			}

			stat_var curr_local(stat_var::LOCAL);
			ds->read(device_status_temperature_probe_extra::LOCAL_INDEX, curr_local);
			if (curr_local.initialized())
			{
				update = true;
				if ((curr_local.get_val() >= 0  && curr_local.get_val() <= 3) ||
						(curr_local.get_val() >= 11 && curr_local.get_val() <= 13))
				{
					local_temp = QString::number(curr_local.get_val() % 10);
					if (curr_local.get_val() >= 11)
						local_temp = local_temp.insert(0, "-");
					else if (curr_local.get_val() >= 1)
						local_temp = local_temp.insert(0, "+");

					isOff = false;
					isAntigelo = false;
				}
				else if (curr_local.get_val() == 4)
				{
					local_temp = "0";
					isOff = true;
					isAntigelo = false;
				}
				else if (curr_local.get_val() == 5)
				{
					local_temp = "0";
					isOff = false;
					isAntigelo = true;
				}
				else
				{
					update = false;
					qDebug("[TERMO] PageProbe::status_changed(): local status case not handled!");
				}
			}

			if (curr_sp.initialized() && !delta_setpoint)
			{
				switch (temp_scale)
				{
				case CELSIUS:
					setpoint = bt2Celsius(static_cast<unsigned>(curr_sp.get_val()));
					break;
				case FAHRENHEIT:
					setpoint = bt2Fahrenheit(static_cast<unsigned>(curr_sp.get_val()));
					break;
				default:
					qWarning("BannProbe: unknown temperature scale, defaulting to celsius");
					setpoint = bt2Celsius(static_cast<unsigned>(curr_sp.get_val()));
				}
				updatePointLabel();
			}

			stat_var curr_stat(stat_var::STAT);
			ds->read(device_status_temperature_probe_extra::STAT_INDEX, curr_stat);
			if (curr_stat.initialized())
			{
				switch (curr_stat.get_val())
				{
				case device_status_temperature_probe_extra::S_MAN:
					status = MANUAL;
					nav_bar->forward_button->setImage(IMG_AUTO);
					update = true;
					break;
				case device_status_temperature_probe_extra::S_AUTO:
					status = AUTOMATIC;
					nav_bar->forward_button->setImage(IMG_MAN);
					update = true;
					break;
				case device_status_temperature_probe_extra::S_ANTIGELO:
				case device_status_temperature_probe_extra::S_TERM:
				case device_status_temperature_probe_extra::S_GEN:
					isOff = false;
					isAntigelo = true;
					update = true;
					break;
				case device_status_temperature_probe_extra::S_OFF:
					isOff = true;
					isAntigelo = false;
					update = true;
					break;
				case device_status_temperature_probe_extra::S_NONE:
					status = AUTOMATIC;
					update = true;
					break;
				default:
					update = false;
					break;
				}
			}
		}
	}

	if (update)
		updateControlState();

	PageSimpleProbe::status_changed(sl);
}

PageFancoil::PageFancoil(QDomNode n, temperature_probe_controlled *_dev, ThermalDevice *thermo_reg, QWidget *parent,
	TemperatureScale scale) : PageProbe(n, _dev, thermo_reg, parent, scale), fancoil_buttons(this)
{
	dev = _dev;
	connect(dev, SIGNAL(status_changed(QList<device_status*>)), SLOT(status_changed(QList<device_status*>)));

	createFancoilButtons();
	fancoil_buttons.setExclusive(true);
	setFancoilStatus(0);

	connect(&fancoil_buttons, SIGNAL(buttonClicked(int)), SLOT(handleFancoilButtons(int)));
}

void PageFancoil::createFancoilButtons()
{
	QHBoxLayout *hbox = new QHBoxLayout();
	for (int i = 0, id = 0; i < 8; i += 2, ++id)
	{
		QString path = QString(IMG_PATH) + FANCOIL_ICONS[i];
		QString path_pressed = QString(IMG_PATH) + FANCOIL_ICONS[i+1];
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

void PageFancoil::status_changed(QList<device_status*> sl)
{
	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (ds->get_type() == device_status::FANCOIL)
		{

			stat_var speed_var(stat_var::FANCOIL_SPEED);
			ds->read((int)device_status_fancoil::SPEED_INDEX, speed_var);

			// Set the fancoil Button in the buttons bar
			if (speed_to_btn_tbl.contains(speed_var.get_val()))
				setFancoilStatus(speed_to_btn_tbl[speed_var.get_val()]);
			else
				qDebug("Fancoil speed val out of range (%d)", speed_var.get_val());
		}
	}

	PageProbe::status_changed(sl);
}

PageManual::PageManual(QWidget *parent, ThermalDevice *_dev, TemperatureScale scale)
	: Page(0), main_layout(this), temp_scale(scale), dev(_dev), setpoint_delta(5)
{
	descr_label = new QLabel(this);
	descr_label->setFont(bt_global::font->get(FontManager::TEXT));
	descr_label->setAlignment(Qt::AlignTop|Qt::AlignHCenter);

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
	temp_label->setFont(bt_global::font->get(FontManager::SUBTITLE));
	temp_label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	temp_label->setProperty("SecondFgColor", true);
	QHBoxLayout *hbox = new QHBoxLayout();

	const QString btn_min_img = QString("%1%2").arg(IMG_PATH).arg("btnmin.png");
	BtButton *btn = new BtButton(this);
	btn->setImage(btn_min_img);
	btn->setAutoRepeat(true);
	connect(btn, SIGNAL(clicked()), this, SLOT(decSetpoint()));
	hbox->addWidget(btn);
	hbox->addStretch(1);

	hbox->addWidget(temp_label);

	hbox->addStretch(1);
	const QString btn_plus_img = QString("%1%2").arg(IMG_PATH).arg("btnplus.png");
	btn = new BtButton(this);
	btn->setImage(btn_plus_img);
	btn->setAutoRepeat(true);
	connect(btn, SIGNAL(clicked()), this, SLOT(incSetpoint()));
	hbox->addWidget(btn);

	main_layout.addLayout(hbox);

	nav_bar = new ThermalNavigation;
	main_layout.addWidget(nav_bar);

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

PageManualTimed::PageManualTimed(QWidget *parent, ThermalDevice4Zones *_dev, TemperatureScale scale)
	: PageManual(parent, _dev, scale),
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

PageSetDate::PageSetDate(QWidget *parent) : Page(0), main_layout(this)
{
	const QString top_img = QString("%1%2").arg(IMG_PATH).arg("calendario.png");
	QLabel *top = new QLabel(this);
	top->setPixmap(*bt_global::icons_cache.getIcon(top_img));
	main_layout.addWidget(top, 0, Qt::AlignHCenter);

	date_edit = new BtDateEdit(this);
	main_layout.addWidget(date_edit);
	main_layout.setSpacing(0);
	main_layout.setContentsMargins(0, 0, 0, 10);

	ThermalNavigation *nav = new ThermalNavigation;
	main_layout.addWidget(nav);

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

PageSetTime::PageSetTime(QWidget *parent) : Page(0), main_layout(this)
{
	const QString i_top_img = QString("%1%2").arg(IMG_PATH).arg("orologio.png");
	QLabel *top = new QLabel(this);
	top->setPixmap(*bt_global::icons_cache.getIcon(i_top_img));
	main_layout.addWidget(top, 0, Qt::AlignHCenter);

	time_edit = new BtTimeEdit(this);
	main_layout.addWidget(time_edit);
	main_layout.setSpacing(0);
	main_layout.setContentsMargins(0, 0, 0, 10);

	ThermalNavigation *nav = new ThermalNavigation;
	main_layout.addWidget(nav);

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

PageTermoReg::PageTermoReg(QDomNode n, QWidget *parent) : NavigationPage(parent)
{
	conf_root = n;

	// Put a sensible default for the description
	QDomNode descr = conf_root.namedItem("descr");
	QString description;
	if (!descr.isNull())
		description = descr.toElement().text();
	else
	{
		qDebug("[TERMO] PageTermoReg ctor: no description found, maybe wrong node conf?");
		description = "Wrong node";
	}
	description_label = new QLabel(this);
	description_label->setFont(bt_global::font->get(FontManager::SUBTITLE));
	description_label->setAlignment(Qt::AlignHCenter);
	description_label->setProperty("SecondFgColor", true);

	season_icon = getLabelWithPixmap(IMG_SUMMER_S, this, Qt::AlignHCenter);

	mode_icon = getLabelWithPixmap(IMG_THERMR, this, Qt::AlignHCenter);

	main_layout.addWidget(mode_icon);
	main_layout.addWidget(description_label);
	main_layout.addWidget(season_icon);
	main_layout.setAlignment(Qt::AlignHCenter);

	date_edit = 0;
	time_edit = 0;
	program_choice = 0;
	temp_scale = static_cast<TemperatureScale>(bt_global::config[TEMPERATURE_SCALE].toInt());

	createNavigationBar(IMG_SETTINGS);
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
	switch (status)
	{
	case ThermalDevice::ST_OFF:
		{
			QPixmap *icon = bt_global::icons_cache.getIcon(IMG_OFF_S);
			mode_icon->setPixmap(*icon);
			hideDescription();
		}
		break;
	case ThermalDevice::ST_PROTECTION:
		{
			QPixmap *icon = bt_global::icons_cache.getIcon(IMG_ANTIFREEZE_S);
			mode_icon->setPixmap(*icon);
			hideDescription();
		}
		break;
	case ThermalDevice::ST_MANUAL:
	case ThermalDevice::ST_MANUAL_TIMED:
		{
			QString i_img = QString(IMG_PATH);
			if (status == ThermalDevice::ST_MANUAL)
				i_img += "manuale.png";
			else
				i_img += "manuale_temporizzato.png";
			QPixmap *icon = bt_global::icons_cache.getIcon(i_img);
			mode_icon->setPixmap(*icon);

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
			const QString i_img = QString(IMG_PATH) + "settimanale.png";
			QPixmap *icon = bt_global::icons_cache.getIcon(i_img);
			mode_icon->setPixmap(*icon);

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
			const QString i_img = QString(IMG_PATH) + "scenari.png";
			QPixmap *icon = bt_global::icons_cache.getIcon(i_img);
			mode_icon->setPixmap(*icon);

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
			const QString i_img = QString(IMG_PATH) + "feriale.png";
			QPixmap *icon = bt_global::icons_cache.getIcon(i_img);
			mode_icon->setPixmap(*icon);
			hideDescription();
		}
		break;
	case ThermalDevice::ST_WEEKEND:
		{
			const QString i_img = QString(IMG_PATH) + "festivo.png";
			QPixmap *icon = bt_global::icons_cache.getIcon(i_img);
			mode_icon->setPixmap(*icon);
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
		QString img = IMG_PATH;
		if (new_season == ThermalDevice::SE_SUMMER)
			img += "estate_s.png";
		else
			img += "inverno_s.png";
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
	QString name = what.left(1);
	QString path = season + "/" + what + "/" + name + QString::number(program_number);
	QDomElement node = getElement(conf_root, path);
	if (node.isNull())
		return "";
	else
		return node.text();
}

void PageTermoReg::createButtonsBanners(SettingsPage *settings, ThermalDevice *dev)
{
	// off banner
	BannOff *off = new BannOff(settings, dev);
	settings->appendBanner(off);
	connect(off, SIGNAL(clicked()), parentWidget(), SLOT(showPage()));

	// antifreeze banner
	BannAntifreeze *antifreeze = new BannAntifreeze(settings, dev);
	settings->appendBanner(antifreeze);
	connect(antifreeze, SIGNAL(clicked()), parentWidget(), SLOT(showPage()));

	// summer_winter banner
	BannSummerWinter *summer_winter = new BannSummerWinter(settings, dev);
	settings->appendBanner(summer_winter);
	connect(summer_winter, SIGNAL(clicked()), parentWidget(), SLOT(showPage()));
}

PageTermoReg4z::PageTermoReg4z(QDomNode n, ThermalDevice4Zones *device, QWidget *parent, QWidget *back)
	: PageTermoReg(n, parent)
{
	_dev = device;
	connect(_dev, SIGNAL(status_changed(const StatusList &)),
		SLOT(status_changed(const StatusList &)));
	createSettingsMenu(back);
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

void PageTermoReg4z::createSettingsMenu(QWidget *back)
{
	settings = new SettingsPage;
	connect(settings, SIGNAL(Closed()), back, SLOT(showPage()));

	weekSettings(settings, conf_root, _dev);
	manualSettings(settings, _dev);

	timedManualSettings(settings, _dev);

	holidaySettings(settings, conf_root, _dev);

	weekendSettings(settings, conf_root, _dev);

	createButtonsBanners(settings, _dev);
}

PageTermoReg99z::PageTermoReg99z(QDomNode n, ThermalDevice99Zones *device, QWidget *parent, QWidget *back)
	: PageTermoReg(n, parent)
{
	_dev = device;
	connect(_dev, SIGNAL(status_changed(const StatusList &)),
		SLOT(status_changed(const StatusList &)));
	createSettingsMenu(back);
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

void PageTermoReg99z::createSettingsMenu(QWidget *back)
{
	settings = new SettingsPage;
	connect(settings, SIGNAL(Closed()), back, SLOT(showPage()));

	weekSettings(settings, conf_root, _dev);
	manualSettings(settings, _dev);

	scenarioSettings(settings, conf_root, _dev);

	holidaySettings(settings, conf_root, _dev);

	weekendSettings(settings, conf_root, _dev);

	createButtonsBanners(settings, _dev);
}

//
// ------------- Utility functions to create thermal regulator settings menus -------------------
//
void PageTermoReg::manualSettings(SettingsPage *settings, ThermalDevice *dev)
{
	// manual banner
	bannPuls *manual = new bannPuls(settings);
	manual->SetIcons(IMG_RIGHT_ARROW, QString(), IMG_PATH + QString("manuale.png"));

	settings->appendBanner(manual);

	PageManual *manual_page = new PageManual(this, dev, temp_scale);

	connect(manual, SIGNAL(sxClick()), manual_page, SLOT(showPage()));

	connect(manual_page, SIGNAL(temperatureSelected(unsigned)), SLOT(manualSelected(unsigned)));
	connect(manual_page, SIGNAL(temperatureSelected(unsigned)), settings, SIGNAL(Closed()));
	connect(manual_page, SIGNAL(Closed()), settings, SLOT(showPage()));
}

void PageTermoReg::manualSelected(unsigned temp)
{
	dev()->setManualTemp(temp);
}

void PageTermoReg::weekSettings(SettingsPage *settings, QDomNode conf, ThermalDevice *dev)
{
	bannPuls *weekly = new bannPuls(settings);
	weekly->SetIcons(IMG_RIGHT_ARROW, QString(), IMG_PATH + QString("settimanale.png"));
	settings->appendBanner(weekly);

	program_menu = new WeeklyMenu(0, conf);

	connect(weekly, SIGNAL(sxClick()), program_menu, SLOT(showPage()));

	connect(program_menu, SIGNAL(Closed()), settings, SLOT(showPage()));
	connect(program_menu, SIGNAL(programClicked(int)), this, SLOT(weekProgramSelected(int)));
	connect(program_menu, SIGNAL(programClicked(int)), settings, SIGNAL(Closed()));
}

void PageTermoReg::weekProgramSelected(int program)
{
	dev()->setWeekProgram(program);
}

void PageTermoReg::holidaySettings(SettingsPage *settings, QDomNode conf, ThermalDevice *dev)
{
	banner *bann = createHolidayWeekendBanner(settings, "feriale.png");
	connect(bann, SIGNAL(sxClick()), this, SLOT(holidaySettingsStart()));
	if (!date_edit)
		date_edit = createDateEdit(settings);
	if (!time_edit)
		time_edit = createTimeEdit(settings);
	if (!program_choice)
		program_choice = createProgramChoice(settings, conf, dev);
}

void PageTermoReg::weekendSettings(SettingsPage *settings, QDomNode conf, ThermalDevice *dev)
{
	banner *bann = createHolidayWeekendBanner(settings, "festivo.png");
	connect(bann, SIGNAL(sxClick()), this, SLOT(weekendSettingsStart()));
	if (!date_edit)
		date_edit = createDateEdit(settings);
	if (!time_edit)
		time_edit = createTimeEdit(settings);
	if (!program_choice)
		program_choice = createProgramChoice(settings, conf, dev);
}

banner *PageTermoReg::createHolidayWeekendBanner(SettingsPage *settings, QString icon)
{
	bannPuls *bann = new bannPuls(settings);
	bann->SetIcons(IMG_RIGHT_ARROW, 0, IMG_PATH + icon);
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

PageSetTime *PageTermoReg::createTimeEdit(SettingsPage *settings)
{
	PageSetTime *time_edit = new PageSetTime;
	connect(time_edit, SIGNAL(timeSelected(BtTime)), SLOT(timeSelected(BtTime)));
	connect(time_edit, SIGNAL(Closed()), SLOT(timeCancelled()));
	return time_edit;
}

WeeklyMenu *PageTermoReg::createProgramChoice(SettingsPage *settings, QDomNode conf, device *dev)
{
	WeeklyMenu *program_choice = new WeeklyMenu(0, conf);
	connect(program_choice, SIGNAL(programClicked(int)), SLOT(weekendHolidaySettingsEnd(int)));
	connect(program_choice, SIGNAL(programClicked(int)), settings, SIGNAL(Closed()));
	connect(program_choice, SIGNAL(Closed()), SLOT(programCancelled()));
	return program_choice;
}

void PageTermoReg::holidaySettingsStart()
{
	weekendHolidayStatus = HOLIDAY;
	date_edit->showPage();
}

void PageTermoReg::weekendSettingsStart()
{
	weekendHolidayStatus = WEEKEND;
	date_edit->showPage();
}

void PageTermoReg::dateSelected(QDate d)
{
	date_end = d;
	time_edit->showPage();
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
}

void PageTermoReg4z::timedManualSettings(SettingsPage *settings, ThermalDevice4Zones *dev)
{
	// timed manual banner
	bannPuls *manual_timed = new bannPuls(settings);
	manual_timed->SetIcons(IMG_RIGHT_ARROW, QString(), IMG_PATH + QString("manuale_temporizzato.png"));

	settings->appendBanner(manual_timed);

	PageManualTimed *timed_manual_page = new PageManualTimed(this, dev, temp_scale);
	timed_manual_page->setMaxHours(25);

	connect(manual_timed, SIGNAL(sxClick()), timed_manual_page, SLOT(showPage()));

	connect(timed_manual_page, SIGNAL(Closed()), settings, SLOT(showPage()));
	connect(timed_manual_page, SIGNAL(timeAndTempSelected(BtTime, int)), SLOT(manualTimedSelected(BtTime, int)));
	connect(timed_manual_page, SIGNAL(timeAndTempSelected(BtTime, int)), settings, SIGNAL(Closed()));
}

void PageTermoReg4z::manualTimedSelected(BtTime time, int temp)
{
	_dev->setManualTempTimed(temp, time);
}

void PageTermoReg99z::scenarioSettings(SettingsPage *settings, QDomNode conf, ThermalDevice99Zones *dev)
{
	bannPuls *scenario = new bannPuls(settings);
	scenario->SetIcons(IMG_RIGHT_ARROW, QString(), IMG_PATH + QString("scenari.png"));
	settings->appendBanner(scenario);

	scenario_menu = new ScenarioMenu(0, conf);

	connect(scenario, SIGNAL(sxClick()), scenario_menu, SLOT(showPage()));

	connect(scenario_menu, SIGNAL(Closed()), settings, SLOT(showPage()));
	connect(scenario_menu, SIGNAL(programClicked(int)), this, SLOT(scenarioSelected(int)));
	connect(scenario_menu, SIGNAL(programClicked(int)), settings, SIGNAL(Closed()));
}

void PageTermoReg99z::scenarioSelected(int scenario)
{
	_dev->setScenario(scenario);
}


BannOff::BannOff(QWidget *parent, ThermalDevice *_dev) : bann3But(parent)
{
	QString i_off = QString("%1%2").arg(IMG_PATH).arg("off.png");

	SetIcons(QString(), QString(), QString(), i_off);
	dev = _dev;
	connect(this, SIGNAL(centerClick()), this, SLOT(performAction()));
	connect(this, SIGNAL(centerClick()), this, SIGNAL(clicked()));
}

void BannOff::performAction()
{
	dev->setOff();
}


BannAntifreeze::BannAntifreeze(QWidget *parent, ThermalDevice *_dev) : bann3But(parent)
{
	QString i_antifreeze = QString("%1%2").arg(IMG_PATH).arg("antigelo.png");

	SetIcons(QString(), QString(), QString(), i_antifreeze);
	dev = _dev;
	connect(this, SIGNAL(centerClick()), this, SLOT(performAction()));
	connect(this, SIGNAL(centerClick()), this, SIGNAL(clicked()));
}

void BannAntifreeze::performAction()
{
	dev->setProtection();
}

BannSummerWinter::BannSummerWinter(QWidget *parent, ThermalDevice *_dev) : bann4But(parent)
{
	QString i_summer = QString("%1%2").arg(IMG_PATH).arg("estate.png");
	QString i_winter = QString("%1%2").arg(IMG_PATH).arg("inverno.png");

	// see banner:195 (don't know why it works, really)
	SetIcons(QString(), QString(), i_summer, i_winter);
	dev = _dev;

	connect(this, SIGNAL(csxClick()), this, SLOT(setWinter()));
	connect(this, SIGNAL(cdxClick()), this, SLOT(setSummer()));
	connect(this, SIGNAL(csxClick()), this, SIGNAL(clicked()));
	connect(this, SIGNAL(cdxClick()), this, SIGNAL(clicked()));
}

void BannSummerWinter::setSummer()
{
	dev->setSummer();
}

void BannSummerWinter::setWinter()
{
	dev->setWinter();
}


BannWeekly::BannWeekly(QWidget *parent) : bannPuls(parent)
{
	program = 1;
	connect(this, SIGNAL(click()), this, SLOT(performAction()));
}

void BannWeekly::setProgram(int prog)
{
	// TODO: check if program number is correct?
	program = prog;
}

void BannWeekly::performAction()
{
	emit programNumber(program);
}
