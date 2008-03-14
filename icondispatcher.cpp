/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief Icon Cache Class
 *
 * This class uses an internal QCache to implement a cache for the icons used in banner objects.
 * The class creates, dispatches and destroys the needed QPixmap.
 *
 * \author Alessandro Della Villa <kemosh@develer.com>
 */

#include "icondispatcher.h"

#include "qfile.h"
#include "main.h"

IconDispatcher::IconDispatcher(QObject *parent, const char *name)
 : QObject(parent, name)
{
	/*
	 * Create Internal QCache object, uses three parameters:
	 *
	 * \param size is the max number of elements in Cache, best if it is a prime number
	 *
	 * \param maxCost is the max total cost of the CACHE (before to erase elements)
	 * the cost is given at the insert time but we gave cost=0 for each element
	 *
	 * \param caseSensitive tells if the hash keys of CACHE are case sensitive or not
	 */
	const int maxCost = 100;
	const int size = 199;
	const bool caseSensitive = TRUE;
	cache = new QCache<QPixmap>(maxCost, size, caseSensitive);
}


IconDispatcher::~IconDispatcher()
{

	// Create Iterator on Cache
	QCacheIterator<QPixmap> iter = QCacheIterator<QPixmap>(*cache);

	/*
	 * Seek & Destroy cached elements,
	 * the Method iter() of QCacheIterator returns an element and points the next one
	 */
	while (!iter.isEmpty())
	{
		delete iter();
	}
	
	// Destroy Cache Object
	delete cache;
}


QPixmap* IconDispatcher::getIconPointer(const char *name)
{
	QPixmap *icon = cache->find(name, FALSE);

	if (icon)
	{
		/*
		 * If already exist in cache a pointer to QPixmap with hash=name
		 * a pointer to this image is returned
		 */
		return icon;
	}
	else
	{
		// QPixmap object is created, (name, pointer) is stored in cache
		QPixmap* image_pointer;
		image_pointer = new QPixmap();

		cache->insert(name, image_pointer, 0, 0);
		return image_pointer;
	}
}


QPixmap* IconDispatcher::getIcon(const char *name, const char * format, QPixmap::ColorMode mode)
{
	/// return NULL if file does not exist or if name == NULL
	if (name == NULL)
		return NULL;

	QPixmap* image_pointer = getIconPointer(name);
	// a QPixmap is null when has zero width, zero height and no contents.
	if (image_pointer->isNull())
	{
		if (!image_pointer->load(name, format, mode))
		{
			qDebug("Error loading icon '%s', using empty icon", name);
			if (!image_pointer->load(ICON_VUOTO, format, mode))
				qDebug("*** FATAL ***: empty icon not found!");
		}
	}
	return image_pointer;
}


QPixmap* IconDispatcher::getIcon(const char *name, const char * format, int conversion_flags)
{
	/// return NULL if file does not exist or if name == NULL
	if (  name == NULL || !QFile::exists(name) ) return NULL;

	QPixmap* image_pointer = getIconPointer(name);
	// a QPixmap is null when has zero width, zero height and no contents.
	if (image_pointer->isNull())
		image_pointer->load(name, format, conversion_flags);
	return image_pointer;
}

