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


#ifndef ICONDISPATCHER_H
#define ICONDISPATCHER_H

#include <QObject>
#include <QString>
#include <QPixmap>
#include <QHash>


class IconDispatcher : public QObject
{
	Q_OBJECT
private:
	/// internal function used by getIcon methods
	QPixmap* getIconPointer(QString name);

public:
	QHash<QString, QPixmap*> cache;
	~IconDispatcher();

	/**
	 * IconDispatcher not only returns a pointer to a QPixmap object (using its internal QCache)
	 * but also pre-loads the asked image in QPixmap.
	 *
	 * The images are loaded in QPixmap objects ONLY ONE TIME.
	 * So to change the loading format (if needed) it is necessary
	 * to manually remove the image from cache.
	 */
	QPixmap* getIcon(QString name, const char * format = 0, Qt::ImageConversionFlags flags = Qt::AutoColor);
};


namespace bt_global { extern IconDispatcher icons_cache; }

#endif
