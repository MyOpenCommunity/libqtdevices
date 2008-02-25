/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief Icon Cache Class (Interface)
 *
 * This class uses an internal QCache to implement a cache for the icons used in banner objects.
 * The class creates, dispatches and destroys the needed QPixmap objects.
 *
 * \author Alessandro Della Villa <kemosh@develer.com>
 */

#ifndef ICONDISPATCHER_H
#define ICONDISPATCHER_H

#include <qobject.h>
#include <qcache.h>
#include <qpixmap.h>

class IconDispatcher : public QObject
{
	Q_OBJECT
private:
	/// internal function used by getIcon methods
	QPixmap* getIconPointer(const char *name);

public:
	QCache<QPixmap> *cache;

	IconDispatcher(QObject *parent = 0, const char *name = 0);
	~IconDispatcher();

	/**
	 * There are two different methods to ask Icons to the IconDispatcher,
	 * the methods realize the two different ways to load images in QPixmap objects.
	 *
	 * IconDispatcher not only returns a pointer to a QPixmap object (using its internal QCache)
	 * but also pre-loads the asked image in QPixmap.
	 *
	 * The images are loaded in QPixmap objects ONLY ONE TIME.
	 * So to change the loading format (if needed) it is necessary
	 * to manually remove the image from cache.
	 */
	QPixmap* getIcon(const char *name, const char * format = 0, QPixmap::ColorMode mode = QPixmap::Auto);
	QPixmap* getIcon(QString name) { return getIcon(name.ascii()); };
	QPixmap* getIcon(const char *name, const char * format, int conversion_flags);
};

#endif
