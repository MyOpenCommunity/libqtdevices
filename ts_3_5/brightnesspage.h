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


#ifndef BRIGHTNESSPAGE_H
#define BRIGHTNESSPAGE_H

#include "singlechoicepage.h"
#include "displaycontrol.h" // BrightnessLevel

class QShowEvent;


/*
 * The page used to choose the brightness to use when the touchscreen is inactive.
 */
class InactiveBrightnessPage : public SingleChoicePage
{
Q_OBJECT
public:
	InactiveBrightnessPage(const QDomNode &config_node);

protected:
	virtual int getCurrentId();
	virtual void bannerSelected(int id);
	// the showEvent method is required because also other objects can change
	// the brightness value.
	virtual void showEvent(QShowEvent *e);

private slots:
	void updateStatus(bool force = false);

private:
	int item_id;
	BrightnessLevel last_brightness;
};

#endif // BRIGHTNESSPAGE_H
