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


#include "sorgentimedia.h"
#include "media_device.h" // VirtualSourceDevice
#include "titlelabel.h" // TextOnImageLabel
#include "skinmanager.h"
#include "btbutton.h"
#include "fontmanager.h"
#include "state_button.h"

#include <QDebug>
#include <QHBoxLayout>


MediaSource::MediaSource(const QString &area, VirtualSourceDevice *dev, const QString &description, Page *details) :
	AudioSource(area, dev, details)
{
	center_icon = new TextOnImageLabel(0, description);
	center_icon->setFont(bt_global::font->get(FontManager::AUDIO_SOURCE_TEXT));

	initBanner(bt_global::skin->getImage("on"), bt_global::skin->getImage("off"), bt_global::skin->getImage("previous"),
		   bt_global::skin->getImage("next"), bt_global::skin->getImage("details"));
	center_icon->setBackgroundImage(bt_global::skin->getImage("source_background"));

	QHBoxLayout *hbox = new QHBoxLayout(this);
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(5);
	hbox->addWidget(left_button);
	hbox->addWidget(center_left_button);
	hbox->addWidget(center_icon);
	hbox->addWidget(center_right_button);
	hbox->addWidget(right_button);
	hbox->addStretch(1);
}
