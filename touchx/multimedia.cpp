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


#include "multimedia.h"
#include "multimedia_filelist.h"
#include "navigation_bar.h"
#include "xml_functions.h"
#include "main.h" // MULTIMEDIA
#include "skinmanager.h"
#include "webcam.h"
#include "feedmanager.h"

#include <QFileSystemWatcher>
#include <QSet>
#include <QTextStream>
#include <QProcess>
#include <QtDebug>


#define MTAB         "/etc/mtab"
#define USB_REMOVED  "/tmp/usb_removed.txt"
#define MOUNT_LIST   "/tmp/mount_list.txt"

#ifdef BT_HARDWARE_X11
    #define MOUNT_PATH "/media"
#else
    #define MOUNT_PATH "/mnt"
#endif

enum
{
	PAGE_USB = 2600,
	PAGE_LAN = 2601,
	PAGE_WEB_RADIO = 2603,
	PAGE_WEB_TV = 2604,
	PAGE_WEB_CAM = 2605,
	PAGE_RSS = 2606,
	PAGE_SD = 2609,
};

/*
 * Mounting and unmounting explained
 *
 * when a new USB device is inserted, it is automatically mounted, and the path
 * is written in /tmp/mount_list.txt
 *
 * when an USB device is removed *AND* a matching line is present inside /tmp/mount_list.txt,
 * a /tmp/usb_removed.txt file is written containing the mount paths
 *
 * it is up to the gui to notice that /tmp/usb_removed.txt changed and to unmount the
 * corresponding paths
 *
 * if for some reason mount_list.txt becomes dirty, hell will break loose, because
 * the automatic mount will not try to remount a device that is already listed as mounted,
 * and removing the device will not help because the usb_removed.txt file is not written if
 * the device ID does not match the one in mount_list.txt
 */

/*
 * How WatchMount works
 *
 * USB mount/umount notifications:
 *
 * watches /etc/mtab, when it changes, computes the difference between old and new
 * state and sends mount/umount notifications
 *
 * USB mounting:
 *
 * automatically performed outside the process
 *
 * USB unmounting:
 *
 * usb_removed.txt is parsed and the mount points unmounted using "umount -l"
 */

// can't create it as a global object: needs to be created after app startup
WatchMounts &WatchMounts::getWatcher()
{
	static WatchMounts watcher;

	return watcher;
}

WatchMounts::WatchMounts()
{
	watcher = new QFileSystemWatcher(this);

	connect(watcher, SIGNAL(fileChanged(const QString &)), SLOT(fileChanged(const QString &)));
	connect(watcher, SIGNAL(directoryChanged(const QString &)), SLOT(directoryChanged(const QString &)));
}

QStringList WatchMounts::mountState() const
{
	return mount_points;
}

QStringList WatchMounts::parseMtab() const
{
	QStringList dirs;

	// parse currently-mounted directories from mtab
	QFile mtab(MTAB);
	if (!mtab.open(QFile::ReadOnly))
		return dirs;
	QTextStream mtab_in(&mtab);

	// skip malformed line and mount points not under "/mnt"
	for (;;)
	{
		QString line = mtab_in.readLine();
		if (line.isEmpty())
			break;
		QStringList parts = line.split(' ', QString::SkipEmptyParts);
		if (!parts[1].startsWith(MOUNT_PATH))
			continue;
		dirs.append(parts[1]);
	}

	return dirs;
}

QStringList WatchMounts::parseUsbRemoved() const
{
	QStringList devices;

	// parse currently-mounted directories from mtab
	QFile file(USB_REMOVED);
	if (!file.open(QFile::ReadOnly))
		return devices;
	QTextStream file_in(&file);

	for (;;)
	{
		QString line = file_in.readLine();
		if (line.isEmpty())
			break;
		devices.append(line);
	}

	return devices;
}

void WatchMounts::startWatching()
{
	mount_points = parseMtab();

	foreach (const QString &dir, mount_points)
		emit directoryMounted(dir);

	watcher->addPath(MTAB);
	watcher->addPath("/tmp");

	if (QFile::exists(USB_REMOVED))
		usbRemoved();
}

void WatchMounts::fileChanged(const QString &file)
{
	qDebug() << "File" << file << "changed";
	if (file == MTAB)
		mtabChanged();
}

void WatchMounts::directoryChanged(const QString &dir)
{
	qDebug() << "Directory" << dir << "changed";
	if (QFile::exists(USB_REMOVED))
		usbRemoved();
}

void WatchMounts::usbRemoved()
{
	qDebug() << "Removed USB device";
	QStringList paths = parseUsbRemoved();

	foreach (const QString &dir, paths)
	{
		qDebug() << "Unmounting" << dir;
		QProcess::startDetached("/bin/umount", QStringList() << "-l" << dir);
	}

	QFile::remove(USB_REMOVED);
	// TODO in case of multiple mount points, needs to filter out unmounted directoryes
	//      instead of just removing the file
	QFile::remove(MOUNT_LIST);
}

void WatchMounts::mtabChanged()
{
	// QFileSystemWatcher stops watching files when they are renamed/removed,
	// so restart watching if the watch list is empty
	if (watcher->files().isEmpty())
		watcher->addPath(MTAB);

	QStringList dirs = parseMtab();

	// compute the mounted/unmounted directories as the difference between the
	// old and new directory list
	QSet<QString> old_dirs = QSet<QString>::fromList(mount_points), new_dirs = QSet<QString>::fromList(dirs);
	QSet<QString> mounted = QSet<QString>(new_dirs).subtract(old_dirs);
	QSet<QString> unmounted = QSet<QString>(old_dirs).subtract(new_dirs);

	foreach (const QString &dir, mounted)
		emit directoryMounted(dir);

	foreach (const QString &dir, unmounted)
		emit directoryUnmounted(dir);

	mount_points = dirs;
}


MultimediaSectionPage::MultimediaSectionPage(const QDomNode &config_node)
{
	buildPage(new IconContent, new NavigationBar, getTextChild(config_node, "descr"));
	loadItems(config_node);
}

int MultimediaSectionPage::sectionId()
{
	return MULTIMEDIA;
}

void MultimediaSectionPage::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		QString icon = bt_global::skin->getImage("link_icon");
		QString descr = getTextChild(item, "descr");

		QDomNode page_node = getPageNodeFromChildNode(item, "lnk_pageID");
		int page_id = getTextChild(page_node, "id").toInt();
		int item_id = getTextChild(item, "id").toInt();

		Page *p = 0;

		// use the item_id for now because some of the items do not
		// have a linked page
		switch (item_id)
		{
		case PAGE_USB:
			p = new MultimediaFileListPage;
			break;
		case PAGE_WEB_CAM:
			p = new WebcamListPage(page_node);
			break;
		case PAGE_RSS:
			p = new FeedManager(page_node);
			break;
		default:
			;// qFatal("Unhandled page id in SettingsTouchX::loadItems");
		};

		if (p)
		{
			p->inizializza();
			addPage(p, descr, icon);
		}
	}
}
