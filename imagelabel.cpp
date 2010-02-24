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


#include "imagelabel.h"

#include <QSize>
#include <QSizePolicy>
#include <QMouseEvent>
#include <QPixmap>


ImageLabel::ImageLabel()
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	setAlignment(Qt::AlignCenter);
}

void ImageLabel::setPixmap(const QPixmap &pixmap)
{
	QSize screen_size = size(), pixmap_size = pixmap.size();
	QPixmap scaled = pixmap;

	// resize the pixmap if it's too big for the screen
	if (pixmap_size.width() > screen_size.width() ||
		pixmap_size.height() > screen_size.height())
		scaled = pixmap.scaled(screen_size, Qt::KeepAspectRatio);

	QLabel::setPixmap(scaled);
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *e)
{
	QLabel::mouseReleaseEvent(e);

	emit clicked();
}

