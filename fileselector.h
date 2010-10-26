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


#ifndef FILE_SELECTOR_H
#define FILE_SELECTOR_H

#include "scrollablepage.h"

#include <QDir>
#include <QMap>
#include <QList>
#include <QLabel>
#include <QTime>

class FileSelectorWaitDialog;


/*!
	\brief Abstract class to navigate a hierarchical tree of files/directories
 */
class TreeBrowser : public QObject
{
Q_OBJECT
public:
	/*!
		\brief Information about a single entry in the file list
	 */
	struct EntryInfo
	{
		/// The name of the entry; can be passed to enterDirectory(), getFileUrl(), getAllFileUrls()
		QString name;
		/// Whether this is a directory
		bool is_directory;

		EntryInfo(const QString &_name, bool _is_directory)
			: name(_name), is_directory(_is_directory) { }
	};

	/*!
		\brief Set the root navigation path for the browser

		At the root, isRoot() return \c true, and exitDirectory() can't be called.

		Implementors might not support setRootPath() and getRootPath().
	 */
	virtual void setRootPath(const QStringList &root_path) {}

	/*!
		\brief Returns the root path set with setRootPath().
	 */
	virtual QStringList getRootPath() { return QStringList(); }

	/*!
		\brief Navigate into a directory

		Emits directoryChanged() after the operation completes.
	 */
	virtual void enterDirectory(const QString &name) = 0;

	/*!
		\brief Navigate out of a directory

		Emits directoryChanged() after the operation completes.
	 */
	virtual void exitDirectory() = 0;

	/*!
		\brief Requests the file URL for playback

		Emits urlReceived() when the operation completes.
	 */
	virtual void getFileUrl(const QString &file) = 0;

	/*!
		\brief Requests the file URL for playback

		Emits allUrlsReceived() when the operation completes.
	 */
	virtual void getAllFileUrls(const QStringList &files) = 0;

	/*!
		\brief Retrieves the directory entries for all items in the current directory.

		Emits listReceived() after the operation completes.
	 */
	virtual void getFileList() = 0;

	/*!
		\brief Returns whether this is the root of the tree hierarchy
	 */
	virtual bool isRoot() = 0;

	/*!
		\brief A string that identifies the level inside the path

		It could simply be the stringified level inside the navigation tree,
		or the concatenation of the directory paths.

		The important property is that the path key of a directory is different
		from all its parents and childs; siblings might have the same path key.
	 */
	virtual QString pathKey() = 0;

	static QList<TreeBrowser::EntryInfo> filterEntries(const QList<TreeBrowser::EntryInfo> &entries, QStringList file_filters);

signals:
	void directoryChanged();
	void directoryChangeError();

	void urlReceived(const QString &url);
	void urlRetrieveError();

	void allUrlsReceived(const QStringList &urls);
	void allUrlsRetrieveError();

	void listReceived(QList<TreeBrowser::EntryInfo> list);
	void listRetrieveError();
};

inline bool operator ==(const TreeBrowser::EntryInfo &a, const TreeBrowser::EntryInfo &b)
{
	return a.name == b.name && a.is_directory == b.is_directory;
}


/*!
	\brief File system navigation for FileSelector
 */
class DirectoryTreeBrowser : public TreeBrowser
{
public:
	DirectoryTreeBrowser();

	virtual void setRootPath(const QStringList &root_path);
	virtual QStringList getRootPath();
	virtual void enterDirectory(const QString &name);
	virtual void exitDirectory();
	virtual void getFileUrl(const QString &file);
	virtual void getAllFileUrls(const QStringList &files);
	virtual void getFileList();
	virtual bool isRoot();
	virtual QString pathKey();

private:
	int level;
	QDir current_dir;
	QString root_path;
};


/*!
	\brief File navigation base class

	Provides basic support for directory navigation.

	Handling clicks on files and displaying file list is delegated to subclasses,
	that must handle the TreeBrowser::listReceived() signal emitted by the TreeBrowser.
 */
class FileSelector : public ScrollablePage
{
Q_OBJECT
public:
	FileSelector(TreeBrowser *browser);

public slots:
	void itemIsClicked(int item);
	void browseUp();
	virtual void showPage();
	virtual void browse(const QString &start_path);

#ifdef BT_HARDWARE_TS_10
	// only meaningful for physical file systems
	void unmount();
#endif

signals:
	void fileClicked(int item);

protected:
	void setRootPath(const QString &start_path);
	QString getRootPath();

	/// returns the list of currently displayed files
	const QList<TreeBrowser::EntryInfo> &getFiles() const;

	/// Must be called by subclasses before displaying the file/directory list
	void setFiles(const QList<TreeBrowser::EntryInfo> &files);

	/// returns the page that should be displayed for the given directory
	int displayedPage(const QDir &directory);

protected:
	TreeBrowser *browser;

	void startOperation();
	void operationCompleted();

private:
	/// Change the current dir, return false in case of error.
	bool changePath(QString new_path);

private slots:
	void directoryChanged();

#ifdef BT_HARDWARE_TS_10
	void unmounted(const QString &dir);
#endif

private:
	FileSelectorWaitDialog *working;
	QList<TreeBrowser::EntryInfo> files_list;
	QMap<QString, unsigned>  pages_indexes;
};


class FileSelectorWaitDialog : public QLabel
{
Q_OBJECT
public:
	FileSelectorWaitDialog(Page *parent, int timeout);

	void waitForTimeout();

public slots:
	void abort();

private:
	QTime elapsed;
	int timeout;
};

#endif // FILE_SELECTOR_H

