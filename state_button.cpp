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


#include "state_button.h"
#include "generic_functions.h" //getBostikName
#include "icondispatcher.h" // icons_cache


StateButton::StateButton(QWidget *parent) : BtButton(parent)
{
	setStatus(OFF);
}

void StateButton::setDisabledImage(const QString &path)
{
	disabled_pixmap = *bt_global::icons_cache.getIcon(path);
}

void StateButton::setStatus(StateButton::Status st)
{
	status = st;
	switch (status)
	{
	case DISABLED:
		setIcon(disabled_pixmap);
		disable();
		break;
	case ON:
		setIcon(pressed_pixmap);
		enable();
		break;
	case OFF:
	default:
		setIcon(pixmap);
		enable();
		break;
	}
}


