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
	typedef QList<int> CidState;

	SkinManager(QString filename);
	QString getStyle();

	// Add/Remove a skin context. Most of the time, you have no need to use these
	// directly, because you can use the wrapper SkinContext, that automatically
	// add a context in the constructor and remove it in the destructor.
	void addToContext(int cid);
	void removeFromContext();

	// Check if an explicit context is set.
	bool hasContext();

	// Return the full image path. The argument name is the same of the tag in
	// the xml file without the prefix "img_".
	QString getImage(QString name);

	// save and restore the context state
	CidState getCidState();
	void setCidState(const CidState &state);

private:
	QString style;
	QList<int> cid_lookup_list;
	QHash<int, QHash<QString, QString> > images;
};


/**
 * \class SkinContext
 *
 * This class is a simply wrapper around the SkinManager::addToContext and
 * SkinManager::removeFromContext, useful to avoid the explicit remove call from
 * the context.
 */
class SkinContext
{
public:
	SkinContext(int cid);
	~SkinContext();
};

namespace bt_global { extern SkinManager *skin; }


#endif // SKINMANAGER_H
