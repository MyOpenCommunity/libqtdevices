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
#include "btbutton.h"
#include "skinmanager.h"
#include "media_device.h" // SourceDevice
#include "titlelabel.h" // TextOnImageLabel
#include "fontmanager.h"
#include "state_button.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>


AuxSource::AuxSource(const QString &area, SourceDevice *dev, const QString &description) :
	AudioSource(area, dev)
{
	center_icon = new TextOnImageLabel(0, description);
	center_icon->setFont(bt_global::font->get(FontManager::AUDIO_SOURCE_TEXT));

	initBanner(bt_global::skin->getImage("on"), bt_global::skin->getImage("off"), bt_global::skin->getImage("previous"),
		   bt_global::skin->getImage("next"), QString());
	center_icon->setBackgroundImage(bt_global::skin->getImage("source_background"));

	QHBoxLayout *hbox = new QHBoxLayout(this);
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(5);
	hbox->addWidget(left_button);
	hbox->addWidget(center_left_button);
	hbox->addWidget(center_icon);
	hbox->addWidget(center_right_button);
	hbox->addStretch(1);
}
