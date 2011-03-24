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


#ifndef MOUNT_WATCHER_H
#define MOUNT_WATCHER_H

#include <QObject>
#include <QStringList>
#include <QPair>
#include <QProcess>

class QFileSystemWatcher;


enum MountType
{
	MOUNT_USB,
	MOUNT_SD,
};


/*!
	\ingroup Core
	\ingroup Multimedia
	\brief Manages mounting/unmounting of USB/SD devices.

	Detects when an USB key or SD card is mounted/unmounted and
	emits a signal to notify interested parties.

	Also provides methods to mount/unmount a device.

	For SD cards it also performs the automatic mounting when the device is inserted.
 */
class MountWatcher : public QObject
{
Q_OBJECT
	typedef QPair<QString, QStringList> ProcessEntry;

public:
	MountWatcher();

	// returns the list of currently-mounted directories
	QStringList mountState() const;

	// mount/unmount the path
	void mount(const QString &device, const QString &dir);
	void unmount(const QString &dir);

	// starts watching for mount/umount events, emits a directoryMounted() signal
	// for every mounted filesystem
	void startWatching();

	// force notifications for all currently-mounted mountpoints
	void notifyAll();

signals:
	void directoryMounted(const QString &dir, MountType type);
	void directoryUnmounted(const QString &dir, MountType type);

private:
	QStringList parseMounts() const;
	QStringList parseUsbRemoved() const;

	void mtabChanged();
	void usbRemoved();
	void enqueueCommand(const QString &command, const QStringList &arguments);
	void runQueue();

	MountType mountType(const QString &dir) const;

private slots:
	void fileChanged(const QString &file);
	void directoryChanged(const QString &directory);
	void checkSD();
	void mountComplete();
	void mountError(QProcess::ProcessError error);

private:
	// the SD has already been mounted; this is unset only after the
	// SD card has been removed, not when it is unmounted
	bool sd_mounted;

	// avoid double initialization
	bool watching;

	// list of currently-mounted filesystems
	QStringList mount_points;

	QFileSystemWatcher *watcher;
	QProcess mount_process;
	QList<ProcessEntry> command_queue;
};


namespace bt_global { extern MountWatcher *mount_watcher; }


#endif // MOUNT_WATCHER_H
