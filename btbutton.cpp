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


BtButton::BtButton(QWidget *parent) : QPushButton(parent)
{
	initButton();
}

BtButton::BtButton(QString icon_path, QWidget *parent) : QPushButton(parent)
{
	initButton();
	setImage(icon_path);
}

void BtButton::initButton()
{
	setFocusPolicy(Qt::NoFocus);
	is_on_off = false;
	is_enabled = true;
	connect(this, SIGNAL(toggled(bool)), SLOT(setStatus(bool)));
}

QSize BtButton::sizeHint() const
{
	if (!pixmap.isNull())
		return pixmap.size();
	return QSize();
}

void BtButton::setOnOff()
{
	is_on_off = true;
	current_status = false;
}

bool BtButton::isToggle()
{
	return isCheckable() || is_on_off;
}

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

void BtButton::setPressedImage(const QString &pressed_icon)
{
	pressed_pixmap = *bt_global::icons_cache.getIcon(pressed_icon);
}

void BtButton::setPressedPixmap(const QPixmap &p)
{
	pressed_pixmap = p;
}

void BtButton::setPixmap(const QPixmap &p)
{
	bool need_update = (pixmap.toImage() != p.toImage());
	pixmap = p;
	// Icon is set only the first time; in other cases the pixmap (pressed
	// or normal) is set when there is a status change.
	if (icon().isNull() || need_update)
	{
		setIcon(pixmap);
		setIconSize(pixmap.size());
		resize(pixmap.size());
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
	// Toggle buttons are managed by toggled slot, that is always called when
	// the button changes its state.
	if (!isToggle() && !pressed_pixmap.isNull())
		setIcon(pressed_pixmap);

	beep();
	QPushButton::mousePressEvent(event);
}

void BtButton::mouseReleaseEvent(QMouseEvent *event)
{
	// Manages only normal buttons. Toggle buttons are managed by toggled slot.
	if (!isToggle() && !pressed_pixmap.isNull())
		setIcon(pixmap);

	QPushButton::mouseReleaseEvent(event);
}

void BtButton::setStatus(bool st)
{
	// If the pressed_pixmap doesn't exists, there is nothing to do (because the
	// pixmap is already the normal one)
	if (!pressed_pixmap.isNull())
		setIcon(!st ? pixmap : pressed_pixmap);

	current_status = st;
}

bool BtButton::getStatus()
{
	return current_status;
}

void BtButton::enable()
{
	is_enabled = true;
}

void BtButton::disable()
{
	is_enabled = false;
}

