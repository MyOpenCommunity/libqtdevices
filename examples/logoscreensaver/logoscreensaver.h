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
#ifndef LOGOSCREENSAVER_H
#define LOGOSCREENSAVER_H

#include "screensaver.h"
#include <QPixmap>
#include <QPoint>
#include <QRect>

class QPaintEvent;

/*!
	\ingroup Core
	\brief Represents a screensaver with the myhome logo bouncing.
*/
class LogoScreenSaver : public ScreenSaver
{
Q_OBJECT
public:
	LogoScreenSaver();

	virtual void start(Window *w);
	virtual ScreenSaver::Type type() { return ScreenSaver::LOGO; }

protected:
	virtual void paintEvent(QPaintEvent *e);

protected slots:
	virtual void refresh();

private:
	void initLogoRect();

	QRect logo_rect;
	QPoint logo_speed;
	QPixmap logo_pixmap;
};

#endif
