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
#ifndef TREEBROWSER_H
#define TREEBROWSER_H

#include "xmldevice.h"
#include "generic_functions.h"

#include <QDir>
#include <QString>


class DirectoryBrowserMemento
{
friend class DirectoryTreeBrowser;
friend class UPnpClientBrowser;
private:
	QStringList context;
	int mask;
	QStringList root_path;
};

/*!
	\ingroup Core
	\brief Abstract class to navigate a hierarchical tree of files/directories
 */
class TreeBrowser : public QObject
{
Q_OBJECT
public:

	enum Types
	{
		DIRECTORY,
		UPNP
	};

	/*!
		\brief Set the root navigation path for the browser

		At the root, isRoot() return \c true, and exitDirectory() can't be called.

		Implementors might not support setRootPath() and getRootPath().
	 */
	virtual void setRootPath(const QStringList &root_path) { Q_UNUSED(root_path); }

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


	/*!
		\brief Sets \a mask to filter file listing results.

		\note The \a mask must be a bitwise-OR between MultimediaFileTypes values.
	*/
	void setFilter(int mask);

	/*!
		\brief Set the list of albums/directories as the context

		This command is a shortcut to re-enter in a directory or album even
		if is inside others directories/albums.
	*/
	virtual void setContext(const QStringList &context) = 0;

	/*!
		\brief Reset the status of the browser

		This command can ben used to re-start the navigation from the beginning.
	*/
	virtual void reset() = 0;

	/*!
		\brief Clone the internal state of the TreeBrowser

		Returned object must be destroyed by caller.
	*/
	virtual DirectoryBrowserMemento *clone() = 0;
	virtual void restore(DirectoryBrowserMemento *m) = 0;

protected:
	/*!
		\brief Constructor.
	*/
	TreeBrowser() : filter_mask(EntryInfo::ALL) {}

	/*!
		\brief Mask to filter file listing results.
	*/
	int filter_mask;

signals:
	/*!
		\brief Emitted after a successful directory change.
	*/
	void directoryChanged();

	/*!
		\brief Emitted after a successful context change.
	*/
	void contextChanged();

	/*!
		\brief Emitted when an error occours changing directory.
	*/
	void directoryChangeError();

	/*!
		\brief Emitted when a generic error occours.
	*/
	void genericError();

	/*!
		\brief Emitted after a successful entry listing.

		\sa EntryInfoList, EntryInfo
	*/
	void listReceived(EntryInfoList list);

	/*!
		\brief Emitted when an error occours during entry listing.
	*/
	void listRetrieveError();

	/*!
		\brief Emitted when we try to change the current directory but the new dir is empty.
	*/
	void emptyDirectory();

	/*!
		\brief Emitted when we change the actual path.
	*/
	void isRootChanged();

	/*!
		\brief Emitted when entering the navigation root, ie. isRoot() == true.
	*/
	void rootDirectoryEntered();
};


/*!
	\brief Allow retrieving directory listings in small chunks

	The number of returned items is ELEMENTS_DISPLAYED, defined in the implementation file
*/
class PagedTreeBrowser : public TreeBrowser
{
Q_OBJECT
public:
	/*!
		\brief Reads the previous page in the file list

		Emits listReceived() on success, listRetrieveError() on failure.
	*/
	virtual void getPreviousFileList() = 0;

	/*!
		\brief Reads the next page in the file list

		Emits listReceived() on success, listRetrieveError() on failure.
	*/
	virtual void getNextFileList() = 0;

	/*!
		\brief Return the total number of elements in the directory
	*/
	virtual int getNumElements() = 0;

	/*!
		\brief Return the current position in the directory listing
	*/
	virtual int getStartingElement() = 0;

	/*!
		\brief Return ELEMENTS_DISPLAYED elements starting at the given position

		Emits listReceived() on success, listRetrieveError() on failure.
	*/
	virtual void getFileList(int starting_element) = 0;

	/*!
		\brief Return a progressive integer for the last issued operation
	 */
	virtual int lastQueuedCommand() const = 0;

	/*!
		\brief Return a progressive integer for operation currently being processed
	 */
	virtual int lastAnsweredCommand() const = 0;
};


/*!
	\ingroup Core
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
	virtual void getFileList();
	virtual bool isRoot();
	virtual QString pathKey();
	virtual void setContext(const QStringList &context);
	virtual void reset();

	virtual DirectoryBrowserMemento *clone();
	virtual void restore(DirectoryBrowserMemento *m);

private:
	int level;
	QDir current_dir;
	QString root_path;
};


/*!
	\ingroup Core
	\brief UPnP navigation for FileSelector
*/
class UPnpClientBrowser : public PagedTreeBrowser
{
Q_OBJECT
friend class TestUPnpClientBrowser;
public:
	UPnpClientBrowser(XmlDevice *dev);

	virtual void enterDirectory(const QString &name);
	virtual void exitDirectory();
	virtual void getFileList();
	virtual bool isRoot();
	virtual QString pathKey();
	virtual void setContext(const QStringList &context);
	virtual void reset();

	virtual void getPreviousFileList();
	virtual void getNextFileList();
	virtual int getNumElements();
	virtual int getStartingElement();
	virtual void getFileList(int starting_element);

	virtual int lastQueuedCommand() const;
	virtual int lastAnsweredCommand() const;

	virtual DirectoryBrowserMemento *clone();
	virtual void restore(DirectoryBrowserMemento *m);

private slots:
	void handleResponse(const XmlResponse &response);
	void handleError(int response, int code);

private:
	XmlDevice *dev;
	int level;
	QStringList context, new_context;

	// To manage the 'lazy' loading of the elements
	int starting_element;
	int num_elements;
	EntryInfoList cached_elements;
};

#endif // TREEBROWSER_H
