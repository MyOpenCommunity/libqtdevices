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

#define I_OK IMG_PATH"btnok.png"
#define I_SETTINGS IMG_PATH"setscen.png"

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

void BannFullScreen::setBGColor(QColor bg)
{
	QObjectList *l = queryList("QWidget");
	QObjectListIt iter(*l);
	QObject *obj;

	while ( (obj = iter.current()) != 0 )
	{
		++iter;
		((QWidget*)obj)->setPaletteBackgroundColor(bg);
	}
	delete l;
	banner::setBGColor(bg);
}

void BannFullScreen::setFGColor(QColor fg)
{
	QObjectList *l = queryList("QWidget");
	QObjectListIt iter(*l);
	QObject *obj;

	while ( (obj = iter.current()) != 0 )
	{
		++iter;
		((QWidget*)obj)->setPaletteForegroundColor(fg);
	}
	delete l;
	banner::setFGColor(fg);
}

FSBannSimpleProbe::FSBannSimpleProbe(QWidget *parent, QDomNode n, const char *name)
	: BannFullScreen(parent, name),
	main_layout(this)
{
	descr_label = new QLabel(this, 0);
	main_layout.addWidget(descr_label);
	main_layout.setStretchFactor(descr_label, 1);

	temp_label = new QLabel(this, 0);
	main_layout.addWidget(temp_label);
	main_layout.setStretchFactor(temp_label, 1);

	temp = "-23.5\272C";
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

void FSBannSimpleProbe::postDisplay()
{
	sottoMenu *parent = static_cast<sottoMenu *> (parentWidget());
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
			qtemp +="\272C";
			temp = qtemp;
			update = true;
		}
	}

	if (update)
		Draw();
}

FSBann4zProbe::FSBann4zProbe(QWidget *parent, QDomNode n, const char *name)
	: FSBannSimpleProbe(parent, n)
{
	setpoint_label = new QLabel(this, 0);
	main_layout.addWidget(setpoint_label);
	main_layout.setStretchFactor(setpoint_label, 1);

	local_temp_label = new QLabel(this, 0);
	main_layout.addWidget(local_temp_label);
	main_layout.setStretchFactor(local_temp_label, 1);

	setpoint = "-23.5\272C";
	local_temp = "0";
	isOff = false;
	isAntigelo = false;
}

void FSBann4zProbe::Draw()
{

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

void FSBann4zProbe::postDisplay()
{
	sottoMenu *parent = static_cast<sottoMenu *> (parentWidget());
	parent->setNavBarMode(3, "");
}

void FSBann4zProbe::status_changed(QPtrList<device_status> list)
{
	QPtrListIterator<device_status> it (list);
	device_status *dev;
	bool update = false;

	while ((dev = it.current()) != 0)
	{
		++it;
		if (dev->get_type() == device_status::THERMR)
		{
			stat_var curr_local(stat_var::LOCAL);
			stat_var curr_sp(stat_var::SP);

			qDebug("Th. regulator status variation");
			dev->read(device_status_temperature_probe_extra::LOCAL_INDEX, curr_local);
			dev->read(device_status_temperature_probe_extra::SP_INDEX, curr_sp);

			qDebug("loc = %d", curr_local.get_val());
			qDebug("sp = %d", curr_sp.get_val());
			if(curr_local.initialized())
			{
				switch(curr_local.get_val())
				{
					case 0:
						local_temp = "0";
						isOff = false;
						isAntigelo = false;
						break;
					case 1:
						local_temp = "+1";
						isOff = false;
						isAntigelo = false;
						break;
					case 2:
						local_temp = "+2";
						isOff = false;
						isAntigelo = false;
						break;
					case 3:
						local_temp = "+3";
						isOff = false;
						isAntigelo = false;
						break;
					case 11:
						local_temp = "-1";
						isOff = false;
						isAntigelo = false;
						break;
					case 12:
						local_temp = "-2";
						isOff = false;
						isAntigelo = false;
						break;
					case 13:
						local_temp = "-3";
						isOff = false;
						isAntigelo = false;
						break;
					case 4:
						local_temp = "0";
						isOff = true;
						isAntigelo = false;
						break;
					case 5:
						local_temp = "0";
						isOff = false;
						isAntigelo = true;
						break;
					default:
						qDebug("[TERMO] FSBann4zProbe::status_changed(): local status case not handled!");
				}
				update = true;
			}
			if(curr_sp.initialized())
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
				setpoint += "\272C";
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
	: FSBann4zProbe(parent, n)
{
}

void FSBannTermoReg4z::Draw()
{
	FSBann4zProbe::Draw();
}

void FSBannTermoReg4z::postDisplay()
{
	sottoMenu *parent = static_cast<sottoMenu *> (parentWidget());
	parent->setNavBarMode(4, I_SETTINGS);
}

FSBann4zFancoil::FSBann4zFancoil(QWidget *parent, QDomNode n, const char *name)
	: FSBann4zProbe(parent, n),
	fancoil_buttons(4, Qt::Horizontal, this)
{
	createFancoilButtons();
	fancoil_buttons.setExclusive(true);
	fancoil_buttons.hide(); // do not show QButtonGroup frame
	main_layout.setStretchFactor(&fancoil_buttons, 1);
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
		btn = new BtButton(this, 0);
		hbox->addWidget(btn);
		btn->setPixmap(*icon);
		btn->setPressedPixmap(*pressed_icon);
		fancoil_buttons.insert(btn, id);
		btn->setToggleButton(true);
	}
	main_layout.insertLayout(-1, hbox);
}

void FSBann4zFancoil::Draw()
{
	FSBann4zProbe::Draw();
}

void FSBann4zFancoil::postDisplay()
{
	sottoMenu *parent = static_cast<sottoMenu *> (parentWidget());
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
			if (fancoil_status != -1)
			{
				qDebug("[TERMO] New fancoil status: %d", fancoil_status);
				fancoil_buttons.setButton(fancoil_status);
			}
		}
	}
	FSBann4zProbe::status_changed(list);
}

FSBannFactory *FSBannFactory::instance = 0;

FSBannFactory *FSBannFactory::getInstance()
{
	if (instance == 0)
	{
		instance = new FSBannFactory;
	}
	return instance;
}

BannFullScreen *FSBannFactory::getBanner(BannID id, QWidget *parent, QDomNode n)
{
	BannFullScreen *bfs = 0;
	switch (id)
	{
		case fs_nc_probe:
			bfs = new FSBannSimpleProbe(parent, n);
			break;
		case fs_4z_probe:
			bfs = new FSBann4zProbe(parent, n);
			break;
		case fs_4z_fancoil:
			bfs = new FSBann4zFancoil(parent, n);
			break;
		case fs_4z_thermal_regulator:
			bfs = new FSBannTermoReg4z(parent, n);
			break;
		case fs_manual:
			bfs = new FSBannManual(parent, 0);
			break;
		case fs_manual_timed:
			bfs = new FSBannManualTimed(parent, 0);
			break;
		case fs_date:
			bfs = new FSBannDate(parent, 0);
			break;
	}
	return bfs;
}

FSBannFactory::FSBannFactory()
{
}

FSBannManual::FSBannManual(QWidget *parent, const char *name)
	: BannFullScreen(parent, "manual"),
	main_layout(this)
{
	temp = 200;
	temp_label = new QLabel(this);
	QHBoxLayout *hbox = new QHBoxLayout();

	QPixmap *icon, *pressed_icon;
	BtButton *btn;
	const QString btn_min_img = QString("%1%2").arg(IMG_PATH).arg("btnmin.png");
	const QString btn_min_img_press = QString("%1%2").arg(IMG_PATH).arg("btnminp.png");
	icon         = icons_library.getIcon(btn_min_img);
	pressed_icon = icons_library.getIcon(btn_min_img_press);
	btn = new BtButton(this, 0);
	btn->setPixmap(*icon);
	btn->setPressedPixmap(*pressed_icon);
	connect(btn, SIGNAL(clicked()), this, SLOT(decSetpoint()));
	hbox->addWidget(btn);

	hbox->addWidget(temp_label);

	const QString btn_plus_img = QString("%1%2").arg(IMG_PATH).arg("btnplus.png");
	const QString btn_plus_img_press = QString("%1%2").arg(IMG_PATH).arg("btnplusp.png");
	icon         = icons_library.getIcon(btn_plus_img);
	pressed_icon = icons_library.getIcon(btn_plus_img_press);
	btn = new BtButton(this, 0);
	btn->setPixmap(*icon);
	btn->setPressedPixmap(*pressed_icon);
	connect(btn, SIGNAL(clicked()), this, SLOT(incSetpoint()));
	hbox->addWidget(btn);

	main_layout.addLayout(hbox);

	//main_layout.addWidget(new QLabel("Manuale", this));
}

void FSBannManual::setThermalRegulator()
{
	// send Open frame
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
	temp_string.append("\272C");
	temp_label->setText(temp_string);
	temp_label->setPaletteForegroundColor(second_fg);
	BannFullScreen::Draw();
}

void FSBannManual::postDisplay()
{
	sottoMenu *parent = static_cast<sottoMenu *> (parentWidget());
	parent->setNavBarMode(10, I_OK);
}

void FSBannManual::status_changed(QPtrList<device_status> list)
{
}

FSBannManualTimed::FSBannManualTimed(QWidget *parent, const char *name)
	: FSBannManual(parent, name)
{
	time_edit = new BtTimeEdit(this, 0);
	time_edit->setMaxHours(9);
	time_edit->setMaxMins(9);
	main_layout.addWidget(time_edit);
}


void FSBannManualTimed::Draw()
{
	FSBannManual::Draw();
}

void FSBannManualTimed::postDisplay()
{
	sottoMenu *parent = static_cast<sottoMenu *> (parentWidget());
	parent->setNavBarMode(10, I_OK);
}

void FSBannManualTimed::status_changed(QPtrList<device_status> list)
{
}

void FSBannManualTimed::setThermalRegulator()
{
	// send Open frame
}

FSBannDate::FSBannDate(QWidget *parent, const char *name)
	: BannFullScreen(parent, name),
	main_layout(this),
	date(QDate::currentDate())
{
	// Buttons to increase day, month, year
	BtButton *btn_up_day, *btn_up_month, *btn_up_year;
	// Buttons to decrease day, month, year
	BtButton *btn_down_day, *btn_down_month, *btn_down_year;

	const QString top_img = QString("%1%2").arg(IMG_PATH).arg("calendario.png");
	BtButton *top = new BtButton(this, 0);
	top->setPixmap(top_img);
	top->setDown(true);
	main_layout.addWidget(top);

	QDate current = QDate::currentDate();

	QPixmap *icon, *pressed_icon;
	const QString btn_up_img = QString("%1%2").arg(IMG_PATH).arg("arrup.png");
	const QString btn_up_img_press = QString("%1%2").arg(IMG_PATH).arg("arrupp.png");
	icon         = icons_library.getIcon(btn_up_img);
	pressed_icon = icons_library.getIcon(btn_up_img_press);
	if (!icon)
		qDebug("[TERMO] FSBannDate: could not get icon, prepare for strangeness");
	if (!pressed_icon)
		qDebug("[TERMO] FSBannDate: could not get pressed icon, prepare for strangeness");
	btn_up_day = new BtButton(this, 0);
	btn_up_month = new BtButton(this, 0);
	btn_up_year = new BtButton(this, 0);
	btn_up_day->setPixmap(*icon);
	btn_up_day->setPressedPixmap(*pressed_icon);
	btn_up_month->setPixmap(*icon);
	btn_up_month->setPressedPixmap(*pressed_icon);
	btn_up_year->setPixmap(*icon);
	btn_up_year->setPressedPixmap(*pressed_icon);

	connect(btn_up_day, SIGNAL(clicked()), this, SLOT(incDay()));
	connect(btn_up_month, SIGNAL(clicked()), this, SLOT(incMonth()));
	connect(btn_up_year, SIGNAL(clicked()), this, SLOT(incYear()));

	QHBoxLayout *h_up_box = new QHBoxLayout(&main_layout);
	h_up_box->addWidget(btn_up_day);
	h_up_box->addWidget(btn_up_month);
	h_up_box->addWidget(btn_up_year);

	date_display = new QLCDNumber(this);
	date_display->setSegmentStyle(QLCDNumber::Flat);
	date_display->setNumDigits(10);
	date_display->display(QString("%1:%2:%3").arg(date.day()).arg(date.month()).arg(date.year()));
	date_display->setFrameStyle(QFrame::NoFrame);
	main_layout.addWidget(date_display);

	const QString btn_down_img = QString("%1%2").arg(IMG_PATH).arg("arrdw.png");
	const QString btn_down_img_press = QString("%1%2").arg(IMG_PATH).arg("arrdwp.png");
	icon         = icons_library.getIcon(btn_down_img);
	pressed_icon = icons_library.getIcon(btn_down_img_press);
	if (!icon)
		qDebug("[TERMO] FSBannDate: could not get icon, prepare for strangeness");
	if (!pressed_icon)
		qDebug("[TERMO] FSBannDate: could not get pressed icon, prepare for strangeness");
	btn_down_day = new BtButton(this, 0);
	btn_down_month = new BtButton(this, 0);
	btn_down_year = new BtButton(this, 0);
	btn_down_day->setPixmap(*icon);
	btn_down_day->setPressedPixmap(*pressed_icon);
	btn_down_month->setPixmap(*icon);
	btn_down_month->setPressedPixmap(*pressed_icon);
	btn_down_year->setPixmap(*icon);
	btn_down_year->setPressedPixmap(*pressed_icon);

	connect(btn_down_day, SIGNAL(clicked()), this, SLOT(decDay()));
	connect(btn_down_month, SIGNAL(clicked()), this, SLOT(decMonth()));
	connect(btn_down_year, SIGNAL(clicked()), this, SLOT(decYear()));

	QHBoxLayout *h_down_box = new QHBoxLayout(&main_layout);
	h_down_box->addWidget(btn_down_day);
	h_down_box->addWidget(btn_down_month);
	h_down_box->addWidget(btn_down_year);
}

QDate FSBannDate::getDate()
{
	return date;
}

void FSBannDate::incDay()
{
	date = date.addDays(1);
	date_display->display(QString("%1:%2:%3").arg(date.day()).arg(date.month()).arg(date.year()));
}

void FSBannDate::incMonth()
{
	date = date.addMonths(1);
	date_display->display(QString("%1:%2:%3").arg(date.day()).arg(date.month()).arg(date.year()));
}

void FSBannDate::incYear()
{
	date = date.addYears(1);
	date_display->display(QString("%1:%2:%3").arg(date.day()).arg(date.month()).arg(date.year()));
}

void FSBannDate::decDay()
{
	if (date.addDays(-1) >= QDate::currentDate())
	{
		date = date.addDays(-1);
		date_display->display(QString("%1:%2:%3").arg(date.day()).arg(date.month()).arg(date.year()));
	}
}

void FSBannDate::decMonth()
{
	if (date.addMonths(-1) >= QDate::currentDate())
	{
		date = date.addMonths(-1);
		date_display->display(QString("%1:%2:%3").arg(date.day()).arg(date.month()).arg(date.year()));
	}
}

void FSBannDate::decYear()
{
	if (date.addYears(-1) >= QDate::currentDate())
	{
		date = date.addYears(-1);
		date_display->display(QString("%1:%2:%3").arg(date.day()).arg(date.month()).arg(date.year()));
	}
}

void FSBannDate::Draw()
{
	BannFullScreen::Draw();
}

void FSBannDate::postDisplay()
{
	sottoMenu *parent = static_cast<sottoMenu *> (parentWidget());
	parent->setNavBarMode(10, I_OK);
}

void FSBannDate::status_changed(QPtrList<device_status> list)
{
}
