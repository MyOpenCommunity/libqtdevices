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
#include "iconpage.h"
#include "gridcontent.h" // GridContent
#include "fileselector.h"
#include "itemlist.h"

class BtButton;
class QLabel;
class QDomNode;
class ImageSelectionHandler;
class ScreensaverTiming;
class SlideshowSelector;

class ScreenSaverPage : public SingleChoicePage
{
Q_OBJECT
public:
	ScreenSaverPage(const QDomNode &conf_node);
	virtual void showPage();

protected:
	virtual int getCurrentId();
	virtual void bannerSelected(int id);

private:
	ScreensaverTiming *timing;
};

#ifdef LAYOUT_TOUCHX

class StorageSelectionPage : public IconPage
{
Q_OBJECT
public:
	StorageSelectionPage();

private:
	void loadItems(const QDomNode &config_node);
	FileSelector *createBrowser();
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
	enum EntryType
	{
		FILE = 0,
		DIRECTORY
	};

	typedef FileList ContentType;

	SlideshowSelector();
	~SlideshowSelector();

public slots:
	virtual void nextItem();
	virtual void prevItem();
	virtual void showPage();
	virtual void showPageNoReload();
	virtual void cleanUp();

protected:
	virtual bool browseFiles(const QDir &directory, QList<QFileInfo> &files);
	virtual int currentPage();

private slots:
	void setSelection(const QString &path, bool selected);
	void unmount();
	void unmounted(const QString &path);

private:
	void saveFileList();
	ImageSelectionHandler *handler;

	// Icon path
	QString browse_directory;
	QString selbutton_on;
	QString selbutton_off;
};

#endif

#endif // SCREENSAVERPAGE_H
