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


#include "screensaverpage.h"
#include "screensaver.h"
#include "displaycontrol.h" // (*bt_global::display)
#include "singlechoicecontent.h"
#include "navigation_bar.h" // NavigationBar
#include "skinmanager.h" // bt_global::skin
#include "btbutton.h" // BtButton
#include "generic_functions.h" //getPressName
#include "xml_functions.h" // getTextChild
#include "itemlist.h"
#include "state_button.h"
#include "bann_settings.h" // ScreensaverTiming
#ifdef LAYOUT_TOUCHX
#include "multimedia.h" // FilesystemBrowseButton
#endif
#include "main.h" // getPageNode(), MULTIMEDIA

#include <QAbstractButton>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>
#include <QTemporaryFile>
#include <errno.h> // errno

#define SLIDESHOW_FILENAME "cfg/extra/slideshow_images.txt"
#define ARRAY_SIZE(x) int(sizeof(x) / sizeof((x)[0]))


enum {
	SELBUTTON_ON = 0,
	SELBUTTON_OFF,
	BUTTON_ICON
};

enum
{
	PAGE_USB = 16002,
	PAGE_SD = 16005,
};

namespace
{
	const char *image_files[] = {"png", "gif", "jpg", "jpeg"};

	// transforms an extension to a pattern (es. "wav" -> "*.[wW][aA][vV]")
	void addFilters(QStringList &filters, const char **extensions, int size)
	{
		for (int i = 0; i < size; ++i)
		{
			QString pattern = "*.";

			for (const char *c = extensions[i]; *c; ++c)
				pattern += QString("[%1%2]").arg(QChar(*c)).arg(QChar::toUpper((unsigned short)*c));

			filters.append(pattern);
		}
	}
}


ScreenSaverPage::ScreenSaverPage()
{
	timing = 0;
	addBanner(SingleChoice::createBanner(tr("No screensaver")), ScreenSaver::NONE);
	addBanner(SingleChoice::createBanner(tr("Time")), ScreenSaver::TIME);
	addBanner(SingleChoice::createBanner(tr("Text")), ScreenSaver::TEXT);
	// TODO: these types will be available on BTouch only
	addBanner(SingleChoice::createBanner(tr("Line")), ScreenSaver::LINES);
	addBanner(SingleChoice::createBanner(tr("Balls")), ScreenSaver::BALLS);

	//addBanner(tr("Deform"), ScreenSaver::DEFORM); // the deform is for now unavailable!
	// TODO maybe we want an OK button for touch 10 as well

#ifdef LAYOUT_TOUCHX
	CheckableBanner *b = SingleChoice::createBanner(tr("Slideshow"), bt_global::skin->getImage("change_settings"));
	addBanner(b, ScreenSaver::SLIDESHOW);
	Page *p = new StorageSelectionPage;
	b->connectRightButton(p);
	connect(b, SIGNAL(pageClosed()), SLOT(showPage()));

	timing = new ScreensaverTiming(tr("Slideshow timeout"), 12000);
	addBottomWidget(timing);
	timing->hide();
#endif
	connect(page_content, SIGNAL(bannerSelected(int)), SLOT(confirmSelection()));
}

void ScreenSaverPage::showPage()
{
	setCheckedId(getCurrentId());
	SingleChoicePage::showPage();
}

int ScreenSaverPage::getCurrentId()
{
	return (*bt_global::display).currentScreenSaver();
}

void ScreenSaverPage::bannerSelected(int id)
{
	// hide timing selection if photo slideshow is not selected
	if (timing)
	{
		// TODO: is there a better way to check photo slideshow
		if (id == 3)
			timing->show();
		else
			timing->hide();
	}

	(*bt_global::display).setScreenSaver(static_cast<ScreenSaver::Type>(id));
	// TODO review when porting the code to BTouch
#ifdef BT_HARDWARE_BTOUCH
	if (id == ScreenSaver::NONE)
		(*bt_global::display).setBrightness(BRIGHTNESS_OFF);
#endif
}

#ifdef LAYOUT_TOUCHX

StorageSelectionPage::StorageSelectionPage()
{
	QDomNode config_node = getPageNode(MULTIMEDIA);
	SkinContext cxt(getTextChild(config_node, "cid").toInt());

	buildPage(new IconContent, new NavigationBar, tr("Slideshow"));
	loadItems(config_node);
}

void StorageSelectionPage::loadItems(const QDomNode &config_node)
{
	FileSelector *browser = NULL;

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		QString icon = bt_global::skin->getImage("link_icon");
		QString descr = getTextChild(item, "descr");

		QDomNode page_node = getPageNodeFromChildNode(item, "lnk_pageID");
		int item_id = getTextChild(item, "id").toInt();

		Page *p = 0;

		// use the item_id for now because some of the items do not
		// have a linked page
		switch (item_id)
		{
		case PAGE_USB:
		case PAGE_SD:
		{
			if (!browser)
				browser = createBrowser();

			QWidget *t = new FileSystemBrowseButton(MountWatcher::getWatcher(), browser,
								item_id == PAGE_USB ? MOUNT_USB : MOUNT_SD, descr,
								bt_global::skin->getImage("mounted"),
								bt_global::skin->getImage("unmounted"));
			page_content->addWidget(t);
			break;
		}
		default:
			;// qFatal("Unhandled page id in StorageSelectionPage::loadItems");
		};

		if (p)
			addPage(p, descr, icon);
	}

	MountWatcher::getWatcher().startWatching();
}

FileSelector *StorageSelectionPage::createBrowser()
{
	FileSelector *browser = new SlideshowSelector;
	connect(browser, SIGNAL(Closed()), SLOT(showPage()));

	return browser;
}

FileList::FileList(QWidget *parent, int rows_per_page) :
		ItemList(parent, rows_per_page), sel_buttons(new QButtonGroup(this))
{
	sel_buttons->setExclusive(false);

	connect(sel_buttons, SIGNAL(buttonClicked(int)), SLOT(checkButton(int)));
}

void FileList::addHorizontalBox(QBoxLayout *layout, const ItemInfo &item, int id_btn)
{
	QFont font = bt_global::font->get(FontManager::TEXT);

	// top level widget (to set background using stylesheet)
	QWidget *boxWidget = new QWidget;
	boxWidget->setFixedHeight(68);

	QHBoxLayout *box = new QHBoxLayout(boxWidget);
	box->setSpacing(5);
	box->setContentsMargins(5, 5, 5, 5);

	QLabel *name = new QLabel(item.name);
	name->setFont(font);
	box->addWidget(name);
	box->addStretch();

	// Retrive item's metadata
	QVariantMap metadata = item.data.toMap();

	// Create the selection button
	StateButton *sel_button = new StateButton;
	sel_button->setCheckable(true);
	sel_button->setOnOff();
	sel_button->setOffImage(item.icons[SELBUTTON_OFF]);
	sel_button->setOnImage(item.icons[SELBUTTON_ON]);
	sel_buttons->addButton(sel_button, id_btn);
	sel_button->setChecked(metadata["selected"].toBool());
	sel_button->setStatus(metadata["selected"].toBool());
	box->addWidget(sel_button, 0, Qt::AlignRight);

	// If the item represent a directory, creates the button to enter in it.
	int type = metadata["type"].toInt();
	if (type == SlideshowSelector::DIRECTORY)
	{
		// button on the right
		BtButton *btn = new BtButton;
		btn->setImage(item.icons[BUTTON_ICON]);
		box->addWidget(btn, 0, Qt::AlignRight);
		buttons_group->addButton(btn, id_btn);
	}
	else
	{
		box->addSpacing(55);
	}

	layout->addWidget(boxWidget);
}

void FileList::checkButton(int btn_id)
{
	int id = current_page * rows_per_page + btn_id;

	StateButton *button = qobject_cast<StateButton *>(sel_buttons->button(id));
	Q_ASSERT_X(button, "FileList::checkButton", "invalid button");

	bool selected = button->getStatus();
	ItemInfo info = item(id);

	info.data.toMap()["selected"] = selected;

	emit itemSelectionChanged(info.description, selected);
}


SlideshowSelector::SlideshowSelector() :
		FileSelector(4, "/"), handler(new ImageSelectionHandler)
{
	FileList *item_list = new FileList(0, 4);
	connect(item_list, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));

	PageTitleWidget *title_widget = new PageTitleWidget(tr("Folder"), SMALL_TITLE_HEIGHT);
	connect(item_list, SIGNAL(contentScrolled(int, int)), title_widget, SLOT(setCurrentPage(int, int)));

	NavigationBar *nav_bar = new NavigationBar("eject");

	connect(&MountWatcher::getWatcher(), SIGNAL(directoryUnmounted(const QString &, MountType)),
		SLOT(unmounted(const QString &)));
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(unmount()));
	browse_directory = bt_global::skin->getImage("browse_directory");
	selbutton_off = bt_global::skin->getImage("unchecked");
	selbutton_on = bt_global::skin->getImage("checked");

	buildPage(item_list, nav_bar, 0, title_widget);
	layout()->setContentsMargins(0, 5, 25, 10);

	connect(nav_bar, SIGNAL(backClick()), SLOT(browseUp()));
	connect(this, SIGNAL(notifyExit()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(upClick()), item_list, SLOT(prevItem()));
	connect(nav_bar, SIGNAL(downClick()), item_list, SLOT(nextItem()));
	connect(item_list, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));

	connect(item_list, SIGNAL(itemSelectionChanged(QString,bool)), SLOT(setSelection(QString,bool)));
}

SlideshowSelector::~SlideshowSelector()
{
	delete handler;
}

void SlideshowSelector::nextItem()
{
	page_content->nextItem();
}

void SlideshowSelector::prevItem()
{
	page_content->prevItem();
}

void SlideshowSelector::showPage()
{
	if (getRootPath().isEmpty())
		emit Closed();
	else
		FileSelector::showPage();
}

void SlideshowSelector::showPageNoReload()
{
	if (getRootPath().isEmpty())
		emit Closed();
	else
		Selector::showPage();
}

bool SlideshowSelector::browseFiles(const QDir &directory, QList<QFileInfo> &files)
{
	QStringList filters;
	addFilters(filters, image_files, ARRAY_SIZE(image_files));

	// Create fileslist from files
	QList<QFileInfo> temp_files_list = directory.entryInfoList(filters);

	if (temp_files_list.empty())
	{
		qDebug() << "[IMAGES] empty directory: " << directory.absolutePath();
		return false;
	}

	files.clear();

	QList<ItemList::ItemInfo> names_list;

	for (int i = 0; i < temp_files_list.size(); ++i)
	{
		const QFileInfo& f = temp_files_list.at(i);
		if (f.fileName() == "." || f.fileName() == "..")
			continue;

		QStringList icons;
		icons << selbutton_on;
		icons << selbutton_off;

		QVariantMap metadata;
		// TODO: get selection state for current file.
		metadata.insert("selected", handler->isItemSelected(f.canonicalFilePath()));

		if (f.isFile())
		{
			metadata.insert("type", SlideshowSelector::FILE);
		}
		else if (f.isDir())
		{
			metadata.insert("type", SlideshowSelector::DIRECTORY);
			icons << browse_directory;
		}

		ItemList::ItemInfo info(f.fileName(), f.canonicalFilePath(), icons, metadata);
		names_list.append(info);
		files.append(f);
	}

	page_content->setList(names_list, displayedPage(directory));
	page_content->showList();

	return true;
}

int SlideshowSelector::currentPage()
{
	return page_content->getCurrentPage();
}

void SlideshowSelector::setSelection(const QString &path, bool selected)
{
	if (selected)
		handler->insertItem(path);
	else
		handler->removeCurrentFile(path, getFiles());
}

void SlideshowSelector::unmounted(const QString &dir)
{
	if (dir == getRootPath() && isVisible())
		emit Closed();
	setRootPath("");
}

void SlideshowSelector::unmount()
{
	MountWatcher::getWatcher().unmount(getRootPath());
}


ImageSelectionHandler::ImageSelectionHandler()
{
	loadSlideshowFromFile();
}

void ImageSelectionHandler::compactDirectory(const QString &dir, const QFileInfoList &items_in_dir)
{
	qDebug() << "compacting directory " << dir;
	foreach (const QFileInfo &fi, items_in_dir)
		removeItem(fi.absoluteFilePath());
	insertItem(dir);
	qDebug() << "Result of compacting the directory: " << selected_images + inserted_images - removed_images;
}

void ImageSelectionHandler::saveSlideshowToFile()
{
	selected_images.unite(inserted_images);
	selected_images.subtract(removed_images);

	QTemporaryFile f("./temp_slideshowXXXXXX.txt");
	if (!f.open())
	{
		qWarning() << "Error creating temporary file for slideshow images";
		return;
	}

	foreach (QString path, selected_images)
	{
		path = path.simplified();
		f.write(path.toLocal8Bit());
		f.write("\n");
	}
	f.close();
	QFileInfo fi(f);
	qDebug() << "Saved file to " << fi.absoluteFilePath();

	if (::rename(qPrintable(fi.absoluteFilePath()), SLIDESHOW_FILENAME))
		qWarning() << "Could not correctly save slideshow file, error code = " << errno;
}

void ImageSelectionHandler::loadSlideshowFromFile()
{
	inserted_images.clear();
	removed_images.clear();

	QFile f(SLIDESHOW_FILENAME);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning() << "Could not read slideshow images file: " << QDir::currentPath() + SLIDESHOW_FILENAME;
		return;
	}

	while (!f.atEnd())
	{
		QString path = f.readLine().simplified();
		selected_images.insert(path);
	}
	qDebug() << "loadSlideshowFromFile, result: " << selected_images;
}

void ImageSelectionHandler::insertItem(const QString &path)
{
	// this is harmless if path is in selected_images
	removed_images.remove(path);
	inserted_images.insert(path);
}

void ImageSelectionHandler::removeItem(const QString &path)
{
	inserted_images.remove(path);
	removed_images.insert(path);
}

void ImageSelectionHandler::removeCurrentFile(const QString &path, const QFileInfoList &items_in_dir)
{
	Q_ASSERT_X(isItemSelected(path), "SlideshowSelectionPage::removeCurrentFile", "Given path is not selected!");
	// simple case: path is a file and is selected explicitly
	if (selected_images.contains(path) || inserted_images.contains(path))
	{
		removeItem(path);
		qDebug() << "Removing explicitly selected file: " << path;
		return;
	}

	QString basename;
	QString parent = getParentDirectory(path, &basename);
	while (!parent.isEmpty())
	{
		Q_ASSERT_X(isItemSelected(parent), "SlideshowSelectionPage::removeCurrentFile", "Parent is not selected.");
		// first add all files and directories excluding current basename
		foreach (const QFileInfo &fi, items_in_dir)
		{
			if (fi.fileName() != basename)
				insertItem(fi.absoluteFilePath());
		}

		// then exclude parent directory itself and terminate if we are the outermost selected directory
		if (selected_images.contains(parent) || inserted_images.contains(parent))
		{
			removeItem(parent);
			qDebug() << "Outermost directory: " << parent;
			break;
		}
		parent = getParentDirectory(parent, &basename);
	}
}

QString ImageSelectionHandler::getParentDirectory(const QString &path, QString *file_path)
{
	// find the previous separator and remove everything from that point
	int pos = path.lastIndexOf(QDir::separator(), -2);
	if (file_path)
		*file_path = path.mid(pos + 1);
	if (pos < 0)
		return QString();
	else
		return path.left(pos);
}

bool ImageSelectionHandler::isItemSelected(QString abs_path)
{
	while (!abs_path.isEmpty())
	{
		if ((selected_images.contains(abs_path) || inserted_images.contains(abs_path)) && !(removed_images.contains(abs_path)))
			return true;

		abs_path = getParentDirectory(abs_path);
	}
	return false;
}

bool ImageSelectionHandler::isItemExplicitlySelected(const QString &abs_path)
{
	// TODO: this should also check if abs_path is in removed_images
	return selected_images.contains(abs_path) || inserted_images.contains(abs_path);
}

QSet<QString> ImageSelectionHandler::getSelectedImages()
{
	return selected_images + inserted_images - removed_images;
}

#endif
