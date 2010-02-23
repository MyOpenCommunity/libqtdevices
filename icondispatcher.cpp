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


#include "icondispatcher.h"
#include "main.h"

#include <QFile>
#include <QDebug>


IconDispatcher::~IconDispatcher()
{
	QMutableHashIterator<QString, QPixmap*> it(cache);
	while (it.hasNext())
	{
		it.next();
		delete it.value();
	}
}

QPixmap* IconDispatcher::getIconPointer(QString name)
{
	QHash<QString, QPixmap*>::const_iterator it = cache.find(name);
	if (it != cache.end())
	{
		/*
		 * If already exist in cache a pointer to QPixmap with hash=name
		 * a pointer to this image is returned
		 */	
		return it.value();
	}
	else
	{
		// QPixmap object is created, (name, pointer) is stored in cache
		QPixmap* image_pointer = new QPixmap();
		cache.insert(name, image_pointer);
		return image_pointer;
	}
}

// TODO: centralizzare qua la gestione dell'IMG_PATH! In questo modo si riducono le dipendenze!
QPixmap* IconDispatcher::getIcon(QString name, const char * format, Qt::ImageConversionFlags flags)
{
	if (name.isNull())
		qWarning("Requested an icon with null string.");

	QPixmap* image_pointer = getIconPointer(name);

	if (image_pointer->isNull())
		if (!image_pointer->load(name, format, flags))
		{
			qDebug() << "Error loading icon" << name << ", using empty icon";
			if (!image_pointer->load(ICON_VUOTO))
				qWarning("*** FATAL ***: empty icon not found!");
		}
	return image_pointer;
}

// The global definition of icons_cache
IconDispatcher bt_global::icons_cache;

