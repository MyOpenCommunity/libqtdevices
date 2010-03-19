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
	is_enabled = true;
}

QSize BtButton::sizeHint() const
{
	if (!pixmap.isNull())
		return pixmap.size();
	return QSize();
}

QPixmap BtButton::loadPressedImage(const QString &icon_path)
{
	QString pressed_name = getPressName(icon_path);
	if (QFile::exists(pressed_name))
		return *bt_global::icons_cache.getIcon(pressed_name);
	else
		return QPixmap();
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
	setPressedPixmap(*bt_global::icons_cache.getIcon(pressed_icon));
}

void BtButton::setPressedPixmap(const QPixmap &p)
{
	pressed_pixmap = p;
	if (isDown() && !pressed_pixmap.isNull())
		setIcon(pressed_pixmap);
}

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
	if (!pressed_pixmap.isNull())
		setIcon(pressed_pixmap);

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
#ifdef LAYOUT_BTOUCH
	// TODO keep default behaviour for BTouch
	QPushButton::paintEvent(e);
#else
	// the default QStyle implementation shifts the pushed button
	// some pixel to the right/bottom, and for the TouchX we do not want
	// that
	QPainter p(this);

	icon().paint(&p, 0, 0, width(), height());
#endif
}

void BtButton::enable()
{
	is_enabled = true;
}

void BtButton::disable()
{
	is_enabled = false;
}

