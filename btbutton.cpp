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


#include "btbutton.h"
#include "icondispatcher.h" // bt_global::icons_cache
#include "hardware_functions.h" // beep
#include "generic_functions.h" // getPressName

#include <QDebug>
#include <QFile>
#include <QEvent>
#include <QPainter>


/*!
	\ingroup Core
	\class BtButton
	\brief Custom button with an image and no borders.

	Defines a custom button that displays an image for its "normal" state, and
	optionally one for the "pressed" state. It can play a sound when it is
	clicked, too.

	The button image can be set in constructor or using the \a setImage()
	method. By default, the button tries to load the icon for the "pressed"
	state, appending "p" to the path of the icon. If this isn't the wanted
	behaviour, you can create a button without setting any icon, than call the
	\a setImage() method using BtButton::NO_FLAG as second parameter.

	If you have QPixmap instances instead of paths, you can use the
	\a setPixmap() and \a setPressedPixmap() methods.

	\sa setImage(), BtButton::IconFlag
 */


/*!
	\enum BtButton::IconFlag
	Flags to control the \a setImage() behaviour.
 */
/*!
	\var BtButton::IconFlag BtButton::NO_FLAG
	Do not try to load the pressed image
 */

/*!
	\var BtButton::IconFlag BtButton::LOAD_PRESSED_ICON
	Try to load the pressed image
 */

/*!
	\var BtButton::pixmap
	The pixmap to show when the button is in normal state
 */

/*!
	\var BtButton::pressed_pixmap;
	The pixmap to show when the button is down
 */


/*!
	\brief Constructor
 */
BtButton::BtButton(QWidget *parent) : QPushButton(parent)
{
	initButton();
}

/*!
	\brief Constructor

	Constructs a new BtButton with the given icon_path.

	\sa setImage(), setPressedImage()
 */
BtButton::BtButton(QString icon_path, QWidget *parent) : QPushButton(parent)
{
	initButton();
	setImage(icon_path);
}

void BtButton::initButton()
{
	setFocusPolicy(Qt::NoFocus);
	is_enabled = true;
	beep_enabled = true;
}

/*!
	\brief Enables or disables beep when the button is clicked.
 */
void BtButton::enableBeep(bool enable)
{
	beep_enabled = enable;
}

QSize BtButton::sizeHint() const
{
	if (!pixmap.isNull())
		return pixmap.size();
	return QSize();
}

/*!
	\brief Helper function to load the image for the "pressed" state.

	It tries to load the image for the "pressed" state returns an invalid
	QPixmap on error, the QPixmap containing the loaded image otherwise.
 */
QPixmap BtButton::loadPressedImage(const QString &icon_path)
{
	QString pressed_name = getPressName(icon_path);
	if (QFile::exists(pressed_name))
		return *bt_global::icons_cache.getIcon(pressed_name);
	else
		return QPixmap();
}

/*!
	\brief Sets the image for the "normal" state.

	It tries to load the image for the "normal" state returns an invalid
	QPixmap on error, the QPixmap containing the loaded image otherwise.

	Depending on the value of \a f, this method tries to load the "pressed"
	state image (the default) or not.

	\sa BtButton::IconFlag, loadPressedImage()
 */
void BtButton::setImage(const QString &icon_path, IconFlag f)
{
	setPixmap(*bt_global::icons_cache.getIcon(icon_path));

	if (f == LOAD_PRESSED_ICON)
	{
		QString pressed_name = getPressName(icon_path);
		if (QFile::exists(pressed_name))
			setPressedImage(pressed_name);
	}
}

/*!
	\brief Sets the image for the "pressed" state

	\sa setPressedPixmap()
 */
void BtButton::setPressedImage(const QString &pressed_icon)
{
	setPressedPixmap(*bt_global::icons_cache.getIcon(pressed_icon));
}

/*!
	\brief Sets the pixmap for the "pressed" state

	\sa setPressedImage(), loadPressedImage()
 */
void BtButton::setPressedPixmap(const QPixmap &p)
{
	pressed_pixmap = p;
	if (isDown() && !pressed_pixmap.isNull())
		setIcon(pressed_pixmap);
}

/*!
	\brief Sets the pixmap for the "normal" state

	\sa setImage()
 */
void BtButton::setPixmap(const QPixmap &p)
{
	bool need_update = (pixmap.toImage() != p.toImage());
	pixmap = p;
	// Icon is set only the first time; in other cases the pixmap (pressed
	// or normal) is set when there is a status change.
	if (icon().isNull() || need_update)
	{
		if (pressed_pixmap.isNull() || !isDown())
			setIcon(pixmap);
		setIconSize(pixmap.size());
		setFixedSize(pixmap.size());
	}
}

bool BtButton::event(QEvent *e)
{
	// this code is taken from QAbstractButton::event().
	// Whenever we change Qt version, check the source to see if the check changed (eg. multitouch mouse events)
	if (!is_enabled)
	{
		switch (e->type())
		{
		case QEvent::TabletPress:
		case QEvent::TabletRelease:
		case QEvent::TabletMove:
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		case QEvent::MouseButtonDblClick:
		case QEvent::MouseMove:
		case QEvent::HoverMove:
		case QEvent::HoverEnter:
		case QEvent::HoverLeave:
		case QEvent::ContextMenu:
	#ifndef QT_NO_WHEELEVENT
		case QEvent::Wheel:
	#endif
			return true;
		default:
			break;
		}
	}
	return QPushButton::event(e);
}

void BtButton::mousePressEvent(QMouseEvent *event)
{
	if (!pressed_pixmap.isNull())
		setIcon(pressed_pixmap);

	if (beep_enabled)
		beep();
	QPushButton::mousePressEvent(event);
}

void BtButton::mouseReleaseEvent(QMouseEvent *event)
{
	if (!pressed_pixmap.isNull())
		setIcon(pixmap);

	QPushButton::mouseReleaseEvent(event);
}

void BtButton::paintEvent(QPaintEvent *e)
{
#ifdef LAYOUT_TS_3_5
	// TODO keep default behaviour for TS 3.5''
	QPushButton::paintEvent(e);
#else
	// the default QStyle implementation shifts the pushed button
	// some pixel to the right/bottom, and for the TS 10'' we do not want
	// that for buttons that have a separate "pressed" icon, BUT if there
	// isn't a separate pressed image set, we want the default behaviour
	if (isDown() && pressed_pixmap.isNull())
	{
		QPushButton::paintEvent(e);
	}
	else
	{
		QPainter p(this);

		icon().paint(&p, 0, 0, width(), height());
	}
#endif
}

/*!
	\brief Sets the button enabled
 */
void BtButton::enable()
{
	is_enabled = true;
}

/*!
	\brief Sets the button disabled
 */
void BtButton::disable()
{
	is_enabled = false;
}

