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


#ifndef STATE_BUTTON_H
#define STATE_BUTTON_H

#include "btbutton.h"

#include <QPixmap>

class QString;


/**
 * A tri-state button that can be on, off, disabled; each state
 * uses a different icon.
 */
class StateButton : public BtButton
{
Q_OBJECT
public:
using BtButton::setImage;

	enum Status
	{
		OFF,
		ON,
		DISABLED
	};

	StateButton(QWidget *parent = 0);

	// Set the image for the three states.  Automatically uses
	// getBostikName to set the pressed button image.
	void setOnImage(const QString &path);
	void setOffImage(const QString &path);
	void setDisabledImage(const QString &path);

	Status getStatus() { return status; }

	// Set the button as a 'on-off button'. This means that the button is almost
	// a toggle button, but instead of changes status when the button is pressed
	// down, changes its status manually, calling the 'setStatus' method.
	// In this case, you can retrieve the current status using the 'getStatus'
	// method.
	void setOnOff();

public slots:
	void setStatus(Status st);
	void setStatus(bool st) { setStatus(st ? ON : OFF); }

private:
	// intentionally not implemented and kept private to hide parent's version,
	// use setOnImage/setOffImage
	void setPressedImage(const QString &pressed_icon);
	void setPressedPixmap(const QPixmap &p);
	void setImage(const QString &icon_path, IconFlag f = LOAD_PRESSED_ICON);
	void setPixmap(const QPixmap &p);

private:
	QPixmap disabled_pixmap, on_pixmap, pressed_on_pixmap, off_pixmap, pressed_off_pixmap;
	Status status;
	bool is_on_off; // a flag that mark if the button is a on-off button
};


#endif // STATE_BUTTON_H
