/*!
 * \bannfullscreen.cpp
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 * \brief A base class for all banners that are fullscreen
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "bannfullscreen.h"
#include "fontmanager.h"
#include "sottomenu.h"
#include "btbutton.h"
#include "btlabel.h"
#include "device.h"
#include "bannsettings.h"
#include "device_cache.h"
#include "scaleconversion.h"

#include <QByteArray>
#include <QVariant>
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


void setWidgetVisible(QWidget *w, bool visible)
{
	if (visible)
		w->show();
	else
		w->hide();
}


BtLabel *getLabelWithPixmap(const char *img, QWidget *parent, int alignment)
{
	BtLabel *tmp = new BtLabel(parent);
	tmp->setPixmap(*icons_library.getIcon(img));
	tmp->setAlignment((Qt::Alignment)alignment);
	return tmp;
}

static const char *FANCOIL_ICONS[] = {"fancoil1off.png", "fancoil1on.png", "fancoil2off.png", "fancoil2on.png",
	"fancoil3off.png", "fancoil3on.png", "fancoilAoff.png", "fancoilAon.png"};

BannFullScreen::BannFullScreen(QWidget *parent, const char *name) : banner(parent, name)
{
}

BtButton *BannFullScreen::getButton(QString img)
{
	BtButton *btn = new BtButton(this);
	btn->setPixmap(*icons_library.getIcon(img));
	btn->setPressedPixmap(*icons_library.getIcon(getPressedIconName(img)));
	return btn;
}


/**
 * Extract the address from the DOM node passed as parameter.
 * \param n The node where the configuration of this item starts.
 * \return The simple address if this item
 */
QString extractAddress(QDomNode n)
{
	if (!n.isNull())
	{
		QDomNode where = n.namedItem("where");
		if (!where.isNull() && where.isElement())
			return where.toElement().text();
	}
	return QString();
}

BannFullScreen *getBanner(BannID id, QWidget *parent, QDomNode n, QString ind_centrale, TemperatureScale scale)
{
	BannFullScreen *bfs = 0;
	QString simple_address = extractAddress(n);
	QString where_composed;
	if (!simple_address.isNull())
		where_composed = simple_address + "#" + ind_centrale;

	switch (id)
	{
	case fs_nc_probe:
		bfs = new FSBannSimpleProbe(parent, n, scale);
		break;
	case fs_4z_probe:
		{
			temperature_probe_controlled *dev = static_cast<temperature_probe_controlled *>(
					btouch_device_cache.get_temperature_probe_controlled(
						where_composed, THERMO_Z4, false, ind_centrale, simple_address));
			QString thermr_where = QString("0#") + ind_centrale;
			thermal_regulator *thermo_reg = static_cast<thermal_regulator *>(
					btouch_device_cache.get_thermal_regulator(thermr_where, THERMO_Z4));
			bfs = new FSBannProbe(n, dev, thermo_reg, parent, scale);
		}
		break;
	case fs_99z_probe:
		{
			temperature_probe_controlled *dev = static_cast<temperature_probe_controlled *>(
					btouch_device_cache.get_temperature_probe_controlled(
						simple_address, THERMO_Z99, false, ind_centrale, simple_address));
			QString thermr_where = ind_centrale;
			thermal_regulator *thermo_reg = static_cast<thermal_regulator *>(
					btouch_device_cache.get_thermal_regulator(thermr_where, THERMO_Z99));
			bfs = new FSBannProbe(n, dev, thermo_reg, parent, scale);
		}
		break;
	case fs_4z_fancoil:
		{
			temperature_probe_controlled *dev = static_cast<temperature_probe_controlled *>(
					btouch_device_cache.get_temperature_probe_controlled(
						where_composed, THERMO_Z4, true, ind_centrale, simple_address));
			QString thermr_where = QString("0#") + ind_centrale;
			thermal_regulator *thermo_reg = static_cast<thermal_regulator *>(
					btouch_device_cache.get_thermal_regulator(thermr_where, THERMO_Z4));
			bfs = new FSBannFancoil(n, dev, thermo_reg, parent, scale);
		}
		break;
	case fs_99z_fancoil:
		{
			temperature_probe_controlled *dev = static_cast<temperature_probe_controlled *>(
					btouch_device_cache.get_temperature_probe_controlled(
						simple_address, THERMO_Z99, true, ind_centrale, simple_address));
			QString thermr_where = ind_centrale;
			thermal_regulator *thermo_reg = static_cast<thermal_regulator *>(
					btouch_device_cache.get_thermal_regulator(thermr_where, THERMO_Z99));
			bfs = new FSBannFancoil(n, dev, thermo_reg, parent, scale);
		}
		break;
	case fs_4z_thermal_regulator:
		{
			where_composed = QString("0#") + ind_centrale;
			thermal_regulator_4z *dev = static_cast<thermal_regulator_4z *>(
					btouch_device_cache.get_thermal_regulator(where_composed, THERMO_Z4));
			bfs = new FSBannTermoReg4z(n, dev, parent);
		}
		break;
	case fs_99z_thermal_regulator:
		{
			where_composed = ind_centrale;
			thermal_regulator_99z *dev = static_cast<thermal_regulator_99z *>(
					btouch_device_cache.get_thermal_regulator(where_composed, THERMO_Z99));
			bfs = new FSBannTermoReg99z(n, dev, parent);
		}
		break;
	case fs_date_edit:
		bfs = new FSBannDate(parent, 0);
		break;
	case fs_time_edit:
		bfs = new FSBannTime(parent, 0);
		break;
	default:
		qFatal("Unknown banner type %d on bannfullscreen", id);
	}
	return bfs;
}

FSBannSimpleProbe::FSBannSimpleProbe(QWidget *parent, QDomNode n, TemperatureScale scale, const char *name)
	: BannFullScreen(parent, name),
	main_layout(this),
	temp_scale(scale)
{
	descr_label = new BtLabel(this);
	main_layout.addWidget(descr_label);

	temp_label = new BtLabel(this);
	main_layout.addWidget(temp_label);
	main_layout.setAlignment(Qt::AlignHCenter);

	temp = 1235;
	descr = n.namedItem("descr").toElement().text();
}


void FSBannSimpleProbe::Draw()
{
	QFont aFont;
	FontManager::instance()->getFont(font_banTermo_tempMis, aFont);
	temp_label->setFont(aFont);
	temp_label->setAlignment(Qt::AlignHCenter);
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


	FontManager::instance()->getFont(font_banTermo_testo, aFont);
	descr_label->setFont(aFont);
	descr_label->setAlignment(Qt::AlignHCenter);
	descr_label->setText(descr);
}

void FSBannSimpleProbe::status_changed(QList<device_status*> sl)
{
	bool update = false;

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (ds->get_type() == device_status::TEMPERATURE_PROBE)
		{
			stat_var curr_temp(stat_var::TEMPERATURE);
			ds->read(device_status_temperature_probe::TEMPERATURE_INDEX, curr_temp);
			temp = curr_temp.get_val();
			update = true;
		}
	}

	if (update)
		Draw();
}

FSBannProbe::FSBannProbe(QDomNode n, temperature_probe_controlled *_dev, thermal_regulator *thermo_reg, QWidget *parent,
	TemperatureScale scale, const char *name) : FSBannSimpleProbe(parent, n, scale),
	delta_setpoint(false),
	setpoint_delay(2000),
	setpoint_delta(5)
{
	status = AUTOMATIC;
	probe_type = thermo_reg->type();
	navbar_button = getButton(IMG_MAN);
	navbar_button->hide();
	conf_root = n;
	dev = _dev;

	connect(dev, SIGNAL(status_changed(QList<device_status*>)), SLOT(status_changed(QList<device_status*>)));
	connect(navbar_button, SIGNAL(clicked()), SLOT(changeStatus()));

	QHBoxLayout *hbox = new QHBoxLayout();

	btn_minus = getButton(IMG_MINUS);
	btn_minus->hide();
	btn_minus->setAutoRepeat(true);
	connect(btn_minus, SIGNAL(clicked()), SLOT(decSetpoint()));
	hbox->addWidget(btn_minus);
	hbox->addStretch();

	setpoint_label = new BtLabel(this);
	setpoint_label->setGeometry(SETPOINT_X, SETPOINT_Y, SETPOINT_WIDTH, SETPOINT_HEIGHT);

	icon_antifreeze = getLabelWithPixmap(IMG_ANTIFREEZE_S, this, Qt::AlignHCenter);
	hbox->addWidget(icon_antifreeze);

	icon_off = getLabelWithPixmap(IMG_OFF_S, this, Qt::AlignHCenter);
	hbox->addWidget(icon_off);

	hbox->addStretch();
	btn_plus = getButton(IMG_PLUS);
	btn_plus->hide();
	btn_plus->setAutoRepeat(true);
	connect(btn_plus, SIGNAL(clicked()), SLOT(incSetpoint()));
	hbox->addWidget(btn_plus);

	main_layout.addLayout(hbox);

	// avoid moving of fancoil buttons bar
	main_layout.addStretch();

	local_temp_label = new BtLabel(this);
	local_temp_label->setGeometry(LOCAL_TEMP_X, LOCAL_TEMP_Y,
			LOCAL_TEMP_WIDTH, LOCAL_TEMP_HEIGHT);


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

	connect(&setpoint_timer, SIGNAL(timeout()), this, SLOT(setSetpoint()));
}

void FSBannProbe::setDeviceToManual()
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

void FSBannProbe::changeStatus()
{
	if (status == AUTOMATIC)
		setDeviceToManual();
	else
		dev->setAutomatic();
}

BtButton *FSBannProbe::customButton()
{
	if (probe_type == THERMO_Z99)
		return navbar_button;
	return 0;
}

void FSBannProbe::incSetpoint()
{
	if (setpoint >= maximum_manual_temp)
		return;
	else
		setpoint += setpoint_delta;
	Draw();
	setpoint_timer.start(setpoint_delay);
	delta_setpoint = true;
}

void FSBannProbe::decSetpoint()
{
	if (setpoint <= minimum_manual_temp)
		return;
	else
		setpoint -= setpoint_delta;
	Draw();
	setpoint_timer.start(setpoint_delay);
	delta_setpoint = true;
}

void FSBannProbe::setSetpoint()
{
	setpoint_timer.stop();
	setDeviceToManual();
}

void FSBannProbe::Draw()
{
	setWidgetVisible(btn_minus, status == MANUAL && probe_type == THERMO_Z99 && !isOff && !isAntigelo);
	setWidgetVisible(btn_plus, status == MANUAL && probe_type == THERMO_Z99 && !isOff && !isAntigelo);
	setWidgetVisible(setpoint_label, !isOff && !isAntigelo);
	setWidgetVisible(local_temp_label, !isOff && !isAntigelo && local_temp != "0");
	setWidgetVisible(icon_off, isOff);
	setWidgetVisible(icon_antifreeze, isAntigelo);
	setWidgetVisible(navbar_button, probe_type == THERMO_Z99 && !isOff && !isAntigelo);

	QFont aFont;
	FontManager::instance()->getFont(font_banTermo_tempImp, aFont);

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

	setpoint_label->setFont(aFont);
	setpoint_label->setAlignment(Qt::AlignHCenter);
	setpoint_label->setProperty("SecondFgColor", true);

	FontManager::instance()->getFont(font_banTermo_testo, aFont);
	local_temp_label->setFont(aFont);
	local_temp_label->setAlignment(Qt::AlignHCenter);
	local_temp_label->setText(local_temp);

	FSBannSimpleProbe::Draw();
}

void FSBannProbe::status_changed(QList<device_status*> sl)
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
				int sp = static_cast<int>(setpoint);
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
				if (curr_local.get_val() >= 0  && curr_local.get_val() <= 3 ||
						curr_local.get_val() >= 11 && curr_local.get_val() <= 13)
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
					qDebug("[TERMO] FSBannProbe::status_changed(): local status case not handled!");
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
				update = true;
			}

			stat_var curr_stat(stat_var::STAT);
			ds->read(device_status_temperature_probe_extra::STAT_INDEX, curr_stat);
			if (curr_stat.initialized())
			{
				switch (curr_stat.get_val())
				{
				case device_status_temperature_probe_extra::S_MAN:
					status = MANUAL;
					navbar_button->setPixmap(*icons_library.getIcon(IMG_AUTO));
					navbar_button->setPressedPixmap(*icons_library.getIcon(getPressedIconName(IMG_AUTO)));
					update = true;
					break;
				case device_status_temperature_probe_extra::S_AUTO:
					status = AUTOMATIC;
					navbar_button->setPixmap(*icons_library.getIcon(IMG_MAN));
					navbar_button->setPressedPixmap(*icons_library.getIcon(getPressedIconName(IMG_MAN)));
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
		Draw();

	FSBannSimpleProbe::status_changed(sl);
}

FSBannFancoil::FSBannFancoil(QDomNode n, temperature_probe_controlled *_dev, thermal_regulator *thermo_reg, QWidget *parent,
	TemperatureScale scale, const char *name) : FSBannProbe(n, _dev, thermo_reg, parent, scale), fancoil_buttons(this)
{
	dev = _dev;
	connect(dev, SIGNAL(status_changed(QList<device_status*>)), SLOT(status_changed(QList<device_status*>)));

	createFancoilButtons();
	fancoil_buttons.setExclusive(true);
	fancoil_status = 0;
	connect(&fancoil_buttons, SIGNAL(buttonClicked(int)), SLOT(handleFancoilButtons(int)));
}

void FSBannFancoil::createFancoilButtons()
{
	QHBoxLayout *hbox = new QHBoxLayout();
	for (int i = 0, id = 0; i < 8; i += 2, ++id)
	{
		QString path = QString(IMG_PATH) + FANCOIL_ICONS[i];
		QString path_pressed = QString(IMG_PATH) + FANCOIL_ICONS[i+1];
		BtButton *btn = new BtButton(this);
		btn->setPixmap(*icons_library.getIcon(path));
		btn->setPressedPixmap(*icons_library.getIcon(path_pressed));
		btn->setToggleButton(true);

		hbox->addWidget(btn);
		fancoil_buttons.addButton(btn, id);

		speed_to_btn_tbl[(id + 1) % 4] = id;
		btn_to_speed_tbl[id] = (id + 1) % 4;
	}
	main_layout.insertLayout(-1, hbox);
}

void FSBannFancoil::Draw()
{
	fancoil_buttons.button(fancoil_status)->setChecked(true);
	FSBannProbe::Draw();
}

void FSBannFancoil::handleFancoilButtons(int pressedButton)
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

void FSBannFancoil::status_changed(QList<device_status*> sl)
{
	bool update = false;

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (ds->get_type() == device_status::FANCOIL)
		{

			stat_var speed_var(stat_var::FANCOIL_SPEED);
			ds->read((int)device_status_fancoil::SPEED_INDEX, speed_var);

			// Set the fancoil Button in the buttons bar
			if (speed_to_btn_tbl.contains(speed_var.get_val()))
			{
				fancoil_status = speed_to_btn_tbl[speed_var.get_val()];
				update = true;
			}
			else
				qDebug("Fancoil speed val out of range (%d)", speed_var.get_val());
		}
	}

	if (update)
		Draw();
	FSBannProbe::status_changed(sl);
}

FSBannManual::FSBannManual(QWidget *parent, const char *name, thermal_regulator *_dev, TemperatureScale scale)
	: BannFullScreen(parent, "manual"),
	main_layout(this),
	temp_scale(scale),
	dev(_dev),
	setpoint_delta(5)
{
	descr = tr("Manual");
	descr_label = new BtLabel(this);
	main_layout.addWidget(descr_label);

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


	navbar_button = getButton(I_OK);
	connect(navbar_button, SIGNAL(clicked()), this, SLOT(performAction()));

	temp_label = new BtLabel(this);
	QHBoxLayout *hbox = new QHBoxLayout();

	const QString btn_min_img = QString("%1%2").arg(IMG_PATH).arg("btnmin.png");
	BtButton *btn = getButton(btn_min_img);
	btn->setAutoRepeat(true);
	connect(btn, SIGNAL(clicked()), this, SLOT(decSetpoint()));
	hbox->addWidget(btn);

	hbox->addWidget(temp_label);

	const QString btn_plus_img = QString("%1%2").arg(IMG_PATH).arg("btnplus.png");
	btn = getButton(btn_plus_img);
	btn->setAutoRepeat(true);
	connect(btn, SIGNAL(clicked()), this, SLOT(incSetpoint()));
	hbox->addWidget(btn);

	main_layout.addLayout(hbox);

	connect(dev, SIGNAL(status_changed(QList<device_status*>)), this, SLOT(status_changed(QList<device_status*>)));
}

void FSBannManual::performAction()
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

void FSBannManual::incSetpoint()
{
	// FIXME: forse c'e' da modificare i controlli in caso di fahrenheit
	if (temp >= maximum_manual_temp)
		return;
	else
		temp += setpoint_delta;
	Draw();
}

void FSBannManual::decSetpoint()
{
	if (temp <= minimum_manual_temp)
		return;
	else
		temp -= setpoint_delta;
	Draw();
}

void FSBannManual::Draw()
{
	QFont aFont;
	FontManager::instance()->getFont(font_banTermo_testo, aFont);
	descr_label->setFont(aFont);
	descr_label->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
	descr_label->setText(descr);

	FontManager::instance()->getFont(font_banTermo_tempImp, aFont);
	temp_label->setFont(aFont);
	temp_label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

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

	temp_label->setProperty("SecondFgColor", true);
}

BtButton *FSBannManual::customButton()
{
	return navbar_button;
}

void FSBannManual::status_changed(QList<device_status*> sl)
{
	bool update = false;

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (ds->get_type() == device_status::THERMAL_REGULATOR_4Z)
		{
			stat_var curr_sp(stat_var::SP);
			ds->read(device_status_thermal_regulator::SP_INDEX, curr_sp);
			if (curr_sp.initialized())
			{
				switch (temp_scale)
				{
				case CELSIUS:
					temp = bt2Celsius(static_cast<unsigned>(curr_sp.get_val()));
					break;
				case FAHRENHEIT:
					temp = bt2Fahrenheit(static_cast<unsigned>(curr_sp.get_val()));
					break;
				default:
					qWarning("BannSimpleProbe: unknown temperature scale, defaulting to celsius");
					temp = bt2Celsius(static_cast<unsigned>(curr_sp.get_val()));
				}
				update = true;
			}
		}
	}

	if (update)
		Draw();
}

FSBannManualTimed::FSBannManualTimed(QWidget *parent, const char *name, thermal_regulator_4z *_dev, TemperatureScale scale)
	: FSBannManual(parent, name, _dev, scale),
	dev(_dev)
{
	time_edit = new BtTimeEdit(this);
	main_layout.addWidget(time_edit);

	connect(dev, SIGNAL(status_changed(QList<device_status*>)), this, SLOT(status_changed(QList<device_status*>)));
	connect(navbar_button, SIGNAL(clicked()), this, SLOT(performAction()));
}

void FSBannManualTimed::performAction()
{
	emit(timeAndTempSelected(time_edit->time(), temp));
}

void FSBannManualTimed::setMaxHours(int max)
{
	time_edit->setMaxHours(max);
}

void FSBannManualTimed::setMaxMinutes(int max)
{
	time_edit->setMaxMinutes(max);
}

FSBannDate::FSBannDate(QWidget *parent, const char *name)
	: BannFullScreen(parent, name),
	main_layout(this)
{
	const QString top_img = QString("%1%2").arg(IMG_PATH).arg("calendario.png");
	BtButton *top = new BtButton(this);
	top->setPixmap(top_img);
	top->setDown(true);
	top->setEnabled(false);
	main_layout.addWidget(top, 0, Qt::AlignHCenter);

	date_edit = new BtDateEdit(this);
	main_layout.addWidget(date_edit);
}

QDate FSBannDate::date()
{
	return date_edit->date();
}

FSBannTime::FSBannTime(QWidget *parent, const char *name)
	: BannFullScreen(parent, name),
	main_layout(this)
{
	const QString i_top_img = QString("%1%2").arg(IMG_PATH).arg("orologio.png");
	BtButton *top = new BtButton(this);
	top->setPixmap(i_top_img);
	top->setDown(true);
	top->setEnabled(false);
	main_layout.addWidget(top, 0, Qt::AlignHCenter);

	time_edit = new BtTimeEdit(this);
	main_layout.addWidget(time_edit);
}

BtTime FSBannTime::time()
{
	return time_edit->time();
}

FSBannTermoReg::FSBannTermoReg(QDomNode n, QWidget *parent, const char *name)
	: BannFullScreen(parent, name), main_layout(this)
{
	conf_root = n;
	navbar_button = getButton(IMG_SETTINGS);

	setProperty("SecondFgColor", true);

	// Put a sensible default for the description
	QDomNode descr = conf_root.namedItem("descr");
	if (!descr.isNull())
		description = descr.toElement().text();
	else
	{
		qDebug("[TERMO] FSBannTermoReg ctor: no description found, maybe wrong node conf?");
		description = "Wrong node";
	}
	description_label = new BtLabel(this);
	description_label->setAlignment(Qt::AlignHCenter);

	description_visible = true;

	season_icon = getLabelWithPixmap(IMG_SUMMER_S, this, Qt::AlignHCenter);

	mode_icon = getLabelWithPixmap(IMG_THERMR, this, Qt::AlignHCenter);

	main_layout.addWidget(mode_icon);
	main_layout.addWidget(description_label);
	main_layout.addWidget(season_icon);
	main_layout.setAlignment(Qt::AlignHCenter);

	date_edit = 0;
	time_edit = 0;
	program_choice = 0;
	temp_scale = readTemperatureScale();
}

BtButton *FSBannTermoReg::customButton()
{
	return navbar_button;
}

void FSBannTermoReg::Draw()
{
	QFont aFont;
	FontManager::instance()->getFont(font_banTermo_tempImp, aFont);
	setWidgetVisible(description_label, description_visible);
	description_label->setFont(aFont);
	description_label->setText(description);
	// should I color text only if it is a setpoint temperature?
	// TODO: verificare che venga impostato correttamente!!
	description_label->setProperty("SecondFgColor", true);
}

void FSBannTermoReg::status_changed(QList<device_status*> sl)
{
	int season = thermal_regulator::SUMMER;

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (ds->get_type() == device_status::THERMAL_REGULATOR_4Z || ds->get_type() == device_status::THERMAL_REGULATOR_99Z)
		{
			stat_var curr_season(stat_var::SEASON);
			ds->read(device_status_thermal_regulator::SEASON_INDEX, curr_season);
			switch (curr_season.get_val())
			{
			case thermal_regulator::SUMMER:
				{
					const QString img = QString(IMG_PATH) + "estate_s.png";
					QPixmap *icon = icons_library.getIcon(img);
					season_icon->setPixmap(*icon);
					season = thermal_regulator::SUMMER;
				}
				break;
			case thermal_regulator::WINTER:
				{
					const QString img = QString(IMG_PATH) + "inverno_s.png";
					QPixmap *icon = icons_library.getIcon(img);
					season_icon->setPixmap(*icon);
					season = thermal_regulator::WINTER;
				}
				break;
			}

			stat_var curr_status(stat_var::THERMR);
			ds->read(device_status_thermal_regulator::STATUS_INDEX, curr_status);
			switch (curr_status.get_val())
			{
			case device_status_thermal_regulator::OFF:
				{
					QPixmap *icon = icons_library.getIcon(IMG_OFF_S);
					mode_icon->setPixmap(*icon);
					description_visible = false;
				}
				break;
			case device_status_thermal_regulator::PROTECTION:
				{
					QPixmap *icon = icons_library.getIcon(IMG_ANTIFREEZE_S);
					mode_icon->setPixmap(*icon);
					description_visible = false;
				}
				break;
			case device_status_thermal_regulator::MANUAL:
			case device_status_thermal_regulator::MANUAL_TIMED:
				{
					QString i_img = QString(IMG_PATH);
					if (curr_status.get_val() == device_status_thermal_regulator::MANUAL)
						i_img += "manuale.png";
					else
						i_img += "manuale_temporizzato.png";
					QPixmap *icon = icons_library.getIcon(i_img);
					mode_icon->setPixmap(*icon);
					stat_var curr_sp(stat_var::SP);
					ds->read(device_status_thermal_regulator::SP_INDEX, curr_sp);
					// remember: stat_var::get_val() returns an int
					switch (temp_scale)
					{
					case CELSIUS:
						description = celsiusString(bt2Celsius(
									static_cast<unsigned>(curr_sp.get_val())));
						break;
					case FAHRENHEIT:
						description = fahrenheitString(bt2Fahrenheit(
									static_cast<unsigned>(curr_sp.get_val())));
						break;
					default:
						qWarning("TermoReg status_changed: unknown scale, defaulting to celsius");
						description = celsiusString(curr_sp.get_val());
					}
					description_visible = true;
				}
				break;
			case device_status_thermal_regulator::WEEK_PROGRAM:
				{
					const QString i_img = QString(IMG_PATH) + "settimanale.png";
					QPixmap *icon = icons_library.getIcon(i_img);
					mode_icon->setPixmap(*icon);

					stat_var curr_program(stat_var::PROGRAM);
					ds->read(device_status_thermal_regulator::PROGRAM_INDEX, curr_program);

					// now search the description in the DOM
					int program = curr_program.get_val();

					switch (season)
					{
					case thermal_regulator::SUMMER:
						description = lookupProgramDescription("summer", program);
						break;
					case thermal_regulator::WINTER:
						description = lookupProgramDescription("winter", program);
						break;
					}
					description_visible = true;
				}
				break;
			case device_status_thermal_regulator::SCENARIO:
				{
					const QString i_img = QString(IMG_PATH) + "scenari.png";
					QPixmap *icon = icons_library.getIcon(i_img);
					mode_icon->setPixmap(*icon);

					stat_var curr_scenario(stat_var::SCENARIO);
					ds->read(device_status_thermal_regulator::SCENARIO_INDEX, curr_scenario);

					int scenario = curr_scenario.get_val();
					switch (season)
					{
					case thermal_regulator::SUMMER:
						description = lookupScenarioDescription("summer", scenario);
						break;
					case thermal_regulator::WINTER:
						description = lookupScenarioDescription("winter", scenario);
						break;
					}
					description_visible = true;
				}
				break;
			case device_status_thermal_regulator::HOLIDAY:
				{
					const QString i_img = QString(IMG_PATH) + "feriale.png";
					QPixmap *icon = icons_library.getIcon(i_img);
					mode_icon->setPixmap(*icon);
					description_visible = false;
				}
				break;
			case device_status_thermal_regulator::WEEKEND:
				{
					const QString i_img = QString(IMG_PATH) + "festivo.png";
					QPixmap *icon = icons_library.getIcon(i_img);
					mode_icon->setPixmap(*icon);
					description_visible = false;
				}
				break;
			default:
				break;
			}
		}
	}

	Draw();
}

QString FSBannTermoReg::lookupProgramDescription(QString season, int program_number)
{
	QDomNode prog = conf_root.namedItem(season);
	if (!prog.isNull())
	{
		prog = prog.namedItem("prog");
		if (!prog.isNull())
		{
			QDomNode iter = prog.firstChild();
			for (int i = 1; i != program_number && !(iter.isNull()); iter = iter.nextSibling(), ++i)
				;
			if (!iter.isNull())
				return iter.toElement().text();
			else
				qWarning("[TERMO] FSBannTermoReg::lookupProgramDescription WEEK PROGRAM: wrong node");
		}
		else
			qWarning("[TERMO] FSBannTermoReg::lookupProgramDescription WEEK PROGRAM: wrong node");
	}
	qDebug("FSBannTermoReg::lookupProgramDescription: You did not supply the correct season.");
	return "";
}

QString FSBannTermoReg::lookupScenarioDescription(QString season, int scenario_number)
{
	QDomNode scen = conf_root.namedItem(season);
	if (!scen.isNull())
	{
		scen = scen.namedItem("scen");
		if (!scen.isNull())
		{
			QDomNode iter = scen.firstChild();
			for (int i = 1; i != scenario_number && !(iter.isNull()); iter = iter.nextSibling(), ++i)
				;
			if (!iter.isNull())
				return iter.toElement().text();
			else
				qWarning("[TERMO] FSBannTermoReg::lookupScenarioDescription SCENARIO PROGRAM: wrong node");
		}
		else
			qWarning("[TERMO] FSBannTermoReg::lookupScenarioDescription SCENARIO PROGRAM: wrong node");
	}
	qDebug("FSBannTermoReg::lookupScenarioDescription: You did not supply the correct season.");
	return "";
}

FSBannTermoReg4z::FSBannTermoReg4z(QDomNode n, thermal_regulator_4z *device, QWidget *parent, const char *name)
	: FSBannTermoReg(n, parent, name)
{
	_dev = device;
	connect(_dev, SIGNAL(status_changed(QList<device_status*>)), SLOT(status_changed(QList<device_status*>)));
	createSettingsMenu();
}

thermal_regulator *FSBannTermoReg4z::dev()
{
	return _dev;
}

void FSBannTermoReg4z::createSettingsMenu()
{
	settings = new sottoMenu(0, "settings");
	if (settings)
	{
		connect(navbar_button, SIGNAL(clicked()), settings, SLOT(show()));
		connect(navbar_button, SIGNAL(clicked()), settings, SLOT(raise()));
		connect(settings, SIGNAL(Closed()), settings, SLOT(hide()));
		// propagate freeze signal to settings submenu
		connect(parentWidget(), SIGNAL(freezePropagate(bool)), settings, SLOT(freezed(bool)));
		connect(parentWidget(), SIGNAL(freezePropagate(bool)), settings, SIGNAL(freezePropagate(bool)));
		// hide children
		connect(parentWidget(), SIGNAL(hideChildren()), settings, SLOT(hide()));
	}
	else
		qFatal("[TERMO] could not create settings menu");


	weekSettings(settings, conf_root, _dev);
	manualSettings(settings, _dev);

	timedManualSettings(settings, _dev);

	holidaySettings(settings, conf_root, _dev);

	weekendSettings(settings, conf_root, _dev);

	// off banner
	BannOff *off = new BannOff(settings, "OFF", _dev);
	settings->appendBanner(off);
	connect(off, SIGNAL(clicked()), settings, SLOT(hide()));

	// antifreeze banner
	BannAntifreeze *antifreeze = new BannAntifreeze(settings, "antifreeze", _dev);
	settings->appendBanner(antifreeze);
	connect(antifreeze, SIGNAL(clicked()), settings, SLOT(hide()));

	// summer_winter banner
	BannSummerWinter *summer_winter = new BannSummerWinter(settings, "Summer/Winter", _dev);
	settings->appendBanner(summer_winter);
	connect(summer_winter, SIGNAL(clicked()), settings, SLOT(hide()));
	settings->hide();
}

FSBannTermoReg99z::FSBannTermoReg99z(QDomNode n, thermal_regulator_99z *device, QWidget *parent, const char *name)
	: FSBannTermoReg(n, parent, name)
{
	_dev = device;
	connect(_dev, SIGNAL(status_changed(QList<device_status*>)), SLOT(status_changed(QList<device_status*>)));
	createSettingsMenu();
}

thermal_regulator *FSBannTermoReg99z::dev()
{
	return _dev;
}

void FSBannTermoReg99z::createSettingsMenu()
{
	settings = new sottoMenu(0, "settings");
	if (settings)
	{
		connect(navbar_button, SIGNAL(clicked()), settings, SLOT(show()));
		connect(navbar_button, SIGNAL(clicked()), settings, SLOT(raise()));
		connect(settings, SIGNAL(Closed()), settings, SLOT(hide()));
		// propagate freeze signal to settings submenu
		connect(parentWidget(), SIGNAL(freezePropagate(bool)), settings, SLOT(freezed(bool)));
		connect(parentWidget(), SIGNAL(freezePropagate(bool)), settings, SIGNAL(freezePropagate(bool)));
		// hide children
		connect(parentWidget(), SIGNAL(hideChildren()), settings, SLOT(hide()));
	}
	else
		qFatal("[TERMO] could not create settings menu");

	weekSettings(settings, conf_root, _dev);
	manualSettings(settings, _dev);

	scenarioSettings(settings, conf_root, _dev);

	holidaySettings(settings, conf_root, _dev);

	weekendSettings(settings, conf_root, _dev);

	// off banner
	BannOff *off = new BannOff(settings, "OFF", _dev);
	settings->appendBanner(off);
	connect(off, SIGNAL(clicked()), settings, SLOT(hide()));

	// antifreeze banner
	BannAntifreeze *antifreeze = new BannAntifreeze(settings, "antifreeze", _dev);
	settings->appendBanner(antifreeze);
	connect(antifreeze, SIGNAL(clicked()), settings, SLOT(hide()));

	// summer_winter banner
	BannSummerWinter *summer_winter = new BannSummerWinter(settings, "Summer/Winter", _dev);
	settings->appendBanner(summer_winter);
	connect(summer_winter, SIGNAL(clicked()), settings, SLOT(hide()));
	settings->hide();
}

//
// ------------- Utility functions to create thermal regulator settings menus -------------------
//
void FSBannTermoReg::manualSettings(sottoMenu *settings, thermal_regulator *dev)
{
	char i_manual[100];
	sprintf(i_manual, "%s%s", IMG_PATH, "manuale.png");

	// manual banner
	bannPuls *manual = new bannPuls(settings, "Manual");
	manual->SetIcons(IMG_RIGHT_ARROW, 0, i_manual);

	settings->appendBanner(manual);
	manual_menu = new sottoMenu(0, "manual", 10, MAX_WIDTH, MAX_HEIGHT, 1);
	connect(manual, SIGNAL(sxClick()), manual_menu, SLOT(show()));
	connect(manual, SIGNAL(sxClick()), manual_menu, SLOT(raise()));
	// propagate freeze signal
	connect(settings, SIGNAL(freezePropagate(bool)), manual_menu, SLOT(freezed(bool)));
	connect(settings, SIGNAL(freezePropagate(bool)), manual_menu, SIGNAL(freezePropagate(bool)));
	// hide children
	connect(settings, SIGNAL(hideChildren()), manual_menu, SLOT(hide()));

	FSBannManual *bann = new FSBannManual(manual_menu, 0, dev, temp_scale);
	bann->setProperty("SecondFgColor", true);

	manual_menu->appendBanner(bann);
	connect(bann, SIGNAL(temperatureSelected(unsigned)), this, SLOT(manualSelected(unsigned)));
	connect(manual_menu, SIGNAL(Closed()), this, SLOT(manualCancelled()));
	manual_menu->hide();
}

void FSBannTermoReg::manualCancelled()
{
	manual_menu->hide();
}

void FSBannTermoReg::manualSelected(unsigned temp)
{
	dev()->setManualTemp(temp);
	manual_menu->hide();
	settings->hide();
}

void FSBannTermoReg::weekSettings(sottoMenu *settings, QDomNode conf, thermal_regulator *dev)
{
	char i_weekly[100];
	sprintf(i_weekly, "%s%s", IMG_PATH, "settimanale.png");

	bannPuls *weekly = new bannPuls(settings, "weekly");
	weekly->SetIcons(IMG_RIGHT_ARROW, 0, i_weekly);
	settings->appendBanner(weekly);

	program_menu = new WeeklyMenu(0, "weekly", conf);
	connect(dev, SIGNAL(status_changed(QList<device_status*>)), program_menu, SLOT(status_changed(QList<device_status*>)));

	connect(weekly, SIGNAL(sxClick()), program_menu, SLOT(show()));
	connect(weekly, SIGNAL(sxClick()), program_menu, SLOT(raise()));
	// propagate freeze signal
	connect(settings, SIGNAL(freezePropagate(bool)), program_menu, SLOT(freezed(bool)));
	connect(settings, SIGNAL(freezePropagate(bool)), program_menu, SIGNAL(freezePropagate(bool)));
	// hide children
	connect(settings, SIGNAL(hideChildren()), program_menu, SLOT(hide()));

	connect(program_menu, SIGNAL(Closed()), this, SLOT(weekProgramCancelled()));
	connect(program_menu, SIGNAL(programClicked(int)), this, SLOT(weekProgramSelected(int)));
	program_menu->hide();
}

void FSBannTermoReg::weekProgramCancelled()
{
	program_menu->hide();
}

void FSBannTermoReg::weekProgramSelected(int program)
{
	dev()->setWeekProgram(program);
	program_menu->hide();
	settings->hide();
}

void FSBannTermoReg::holidaySettings(sottoMenu *settings, QDomNode conf, thermal_regulator *dev)
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

void FSBannTermoReg::weekendSettings(sottoMenu *settings, QDomNode conf, thermal_regulator *dev)
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

banner *FSBannTermoReg::createHolidayWeekendBanner(sottoMenu *settings, QString icon)
{
	QByteArray bann_img = QString(QString(IMG_PATH) + icon).toAscii();

	bannPuls *bann = new bannPuls(settings, 0);
	bann->SetIcons(IMG_RIGHT_ARROW, 0, bann_img);
	settings->appendBanner(bann);
	return bann;
}

DateEditMenu *FSBannTermoReg::createDateEdit(sottoMenu *settings)
{
	DateEditMenu *date_edit = new DateEditMenu(0, "date edit");
	// propagate freeze signal
	connect(settings, SIGNAL(freezePropagate(bool)), date_edit, SLOT(freezed(bool)));
	connect(settings, SIGNAL(freezePropagate(bool)), date_edit, SIGNAL(freezePropagate(bool)));
	// hide children
	connect(settings, SIGNAL(hideChildren()), date_edit, SLOT(hide()));
	date_edit->hide();
	connect(date_edit, SIGNAL(Closed()), this, SLOT(dateCancelled()));
	connect(date_edit, SIGNAL(dateSelected(QDate)), this, SLOT(dateSelected(QDate)));
	return date_edit;
}

TimeEditMenu *FSBannTermoReg::createTimeEdit(sottoMenu *settings)
{
	TimeEditMenu *time_edit = new TimeEditMenu(0, "time edit");
	// propagate freeze signal
	connect(settings, SIGNAL(freezePropagate(bool)), time_edit, SLOT(freezed(bool)));
	connect(settings, SIGNAL(freezePropagate(bool)), time_edit, SIGNAL(freezePropagate(bool)));
	// hide children
	connect(settings, SIGNAL(hideChildren()), time_edit, SLOT(hide()));
	time_edit->hide();
	connect(time_edit, SIGNAL(timeSelected(BtTime)), this, SLOT(timeSelected(BtTime)));
	connect(time_edit, SIGNAL(Closed()), this, SLOT(timeCancelled()));
	return time_edit;
}

WeeklyMenu *FSBannTermoReg::createProgramChoice(sottoMenu *settings, QDomNode conf, device *dev)
{
	WeeklyMenu *program_choice = new WeeklyMenu(0, "weekly program edit", conf);
	connect(dev, SIGNAL(status_changed(QList<device_status*>)), program_choice, SLOT(status_changed(QList<device_status*>)));
	// propagate freeze signal
	connect(settings, SIGNAL(freezePropagate(bool)), program_choice, SLOT(freezed(bool)));
	connect(settings, SIGNAL(freezePropagate(bool)), program_choice, SIGNAL(freezePropagate(bool)));
	// hide children
	connect(settings, SIGNAL(hideChildren()), program_choice, SLOT(hide()));
	program_choice->hide();
	connect(program_choice, SIGNAL(programClicked(int)), this, SLOT(weekendHolidaySettingsEnd(int)));
	connect(program_choice, SIGNAL(Closed()), this, SLOT(programCancelled()));
	return program_choice;
}

void FSBannTermoReg::holidaySettingsStart()
{
	weekendHolidayStatus = HOLIDAY;
	date_edit->show();
	date_edit->raise();
}

void FSBannTermoReg::weekendSettingsStart()
{
	weekendHolidayStatus = WEEKEND;
	date_edit->show();
	date_edit->raise();
}

void FSBannTermoReg::dateCancelled()
{
	date_edit->hide();
}

void FSBannTermoReg::dateSelected(QDate d)
{
	date_end = d;
	time_edit->show();
	time_edit->raise();
}

void FSBannTermoReg::timeCancelled()
{
	time_edit->hide();
}

void FSBannTermoReg::timeSelected(BtTime t)
{
	time_end = t;
	program_choice->show();
	program_choice->raise();
}

void FSBannTermoReg::programCancelled()
{
	program_choice->hide();
}

void FSBannTermoReg::weekendHolidaySettingsEnd(int program)
{
	if (weekendHolidayStatus == WEEKEND)
		dev()->setWeekendDateTime(date_end, time_end, program);
	else if (weekendHolidayStatus == HOLIDAY)
		dev()->setHolidayDateTime(date_end, time_end, program);
	else
		qWarning("FSBannTermoReg::weekendHolidaySettingsEnd: unknown status");
	program_choice->hide();
	time_edit->hide();
	date_edit->hide();
	settings->hide();
}

void FSBannTermoReg4z::timedManualSettings(sottoMenu *settings, thermal_regulator_4z *dev)
{
	char i_manual[100];
	sprintf(i_manual, "%s%s", IMG_PATH, "manuale_temporizzato.png");
	// timed manual banner
	bannPuls *manual_timed = new bannPuls(settings, "manual_timed");
	manual_timed->SetIcons(IMG_RIGHT_ARROW, 0, i_manual);

	settings->appendBanner(manual_timed);
	timed_manual_menu = new sottoMenu(0, "manual_timed", 10, MAX_WIDTH, MAX_HEIGHT, 1);

	FSBannManualTimed *bann = new FSBannManualTimed(timed_manual_menu, 0, dev);

	bann->setProperty("SecondFgColor", true);
	bann->setMaxHours(25);

	timed_manual_menu->appendBanner(bann);
	connect(manual_timed, SIGNAL(sxClick()), timed_manual_menu, SLOT(show()));
	connect(manual_timed, SIGNAL(sxClick()), timed_manual_menu, SLOT(raise()));
	// propagate freeze signal
	connect(settings, SIGNAL(freezePropagate(bool)), timed_manual_menu, SLOT(freezed(bool)));
	connect(settings, SIGNAL(freezePropagate(bool)), timed_manual_menu, SIGNAL(freezePropagate(bool)));
	// hide children
	connect(settings, SIGNAL(hideChildren()), timed_manual_menu, SLOT(hide()));

	connect(timed_manual_menu, SIGNAL(Closed()), this, SLOT(manualTimedCancelled()));
	connect(bann, SIGNAL(timeAndTempSelected(BtTime, int)), this, SLOT(manualTimedSelected(BtTime, int)));
	timed_manual_menu->hide();
}

void FSBannTermoReg4z::manualTimedCancelled()
{
	timed_manual_menu->hide();
}

void FSBannTermoReg4z::manualTimedSelected(BtTime time, int temp)
{
	_dev->setManualTempTimed(temp, time);
	timed_manual_menu->hide();
	settings->hide();
}

void FSBannTermoReg99z::scenarioSettings(sottoMenu *settings, QDomNode conf, thermal_regulator_99z *dev)
{
	char i_scenario[100];
	sprintf(i_scenario, "%s%s", IMG_PATH, "scenari.png");

	bannPuls *scenario = new bannPuls(settings, "scenario");
	scenario->SetIcons(IMG_RIGHT_ARROW, 0, i_scenario);
	settings->appendBanner(scenario);

	scenario_menu = new ScenarioMenu(0, "weekly", conf);
	connect(dev, SIGNAL(status_changed(QList<device_status*>)), scenario_menu, SLOT(status_changed(QList<device_status*>)));

	connect(scenario, SIGNAL(sxClick()), scenario_menu, SLOT(show()));
	connect(scenario, SIGNAL(sxClick()), scenario_menu, SLOT(raise()));
	// propagate freeze signal
	connect(settings, SIGNAL(freezePropagate(bool)), scenario_menu, SLOT(freezed(bool)));
	connect(settings, SIGNAL(freezePropagate(bool)), scenario_menu, SIGNAL(freezePropagate(bool)));
	// hide children
	connect(settings, SIGNAL(hideChildren()), scenario_menu, SLOT(hide()));

	connect(scenario_menu, SIGNAL(Closed()), this, SLOT(scenarioCancelled()));
	connect(scenario_menu, SIGNAL(programClicked(int)), this, SLOT(scenarioSelected(int)));
	scenario_menu->hide();
}

void FSBannTermoReg99z::scenarioCancelled()
{
	scenario_menu->hide();
}

void FSBannTermoReg99z::scenarioSelected(int scenario)
{
	_dev->setScenario(scenario);
	scenario_menu->hide();
	settings->hide();
}
