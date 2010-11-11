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


#include "bann_videodoorentry.h"
#include "skinmanager.h"
#include "btbutton.h"



EntrancePanel::EntrancePanel(QString descr, QString _where, bool light, bool key)
{
	where = _where;
	initBanner(key ? bt_global::skin->getImage("entrance_panel_key") : QString(),
			bt_global::skin->getImage("entrance_panel"),
			light ? bt_global::skin->getImage("entrance_panel_light") : QString(),
			descr);

	connect(right_button, SIGNAL(pressed()), SLOT(rightPressed()));
	connect(right_button, SIGNAL(released()), SLOT(rightReleased()));
	connect(left_button, SIGNAL(pressed()), SLOT(leftPressed()));
	connect(left_button, SIGNAL(released()), SLOT(leftReleased()));
}

void EntrancePanel::rightPressed()
{
	emit stairLightActivate(where);
}

void EntrancePanel::rightReleased()
{
	emit stairLightRelease(where);
}

void EntrancePanel::leftPressed()
{
	emit openLock(where);
}

void EntrancePanel::leftReleased()
{
	emit releaseLock(where);
}



