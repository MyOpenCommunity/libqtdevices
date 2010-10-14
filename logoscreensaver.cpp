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

#include "logoscreensaver.h"

#include <QPainter>

#include "skinmanager.h"


LogoScreenSaver::LogoScreenSaver()
	: ScreenSaver(500)
{
	SkinContext ctx(12345);

	QString logo_path = bt_global::skin->getImage("screensaver_logo");
	logo_pixmap.load(logo_path);

	logo_speed.rx() = -1;
	logo_speed.ry() = -1;
}

void LogoScreenSaver::start(Window *w)
{
	ScreenSaver::start(w);
	initLogoRect();
	showWindow();
}

void LogoScreenSaver::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	painter.fillRect(this->rect(), Qt::black);
	painter.drawPixmap(logo_rect, logo_pixmap, logo_pixmap.rect());
}

void LogoScreenSaver::refresh()
{
	if (logo_rect.x() <= window->x() || logo_rect.x() + logo_rect.width() >= window->width())
		logo_speed.rx() = -logo_speed.x();

	if (logo_rect.y() <= window->y() || logo_rect.y() + logo_rect.height() >= window->height())
		logo_speed.ry() = -logo_speed.y();

	logo_rect.translate(logo_speed);
	update();
}

void LogoScreenSaver::initLogoRect()
{
	int x = (window->width() - logo_pixmap.width()) / 2;
	int y = (window->height() - logo_pixmap.height()) / 2;

	logo_rect.moveTo(x, y);
	logo_rect.setSize(logo_pixmap.size());
}
