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

#include <QHash>
#include <QString>

/**
 * \class SkinManager
 *
 * This class load the stylesheet and images from the skin configuration file.
 * To get images, you should set the cid before getting image as the "context".
 */
class SkinManager
{
public:
	SkinManager(QString filename);
	QString getStyle();
	void setCid(int cid);

	// Return the full image path
	QString getImage(QString name);

private:
	QString style;
	int current_cid;
	QHash<int, QHash<QString, QString> > images;
};


namespace bt_global { extern SkinManager *skin; }


#endif // SKINMANAGER_H
