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


/*!
	\class StateButton
	\brief A tri-state button that can be on, off, disabled; each state
	uses a different icon.

	StateButton has essentially to work modes: one in which the status is set
	automatically when clicked, and one in which the status is set
	programmatically, using the \a setStatus() method.
	To switch work mode, you can use the \a setOnOff() method.

	Image for the various states can be set using the \a setOnImage(),
	\a setOffImage() and \a setDisabledImage() methods the pressed versions are
	automatically set with the same behaviour of \a BtButton ones.

	\sa setOnOff(), setStatus(), \a setOnImage(), \a setOffImage(),
	\a setDisabledImage()
*/

/*!
	\enum StateButton::Status
	State button states
 */

/*!
	\var StateButton::Status StateButton::OFF
	Button unchecked
 */

/*!
	\var StateButton::Status StateButton::ON
	Button checked
 */

/*!
	\var StateButton::Status StateButton::DISABLED
	Button disabled
 */


/*!
	\brief Constructor
 */
StateButton::StateButton(QWidget *parent) : BtButton(parent)
{
	setStatus(OFF);
	connect(this, SIGNAL(toggled(bool)), SLOT(setStatus(bool)));
	is_on_off = false;
}

/*!
	\brief Sets the image for the "disabled" state
 */
void StateButton::setDisabledImage(const QString &path)
{
	disabled_pixmap = *bt_global::icons_cache.getIcon(path);
}

/*!
	\brief Sets the image for the "on" state
 */
void StateButton::setOnImage(const QString &path)
{
	on_pixmap = *bt_global::icons_cache.getIcon(path);
	QPixmap p = loadPressedImage(path);
	if (!p.isNull())
		pressed_on_pixmap = p;

	setStatus(getStatus());
}

/*!
	\brief Sets the image for the "off" state
 */
void StateButton::setOffImage(const QString &path)
{
	off_pixmap = *bt_global::icons_cache.getIcon(path);
	QPixmap p = loadPressedImage(path);
	if (!p.isNull())
		pressed_off_pixmap = p;

	setStatus(getStatus());
}

/*!
	\fn Status StateButton::getStatus()
	\brief Returns the current \a Status of the button

	\sa setStatus(), StateButton::Status
 */

/*!
	\brief Sets the status of the button

	\sa StateButton::Status
 */
void StateButton::setStatus(StateButton::Status st)
{
	status = st;
	switch (status)
	{
	case DISABLED:
		if (!disabled_pixmap.isNull())
			BtButton::setPixmap(disabled_pixmap);
		disable();
		break;
	case ON:
		if (!on_pixmap.isNull())
			BtButton::setPixmap(on_pixmap);
		if (!pressed_on_pixmap.isNull())
			BtButton::setPressedPixmap(pressed_on_pixmap);
		enable();
		break;
	case OFF:
	default:
		// We assume that the initial image is always present
		BtButton::setPixmap(off_pixmap);
		BtButton::setPressedPixmap(pressed_off_pixmap);
		enable();
		break;
	}
}

/*!
	\fn void StateButton::setStatus(bool st)
	\brief Sets the status of the button

	\overload void StateButton::setStatus(bool st)
 */

/*!
	\brief Set the button as a 'on-off button'.

	This means that the button is almost a toggle button, but instead of changes
	status when the button is pressed down, changes its status manually, calling
	the \a setStatus() method.

	In this case, you can retrieve the current status using the \a getStatus()
	method.
 */
void StateButton::setOnOff()
{
	is_on_off = true;
}

