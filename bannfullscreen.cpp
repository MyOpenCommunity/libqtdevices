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

#include <qobjectlist.h>

#define I_OK  IMG_PATH"btnok.png"
#define I_SETTINGS IMG_PATH"setscen.png"
#define IMG_PLUS IMG_PATH "btnplus.png"
#define IMG_MINUS IMG_PATH "btnmin.png"
#define IMG_AUTO IMG_PATH "btnauto.png"
#define IMG_MAN IMG_PATH "btnman.png"

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

BannFullScreen *getBanner(BannID id, QWidget *parent, QDomNode n)
{
	BannFullScreen *bfs = 0;
	switch (id)
	{
		case fs_nc_probe:
			bfs = new FSBannSimpleProbe(parent, n);
			break;
		case fs_4z_probe:
			bfs = new FSBannProbe(n, false, parent);
			break;
		case fs_99z_probe:
			bfs = new FSBannProbe(n, true, parent);
			break;
		case fs_4z_fancoil:
			bfs = new FSBann4zFancoil(n, false, parent);
			break;
		case fs_99z_fancoil:
			bfs = new FSBann4zFancoil(n, true, parent);
			break;
		case fs_4z_thermal_regulator:
			bfs = new FSBannTermoReg4z(parent, n);
			break;
		case fs_date_edit:
			bfs = new FSBannDate(parent, 0);
			break;
		case fs_time_edit:
			bfs = new FSBannTime(parent, 0);
			break;
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

void FSBannSimpleProbe::postDisplay(sottoMenu *parent)
{
	parent->setNavBarMode(3, "");
	qDebug("[TERMO] bfs::postDisplay() done");
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

FSBannProbe::FSBannProbe(QDomNode n, bool change_status, QWidget *parent,const char *name)
	: FSBannSimpleProbe(parent, n)
{
	QHBoxLayout *hbox = new QHBoxLayout(&main_layout);

	status = AUTOMATIC;
	status_change_enabled = change_status;
	navbar_button = getButton(status == AUTOMATIC ? IMG_MAN : IMG_AUTO);

	btn_minus = getButton(IMG_MINUS);
	btn_minus->hide();
	hbox->addWidget(btn_minus);

	setpoint_label = new QLabel(this);
	hbox->addWidget(setpoint_label);

	btn_plus = getButton(IMG_PLUS);
	btn_plus->hide();
	hbox->addWidget(btn_plus);

	btn_antifreeze = getIcon("antigelop.png");
	hbox->addWidget(btn_antifreeze);

	btn_off = getIcon("offp.png");
	hbox->addWidget(btn_off);

	main_layout.addLayout(hbox);
	main_layout.setStretchFactor(hbox, 1);

	local_temp_label = new QLabel(this);
	main_layout.addWidget(local_temp_label);
	main_layout.setStretchFactor(local_temp_label, 1);

	setpoint = "-23.5"TEMP_DEGREES"C";
	local_temp = "0";
	isOff = false;
	isAntigelo = false;
}

BtButton *FSBannProbe::customButton()
{
	if (status_change_enabled)
		return navbar_button;
	return 0;
}

BtButton *FSBannProbe::getIcon(const char *img)
{
	// FIX: use QLabel and QPixmap instead of QButton!!
	BtButton *btn = new BtButton(this);
	QPixmap *icon = icons_library.getIcon(QString("%1%2").arg(IMG_PATH).arg(img).ascii());
	btn->setPixmap(*icon);
	btn->setPressedPixmap(*icon);
	btn->setDown(true);
	btn->setEnabled(false);
	btn->hide();
	return btn;
}

void FSBannProbe::Draw()
{
	if (isOff)
	{
		if (status == MANUAL)
		{
			btn_minus->hide();
			btn_plus->hide();
		}
		setpoint_label->hide();
		//local_temp = "";
		local_temp_label->hide();
		btn_off->show();
		btn_antifreeze->hide();
	}
	else if (isAntigelo)
	{
		if (status == MANUAL)
		{
			btn_minus->hide();
			btn_plus->hide();
		}
		setpoint_label->hide();
		//local_temp = "";
		local_temp_label->hide();
		btn_off->hide();
		btn_antifreeze->show();
	}
	else
	{
		if (status == MANUAL)
		{
			btn_minus->show();
			btn_plus->show();
		}
		setpoint_label->show();
		local_temp_label->show();
		btn_off->hide();
		btn_antifreeze->hide();

	}
	QFont aFont;
	FontManager::instance()->getFont(font_banTermo_tempImp, aFont);
	setpoint_label->setFont(aFont);
	setpoint_label->setAlignment(AlignHCenter);
	setpoint_label->setText(setpoint);
	setpoint_label->setPaletteForegroundColor(second_fg);

	FontManager::instance()->getFont(font_banTermo_testo, aFont);
	local_temp_label->setFont(aFont);
	local_temp_label->setAlignment(AlignHCenter);
	local_temp_label->setText(local_temp);

	FSBannSimpleProbe::Draw();
}

void FSBannProbe::postDisplay(sottoMenu *parent)
{
	parent->setNavBarMode(3, "");
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
			stat_var curr_local(stat_var::LOCAL);
			stat_var curr_sp(stat_var::SP);

			qDebug("Th. regulator status variation");
			dev->read(device_status_temperature_probe_extra::LOCAL_INDEX, curr_local);
			dev->read(device_status_temperature_probe_extra::SP_INDEX, curr_sp);

			qDebug("loc = %d", curr_local.get_val());
			qDebug("sp = %d", curr_sp.get_val());
			if (curr_local.initialized())
			{
				update = true;
				switch (curr_local.get_val())
				{
					case 0:
						local_temp = "0";
						break;
					case 1:
						local_temp = "+1";
						break;
					case 2:
						local_temp = "+2";
						break;
					case 3:
						local_temp = "+3";
						break;
					case 11:
						local_temp = "-1";
						break;
					case 12:
						local_temp = "-2";
						break;
					case 13:
						local_temp = "-3";
						break;
						//FIXME: maybe this is broken, case 4 should be off (check documentation)
					case 4:
						local_temp = "0";
						break;
					case 5:
						local_temp = "0";
						break;
					default:
						update = false;
						qDebug("[TERMO] FSBannProbe::status_changed(): local status case not handled!");
				}

				if (update)
				{
					isOff = true;
					isAntigelo = false;
				}
			}

			if (curr_sp.initialized())
			{
				float icx;
				char tmp[10];
				icx = curr_sp.get_val();
				qDebug("temperatura setpoint: %d",(int)icx);
				setpoint = "";
				if (icx>=1000)
				{
					setpoint = "-";
					icx=icx-1000;
				}
				icx/=10;
				sprintf(tmp,"%.1f",icx);
				setpoint += tmp;
				setpoint += TEMP_DEGREES"C";
				update = true;
				break;
			}
		}
	}
	FSBannSimpleProbe::status_changed(list);

	if (update)
		Draw();
}

FSBannTermoReg4z::FSBannTermoReg4z(QWidget *parent, QDomNode n, const char *name)
	: BannFullScreen(parent, name),
	main_layout(this)
{
	conf_root = n;

	// Put a sensible default for the description
	QDomNode descr = conf_root.namedItem("descr");
	if (!descr.isNull())
		description = descr.toElement().text();
	else
	{
		qDebug("[TERMO] FSBannTermoReg4z ctor: no description found, maybe wrong node conf?");
		description = "Wrong node";
	}
	description_label = new QLabel(this);

	const QString i_summer = QString(IMG_PATH) + "estate_s.png";
	QPixmap *icon = icons_library.getIcon(i_summer.ascii());
	season_btn = new BtButton(this);
	season_btn->setPixmap(*icon);
	season_btn->setDown(true);
	season_btn->setEnabled(false);

	// is this a sensible default for mode icon?
	const QString i_thermal_reg = QString(IMG_PATH) + "centrale.png";
	icon = icons_library.getIcon(i_thermal_reg.ascii());
	mode_btn = new BtButton(this);
	mode_btn->setPixmap(*icon);
	mode_btn->setDown(true);
	mode_btn->setEnabled(false);

	main_layout.addWidget(mode_btn);
	main_layout.addWidget(description_label);
	main_layout.addWidget(season_btn);
	main_layout.setAlignment(Qt::AlignHCenter);
}

void FSBannTermoReg4z::Draw()
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

void FSBannTermoReg4z::postDisplay(sottoMenu *parent)
{
	parent->setNavBarMode(4, I_SETTINGS);
}

void FSBannTermoReg4z::status_changed(QPtrList<device_status> list)
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
						season_btn->setPixmap(*icon);
						season = thermal_regulator::SUMMER;
					}
					break;
				case thermal_regulator::WINTER:
					{
						const QString img = QString(IMG_PATH) + "inverno_s.png";
						QPixmap *icon = icons_library.getIcon(img.ascii());
						season_btn->setPixmap(*icon);
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
						const QString i_img = QString(IMG_PATH) + "offp.png";
						QPixmap *icon = icons_library.getIcon(i_img.ascii());
						mode_btn->setPixmap(*icon);
						description_label->hide();
					}
					break;
				case device_status_thermal_regulator::PROTECTION:
					{
						const QString i_img = QString(IMG_PATH) + "antigelop.png";
						QPixmap *icon = icons_library.getIcon(i_img.ascii());
						mode_btn->setPixmap(*icon);
						description_label->hide();
					}
					break;
				case device_status_thermal_regulator::MANUAL:
					{
						const QString i_img = QString(IMG_PATH) + "manuale.png";
						QPixmap *icon = icons_library.getIcon(i_img.ascii());
						mode_btn->setPixmap(*icon);
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
						mode_btn->setPixmap(*icon);

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
						mode_btn->setPixmap(*icon);
						description_label->hide();
					}
					break;
				case device_status_thermal_regulator::WEEKEND:
					{
						const QString i_img = QString(IMG_PATH) + "festivo.png";
						QPixmap *icon = icons_library.getIcon(i_img.ascii());
						mode_btn->setPixmap(*icon);
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

QString FSBannTermoReg4z::lookupProgramDescription(QString season, int program_number)
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
				qWarning("[TERMO] FSBannTermoReg4z::lookupProgramDescription WEEK PROGRAM: wrong node");
		}
		else
			qWarning("[TERMO] FSBannTermoReg4z::lookupProgramDescription WEEK PROGRAM: wrong node");
	}
	qDebug("FSBannTermoReg4z::lookupProgramDescription: You did not supply the correct season.");
	return "";
}

FSBann4zFancoil::FSBann4zFancoil(QDomNode n, bool change_status, QWidget *parent, const char *name)
	: FSBannProbe(n, change_status, parent),
	fancoil_buttons(4, Qt::Horizontal, this)
{
	createFancoilButtons();
	fancoil_buttons.setExclusive(true);
	fancoil_buttons.hide(); // do not show QButtonGroup frame
	fancoil_status = 0;
}

void FSBann4zFancoil::createFancoilButtons()
{
	const char *icon_path[] = {"fancoil1off.png", "fancoil1on.png", "fancoil2off.png", "fancoil2on.png",
		"fancoil3off.png", "fancoil3on.png", "fancoilAoff.png", "fancoilAon.png"};
	QHBoxLayout *hbox = new QHBoxLayout();
	for (int i = 0, id = 0; i < 8; i = i+2, ++id)
	{
		QPixmap *icon, *pressed_icon;
		BtButton *btn;
		icon         = icons_library.getIcon(QString(IMG_PATH) + icon_path[i]);
		pressed_icon = icons_library.getIcon(QString(IMG_PATH) + icon_path[i+1]);
		btn = new BtButton(this);
		hbox->addWidget(btn);
		btn->setPixmap(*icon);
		btn->setPressedPixmap(*pressed_icon);
		fancoil_buttons.insert(btn, id);
		btn->setToggleButton(true);
	}
	main_layout.insertLayout(-1, hbox);
	main_layout.setStretchFactor(&fancoil_buttons, 2);
}

void FSBann4zFancoil::Draw()
{
	FSBannProbe::Draw();
}

void FSBann4zFancoil::postDisplay(sottoMenu *parent)
{
	parent->setNavBarMode(3, "");
}

void FSBann4zFancoil::status_changed(QPtrList<device_status> list)
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
			fancoil_status = -1;
			switch (speed_var.get_val())
			{
				case 0: // auto
					fancoil_status = 3;
					break;
				case 1: // min
					fancoil_status = 0;
					break;
				case 2: // medium
					fancoil_status = 1;
					break;
				case 3: // max
					fancoil_status = 2;
					break;
				default:
					qDebug("Fancoil speed val out of range (%d)", speed_var.get_val());
			}
			update = true;
			if (fancoil_status != -1)
			{
				qDebug("[TERMO] New fancoil status: %d", fancoil_status);
				fancoil_buttons.setButton(fancoil_status);
			}
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
	// FIXME: use string::fromUtf8("%1°C")
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

void FSBannManual::postDisplay(sottoMenu *parent)
{
	parent->setNavBarMode(10, I_OK);
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
			temp = curr_sp.get_val();
			update = true;
		}
	}
	if (update)
		Draw();
}

FSBannManualTimed::FSBannManualTimed(QWidget *parent, const char *name, thermal_regulator_4z *_dev)
	: FSBannManual(parent, name, _dev),
	dev(_dev)
{
	const int bt_max_hours = 99;
	const int bt_max_mins = 99;
	time_edit = new BtTimeEdit(this);
	time_edit->setMaxHours(bt_max_hours);
	time_edit->setMaxMins(bt_max_mins);
	main_layout.addWidget(time_edit);

	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), this, SLOT(status_changed(QPtrList<device_status>)));
}


void FSBannManualTimed::Draw()
{
	FSBannManual::Draw();
}

void FSBannManualTimed::postDisplay(sottoMenu *parent)
{
	parent->setNavBarMode(10, I_OK);
}

void FSBannManualTimed::status_changed(QPtrList<device_status> list)
{
	FSBannManual::status_changed(list);
}

FSBannDate::FSBannDate(QWidget *parent, const char *name)
	: BannFullScreen(parent, name),
	main_layout(this)
{
	navbar_button = getButton(I_OK);

	const QString top_img = QString("%1%2").arg(IMG_PATH).arg("calendario.png");
	BtButton *top = new BtButton(this);
	top->setPixmap(top_img);
	top->setDown(true);
	top->setEnabled(false);
	main_layout.addWidget(top, 0, Qt::AlignHCenter);

	date_edit = new BtDateEdit(this);
	main_layout.addWidget(date_edit);

	connect(date_edit, SIGNAL(valueChanged(QDate)), this, SIGNAL(dateChanged(QDate)));
}

BtButton *FSBannDate::customButton()
{
	return navbar_button;
}

QDate FSBannDate::date()
{
	return _date;
}

void FSBannDate::Draw()
{
	BannFullScreen::Draw();
}

void FSBannDate::postDisplay(sottoMenu *parent)
{
	parent->setNavBarMode(10, I_OK);
}

void FSBannDate::status_changed(QPtrList<device_status> list)
{
}

FSBannTime::FSBannTime(QWidget *parent, const char *name)
	: BannFullScreen(parent, name),
	main_layout(this)
{
	navbar_button = getButton(I_OK);

	const QString i_top_img = QString("%1%2").arg(IMG_PATH).arg("orologio.png");
	BtButton *top = new BtButton(this);
	top->setPixmap(i_top_img);
	top->setDown(true);
	top->setEnabled(false);
	main_layout.addWidget(top, 0, Qt::AlignHCenter);

	time_edit = new BtTimeEdit(this);
	main_layout.addWidget(time_edit);

	connect(time_edit, SIGNAL(valueChanged(int, int)), this, SLOT(setTime(int, int)));
}

BtButton *FSBannTime::customButton()
{
	return navbar_button;
}

void FSBannTime::Draw()
{
	BannFullScreen::Draw();
}

void FSBannTime::postDisplay(sottoMenu *parent)
{
	parent->setNavBarMode(10, I_OK);
}

void FSBannTime::status_changed(QPtrList<device_status> list)
{
}

void FSBannTime::setTime(int hrs, int mins)
{
	hours = hrs;
	minutes = mins;
	if (QTime::isValid(hours, minutes, 0))
		emit timeChanged(QTime(hours, minutes));
}

QTime FSBannTime::time()
{
	return QTime(hours, minutes);
}

