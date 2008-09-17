/**
 * \file
 * <!--
 * Copyright 2007, 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief Icon Cache Class (Interface)
 *
 * This class implement a cache for the icons used in banner objects.
 * The class creates, dispatches and destroys the needed QPixmap objects.
 *
 * \author Alessandro Della Villa <kemosh@develer.com>
 * \author Gianni Valdambrini <aleister@develer.com>
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

	IconDispatcher(QObject *parent = 0);
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

#endif
