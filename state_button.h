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


/*!
	\ingroup Core
	\brief A tri-state button that can be on, off, disabled; each state
	uses a different icon.

	StateButton has essentially to work modes: one in which the status is set
	automatically when clicked, and one in which the status is set
	programmatically, using the setStatus() method.
	To switch work mode, you can use the setOnOff() method.

	Image for the various states can be set using the setOnImage(),
	setOffImage() and setDisabledImage() methods the pressed versions are
	automatically set with the same behaviour of BtButton ones.
*/
class StateButton : public BtButton
{
Q_OBJECT
public:
using BtButton::setImage;

	/*!
		\brief State button states.
	*/
	enum Status
	{
		OFF,           /*!< Button unchecked */
		ON,            /*!< Button checked */
		DISABLED       /*!< Button disabled */
	};

	StateButton(QWidget *parent = 0);

	/*!
		\brief Sets the image for the "on" state.
	*/
	void setOnImage(const QString &path);

	/*!
		\brief Sets the image for the "off" state.
	*/
	void setOffImage(const QString &path);

	/*!
		\brief Sets the image for the "disabled" state.
	*/
	void setDisabledImage(const QString &path);

	/*!
		\brief Returns the current Status of the button.

		\sa setStatus(), StateButton::Status
	*/
	Status getStatus() { return status; }

	/*!
		\brief Set the button as a 'on-off button'.

		This means that the button is almost a toggle button, but instead of changes
		status when the button is pressed down, changes its status manually, calling
		the setStatus() method.

		In this case, you can retrieve the current status using getStatus().
	*/
	void setOnOff();

public slots:
	/*!
		\brief Sets the status of the button.

		\sa StateButton::Status
	*/
	void setStatus(StateButton::Status st);

	/*!
		\brief Sets the status of the button.

		This is an overloaded member function, provided for convenience. The
		status is set to StateButton::ON if \a st is true or StateButton::OFF
		if \a st is false.

		\sa StateButton::Status
	*/
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
