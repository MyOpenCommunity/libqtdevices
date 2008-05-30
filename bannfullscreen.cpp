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

BannFullScreen::BannFullScreen(QWidget *parent, const char *name)
	: banner(parent, name)
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
	//temp_label -> setAlignment(AlignHCenter|AlignVCenter);
	temp_label -> setText(temp);
	FontManager::instance()->getFont(font_banTermo_testo, aFont);
	descr_label -> setFont(aFont);
	//descr_label -> setAlignment(AlignHCenter|AlignVCenter);
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

FSBann4zProbe::FSBann4zProbe(QWidget *parent, const char *name)
	: BannFullScreen(parent, name)
{
	setup_temp_label = new QLabel(this, 0);
	main_layout.addWidget(setup_temp_label);

	setup_temp = "-23.5\272C";
}

void FSBann4zProbe::Draw()
{
	QFont aFont;
	FontManager::instance()->getFont(font_banTermo_tempImp, aFont);
	setup_temp_label->setFont(aFont);
	//setup_temp_label->setAlignment(AlignHCenter|AlignVCenter);
	setup_temp_label->setText(setup_temp);

	BannFullScreen::Draw();
}

void FSBann4zProbe::postDisplay()
{
	sottoMenu *parent = static_cast<sottoMenu *> (parentWidget());
	parent->setNavBarMode(3, "");
}

FSBann4zFancoil::FSBann4zFancoil(QWidget *parent, const char *name)
	: FSBann4zProbe(parent, name),
	fancoil_buttons(4, Qt::Horizontal, this) // bisognera' dare un orientamento???
{
	// set fancoil icons
	//main_layout.addWidget(&fancoil_buttons);
	createFancoilButtons();
	// creare dei btbutton con padre `fancoil_buttons'
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
		// ma cazzo! Come faccio a prendere il colore di sfondo?!
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

FSBannFactory *FSBannFactory::instance = 0;

FSBannFactory *FSBannFactory::getInstance()
{
	if (instance == 0)
	{
		instance = new FSBannFactory;
	}
	return instance;
}

BannFullScreen *FSBannFactory::getBanner(BannID id, QWidget *parent)
{
	BannFullScreen *bfs = 0;
	switch (id)
	{
		case fs_nc_probe:
			bfs = new BannFullScreen(parent);
			break;
		case fs_4z_probe:
			bfs = new FSBann4zProbe(parent);
			break;
		case fs_4z_fancoil:
			bfs = new FSBann4zFancoil(parent);
			break;
	}
	return bfs;
}

FSBannFactory::FSBannFactory()
{
}

