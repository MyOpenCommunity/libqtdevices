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
#include "main.h"
#include "sottomenu.h"
#include "btbutton.h"
#include "device.h"
#include "bannsettings.h"
#include "device_cache.h"

#include <qobjectlist.h>

#define I_OK  IMG_PATH"btnok.png"
#define IMG_SETTINGS IMG_PATH"setscen.png"
#define IMG_PLUS IMG_PATH "btnplus.png"
#define IMG_MINUS IMG_PATH "btnmin.png"
#define IMG_AUTO IMG_PATH "btnauto.png"
#define IMG_MAN IMG_PATH "btnman.png"
#define IMG_RIGHT_ARROW IMG_PATH "arrrg.png"
#define IMG_SUMMER_S IMG_PATH "estate_s.png"
#define IMG_THERMR IMG_PATH "centrale.png"
#define IMG_OFF_S IMG_PATH "sondaoff.png"
#define IMG_ANTIFREEZE_S IMG_PATH "sondaantigelo.png"

void setVisible(QWidget *w, bool visible)
{
	if (visible)
		w->show();
	else
		w->hide();
}

QLabel *getLabelWithPixmap(const char *img, QWidget *parent, int alignment)
{
	QLabel *tmp = new QLabel(parent);
	tmp->setPixmap(*icons_library.getIcon(img));
	tmp->setAlignment(alignment);
	return tmp;
}

static const char *FANCOIL_ICONS[] = {"fancoil1off.png", "fancoil1on.png", "fancoil2off.png", "fancoil2on.png",
	"fancoil3off.png", "fancoil3on.png", "fancoilAoff.png", "fancoilAon.png"};

BannFullScreen::BannFullScreen(QWidget *parent, const char *name)
	: banner(parent, name)
{
}

void BannFullScreen::setSecondForeground(QColor fg2)
{
	second_fg = fg2;
}

void BannFullScreen::Draw()
{
	banner::Draw();
}

BtButton *BannFullScreen::getButton(const char *img)
{
	BtButton *btn = new BtButton(this);
	btn->setPixmap(*icons_library.getIcon(img));
	btn->setPressedPixmap(*icons_library.getIcon(getPressedIconName(img)));
	return btn;
}

void BannFullScreen::setBGColor(QColor bg)
{
	QObjectList *l = queryList("QWidget");

	for (QObjectListIt iter(*l); QObject *obj = iter.current(); ++iter)
		((QWidget*)obj)->setPaletteBackgroundColor(bg);
	delete l;
	banner::setBGColor(bg);
}

void BannFullScreen::setFGColor(QColor fg)
{
	QObjectList *l = queryList("QWidget");

	for (QObjectListIt iter(*l); QObject *obj = iter.current(); ++iter)
		((QWidget*)obj)->setPaletteForegroundColor(fg);
	delete l;
	banner::setFGColor(fg);
}

QString BannFullScreen::extractAddress(QDomNode n)
{
	if (!n.isNull())
	{
		QDomNode where = n.namedItem("where");
		if (!where.isNull() && where.isElement())
			return where.toElement().text();
	}
	return QString();
}

BannFullScreen *getBanner(BannID id, QWidget *parent, QDomNode n, QString ind_centrale)
{
	BannFullScreen *bfs = 0;
	switch (id)
	{
		case fs_nc_probe:
			bfs = new FSBannSimpleProbe(parent, n);
			break;
		case fs_4z_probe:
			bfs = new FSBannProbe(n, ind_centrale, false, parent);
			break;
		case fs_99z_probe:
			bfs = new FSBannProbe(n, ind_centrale, true, parent);
			break;
		case fs_4z_fancoil:
			bfs = new FSBannFancoil(n, ind_centrale, THERMO_Z4, false, parent);
			break;
		case fs_99z_fancoil:
			bfs = new FSBannFancoil(n, ind_centrale, THERMO_Z99, true, parent);
			break;
		case fs_4z_thermal_regulator:
			bfs = new FSBannTermoReg4z(n, ind_centrale, parent);
			break;
		case fs_99z_thermal_regulator:
			bfs = new FSBannSimpleProbe(parent, n);
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

FSBannSimpleProbe::FSBannSimpleProbe(QWidget *parent, QDomNode n, const char *name)
	: BannFullScreen(parent, name),
	main_layout(this)
{
	descr_label = new QLabel(this);
	main_layout.addWidget(descr_label);

	temp_label = new QLabel(this);
	main_layout.addWidget(temp_label);
	main_layout.setAlignment(Qt::AlignHCenter);

	temp = "-23.5"TEMP_DEGREES"C";
	descr = n.namedItem("descr").toElement().text();
}


void FSBannSimpleProbe::Draw()
{
	QFont aFont;
	FontManager::instance()->getFont(font_banTermo_tempMis, aFont);
	temp_label->setFont(aFont);
	temp_label->setAlignment(AlignHCenter);
	temp_label->setText(temp);

	FontManager::instance()->getFont(font_banTermo_testo, aFont);
	descr_label->setFont(aFont);
	descr_label->setAlignment(AlignHCenter);
	descr_label->setText(descr);

	banner::Draw();
}

void FSBannSimpleProbe::status_changed(QPtrList<device_status> list)
{
	QPtrListIterator<device_status> it (list);
	device_status *dev;
	bool update = false;

	while ((dev = it.current()) != 0)
	{
		++it;
		if (dev->get_type() == device_status::TEMPERATURE_PROBE)
		{
			stat_var curr_temp(stat_var::TEMPERATURE);
			dev->read(device_status_temperature_probe::TEMPERATURE_INDEX, curr_temp);
			float icx = curr_temp.get_val();
			QString qtemp = "";
			char tmp[10];
			if (icx >= 1000)
			{
				icx = icx - 1000;
				qtemp = "-";
			}
			icx /= 10;
			sprintf(tmp, "%.1f", icx);
			qtemp += tmp;
			qtemp += TEMP_DEGREES"C";
			temp = qtemp;
			update = true;
		}
	}

	if (update)
		Draw();
}

FSBannProbe::FSBannProbe(QDomNode n, QString ind_centrale, bool change_status, QWidget *parent,const char *name)
	: FSBannSimpleProbe(parent, n),
	delta_setpoint(false),
	setpoint_delay(2000),
	setpoint_delta(5),
	minimum_manual_temp(35),
	maximum_manual_temp(395)
{
	status = AUTOMATIC;
	status_change_enabled = change_status;
	navbar_button = getButton(IMG_MAN);
	navbar_button->hide();
	conf_root = n;

	QString simple_address = extractAddress(conf_root);
	if (simple_address.isNull())
		qFatal("FSBannProbe ctor: wrong address, configuration is wrong");
	QString where_composed = simple_address + "#" + ind_centrale;

	thermo_type_t probe_type = status_change_enabled ? THERMO_Z99 : THERMO_Z4;

	dev = static_cast<temperature_probe_controlled *>
		(btouch_device_cache.get_temperature_probe_controlled(where_composed.ascii(), probe_type, false, ind_centrale.ascii(), simple_address.ascii()));

	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), SLOT(status_changed(QPtrList<device_status>)));
	connect(navbar_button, SIGNAL(clicked()), SLOT(changeStatus()));

	QHBoxLayout *hbox = new QHBoxLayout();

	btn_minus = getButton(IMG_MINUS);
	btn_minus->hide();
	hbox->addWidget(btn_minus);

	setpoint_label = new QLabel(this);
	hbox->addWidget(setpoint_label);

	btn_plus = getButton(IMG_PLUS);
	btn_plus->hide();
	hbox->addWidget(btn_plus);

	icon_antifreeze = getLabelWithPixmap(IMG_ANTIFREEZE_S, this, AlignHCenter);
	hbox->addWidget(icon_antifreeze);

	icon_off = getLabelWithPixmap(IMG_OFF_S, this, AlignHCenter);
	hbox->addWidget(icon_off);

	main_layout.addLayout(hbox);
	main_layout.setStretchFactor(hbox, 1);

	local_temp_label = new QLabel(this);
	main_layout.addWidget(local_temp_label);
	main_layout.setStretchFactor(local_temp_label, 1);

	setpoint = 1235;
	local_temp = "0";
	isOff = false;
	isAntigelo = false;

	connect(&setpoint_timer, SIGNAL(timeout()), this, SLOT(setSetpoint()));
}

void FSBannProbe::changeStatus()
{
	if (status == AUTOMATIC)
		dev->setManual(setpoint);
	else
		dev->setAutomatic();
}

BtButton *FSBannProbe::customButton()
{
	if (status_change_enabled)
		return navbar_button;
	return 0;
}

void FSBannProbe::incSetpoint()
{
	if (setpoint > maximum_manual_temp)
		return;
	else
		setpoint += setpoint_delta;
	Draw();
	setpoint_timer.start(setpoint_delay);
	delta_setpoint = true;
}

void FSBannProbe::decSetpoint()
{
	if (setpoint < minimum_manual_temp)
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
	dev->setManual(setpoint);
}

void FSBannProbe::Draw()
{
	setVisible(btn_minus, status == MANUAL && status_change_enabled);
	setVisible(btn_plus, status == MANUAL && status_change_enabled);
	setVisible(setpoint_label, !isOff && !isAntigelo);
	setVisible(local_temp_label, local_temp != "0");
	setVisible(icon_off, isOff);
	setVisible(icon_antifreeze, isAntigelo);

	QFont aFont;
	FontManager::instance()->getFont(font_banTermo_tempImp, aFont);

	QString str;
	float icx;
	char tmp[10];
	icx = setpoint;
	qDebug("temperatura setpoint: %d",(int)icx);
	str = "";
	if (icx>=1000)
	{
		str = "-";
		icx=icx-1000;
	}
	icx/=10;
	sprintf(tmp,"%.1f",icx);
	str += tmp;
	str += TEMP_DEGREES"C";
	setpoint_label->setText(str);

	setpoint_label->setFont(aFont);
	setpoint_label->setAlignment(AlignHCenter);
	setpoint_label->setPaletteForegroundColor(second_fg);

	FontManager::instance()->getFont(font_banTermo_testo, aFont);
	local_temp_label->setFont(aFont);
	local_temp_label->setAlignment(AlignHCenter);
	local_temp_label->setText(local_temp);

	FSBannSimpleProbe::Draw();
}

void FSBannProbe::status_changed(QPtrList<device_status> list)
{
	QPtrListIterator<device_status> it (list);
	device_status *dev;
	bool update = false;

	while ((dev = it.current()) != 0)
	{
		++it;
		if (dev->get_type() == device_status::TEMPERATURE_PROBE_EXTRA)
		{
			stat_var curr_sp(stat_var::SP);
			dev->read(device_status_temperature_probe_extra::SP_INDEX, curr_sp);
			if (delta_setpoint)
			{
				int sp = static_cast<int>(setpoint);
				curr_sp.set_val(sp);
				dev->write_val(device_status_temperature_probe_extra::SP_INDEX, curr_sp);
				dev->read(device_status_temperature_probe_extra::SP_INDEX, curr_sp);
				delta_setpoint = false;
			}

			stat_var curr_local(stat_var::LOCAL);
			dev->read(device_status_temperature_probe_extra::LOCAL_INDEX, curr_local);
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
				setpoint = curr_sp.get_val();
				update = true;
				break;
			}

			stat_var curr_stat(stat_var::STAT);
			dev->read(device_status_temperature_probe_extra::STAT_INDEX, curr_stat);
			if (curr_stat.initialized())
			{
				switch (curr_stat.get_val())
				{
					case device_status_temperature_probe_extra::S_MAN:
						status = MANUAL;
						break;
					case device_status_temperature_probe_extra::S_AUTO:
						status = AUTOMATIC;
						break;
					default:
						break;
				}
			}
		}
	}
	FSBannSimpleProbe::status_changed(list);

	if (update)
		Draw();
}

FSBannTermoReg::FSBannTermoReg(QDomNode n, QWidget *parent, const char *name)
	: BannFullScreen(parent, name),
	main_layout(this)
{
	conf_root = n;
	navbar_button = getButton(IMG_SETTINGS);

	QColor *bg, *fg1, *fg2;
	readExtraConf(&bg, &fg1, &fg2);
	setSecondForeground(*fg2);

	// Put a sensible default for the description
	QDomNode descr = conf_root.namedItem("descr");
	if (!descr.isNull())
		description = descr.toElement().text();
	else
	{
		qDebug("[TERMO] FSBannTermoReg ctor: no description found, maybe wrong node conf?");
		description = "Wrong node";
	}
	description_label = new QLabel(this);
	description_label->setAlignment(AlignHCenter);

	season_icon = getLabelWithPixmap(IMG_SUMMER_S, this, AlignHCenter);

	mode_icon = getLabelWithPixmap(IMG_THERMR, this, AlignHCenter);

	main_layout.addWidget(mode_icon);
	main_layout.addWidget(description_label);
	main_layout.addWidget(season_icon);
	main_layout.setAlignment(Qt::AlignHCenter);
}

BtButton *FSBannTermoReg::customButton()
{
	return navbar_button;
}

void FSBannTermoReg::Draw()
{
	QFont aFont;
	FontManager::instance()->getFont(font_banTermo_tempImp, aFont);
	description_label->setFont(aFont);
	description_label->setText(description);
	// should I color text only if it is a setpoint temperature?
	description_label->setPaletteForegroundColor(second_fg);
	description_label->show();

	BannFullScreen::Draw();
}

void FSBannTermoReg::status_changed(QPtrList<device_status> list)
{
	bool update = false;
	int season = thermal_regulator::SUMMER;

	for (QPtrListIterator<device_status> it(list); device_status *ds = it.current(); ++it)
	{
		if (ds->get_type() == device_status::THERMAL_REGULATOR_4Z || ds->get_type() == device_status::THERMAL_REGULATOR_99Z)
		{
			stat_var curr_season(stat_var::SEASON);
			ds->read(device_status_thermal_regulator::SEASON_INDEX, curr_season);
			switch (curr_season.get_val())
			{
				case thermal_regulator::SUMMER:
					{
						const QString img = QString(IMG_PATH) + "estate_s.png";
						QPixmap *icon = icons_library.getIcon(img.ascii());
						season_icon->setPixmap(*icon);
						season = thermal_regulator::SUMMER;
					}
					break;
				case thermal_regulator::WINTER:
					{
						const QString img = QString(IMG_PATH) + "inverno_s.png";
						QPixmap *icon = icons_library.getIcon(img.ascii());
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
						description_label->hide();
					}
					break;
				case device_status_thermal_regulator::PROTECTION:
					{
						QPixmap *icon = icons_library.getIcon(IMG_ANTIFREEZE_S);
						mode_icon->setPixmap(*icon);
						description_label->hide();
					}
					break;
				case device_status_thermal_regulator::MANUAL:
					{
						const QString i_img = QString(IMG_PATH) + "manuale.png";
						QPixmap *icon = icons_library.getIcon(i_img.ascii());
						mode_icon->setPixmap(*icon);
						stat_var curr_sp(stat_var::SP);
						ds->read(device_status_thermal_regulator::SP_INDEX, curr_sp);
						// remember: stat_var::get_val() returns an int
						int temp = curr_sp.get_val();
						description = "";
						if (temp > 1000)
						{
							description = "-";
							temp -= 1000;
						}
						description += QString::number(temp);
						description.insert(description.length() - 1, ".");
						description += TEMP_DEGREES"C";
						update = true;
					}
					break;
				case device_status_thermal_regulator::WEEK_PROGRAM:
					{
						const QString i_img = QString(IMG_PATH) + "settimanale.png";
						QPixmap *icon = icons_library.getIcon(i_img.ascii());
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
					}
					update = true;
					break;
				case device_status_thermal_regulator::HOLIDAY:
					{
						const QString i_img = QString(IMG_PATH) + "feriale.png";
						QPixmap *icon = icons_library.getIcon(i_img.ascii());
						mode_icon->setPixmap(*icon);
						description_label->hide();
					}
					break;
				case device_status_thermal_regulator::WEEKEND:
					{
						const QString i_img = QString(IMG_PATH) + "festivo.png";
						QPixmap *icon = icons_library.getIcon(i_img.ascii());
						mode_icon->setPixmap(*icon);
						description_label->hide();
					}
					break;
				default:
					break;
			}
		}
	}
	if (update)
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


FSBannTermoReg4z::FSBannTermoReg4z(QDomNode n, QString ind_centrale, QWidget *parent, const char *name)
	: FSBannTermoReg(n, parent, name)
{
	QString where_composed = QString("0#") + ind_centrale;
	_dev = static_cast<thermal_regulator_4z *>(btouch_device_cache.get_thermal_regulator(where_composed.ascii(), THERMO_Z4));

	connect(_dev, SIGNAL(status_changed(QPtrList<device_status>)), SLOT(status_changed(QPtrList<device_status>)));

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
	}
	else
		qFatal("[TERMO] could not create settings menu");


	weekSettings(settings, conf_root, _dev);
	manualSettings(settings, _dev);

	timedManualSettings(settings, _dev);

	holidaySettings(settings, conf_root, _dev);

	// off banner
	BannOff *off = new BannOff(settings, "OFF", _dev);
	settings->appendBanner(off);

	// antifreeze banner
	BannAntifreeze *antifreeze = new BannAntifreeze(settings, "antifreeze", _dev);
	settings->appendBanner(antifreeze);

	// summer_winter banner
	BannSummerWinter *summer_winter = new BannSummerWinter(settings, "Summer/Winter", _dev);
	settings->appendBanner(summer_winter);

	settings->setAllFGColor(paletteForegroundColor());
	settings->setAllBGColor(paletteBackgroundColor());
	settings->hide();
}

FSBannTermoReg99z::FSBannTermoReg99z(QDomNode n, QString ind_centrale, QWidget *parent, const char *name)
	: FSBannTermoReg(n, parent, name)
{
	_dev = static_cast<thermal_regulator_99z *>(btouch_device_cache.get_thermal_regulator(ind_centrale.ascii(), THERMO_Z99));
	connect(_dev, SIGNAL(status_changed(QPtrList<device_status>)), SLOT(status_changed(QPtrList<device_status>)));
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
	}
	else
		qFatal("[TERMO] could not create settings menu");

	weekSettings(settings, conf_root, _dev);
	manualSettings(settings, _dev);

	scenarioSettings(settings, conf_root, _dev);

	holidaySettings(settings, conf_root, _dev);

	// off banner
	BannOff *off = new BannOff(settings, "OFF", _dev);
	settings->appendBanner(off);

	// antifreeze banner
	BannAntifreeze *antifreeze = new BannAntifreeze(settings, "antifreeze", _dev);
	settings->appendBanner(antifreeze);

	// summer_winter banner
	BannSummerWinter *summer_winter = new BannSummerWinter(settings, "Summer/Winter", _dev);
	settings->appendBanner(summer_winter);

	settings->setAllFGColor(paletteForegroundColor());
	settings->setAllBGColor(paletteBackgroundColor());
	settings->hide();
}

FSBannFancoil::FSBannFancoil(QDomNode n, QString ind_centrale, thermo_type_t type, bool change_status, QWidget *parent, const char *name)
	: FSBannProbe(n, ind_centrale, change_status, parent),
	fancoil_buttons(4, Qt::Horizontal, this)
{
	QString simple_address = extractAddress(conf_root);
	if (simple_address.isNull())
		qFatal("FSBannProbe ctor: wrong address, configuration is wrong");
	QString where_composed = simple_address + "#" + ind_centrale;
	dev = static_cast<temperature_probe_controlled *> (btouch_device_cache.get_temperature_probe_controlled(where_composed.ascii(), type,
				true, ind_centrale.ascii(), simple_address.ascii()));
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), SLOT(status_changed(QPtrList<device_status>)));

	createFancoilButtons();
	fancoil_buttons.setExclusive(true);
	fancoil_buttons.hide(); // do not show QButtonGroup frame
	fancoil_status = 0;
	connect(&fancoil_buttons, SIGNAL(clicked(int)), SLOT(handleFancoilButtons(int)));
}

void FSBannFancoil::createFancoilButtons()
{
	QHBoxLayout *hbox = new QHBoxLayout();
	for (int i = 0, id = 0; i < 8; i += 2, ++id)
	{
		QString path = QString(IMG_PATH) + FANCOIL_ICONS[i];
		QString path_pressed = QString(IMG_PATH) + FANCOIL_ICONS[i+1];
		BtButton *btn = new BtButton(this);
		btn->setPixmap(*icons_library.getIcon(path.ascii()));
		btn->setPressedPixmap(*icons_library.getIcon(path_pressed.ascii()));
		btn->setToggleButton(true);

		hbox->addWidget(btn);
		fancoil_buttons.insert(btn, id);

		speed_to_btn_tbl[(id + 1) % 4] = id;
		btn_to_speed_tbl[id] = (id + 1) % 4;
	}
	main_layout.insertLayout(-1, hbox);
	main_layout.setStretchFactor(&fancoil_buttons, 2);
}

void FSBannFancoil::Draw()
{
	fancoil_buttons.setButton(fancoil_status);
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

void FSBannFancoil::status_changed(QPtrList<device_status> list)
{
	QPtrListIterator<device_status> it (list);
	device_status *dev;
	bool update = false;

	while ((dev = it.current()) != 0)
	{
		++it;
		if (dev->get_type() == device_status::FANCOIL)
		{
			stat_var speed_var(stat_var::FANCOIL_SPEED);
			dev->read((int)device_status_fancoil::SPEED_INDEX, speed_var);

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
	FSBannProbe::status_changed(list);
}

FSBannManual::FSBannManual(QWidget *parent, const char *name, thermal_regulator *_dev)
	: BannFullScreen(parent, "manual"),
	main_layout(this),
	dev(_dev)
{
	descr = tr("Manual");
	descr_label = new QLabel(this);
	main_layout.addWidget(descr_label);

	navbar_button = getButton(I_OK);
	connect(navbar_button, SIGNAL(clicked()), this, SLOT(performAction()));

	temp = 200;
	temp_label = new QLabel(this);
	QHBoxLayout *hbox = new QHBoxLayout();

	const QString btn_min_img = QString("%1%2").arg(IMG_PATH).arg("btnmin.png");
	BtButton *btn = getButton(btn_min_img.ascii());
	connect(btn, SIGNAL(clicked()), this, SLOT(decSetpoint()));
	hbox->addWidget(btn);

	hbox->addWidget(temp_label);

	const QString btn_plus_img = QString("%1%2").arg(IMG_PATH).arg("btnplus.png");
	btn = getButton(btn_plus_img.ascii());
	connect(btn, SIGNAL(clicked()), this, SLOT(incSetpoint()));
	hbox->addWidget(btn);

	main_layout.addLayout(hbox);

	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), this, SLOT(status_changed(QPtrList<device_status>)));
}

void FSBannManual::performAction()
{
	emit(temperatureSelected(temp));
}

void FSBannManual::incSetpoint()
{
	temp += 5;
	Draw();
}

void FSBannManual::decSetpoint()
{
	temp -= 5;
	Draw();
}

void FSBannManual::Draw()
{
	QFont aFont;
	FontManager::instance()->getFont(font_banTermo_testo, aFont);
	descr_label->setFont(aFont);
	descr_label->setAlignment(AlignTop|AlignHCenter);
	descr_label->setText(descr);

	FontManager::instance()->getFont(font_banTermo_tempImp, aFont);
	temp_label->setFont(aFont);
	temp_label->setAlignment(AlignHCenter|AlignVCenter);

	QString temp_string;

	int temp_format = temp;
	if (temp_format >= 1000)
	{
		temp_string.append("-");
		temp_format = temp_format - 1000;
	}
	temp_string = temp_string.append("%1").arg(temp_format);
	temp_string.insert(temp_string.length() - 1, ".");
	temp_string.append(TEMP_DEGREES"C");
	temp_label->setText(temp_string);
	temp_label->setPaletteForegroundColor(second_fg);
	BannFullScreen::Draw();
}

BtButton *FSBannManual::customButton()
{
	return navbar_button;
}

void FSBannManual::status_changed(QPtrList<device_status> list)
{
	bool update = false;
	for (QPtrListIterator<device_status> iter(list); device_status *ds = iter.current(); ++iter)
	{
		if (ds->get_type() == device_status::THERMAL_REGULATOR_4Z)
		{
			stat_var curr_sp(stat_var::SP);
			ds->read(device_status_thermal_regulator::SP_INDEX, curr_sp);
			if (curr_sp.initialized())
			{
				temp = curr_sp.get_val();
				update = true;
			}
		}
	}
	if (update)
		Draw();
}

FSBannManualTimed::FSBannManualTimed(QWidget *parent, const char *name, thermal_regulator_4z *_dev)
	: FSBannManual(parent, name, _dev),
	dev(_dev)
{
	time_edit = new BtTimeEdit(this);
	main_layout.addWidget(time_edit);

	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), this, SLOT(status_changed(QPtrList<device_status>)));
	connect(navbar_button, SIGNAL(clicked()), this, SLOT(performAction()));
}

void FSBannManualTimed::performAction()
{
	emit(timeAndTempSelected(time_edit->time(), temp));
}

void FSBannManualTimed::Draw()
{
	FSBannManual::Draw();
}

void FSBannManualTimed::status_changed(QPtrList<device_status> list)
{
	FSBannManual::status_changed(list);
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

void FSBannDate::Draw()
{
	BannFullScreen::Draw();
}

void FSBannDate::status_changed(QPtrList<device_status> list)
{
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

void FSBannTime::Draw()
{
	BannFullScreen::Draw();
}

void FSBannTime::status_changed(QPtrList<device_status> list)
{
}

QTime FSBannTime::time()
{
	return time_edit->time();
}

//
// ------------- Utility functions to create thermal regulator settings menus -------------------
//
void FSBannTermoReg::manualSettings(sottoMenu *settings, thermal_regulator *dev)
{
	const QString i_manual = QString("%1%2").arg(IMG_PATH).arg("manuale.png");
	// manual banner
	bannPuls *manual = new bannPuls(settings, "Manual");
	manual->SetIcons(IMG_RIGHT_ARROW, 0, i_manual.ascii());

	settings->appendBanner(manual);
	manual_menu = new sottoMenu(0, "manual", 10, MAX_WIDTH, MAX_HEIGHT, 1);
	connect(manual, SIGNAL(sxClick()), manual_menu, SLOT(show()));
	connect(manual, SIGNAL(sxClick()), manual_menu, SLOT(raise()));
	connect(manual, SIGNAL(sxClick()), settings, SLOT(hide()));
	// propagate freeze signal
	connect(settings, SIGNAL(freezePropagate(bool)), manual_menu, SLOT(freezed(bool)));
	connect(settings, SIGNAL(freezePropagate(bool)), manual_menu, SIGNAL(freezePropagate(bool)));

	FSBannManual *bann = new FSBannManual(manual_menu, 0, dev);
	bann->setSecondForeground(second_fg);

	manual_menu->appendBanner(bann);
	manual_menu->setAllFGColor(paletteForegroundColor());
	manual_menu->setAllBGColor(paletteBackgroundColor());

	connect(bann, SIGNAL(temperatureSelected(unsigned)), this, SLOT(manualSelected(unsigned)));
	connect(manual_menu, SIGNAL(Closed()), this, SLOT(manualCancelled()));
	manual_menu->hide();
}

void FSBannTermoReg::manualCancelled()
{
	manual_menu->hide();
	settings->show();
	settings->raise();
}

void FSBannTermoReg::manualSelected(unsigned temp)
{
	dev()->setManualTemp(temp);
	manual_menu->hide();
	sottoMenu *parent = static_cast<sottoMenu *>(parentWidget());
	parent->show();
	parent->raise();
}

void FSBannTermoReg::weekSettings(sottoMenu *settings, QDomNode conf, thermal_regulator *dev)
{
	const QString i_weekly = QString("%1%2").arg(IMG_PATH).arg("settimanale.png");

	bannPuls *weekly = new bannPuls(settings, "weekly");
	weekly->SetIcons(IMG_RIGHT_ARROW, 0, i_weekly.ascii());
	settings->appendBanner(weekly);

	program_menu = new WeeklyMenu(0, "weekly", conf);
	program_menu->setAllBGColor(paletteBackgroundColor());
	program_menu->setAllFGColor(paletteForegroundColor());
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), program_menu, SLOT(status_changed(QPtrList<device_status>)));

	connect(weekly, SIGNAL(sxClick()), program_menu, SLOT(show()));
	connect(weekly, SIGNAL(sxClick()), program_menu, SLOT(raise()));
	connect(weekly, SIGNAL(sxClick()), settings, SLOT(hide()));
	// propagate freeze signal
	connect(settings, SIGNAL(freezePropagate(bool)), program_menu, SLOT(freezed(bool)));
	connect(settings, SIGNAL(freezePropagate(bool)), program_menu, SIGNAL(freezePropagate(bool)));

	connect(program_menu, SIGNAL(Closed()), this, SLOT(weekProgramCancelled()));
	connect(program_menu, SIGNAL(programClicked(int)), this, SLOT(weekProgramSelected(int)));
	program_menu->hide();
}

void FSBannTermoReg::weekProgramCancelled()
{
	program_menu->hide();
	settings->show();
	settings->raise();
}

void FSBannTermoReg::weekProgramSelected(int program)
{
	dev()->setWeekProgram(program);
	program_menu->hide();
	sottoMenu *parent = static_cast<sottoMenu *>(parentWidget());
	parent->show();
	parent->raise();
}

void FSBannTermoReg::holidaySettings(sottoMenu *settings, QDomNode conf, thermal_regulator *dev)
{
	const QString i_holiday = QString("%1%2").arg(IMG_PATH).arg("feriale.png");

	bannPuls *holiday = new bannPuls(settings, "holiday");
	holiday->SetIcons(IMG_RIGHT_ARROW, 0, i_holiday.ascii());
	settings->appendBanner(holiday);
	connect(holiday, SIGNAL(sxClick()), this, SLOT(holidaySettingsStart()));

	// the banner inside date_edit does not have second foreground set
	date_edit = new DateEditMenu(0, "date edit");
	date_edit->setAllBGColor(paletteBackgroundColor());
	date_edit->setAllFGColor(paletteForegroundColor());
	connect(date_edit, SIGNAL(Closed()), this, SLOT(dateCancelled()));
	connect(date_edit, SIGNAL(dateSelected(QDate)), this, SLOT(dateSelected(QDate)));
	// propagate freeze signal
	connect(settings, SIGNAL(freezePropagate(bool)), date_edit, SLOT(freezed(bool)));
	connect(settings, SIGNAL(freezePropagate(bool)), date_edit, SIGNAL(freezePropagate(bool)));

	time_edit = new TimeEditMenu(0, "time edit");
	time_edit->setAllBGColor(paletteBackgroundColor());
	time_edit->setAllFGColor(paletteForegroundColor());
	connect(time_edit, SIGNAL(timeSelected(QTime)), this, SLOT(timeSelected(QTime)));
	connect(time_edit, SIGNAL(Closed()), this, SLOT(timeCancelled()));
	// propagate freeze signal
	connect(date_edit, SIGNAL(freezePropagate(bool)), time_edit, SLOT(freezed(bool)));
	connect(date_edit, SIGNAL(freezePropagate(bool)), time_edit, SIGNAL(freezePropagate(bool)));

	program_choice = new WeeklyMenu(0, "weekly program edit", conf);
	program_choice->setAllBGColor(paletteBackgroundColor());
	program_choice->setAllFGColor(paletteForegroundColor());
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), program_choice, SLOT(status_changed(QPtrList<device_status>)));
	connect(program_choice, SIGNAL(programClicked(int)), this, SLOT(holidaySettingsEnd(int)));
	connect(program_choice, SIGNAL(Closed()), this, SLOT(programCancelled()));
	// propagate freeze signal
	connect(time_edit, SIGNAL(freezePropagate(bool)), program_choice, SLOT(freezed(bool)));
	connect(time_edit, SIGNAL(freezePropagate(bool)), program_choice, SIGNAL(freezePropagate(bool)));
	date_edit->hide();
	time_edit->hide();
	program_choice->hide();
}

void FSBannTermoReg::holidaySettingsStart()
{
	settings->hide();
	date_edit->show();
	date_edit->raise();
}

void FSBannTermoReg::dateCancelled()
{
	date_edit->hide();
	settings->show();
}

void FSBannTermoReg::dateSelected(QDate d)
{
	holiday_date_end = d;
	date_edit->hide();
	time_edit->show();
	time_edit->raise();
}

void FSBannTermoReg::timeCancelled()
{
	time_edit->hide();
	date_edit->show();
}

void FSBannTermoReg::timeSelected(QTime t)
{
	holiday_time_end = t;
	time_edit->hide();
	program_choice->show();
	program_choice->raise();
}

void FSBannTermoReg::programCancelled()
{
	program_choice->hide();
	time_edit->show();
}

void FSBannTermoReg::holidaySettingsEnd(int program)
{
	dev()->setHolidayDateTime(holiday_date_end, holiday_time_end, program);
	program_choice->hide();
	sottoMenu *parent = static_cast<sottoMenu *>(parentWidget());
	parent->show();
}

void FSBannTermoReg4z::timedManualSettings(sottoMenu *settings, thermal_regulator_4z *dev)
{
	const QString i_manual = QString("%1%2").arg(IMG_PATH).arg("manuale_temporizzato.png");
	// timed manual banner
	bannPuls *manual_timed = new bannPuls(settings, "manual_timed");
	manual_timed->SetIcons(IMG_RIGHT_ARROW, 0, i_manual.ascii());

	settings->appendBanner(manual_timed);
	timed_manual_menu = new sottoMenu(0, "manual_timed", 10, MAX_WIDTH, MAX_HEIGHT, 1);

	FSBannManualTimed *bann = new FSBannManualTimed(timed_manual_menu, 0, dev);
	bann->setSecondForeground(second_fg);

	timed_manual_menu->appendBanner(bann);
	timed_manual_menu->setAllFGColor(paletteForegroundColor());
	timed_manual_menu->setAllBGColor(paletteBackgroundColor());

	connect(manual_timed, SIGNAL(sxClick()), timed_manual_menu, SLOT(show()));
	connect(manual_timed, SIGNAL(sxClick()), timed_manual_menu, SLOT(raise()));
	connect(manual_timed, SIGNAL(sxClick()), settings, SLOT(hide()));
	// propagate freeze signal
	connect(settings, SIGNAL(freezePropagate(bool)), timed_manual_menu, SLOT(freezed(bool)));
	connect(settings, SIGNAL(freezePropagate(bool)), timed_manual_menu, SIGNAL(freezePropagate(bool)));

	connect(timed_manual_menu, SIGNAL(Closed()), this, SLOT(manualTimedCancelled()));
	connect(bann, SIGNAL(timeAndTempSelected(QTime, int)), this, SLOT(manualTimedSelected(QTime, int)));
	timed_manual_menu->hide();
}

void FSBannTermoReg4z::manualTimedCancelled()
{
	timed_manual_menu->hide();
	settings->show();
	settings->raise();
}

void FSBannTermoReg4z::manualTimedSelected(QTime time, int temp)
{
	_dev->setManualTempTimed(temp, time);
	timed_manual_menu->hide();
	sottoMenu *parent = static_cast<sottoMenu *>(parentWidget());
	parent->show();
	parent->raise();
}

void FSBannTermoReg99z::scenarioSettings(sottoMenu *settings, QDomNode conf, thermal_regulator_99z *dev)
{
	const QString i_scenario = QString("%1%2").arg(IMG_PATH).arg("scenari.png");

	bannPuls *scenario = new bannPuls(settings, "scenario");
	scenario->SetIcons(IMG_RIGHT_ARROW, 0, i_scenario.ascii());
	settings->appendBanner(scenario);

	scenario_menu = new ScenarioMenu(0, "weekly", conf);
	scenario_menu->setAllBGColor(paletteBackgroundColor());
	scenario_menu->setAllFGColor(paletteForegroundColor());

	connect(scenario, SIGNAL(sxClick()), scenario_menu, SLOT(show()));
	connect(scenario, SIGNAL(sxClick()), scenario_menu, SLOT(raise()));
	connect(scenario, SIGNAL(sxClick()), settings, SLOT(hide()));
	// propagate freeze signal
	connect(settings, SIGNAL(freezePropagate(bool)), scenario_menu, SLOT(freezed(bool)));
	connect(settings, SIGNAL(freezePropagate(bool)), scenario_menu, SIGNAL(freezePropagate(bool)));

	connect(scenario_menu, SIGNAL(Closed()), this, SLOT(scenarioCancelled()));
	connect(scenario_menu, SIGNAL(programClicked(int)), this, SLOT(scenarioSelected(int)));
}

void FSBannTermoReg99z::scenarioCancelled()
{
	scenario_menu->hide();
	settings->show();
	settings->raise();
}

void FSBannTermoReg99z::scenarioSelected(int scenario)
{
	_dev->setScenario(scenario);
	scenario_menu->hide();
	sottoMenu *parent = static_cast<sottoMenu *>(parentWidget());
	parent->show();
	parent->raise();
}
