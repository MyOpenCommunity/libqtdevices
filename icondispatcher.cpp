/**
 * \file
 * <!--
 * Copyright 2007, 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief Icon Cache Class
 *
 * This class implement a cache for the icons used in banner objects.
 * The class creates, dispatches and destroys the needed QPixmap.
 *
 * \author Alessandro Della Villa <kemosh@develer.com>
 * \author Gianni Valdambrini <aleister@develer.com>
 */


#include "icondispatcher.h"
#include "main.h"

#include <QFile>
#include <QDebug>

IconDispatcher::IconDispatcher(QObject *parent) : QObject(parent) { }

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

QPixmap* IconDispatcher::getIcon(QString name, const char * format, Qt::ImageConversionFlags flags)
{
	if (name.isNull())
		qFatal("Requested an icon with empty string.");

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
