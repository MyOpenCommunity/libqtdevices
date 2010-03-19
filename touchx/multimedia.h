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

class QDomNode;
class StateButton;
class QFileSystemWatcher;
class MultimediaFileListPage;


enum MountType
{
	MOUNT_USB,
	MOUNT_SD,
};

/**
 * Handles mounting/unmounting of USB/SD devices
 */
class WatchMounts : public QObject
{
Q_OBJECT
public:
	// returns the global mount watcher object
	static WatchMounts &getWatcher();

	// returns the list of currently-mounted directories
	QStringList mountState() const;

	// mount/unmount the path
	void mount(const QString &device, const QString &dir);
	void unmount(const QString &dir);

	// starts watching for mount/umount events, emits a directoryMounted() signal
	// for every mounted filesystem
	void startWatching();

signals:
	void directoryMounted(const QString &dir, MountType type);
	void directoryUnmounted(const QString &dir, MountType type);

private:
	WatchMounts();

	QStringList parseMtab() const;
	QStringList parseUsbRemoved() const;

	void mtabChanged();
	void usbRemoved();

	MountType mountType(const QString &dir) const;

private slots:
	void fileChanged(const QString &file);
	void directoryChanged(const QString &directory);
	void checkSD();

private:
	// the SD has already been mounted; this is unset only after the
	// SD card has been removed, not when it is unmounted
	bool sd_mounted;

	// list of currently-mounted filesystems
	QStringList mount_points;

	QFileSystemWatcher *watcher;
};


class FileSystemBrowseButton : public IconPageButton
{
Q_OBJECT
public:
	FileSystemBrowseButton(WatchMounts &watch, MultimediaFileListPage *browser,
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

