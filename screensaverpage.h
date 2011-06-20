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
#include "fileselector.h"
#include "itemlist.h"

class BtButton;
class QLabel;
class QDomNode;
class ImageSelectionHandler;
class ScreensaverTiming;
class SlideshowSelector;

/*!
	\ingroup Settings
	\brief Allows the user to choose the ScreenSaver.
*/
class ScreenSaverPage : public SingleChoicePage
{
Q_OBJECT
public:
	ScreenSaverPage(const QDomNode &conf_node);
	virtual void showPage();
	virtual void cleanUp();

protected:
	virtual int getCurrentId();
	virtual void bannerSelected(int id);

private:
	ScreensaverTiming *timing;
	int item_id;
};

#ifdef LAYOUT_TS_10

/**
 * The file list
 */
class FileList : public ItemList
{
Q_OBJECT
public:
	FileList(int rows_per_page = ITEM_PER_PAGE, QWidget *parent = 0);

signals:
	void itemSelectionChanged(const QString &path, bool selected);

protected:
	virtual void addHorizontalBox(QGridLayout *layout, const ItemInfo &item, int id_btn);

private slots:
	void checkButton(int btn_id);

private:
	QButtonGroup *sel_buttons;
};


/*
 * SlideshowSelector
 *
 * Display the filesystem and permit to select images and directories.
 */
class SlideshowSelector : public FileSelector
{
Q_OBJECT
public:
	typedef FileList ContentType;

	SlideshowSelector();
	~SlideshowSelector();

public slots:
	virtual void cleanUp();

private slots:
	void setSelection(const QString &path, bool selected);
	void displayFiles(const EntryInfoList &list);

private:
	void saveFileList();
	ImageSelectionHandler *handler;

	// Icon path
	QString browse_directory;
	QString selbutton_on;
	QString selbutton_off;
};


class SlideshowSelectorFactory : public FileSelectorFactory
{
public:
	virtual FileSelector* getFileSelector();
};

#endif

#endif // SCREENSAVERPAGE_H
