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


#include "sorgentiaux.h"
#include "aux.h" // class Aux
#include "generic_functions.h" // createCommandFrame
#include "btbutton.h"
#include "skinmanager.h"
#include "media_device.h" // SourceDevice
#include "titlelabel.h" // TextOnImageLabel
#include "fontmanager.h"

#include <QWidget>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>


AuxSource::AuxSource(const QString &area, SourceDevice *dev, const QString &description) :
	AudioSource(area, dev)
{
	left_button = new BtButton;
	center_left_button = new BtButton;
	center_right_button = new BtButton;
	center_icon = new TextOnImageLabel(0, description);
	center_icon->setFont(bt_global::font->get(FontManager::AUDIO_SOURCE_TEXT));
	initBanner(bt_global::skin->getImage("on"), bt_global::skin->getImage("previous"), bt_global::skin->getImage("aux_dummy"),
		bt_global::skin->getImage("next"));
	QHBoxLayout *hbox = new QHBoxLayout(this);
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(5);
	hbox->addWidget(left_button);
	hbox->addWidget(center_left_button);
	hbox->addWidget(center_icon);
	hbox->addWidget(center_right_button);
	hbox->addStretch(1);

	connect(left_button, SIGNAL(clicked()), SLOT(turnOn()));
	connect(center_left_button, SIGNAL(clicked()), dev, SLOT(prevTrack()));
	connect(center_right_button, SIGNAL(clicked()), dev, SLOT(nextTrack()));
}

void AuxSource::initBanner(const QString &left, const QString &center_left, const QString &center,
	const QString &center_right)
{
	initButton(left_button, left);
	initButton(center_left_button, center_left);
	initButton(center_right_button, center_right);
	center_icon->setBackgroundImage(center);
}
