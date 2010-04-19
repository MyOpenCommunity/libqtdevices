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


#ifndef MULTIMEDIA_H
#define MULTIMEDIA_H

#include "iconpage.h"
#include "mount_watcher.h"

class QDomNode;
class StateButton;
class MultimediaFileListPage;


class FileSystemBrowseButton : public IconPageButton
{
Q_OBJECT
public:
	FileSystemBrowseButton(MountWatcher &watch, MultimediaFileListPage *browser,
			       MountType type, const QString &label,
			       const QString &icon_mounted, const QString &icon_unmounted);

private slots:
	void mounted(const QString &path, MountType type);
	void unmounted(const QString &path, MountType type);
	void browse();

private:
	MultimediaFileListPage *browser;
	QString directory;
	MountType type;
};


class MultimediaSectionPage : public IconPage
{
Q_OBJECT
public:
	MultimediaSectionPage(const QDomNode &config_node);

	virtual int sectionId();

private:
	void loadItems(const QDomNode &config_node);
	MultimediaFileListPage *createBrowser();
};

#endif // MULTIMEDIA_H

