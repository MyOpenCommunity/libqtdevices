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


#ifndef SCREENSAVERPAGE_H
#define SCREENSAVERPAGE_H

#include "singlechoicepage.h"
#include "gridcontent.h" // GridContent
#include "fileselector.h"
#include "itemlist.h"

#include <QDir>
#include <QSet>

class BtButton;
class QLabel;
class QDomNode;
class ImageSelectionHandler;

class ScreenSaverPage : public SingleChoicePage
{
Q_OBJECT
public:
	ScreenSaverPage();
	virtual void showPage();

protected:
	virtual int getCurrentId();
	virtual void bannerSelected(int id);
};


/**
 * The file list
 */
class FileList : public ItemList
{
Q_OBJECT
public:
	FileList(QWidget *parent, int rows_per_page);

signals:
	void itemSelectionChanged(const QString &path, bool selected);

protected:
	virtual void addHorizontalBox(QBoxLayout *layout, const ItemInfo &item, int id_btn);

private slots:
	void checkButton(int btn_id);

private:
	QButtonGroup *sel_buttons;
};


/**
 * SlideShowSelector
 *
 * Display the filesystem and permit to select images and directories.
 */
class SlideshowSelector : public FileSelector
{
Q_OBJECT
public:
	enum EntryType {
		FILE = 0,
		DIRECTORY
	};

	typedef FileList ContentType;

	SlideshowSelector();

	void browse(const QString &dir);

public slots:
	virtual void nextItem();
	virtual void prevItem();
	virtual void showPage();
	virtual void showPageNoReload();

protected:
	virtual bool browseFiles(const QDir &directory, QList<QFileInfo> &files);
	virtual int currentPage();

private slots:
	void setSelection(const QString &path, bool selected);

private:
	// root path, used to unmount the device
	QString root_path;

	ImageSelectionHandler *handler;

	// Icon path
	QString browse_directory;
	QString selbutton_on;
	QString selbutton_off;
};



/**
 * Display items in a grid with 2 columns.
 */
class SlideshowImageContent : public GridContent
{
Q_OBJECT
public:
	SlideshowImageContent(QWidget *parent=0);
	void addItem(QWidget *item);
	void clearContent();
	void showContent();

public slots:
	void nextItem();
	void prevItem();

protected:
	virtual void drawContent();

private:
	QList<QWidget *> items;
};


/**
 * Base class for slideshow thumbnail items.
 * Derived classes must provide their own layout.
 */
class SlideshowItem : public QWidget
{
Q_OBJECT
public:
	SlideshowItem(const QString &path, const QString &icon, const QString &pressed_icon);
	void setChecked(bool check);
	void setCheckable(bool is_checkable);

protected:
	BtButton *check_button;
	QLabel *text;
	QString item_path;

private slots:
	void checked(bool check);

signals:
	void itemToggled(bool, QString);
};


/**
 * Slideshow content item to display a directory (directory button, check button, label below).
 */
class SlideshowItemDir : public QWidget
{
Q_OBJECT
public:
	/**
	 * \param path Returned when directoryToggled() is emitted.
	 */
	SlideshowItemDir(const QString &path, const QString &checked_icon, const QString &unchecked_icon, const QString &main_icon);
	void setChecked(bool check);

private:
	BtButton *dir_button, *check_button;
	QLabel *text;
	QString dir_path;

private slots:
	void checked(bool check);
	void dirButtonClicked();

signals:
	void browseDirectory(QString);
	void directoryToggled(bool, QString);
};


/**
 * Slideshow content item to display a thumbnail (thumbnail label, check button, label below)
 */
class SlideshowItemImage : public QWidget
{
Q_OBJECT
public:
	/**
	 * \param filename Returned when fileToggled() is emitted.
	 * \param working_dir Used internally to load the image with path working_dir + filename
	 */
	SlideshowItemImage(const QString &filename, const QString &working_dir, const QString &pressed_icon, const QString &icon);
	void setChecked(bool check);

private:
	BtButton *check_button;
	QLabel *thumbnail, *text;
	QString file_name;

private slots:
	void checked(bool check);

signals:
	void fileToggled(bool, QString);
};



/**
 * Select slideshow settings: add images, remove all, delay between images.
 */
class SlideshowSettings : public QWidget
{
Q_OBJECT
public:
	SlideshowSettings();

signals:
	void clearAllImages();
	void addMoreImages();
};


/**
 * Handles image selection/removal and saving to file
 */
class ImageSelectionHandler
{
public:
	ImageSelectionHandler();
	/*
	 * Remove a file from currently selected set.
	 * Assumes that either path or one of its containing directories is selected.
	 * It operates by looking for the containing directory, deselects it, then adds all files and directories
	 * that do NOT contain the path; then recurse into the not selected directory.
	 * Eg. path = /usr/local/images/slideshow/photo.jpg. If /usr/local/images is selected:
	 * 1. remove /usr/local/images
	 * 2. add /usr/local/images/ *.jpg and directories (except slideshow/)
	 * 3. add /usr/local/images/slideshow/ *.jpg and directories (except photo.jpg)
	 */
	void removeCurrentFile(const QString &path, const QFileInfoList &items_in_dir);

	/*
	 * Compact a directory when all its enclosed items are selected.
	 * \param dir Directory to be compacted
	 * \param items_in_dir List of items individually selected. This should contain the same items as a call
	 * to getFilteredFiles(dir) [otherwise there's a bug somewhere...]
	 */
	void compactDirectory(const QString &dir, const QFileInfoList &items_in_dir);

	/*
	 * Check if an item is selected.
	 * By design we don't query the interface but rely only on the QSets below (it may well be that the interface
	 * doesn't hold as many buttons as items in the directory).
	 * Compared to pixmap loading or scaling, this operation is rather cheap; however, use it sparingly since
	 * it involves 3 set lookups for each level of depth of the given path. For example the path:
	 * /media/disk/photos/img001.jpg
	 * will require 12 (3 * 4) lookups if the file and any directory below it are not selected.
	 */
	bool isItemSelected(QString abs_path);

	/*
	 * Check if an item is explicitly selected, don't check for parent directories.
	 */
	bool isItemExplicitlySelected(const QString &abs_path);

	/*
	 * Add an item
	 * Must first remove from removed_images set then add to inserted_images.
	 */
	void insertItem(const QString &path);

	void saveSlideshowToFile();
	void loadSlideshowFromFile();

	QSet<QString> getSelectedImages();

private:
	// Utility functions that correctly add or remove a path. Remember that any time a path can in only one
	// between removed or inserted images sets (selected_images is really useful only for cancel operations).
	/*
	 * Remove an item.
	 * Must first remove from inserted_images set then add to removed_images.
	 */
	void removeItem(const QString &path);

	/*
	 * Extract parent directory path from parameter.
	 * Eg. /usr/local/file.txt will return /usr/local (without final separator). file_path will be file.txt
	 *
	 * \param path Path you want the parent directory of
	 * \param file_path If not NULL, name of the file or directory stripped away
	 * \return Empty string if no parent is found, the parent directory otherwise.
	 */
	QString getParentDirectory(const QString &path, QString *file_path = 0);

	// removed_images: will be removed from selected_images
	// inserted_images: will be added to selected_images
	// selected_images: contains full path names for files. Full path names for directories if all files and dir
	//                  below are selected.
	QSet<QString> selected_images, removed_images, inserted_images;
};



/**
 * Select images to show during the slideshow.
 */
class SlideshowSelectionPage : public Page
{
Q_OBJECT
public:
	typedef SlideshowImageContent ContentType;

	SlideshowSelectionPage(const QString &start_path);
	virtual void showPage();

public slots:
	void browseUp();

private:
	void showFiles();
	void refreshContent();
	bool areAllItemsSelected(const QFileInfoList &file_list);

	QDir current_dir;
	int level;
	QString checked_icon, unchecked_icon, photo_icon;
	ImageSelectionHandler *image_handler;

private slots:
	void enterDirectory(QString dir);
	void confirmSelection();
	void itemSelected(bool is_selected, QString relative_path);
};


/**
 * Show currently selected images for slideshow with option to remove them. Also select slideshow preferences.
 */
class ImageRemovalPage : public Page
{
Q_OBJECT
public:
	typedef SlideshowImageContent ContentType;

	ImageRemovalPage();
	virtual void showPage();
	virtual void activateLayout();

private slots:
	void imagesChanged();

private:
	void refreshContent();
	void loadImages();
	void addItemToContent(const QString &path);
	QString button_icon;
	ImageSelectionHandler *image_handler;
};

#endif // SCREENSAVERPAGE_H
