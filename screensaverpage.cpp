#include "screensaverpage.h"
#include "screensaver.h"
#include "displaycontrol.h" // bt_global::display
#include "hardware_functions.h"
#include "singlechoicecontent.h"
#include "navigation_bar.h" // NavigationBar
#include "skinmanager.h" // bt_global::skin
#include "btbutton.h" // BtButton

#include <QAbstractButton>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>
#include <QTemporaryFile>
#include <errno.h>

#define SLIDESHOW_FILENAME "cfg/extra/slideshow_images.txt"

ScreenSaverPage::ScreenSaverPage()
{
	addBanner(SingleChoice::createBanner(tr("No screensaver")), ScreenSaver::NONE);
	addBanner(SingleChoice::createBanner(tr("Line")), ScreenSaver::LINES);
	addBanner(SingleChoice::createBanner(tr("Balls")), ScreenSaver::BALLS);
	addBanner(SingleChoice::createBanner(tr("Time")), ScreenSaver::TIME);
	addBanner(SingleChoice::createBanner(tr("Text")), ScreenSaver::TEXT);
	//addBanner(tr("Deform"), ScreenSaver::DEFORM); // the deform is for now unavailable!

	// TODO maybe we want an OK button for touch 10 as well
	if (hardwareType() == TOUCH_X)
	{
		CheckableBanner *b = SingleChoice::createBanner(tr("Slideshow"), bt_global::skin->getImage("change_settings"));
		addBanner(b, ScreenSaver::SLIDESHOW);
		Page *p = new ImageRemovalPage;
		b->connectRightButton(p);
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		connect(page_content, SIGNAL(bannerSelected(int)), SLOT(confirmSelection()));
	}
}

void ScreenSaverPage::showPage()
{
	setCheckedId(getCurrentId());
	SingleChoicePage::showPage();
}

int ScreenSaverPage::getCurrentId()
{
	return bt_global::display.currentScreenSaver();
}

void ScreenSaverPage::bannerSelected(int id)
{
	bt_global::display.setScreenSaver(static_cast<ScreenSaver::Type>(id));
	if (id == ScreenSaver::NONE)
		bt_global::display.setBrightness(BRIGHTNESS_OFF);
}



SlideshowImageContent::SlideshowImageContent(QWidget *parent) :
	GridContent(parent)
{

}

void SlideshowImageContent::addItem(QWidget *item)
{
	items.append(item);
}

void SlideshowImageContent::drawContent()
{
	QGridLayout *l = qobject_cast<QGridLayout*>(layout());

	if (pages.size() == 0)
	{
		// compute the page list
		prepareLayout(items, 2);

		// add icons to the layout
		for (int i = 0; i < pages.size() - 1; ++i)
		{
			int base = pages[i];
			for (int j = 0; base + j < pages[i + 1]; ++j)
				l->addWidget(items.at(base + j), j / 2, j % 2);
		}

		l->setRowStretch(l->rowCount(), 1);
	}

	updateLayout(items);
}

void SlideshowImageContent::nextItem()
{
	current_page += 1;
	// wrap around to the first page
	if (current_page >= pageCount())
		current_page = 0;
	drawContent();
}

void SlideshowImageContent::prevItem()
{
	current_page -= 1;
	// wrap around to the last page
	if (current_page < 0)
		current_page = pageCount() - 1;
	drawContent();
}

void SlideshowImageContent::clearContent()
{
	foreach (QWidget *i, items)
	{
		i->disconnect();
		i->hide();
		i->deleteLater();
	}
	items.clear();
	pages.clear();
}

void SlideshowImageContent::showContent()
{
	drawContent();
}



SlideshowItemDir::SlideshowItemDir(const QString &path, const QString &checked_icon, const QString &unchecked_icon,
		const QString &main_icon) :
	QWidget(0),
	dir_path(path)
{
	dir_button = new BtButton;
	dir_button->setImage(main_icon);
	connect(dir_button, SIGNAL(clicked()), SLOT(dirButtonClicked()));
	text = new QLabel(path);
	check_button = new BtButton;
	check_button->setImage(unchecked_icon, BtButton::NO_FLAG);
	check_button->setPressedImage(checked_icon);
	check_button->setCheckable(true);
	connect(check_button, SIGNAL(toggled(bool)), SLOT(checked(bool)));

	// position main button and dir path below it
	QVBoxLayout *left = new QVBoxLayout;
	left->addWidget(dir_button);
	left->addWidget(text, 0, Qt::AlignHCenter);
	// position the check button
	QHBoxLayout *l = new QHBoxLayout(this);
	l->addLayout(left);
	l->addWidget(check_button, 0, Qt::AlignTop);
}

void SlideshowItemDir::checked(bool check)
{
	emit directoryToggled(check, dir_path);
}

void SlideshowItemDir::dirButtonClicked()
{
	emit browseDirectory(dir_path);
}

void SlideshowItemDir::setChecked(bool check)
{
	check_button->setChecked(check);
}



SlideshowItemImage::SlideshowItemImage(const QString &filename, const QString &working_dir, const QString &checked_icon, const QString &unchecked_icon) :
	QWidget(0),
	file_name(filename)
{
	QPixmap p(working_dir + file_name);
	p = p.scaled(QSize(100, 100), Qt::KeepAspectRatio);
	thumbnail = new QLabel;
	thumbnail->setPixmap(p);
	text = new QLabel(file_name);
	check_button = new BtButton;
	check_button->setImage(unchecked_icon, BtButton::NO_FLAG);
	check_button->setPressedImage(checked_icon);
	check_button->setCheckable(true);
	connect(check_button, SIGNAL(toggled(bool)), SLOT(checked(bool)));

	// position main button and dir path below it
	QVBoxLayout *left = new QVBoxLayout;
	left->addWidget(thumbnail, 1, Qt::AlignTop);
	left->addWidget(text, 0, Qt::AlignHCenter);
	// position the check button
	QHBoxLayout *l = new QHBoxLayout(this);
	l->addLayout(left);
	l->addWidget(check_button, 0, Qt::AlignTop);
}

void SlideshowItemImage::checked(bool check)
{
	emit fileToggled(check, file_name);
}

void SlideshowItemImage::setChecked(bool check)
{
	check_button->setChecked(check);
}


SlideshowSettings::SlideshowSettings() :
	QWidget(0)
{
	BtButton *add_images = new BtButton;
	add_images->setImage(bt_global::skin->getImage("add_image"));
	connect(add_images, SIGNAL(clicked()), SIGNAL(addMoreImages()));
	BtButton *remove_images = new BtButton;
	remove_images->setImage(bt_global::skin->getImage("remove_image"));
	connect(remove_images, SIGNAL(clicked()), SIGNAL(clearAllImages()));

	QHBoxLayout *l = new QHBoxLayout(this);
	l->addWidget(add_images);
	l->addWidget(remove_images);
}



SlideshowSelectionPage::SlideshowSelectionPage(const QString &start_path) :
	Page(0),
	current_dir(start_path),
	level(0)
{
	SlideshowImageContent *content = new SlideshowImageContent;
	PageTitleWidget *title_widget = new PageTitleWidget("Select photos", Page::TITLE_HEIGHT);
	connect(content, SIGNAL(contentScrolled(int, int)), title_widget, SLOT(setCurrentPage(int,int)));

	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("ok"));
	connect(nav_bar, SIGNAL(backClick()), SLOT(browseUp()));
	connect(nav_bar, SIGNAL(upClick()), content, SLOT(prevItem()));
	connect(nav_bar, SIGNAL(downClick()), content, SLOT(nextItem()));
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(confirmSelection()));
	connect(content, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));

	buildPage(content, nav_bar, 0, title_widget);
	connect(this, SIGNAL(Closed()), SLOT(clearCaches()));

	checked_icon = bt_global::skin->getImage("checked");
	unchecked_icon = bt_global::skin->getImage("unchecked");
	photo_icon = bt_global::skin->getImage("photo_album");

	current_dir.setSorting(QDir::DirsFirst | QDir::Name);
	current_dir.setFilter(QDir::AllDirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable);
}

void SlideshowSelectionPage::showPage()
{
	loadSlideshowFromFile();
	refreshContent();
	Page::showPage();
}

void SlideshowSelectionPage::showFiles()
{
	QStringList filters;
	filters << "*.jpg" << "*.png";

	QFileInfoList list = current_dir.entryInfoList(filters);
	bool are_all_selected = true;
	foreach (const QFileInfo &fi, list)
	{
		QString abs_path = current_dir.absolutePath() + QDir::separator() + fi.fileName();
		// if there's at least one not selected item we can't compact the directory
		bool is_selected = isItemSelected(abs_path);
		if (!is_selected)
			are_all_selected = false;
		QWidget *w = 0;
		if (fi.isDir())
		{
			SlideshowItemDir *it = new SlideshowItemDir(fi.fileName(), checked_icon, unchecked_icon, photo_icon);
			if (is_selected)
				it->setChecked(true);
			connect(it, SIGNAL(browseDirectory(QString)), SLOT(enterDirectory(QString)));
			connect(it, SIGNAL(directoryToggled(bool,QString)), SLOT(itemSelected(bool, QString)));
			w = it;
		}

		if (fi.isFile())
		{
			SlideshowItemImage *im = new SlideshowItemImage(fi.fileName(), current_dir.absolutePath() + QDir::separator(), checked_icon, unchecked_icon);
			if (is_selected)
				im->setChecked(true);
			connect(im, SIGNAL(fileToggled(bool,QString)), SLOT(itemSelected(bool, QString)));
			w = im;
		}
		Q_ASSERT_X(w!=0, "SlideshowImageSelection::browseFiles", "w is 0");
		page_content->addItem(w);
	}
	if (are_all_selected)
		//compactDir(current_dir.absolutePath());
		qDebug() << "Compacting directory: " << current_dir.absolutePath();
}

void SlideshowSelectionPage::browseUp()
{
	if (level > 0)
	{
		if (current_dir.cdUp())
		{
			--level;
			refreshContent();
		}
	}
	else
		emit Closed();
}

void SlideshowSelectionPage::enterDirectory(QString dir)
{
	if (current_dir.cd(dir))
	{
		++level;
		refreshContent();
	}
	else
		qWarning() << "Selected directory is not readable: " << current_dir.absolutePath() + "/" + dir;
}

void SlideshowSelectionPage::confirmSelection()
{
	selected_images.subtract(removed_images);
	selected_images.unite(inserted_images);
	clearCaches();
	// TODO: emit signal to notify image changes
	saveSlideshowToFile();
	// TODO: unroll directories
	emit Closed();
}

void SlideshowSelectionPage::itemSelected(bool is_checked, QString relative_path)
{
	QString abs_path = current_dir.absolutePath() + QDir::separator() + relative_path;
	if (is_checked)
	{
		// this check is to avoid incorrect results when the user clicks more than once on the button
		if (removed_images.contains(abs_path))
			removed_images.remove(abs_path);
		inserted_images.insert(abs_path);
	}
	else
	{
		if (inserted_images.contains(abs_path))
			inserted_images.remove(abs_path);
		// TODO: this must become removeCurrentFile()
		removed_images.insert(abs_path);
		qDebug() << "Removing current file and unselecting parent directory";
		//removeCurrentFile(abs_path);
	}
}

void SlideshowSelectionPage::saveSlideshowToFile()
{
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

void SlideshowSelectionPage::loadSlideshowFromFile()
{
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

bool SlideshowSelectionPage::isItemSelected(QString abs_path)
{
	while (!abs_path.isEmpty())
	{
		if ((selected_images.contains(abs_path) || inserted_images.contains(abs_path)) && !(removed_images.contains(abs_path)))
			return true;

		// find the previous separator and remove everything from that point
		int pos = abs_path.lastIndexOf(QDir::separator(), -2);
		if (pos < 0)
			// terminate the loop
			abs_path = QString();
		else
			abs_path = abs_path.left(pos);
	}
	return false;
}

void SlideshowSelectionPage::refreshContent()
{
	page_content->clearContent();
	showFiles();
	page_content->showContent();
}

void SlideshowSelectionPage::clearCaches()
{
	removed_images.clear();
	inserted_images.clear();
}



ImageRemovalPage::ImageRemovalPage() :
	Page(0)
{
	SlideshowImageContent *content = new SlideshowImageContent;
	PageTitleWidget *title_widget = new PageTitleWidget("Select photos", Page::TITLE_HEIGHT);
	connect(content, SIGNAL(contentScrolled(int, int)), title_widget, SLOT(setCurrentPage(int,int)));

	NavigationBar *nav_bar = new NavigationBar;
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(upClick()), content, SLOT(prevItem()));
	connect(nav_bar, SIGNAL(downClick()), content, SLOT(nextItem()));
	connect(content, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));

	SlideshowSettings *settings = new SlideshowSettings;
	Page *p = new SlideshowSelectionPage("cfg/slideshow");
	connect(settings, SIGNAL(addMoreImages()), p, SLOT(showPage()));
	connect(p, SIGNAL(Closed()), SLOT(showPage()));
	buildPage(content, nav_bar, settings, title_widget);

	loadImages();
}

void ImageRemovalPage::loadImages()
{
	// TODO: load images from file
}
