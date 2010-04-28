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
#include "imageselectionhandler.h"
#ifdef LAYOUT_TOUCHX
#include "multimedia.h" // FilesystemBrowseButton
#endif
#include "main.h" // getPageNode(), MULTIMEDIA

#include <QAbstractButton>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>

#define ARRAY_SIZE(x) int(sizeof(x) / sizeof((x)[0]))


enum
{
	SELBUTTON_ON = 0,
	SELBUTTON_OFF,
	BUTTON_ICON
};

enum
{
	PAGE_USB = 16002,
	PAGE_SD = 16005
};


ScreenSaverPage::ScreenSaverPage(const QDomNode &conf_node) :
	SingleChoicePage(getTextChild(conf_node, "descr"))
{
	timing = 0;
	addBanner(SingleChoice::createBanner(tr("No screensaver")), ScreenSaver::NONE);
	addBanner(SingleChoice::createBanner(tr("Time")), ScreenSaver::TIME);
	addBanner(SingleChoice::createBanner(tr("Text")), ScreenSaver::TEXT);
	// TODO: these types will be available on BTouch only
#ifdef LAYOUT_BTOUCH
	addBanner(SingleChoice::createBanner(tr("Line")), ScreenSaver::LINES);
	addBanner(SingleChoice::createBanner(tr("Balls")), ScreenSaver::BALLS);
#endif
	//addBanner(tr("Deform"), ScreenSaver::DEFORM); // the deform is for now unavailable!
	// TODO maybe we want an OK button for touch 10 as well

#ifdef LAYOUT_TOUCHX
	CheckableBanner *b = SingleChoice::createBanner(tr("Slideshow"), bt_global::skin->getImage("change_settings"));
	addBanner(b, ScreenSaver::SLIDESHOW);
	Page *p = new MultimediaSectionPage(getPageNode(MULTIMEDIA), MultimediaSectionPage::ITEMS_FILESYSTEM, new SlideshowSelector);
	b->connectRightButton(p);
	connect(b, SIGNAL(pageClosed()), SLOT(showPage()));

	timing = new ScreensaverTiming(tr("Slideshow timeout"), getTextChild(conf_node, "timeSlideShow").toInt());
	addBottomWidget(timing);
	timing->hide();
#endif
	connect(page_content, SIGNAL(bannerSelected(int)), SLOT(confirmSelection()));
	connect(this, SIGNAL(Closed()), SLOT(cleanUp()));
	bannerSelected(getTextChild(conf_node, "type").toInt());

	// to save parameters
	item_id = getTextChild(conf_node, "itemID").toInt();
}

void ScreenSaverPage::cleanUp()
{
	QMap<QString, QString> data;
	data["type"] = QString::number(bt_global::display->currentScreenSaver());
	if (timing)
		data["timeSlideShow"] = QString::number(timing->getTiming());
	setCfgValue(data, item_id);
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
		if (id == ScreenSaver::SLIDESHOW)
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
	sel_button->setChecked(metadata["selected"].toBool());
	sel_button->setStatus(metadata["selected"].toBool());

	sel_buttons->addButton(sel_button, id_btn);

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
	StateButton *button = qobject_cast<StateButton *>(sel_buttons->button(btn_id));
	Q_ASSERT_X(button, "FileList::checkButton", "invalid button");

	ItemInfo &info = item(current_page * rows_per_page + btn_id);
	QVariantMap metadata = info.data.toMap();
	bool selected = button->getStatus();

	metadata["selected"] = selected;
	info.data = metadata;

	emit itemSelectionChanged(info.description, selected);
}


SlideshowSelector::SlideshowSelector() :
		FileSelector(4, "/"), handler(new ImageSelectionHandler(SLIDESHOW_FILENAME))
{
	handler->setFileFilter(getImageFileFilter());

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
	connect(this, SIGNAL(Closed()), SLOT(cleanUp()));
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

void SlideshowSelector::cleanUp()
{
	saveFileList();
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
	// Create fileslist from files
	QList<QFileInfo> temp_files_list = directory.entryInfoList(getImageFileFilter());

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
		handler->removeItem(path);
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

void SlideshowSelector::saveFileList()
{
	Q_ASSERT_X(handler, "SlideshowSelector::saveFileList()", "handler is null");

	handler->saveSlideshowToFile();
}

#endif
