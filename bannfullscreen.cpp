/*!
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
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

BannFullScreen::BannFullScreen(QWidget *parent, QDomNode n, const char *name)
	: banner(parent, name),
	main_layout(this)
{
	descr_label = new QLabel(this, 0);
	main_layout.addWidget(descr_label);

	temp_label = new QLabel(this, 0);
	main_layout.addWidget(temp_label);

	temp = "-23.5\272C";
	descr = "Zone";
}


void BannFullScreen::Draw()
{
	QFont aFont;
	FontManager::instance()->getFont(font_banTermo_tempMis, aFont);
	temp_label -> setFont(aFont);
	temp_label -> setAlignment(AlignHCenter);
	temp_label -> setText(temp);
	FontManager::instance()->getFont(font_banTermo_testo, aFont);
	descr_label -> setFont(aFont);
	descr_label -> setAlignment(AlignHCenter);
	descr_label -> setText(descr);

	banner::Draw();
}

void BannFullScreen::postDisplay()
{
	sottoMenu *parent = static_cast<sottoMenu *> (parentWidget());
	parent->setNavBarMode(3, "");
	qDebug("[TERMO] bfs::postDisplay() done");
}

void BannFullScreen::setSecondForeground(QColor fg2)
{
	second_fg = fg2;
}

void BannFullScreen::status_changed(QPtrList<device_status> list)
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
	: BannFullScreen(parent, n)
{
	setpoint_label = new QLabel(this, 0);
	main_layout.addWidget(setpoint_label);
	local_temp_label = new QLabel(this, 0);
	main_layout.addWidget(local_temp_label);

	setpoint = "-23.5\272C";
	local_temp = "0";
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

	BannFullScreen::Draw();
}

void FSBann4zProbe::postDisplay()
{
	sottoMenu *parent = static_cast<sottoMenu *> (parentWidget());
	parent->setNavBarMode(3, "");
}

FSBann4zFancoil::FSBann4zFancoil(QWidget *parent, QDomNode n, const char *name)
	: FSBann4zProbe(parent, n),
	fancoil_buttons(4, Qt::Horizontal, this)
{
	// set fancoil icons
	//main_layout.addWidget(&fancoil_buttons);
	createFancoilButtons();
	fancoil_buttons.setExclusive(true);
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
				fancoil_buttons.setButton(fancoil_status);
		}
	}
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
			bfs = new BannFullScreen(parent, n);
			break;
		case fs_4z_probe:
			bfs = new FSBann4zProbe(parent, n);
			break;
		case fs_4z_fancoil:
			bfs = new FSBann4zFancoil(parent, n);
			break;
	}
	return bfs;
}

FSBannFactory::FSBannFactory()
{
}

