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


#ifndef BTBUTTON_H
#define BTBUTTON_H

#include <QPushButton>


/*!
	\ingroup Core
	\brief Custom button with an image and no borders.

	Defines a custom button that displays an image for its "normal" state, and
	optionally one for the "pressed" state. It can play a sound when it is
	clicked, too.

	The button image can be set in constructor or using the setImage()
	method. By default, the button tries to load the icon for the "pressed"
	state, appending "p" to the path of the icon. If this isn't the wanted
	behaviour, you can create a button without setting any icon, than call the
	setImage() method using BtButton::NO_FLAG as second parameter.

	If you have QPixmap instances instead of paths, you can use the
	setPixmap() and setPressedPixmap() methods.

	\sa setImage(), BtButton::IconFlag
*/
class BtButton : public QPushButton
{
Q_OBJECT
public:

	/*!
		\brief Flags to control the setImage() behaviour.
	*/
	enum IconFlag
	{
		NO_FLAG,            /*!< Do not try to load the pressed image */
		LOAD_PRESSED_ICON,  /*!< Try to load the pressed image */
	};
	BtButton(QWidget *parent = 0);
	BtButton(QString icon_path, QWidget *parent = 0);

	/*!
		\brief Sets the image \a icon_path for the "normal" state.

		It tries to load the image for the "normal" state returns an invalid
		QPixmap on error, the QPixmap containing the loaded image otherwise.

		Depending on the value of \a f, this method tries to load the "pressed"
		state image (the default) or not.

		\sa BtButton::IconFlag, loadPressedImage()
	*/
	void setImage(const QString &icon_path, IconFlag f = LOAD_PRESSED_ICON);

	/*!
		\brief Sets the image \a pressed_icon for the "pressed" state

		\sa setPressedPixmap()
	*/
	void setPressedImage(const QString &pressed_icon);

	/*!
		\brief Sets the button enabled.
	*/
	virtual void enable();

	/*!
		\brief Sets the button disabled.
	*/
	virtual void disable();

	/*!
		\brief Enables or disables beep when the button is clicked.
	*/
	void enableBeep(bool enable);

	// The sizeHint method is required to obtain a layout management that works fine.
	virtual QSize sizeHint() const;

protected:
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void paintEvent(QPaintEvent *e);
	virtual bool event(QEvent *e);

	/*!
		\brief Sets the pixmap \a p for the "pressed" state.

		\sa setPressedImage(), loadPressedImage()
	*/
	void setPressedPixmap(const QPixmap &p);

	/*!
		\brief Sets the pixmap \a p for the "normal" state.

		\sa setImage()
	*/
	void setPixmap(const QPixmap &p);

	/*!
		\brief Helper function to load the image for the "pressed" state.

		It tries to load the image for the "pressed" state returns an invalid
		QPixmap on error, the QPixmap containing the loaded image otherwise.
	 */
	QPixmap loadPressedImage(const QString &icon_path);

	/*! The pixmap to show when the button is in normal state */
	QPixmap pixmap;
	/*! The pixmap to show when the button is down */
	QPixmap pressed_pixmap;

#if DEBUG
	QString image_path;
#endif

private:
	bool is_enabled;
	bool beep_enabled;
	void initButton();
};

#endif // BTBUTTON_H
