/**
 * \file
 * <!--
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This file contains the class used to manage the style and images.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 */

#ifndef SKINMANAGER_H
#define SKINMANAGER_H

#include <QList>
#include <QHash>
#include <QString>

/**
 * \class SkinManager
 *
 * This class load the stylesheet and images from the skin configuration file.
 * To get images, you often should set (adding or removing cid) the "explicit" context
 * before getting them. An "implicit" context, which can be used to retrieve common
 * images, is always set.
 */
class SkinManager
{
public:
	SkinManager(QString filename);
	QString getStyle();
	void addToContext(int cid);
	void removeFromContext();
	// Check if an explicit context is set.
	bool hasContext();

	// Return the full image path. The argument name is the same of the tag in
	// the xml file without the prefix "img_".
	QString getImage(QString name);

private:
	QString style;
	QList<int> cid_lookup_list;
	QHash<int, QHash<QString, QString> > images;
};


namespace bt_global { extern SkinManager *skin; }


#endif // SKINMANAGER_H
